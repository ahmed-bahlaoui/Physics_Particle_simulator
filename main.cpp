#include "Projectile.hpp"
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
        float random_radius = (10 * (float)rand() / RAND_MAX) + 5; // [5, 15]
        float random_X =
            window_width * ((float)rand() / RAND_MAX); // [0,window_width]
        float random_Y =
            window_height * ((float)rand() / RAND_MAX); // [0,window_height];
        float random_Vx = (100 * (float)rand() / RAND_MAX) - 50; //[-50, 50]
        float random_Vy = (100 * (float)rand() / RAND_MAX) - 50; //[-50, 50]
        if (i % 2 == 0) {
            // color Red
            new_list.push_back(Projectile(
                random_mass, random_radius, Vector2D(random_X, random_Y),
                Vector2D(random_Vx, random_Vy), sf::Color::Red));
        } else {
            // color Green
            new_list.push_back(Projectile(
                random_mass, random_radius, Vector2D(random_X, random_Y),
                Vector2D(random_Vx, random_Vy), sf::Color::Green));
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
    if (!font.openFromFile("C:/Windows/Fonts/Arial.ttf")) {
        // If this fails, make sure you have a .ttf file in your project folder
        // and change the path to "arial.ttf"
        return -1;
    }
    sf::Color bg_color = sf::Color::White;
    sf::Text eText(font);
    eText.setCharacterSize(24);
    if (bg_color == sf::Color::Black) {
        eText.setFillColor(sf::Color::White);
    } else {
        eText.setFillColor(sf::Color::Black);
    }
    // ------------------------------

    // 2. SETUP Physics

    float dt = 0.1;
    float e = 1.0f;                     // Start with elastic collision
    projectiles = spawnProjectiles(50); // Let's start with 50 balls!

    while (window.isOpen()) {
        while (const auto event = window.pollEvent()) {
            if (event->is<sf::Event::Closed>()) {
                window.close();
            }

            // --- NEW: INPUT HANDLING ---
            // In SFML 3, we use getIf to safely check for specific event types
            if (const auto *keyPress = event->getIf<sf::Event::KeyPressed>()) {
                if (keyPress->code == sf::Keyboard::Key::Up) {
                    e += 0.1f;
                } else if (keyPress->code == sf::Keyboard::Key::Down) {
                    e -= 0.1f;
                }

                // Keep e between 0.0 and 1.0
                if (e > 1.0f)
                    e = 1.0f;
                if (e < 0.0f)
                    e = 0.0f;
            }
            // ---------------------------
        }

        // --- NEW: UPDATE TEXT ---
        // Convert float to string and update text
        // (std::to_string can result in "0.500000", so we can substring it if
        // we want it cleaner, but raw to_string is fine for now)
        eText.setString("e = " + std::to_string(e).substr(0, 3));

        // Recalculate position to keep it in the bottom right
        // We do this every frame in case the text width changes (e.g. 0.9
        // -> 1.0)
        sf::FloatRect textBounds = eText.getLocalBounds();
        eText.setPosition({window_width - textBounds.size.x -
                               20.f, // 20 pixels padding from right
                           window_height - textBounds.size.y - 20.f}
                          // 20 pixels padding from bottom
        );
        // ------------------------

        // C. Render

        window.clear(bg_color);
        for (auto &p : projectiles) {
            sf::CircleShape shape(p.getRadius());
            shape.setPosition({p.getPosition().getX() - p.getRadius(),
                               p.getPosition().getY() - p.getRadius()});
            shape.setFillColor(p.getColor());
            window.draw(shape);
            // 1. Update physics using dt
            p.update(dt);
            // 2. Check boundaries using window_width and window_height
            p.checkBoundaries(window_width, window_height);
        }
        // 3. Check collision with with every other ball
        for (int i = 0; i < projectiles.size(); i++) {
            // Inner loop: compares 'i' with every other ball 'j'
            for (int j = i + 1; j < projectiles.size(); j++) {
                if (projectiles[i].isColliding(projectiles[j])) {
                    projectiles[i].resolveCollision(projectiles[j], e);
                }
            }
        }

        // Don't forget to draw the text!
        window.draw(eText);

        window.display();
    }

    return 0;
}