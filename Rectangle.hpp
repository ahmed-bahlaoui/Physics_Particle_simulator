#pragma once
#include "Projectile.hpp"
#include <vector>

using namespace std;

class Rectangle {
  private:
    float x, y, w, h;

  public:
    Rectangle(float _x, float _y, float _w, float _h)
        : x(_x), y(_y), w(_w), h(_h) {}

    bool contains(const Vector2D &point) {
        float x_pos = point.getX();
        float y_pos = point.getY();
        if ((x <= x_pos) && (x_pos <= x + w) && (y <= y_pos) &&
            (y_pos <= y + h)) {
            return true;
        } else {
            return false;
        }
    };

    bool intersects(const Rectangle &other) {
        // Return TRUE if NOT separated
        return !(x + w < other.x || other.x + other.w < x || y + h < other.y ||
                 other.y + other.h < y);
    }

    float getX() const { return x; };
    float getY() const { return y; };
    float getWidth() const { return w; };
    float getheight() const { return h; };
};

class QuadTree {
  private:
    Rectangle rectangle;
    int capacity;
    vector<Projectile *> projectiles;
    bool divided = false;
    QuadTree *northWest = nullptr;
    QuadTree *northEast = nullptr;
    QuadTree *southWest = nullptr;
    QuadTree *southEast = nullptr;

  public:
    QuadTree(Rectangle boundary, int n) : rectangle(boundary), capacity(n) {};
    void subdivide() {
        float x = rectangle.getX();
        float y = rectangle.getY();
        float w = rectangle.getWidth();
        float h = rectangle.getheight();

        float halfW = w / 2;
        float halfH = h / 2;

        // Top Row
        northWest = new QuadTree(Rectangle(x, y, halfW, halfH), capacity);
        northEast =
            new QuadTree(Rectangle(x + halfW, y, halfW, halfH), capacity);

        // Bottom Row (Y increases downwards!)
        southWest =
            new QuadTree(Rectangle(x, y + halfH, halfW, halfH), capacity);
        southEast = new QuadTree(Rectangle(x + halfW, y + halfH, halfW, halfH),
                                 capacity);

        divided = true;
    };
    bool insert(Projectile *p) {
        if (!rectangle.contains(p->getPosition())) {
            return false; // Out of bounds
        }

        if (projectiles.size() < capacity) {
            projectiles.push_back(p);
            return true;
        } else {
            if (!divided) {
                subdivide();
            }
            if (northEast->insert(p)) {
                return true;
            };
            if (northWest->insert(p)) {
                return true;
            };
            if (southEast->insert(p)) {
                return true;
            };
            if (southWest->insert(p)) {
                return true;
            };

            // 2. Try to insert into children one by one
        }

        return false; // Should theoretically never reach here if point is
                      // inside bounds
    };
    void query(Rectangle range, vector<Projectile *> &found) {
        // 1. CHECK OVERLAP
        // If this Quadtree's boundary does NOT intersect the search 'range',
        // we can stop immediately. (Use your new function!)
        if (!rectangle.intersects(range)) {
            return;
        }

        // 2. CHECK PARTICLES
        // Loop through the 'projectiles' in this node.
        for (auto p : projectiles) {
            if (range.contains(p->getPosition())) {
                found.push_back(p);
            }
        }
        // If a particle is strictly inside the 'range', add it to the 'found'
        // list. (Hint: Use range.contains(p->getPosition()))

        // 3. CHECK CHILDREN
        if (divided) {
            northEast->query(range, found);
            northWest->query(range, found);
            southEast->query(range, found);
            southWest->query(range, found);
        }
        // If this node is 'divided', call query(range, found) on all 4
        // children.
    }
};