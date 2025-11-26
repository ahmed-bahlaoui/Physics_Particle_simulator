#pragma once
#include <SFML/Graphics.hpp>
#include <cmath>
#include <iostream>
using namespace std;

class Vector2D {
  private:
    float x;
    float y;

  public:
    Vector2D(float x_coord, float y_coord) {
        x = x_coord;
        y = y_coord;
    }
    Vector2D operator+(const Vector2D &other) const {
        return Vector2D(x + other.x, y + other.y);
    }

    Vector2D operator-(const Vector2D &other) const {
        return Vector2D(x - other.x, y - other.y);
    }
    Vector2D operator*(float k) const { return Vector2D(x * k, y * k); }

    Vector2D operator/(float k) const { return Vector2D(x / k, y / k); }

    float dot(const Vector2D &other) const {
        return (x * other.x) + (y * other.y);
    }

    float magnitude() const { return sqrt(x * x + y * y); }

    float getX() const { return x; };
    float getY() const { return y; };
    void setX(float newX) { x = newX; }
    void setY(float newY) { y = newY; }
};

class Projectile {
  private:
    float mass;
    float radius;
    Vector2D position;
    Vector2D velocity;
    sf::Color color;

  public:
    Projectile(float m, float r, Vector2D pos, Vector2D vel, sf::Color c)
        : mass(m), radius(r), position(pos), velocity(vel), color(c) {};
    void update(float dt) { position = position + velocity * dt; }
    bool isColliding(const Projectile &other) const {

        Vector2D diff = position - other.position;

        if (diff.dot(diff) < pow((radius + other.radius), 2)) {
            return true;
        } else
            return false;
    }
    void resolveCollision(Projectile &other, float e) {
        Vector2D diff = position - other.position;
        float diff_magnitude = diff.magnitude();
        Vector2D n = diff / diff_magnitude;
        Vector2D vRel = velocity - other.velocity; // Relative velocity
        float j = -(1 + e) * (vRel.dot(n)) / ((1 / mass) + (1 / other.mass));
        velocity = velocity + n * (j / mass);
        other.velocity = other.velocity - n * (j / other.mass);
        float overlap_amount =
            std::abs(diff_magnitude - (radius + other.radius));
        position = position + n * (overlap_amount / 2);
        other.position = other.position - n * (overlap_amount / 2);
    };
    float getRadius() const { return radius; };
    sf::Color getColor() const { return color; }
    Vector2D getPosition() const { return position; };

    void checkBoundaries(unsigned int width, unsigned int height) {
        // 1. Check Right Wall (width)
        if (position.getX() + radius >= width) {
            velocity.setX(velocity.getX() * -1);
            position.setX(width - radius);
        };
        // 2. Check Left Wall (0)

        if (position.getX() - radius <= 0) {
            velocity.setX(velocity.getX() * -1);
            position.setX(0 + radius);
        };

        // 3. Check Bottom Wall (height)

        if (position.getY() + radius >= height) {
            velocity.setY(velocity.getY() * -1);
            position.setY(height - radius);
        };
        // 4. Check Top Wall (0)

        if (position.getY() - radius <= 0) {
            velocity.setY(velocity.getY() * -1);
            position.setY(0 + radius);
        };
    };
};
