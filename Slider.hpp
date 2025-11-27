#pragma once
#include <SFML/Graphics.hpp>
#include <algorithm>
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
        if (sf::Mouse::isButtonPressed(sf::Mouse::Button::Left)) {
            // OLD WAY (Breaks on resize):
            // sf::Vector2i mousePos = sf::Mouse::getPosition(window);

            // NEW WAY (Fixes resize):
            // Convert screen pixels to world coordinates
            sf::Vector2f mousePos =
                window.mapPixelToCoords(sf::Mouse::getPosition(window));

            sf::FloatRect knobBounds = knob.getGlobalBounds();

            // Remove the "sf::Vector2f" cast below since mousePos is already a
            // float vector now
            if (knobBounds.contains(mousePos) ||
                track.getGlobalBounds().contains(mousePos) || isDragging) {
                isDragging = true;

                float trackLeft = track.getPosition().x;
                float trackRight = trackLeft + track.getSize().x;

                // mousePos.x is already a float now
                float newX = std::clamp(mousePos.x, trackLeft, trackRight);

                knob.setPosition({newX, track.getPosition().y + 2});

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
    void setValue(float newValue) {
        // 1. Clamp the value so it stays within range (e.g., 1 to 100)
        currentValue = std::clamp(newValue, minValue, maxValue);

        // 2. Calculate where the knob should be visually
        // (This is the reverse of the math in the update() function!)
        float percentage = (currentValue - minValue) / (maxValue - minValue);

        // Use the track's stored position and size
        float newX = track.getPosition().x + (percentage * track.getSize().x);
        knob.setPosition({newX, track.getPosition().y + 2});
    }

  private:
    void updateKnobPosition(float x, float width) {
        float percentage = (currentValue - minValue) / (maxValue - minValue);
        float newX = x + percentage * width;
        knob.setPosition({newX, track.getPosition().y + 2});
    }
};