#include "Projectile.hpp"
#include "Rectangle.hpp"
#include "Slider.hpp"
#include <SFML/Graphics.hpp>
#include <algorithm> // <--- NEW: Needed for std::clamp (optional but good)
#include <string>    // <--- NEW: Needed to convert float to string
#include <vector>

#define MAX_FRAMERATE 60

vector<Projectile> projectiles;
unsigned int window_height = 600;
unsigned int window_width = 800;
std::vector<Projectile> spawnProjectiles(int n) {
    std::vector<Projectile> new_list;
    for (int i = 0; i < n; i++) {
        float random_mass = (10 * (float)rand() / RAND_MAX) + 5;
        float random_radius = (10 * (float)rand() / RAND_MAX) + 10; // [5, 20]
        float random_X =
            window_width * ((float)rand() / RAND_MAX); // [0,window_width]
        float random_Y =
            window_height * ((float)rand() / RAND_MAX); // [0,window_height];
        float random_Vx = (100 * (float)rand() / RAND_MAX) - 50; //[-50, 50]
        float random_Vy = (100 * (float)rand() / RAND_MAX) - 50; //[-50, 50]
        if (i % 2 == 0) {
            // color RED
            new_list.push_back(Projectile(
                random_mass, random_radius, Vector2D(random_X, random_Y),
                Vector2D(random_Vx, random_Vy), sf::Color::Red));
        } else {
            // color BLUE
            new_list.push_back(Projectile(
                random_mass, random_radius, Vector2D(random_X, random_Y),
                Vector2D(random_Vx, random_Vy), sf::Color::Blue));
        }
    }
    return new_list;
}

int main() {
    sf::RenderWindow window(sf::VideoMode({window_width, window_height}),
                            "Physics Collision Simulation");
    window.setFramerateLimit(MAX_FRAMERATE);

    // --- NEW: FONT & TEXT SETUP ---
    sf::Font font;
    // Try loading Arial from the Windows system folder
    // (If you are on Linux/Mac, you might need a different path or a local
    // file)
    if (!font.openFromFile("arial.ttf")) {
        // If this fails, make sure you have a .ttf file in your project folder
        // and change the path to "arial.ttf"
        return -1;
    }
    int projectile_count = 50;
    sf::Color bg_color = sf::Color::Black;
    sf::Text eText(font);
    eText.setCharacterSize(24);
    if (bg_color == sf::Color::Black) {
        eText.setFillColor(sf::Color::White);
    } else {
        eText.setFillColor(sf::Color::Black);
    }
    // ------------------------------

    // Create a slider: X=50, Y=550, Width=200, Range=1 to 100, Start=20
    SimpleSlider projectileSlider(50.f, window_height - 50.f, 200.f, 1.f, 100.f,
                                  (float)projectile_count);
    sf::Text countText(font);
    countText.setCharacterSize(20);
    countText.setPosition({270.f, window_height - 60.f}); // Next to slider
    if (bg_color == sf::Color::White) {
        countText.setFillColor(sf::Color::Black);
    } else {
        countText.setFillColor(sf::Color::White);
    }

    // 2. SETUP Physics

    float dt = 0.1;
    // Start with elastic collision
    float e = 1.0f;
    projectiles =
        spawnProjectiles(projectile_count); // Let's start with 50 balls!

    while (window.isOpen()) {
        // --- INPUT HANDLING ---
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>())
                window.close();

            if (const auto *keyPress = event->getIf<sf::Event::KeyPressed>()) {
                // 1. Elasticity Controls (Up/Down)
                if (keyPress->code == sf::Keyboard::Key::Up) {
                    e += 0.1f;
                } else if (keyPress->code == sf::Keyboard::Key::Down) {
                    e -= 0.1f;
                }

                // FIX: Clamp e so it stays valid [0.0, 1.0]
                e = std::clamp(e, 0.0f, 1.0f);

                // 2. Slider Controls (Left/Right)
                if (keyPress->code == sf::Keyboard::Key::Left) {
                    projectileSlider.setValue(projectileSlider.getValue() - 5);
                } else if (keyPress->code == sf::Keyboard::Key::Right) {
                    projectileSlider.setValue(projectileSlider.getValue() + 5);
                }

                // 3. Respawn (R)
                if (keyPress->code == sf::Keyboard::Key::R) {
                    projectiles = spawnProjectiles(projectileSlider.getValue());
                }
            }
        }

        // --- TEXT/SLIDER UPDATES (Keep this) ---
        projectileSlider.update(window);
        eText.setString("elasticity coefficient = " +
                        std::to_string(e).substr(0, 3));
        countText.setString(
            "Count: " + std::to_string(projectileSlider.getValue()) +
            " (Press R)");

        // =====================================================
        // PHASE 1: UPDATE PHYSICS 🏃
        // =====================================================
        for (auto &p : projectiles) {
            p.update(dt);
            p.checkBoundaries(window_width, window_height);
        }

        // =====================================================
        // PHASE 2: BUILD QUADTREE 🌳
        // =====================================================
        // Create the tree ONCE per frame
        Rectangle root_boundary(0, 0, window_width, window_height);
        QuadTree root(root_boundary, 4);

        // Fill it with ALL particles
        for (auto &p : projectiles) {
            root.insert(&p);
        }

        // =====================================================
        // PHASE 3: RESOLVE COLLISIONS 💥
        // =====================================================
        for (auto &p : projectiles) {
            // Define search area around the particle
            float r = p.getRadius();
            // Search slightly larger area (r*2) to catch touching neighbors
            Rectangle range(
                p.getPosition().getX() - (r * 2), // Start 2 radii to the left
                p.getPosition().getY() - (r * 2), // Start 2 radii up
                r * 4,                            // Width is 4 radii
                r * 4                             // Height is 4 radii
            );
            // Ask tree for neighbors
            std::vector<Projectile *> neighbors;
            root.query(range, neighbors);

            // Check collision only against these neighbors
            for (Projectile *other : neighbors) {
                if (&p == other)
                    continue; // Don't check self

                if (p.isColliding(*other)) {
                    p.resolveCollision(*other, e);
                }
            }
        }

        // =====================================================
        // PHASE 4: RENDER 🎨
        // =====================================================
        window.clear(bg_color);

        // Draw particles
        for (const auto &p : projectiles) {
            sf::CircleShape shape(p.getRadius());
            shape.setPosition({p.getPosition().getX() - p.getRadius(),
                               p.getPosition().getY() - p.getRadius()});
            shape.setFillColor(p.getColor());
            window.draw(shape);
        }

        // Draw UI
        projectileSlider.draw(window);
        window.draw(countText);
        window.draw(eText);

        window.display();
    }
    return 0;
}