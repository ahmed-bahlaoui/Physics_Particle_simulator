#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>

class SimpleSlider {
  private:
    sf::RectangleShape track;
    sf::CircleShape knob;
    float minValue;
    float maxValue;
    float currentValue;
    bool isDragging;

  public:
    SimpleSlider(float x, float y, float width, float minVal, float maxVal,
                 float startVal)
        : minValue(minVal), maxValue(maxVal), currentValue(startVal),
          isDragging(false) {
        // 1. Setup the Track (the line)
        track.setPosition({x, y + 10}); // Offset slightly to center with knob
        track.setSize({width, 5});
        track.setFillColor(sf::Color(100, 100, 100)); // Dark Gray

        // 2. Setup the Knob (the circle)
        knob.setRadius(12);
        knob.setOrigin({12, 12}); // Center the origin
        knob.setFillColor(sf::Color::White);
        updateKnobPosition(x, width); // Set initial position
    }

    // Update logic (call this every frame)
    void update(const sf::RenderWindow &window) {
        // Check if mouse is clicking/dragging
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            sf::Vector2i mousePos = sf::Mouse::getPosition(window);
            sf::FloatRect knobBounds = knob.getGlobalBounds();

            // Allow dragging if we click the knob OR the track
            if (knobBounds.contains(sf::Vector2f(mousePos)) ||
                track.getGlobalBounds().contains(sf::Vector2f(mousePos)) ||
                isDragging) {
                isDragging = true;

                // Move knob to mouse X, clamped to the track width
                float trackLeft = track.getPosition().x;
                float trackRight = trackLeft + track.getSize().x;
                float newX =
                    std::clamp((float)mousePos.x, trackLeft, trackRight);

                knob.setPosition({newX, track.getPosition().y +
                                            2}); // +2 for visual alignment

                // Calculate value based on percentage
                float percentage = (newX - trackLeft) / track.getSize().x;
                currentValue = minValue + percentage * (maxValue - minValue);
            }
        } else {
            isDragging = false;
        }
    }

    void draw(sf::RenderWindow &window) {
        window.draw(track);
        window.draw(knob);
    }

    int getValue() const { return static_cast<int>(currentValue); }

  private:
    void updateKnobPosition(float x, float width) {
        float percentage = (currentValue - minValue) / (maxValue - minValue);
        float newX = x + percentage * width;
        knob.setPosition({newX, track.getPosition().y + 2});
    }
};