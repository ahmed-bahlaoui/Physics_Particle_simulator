# 🎱 2D Physics & Collision Simulator

A real-time 2D physics engine built from scratch in C++ using SFML. It features elastic/inelastic collisions, spatial partitioning (Quadtrees) for performance, and interactive UI controls.

## ✨ Features

- **Custom Physics Engine:** Impulse resolution and positional correction for stable collisions.
- **Quadtree Optimization:** Handles 500+ particles at 60 FPS by reducing check complexity from O(N²) to O(N log N).
- **Interactive UI:**
  - **Slider:** Adjust particle count dynamically.
  - **Elasticity:** Change coefficient of restitution (bounciness) in real-time.
  - **Pause Mode:** Freeze time

https://github.com/user-attachments/assets/8940579d-22ee-486f-bcef-6bb9929f8fb7

o inspect collisions.

## 🎮 Controls

| Key                     | Action                                      |
| :---------------------- | :------------------------------------------ |
| **Left / Right Arrows** | Decrease / Increase particle count          |
| **Up / Down Arrows**    | Increase / Decrease elasticity (bounciness) |
| **R**                   | Respawn particles                           |
| **P**                   | Pause / Resume simulation                   |
| **Mouse Drag**          | Adjust the slider manually                  |

## 🛠️ How to Build

This project uses **CMake**, so it works on Windows, macOS, and Linux.

### Prerequisites

- C++ Compiler (MinGW, Clang, or MSVC)
- CMake
- SFML 3.0 (The CMake script will try to find it, or you can install it via package manager)

### Building

```bash
mkdir build
cd build
cmake ..
cmake --build .
```
