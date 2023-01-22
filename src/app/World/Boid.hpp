#pragma once

#include "pch.hpp"

inline Vector magnitude(Vector vec, float mag) {
    const float l2 = glm::length2(vec);
    if (l2 != 0.0f) {
        return vec * glm::fastInverseSqrt(l2) * mag;
    } else {
        return vec;
    }
}

constexpr std::size_t BoidColorCount = 5;
constexpr Color BoidColors[BoidColorCount] {
    Color{1.00000f, 1.00000f, 1.00000f},  // White
    Color{0.76471f, 0.04314f, 0.30588f},  // Pictoral Carmine
    Color{1.00000f, 0.00000f, 0.00000f},  // Red
    Color{0.94118f, 0.63529f, 0.00784f},  // Marigold
    Color{0.05098f, 0.19608f, 0.30196f},  // Prussian Blue
};

struct Boid {
    static float scale;
    static float maxSpeed, maxForce;
    static float disruptiveRadius, cohesiveRadius;

    // These weights get normalized, so they do not have to add to 1.
    static constexpr float primadonnaWeight = 0.4f;  // Desire to be on-screen. Must be stronger than alignment.
    static constexpr float alignmentWeight = 0.375f;   // Desire to move in the same direction as other boids.
    static constexpr float separationWeight = 0.75f;  // Desire to have personal space.
    static constexpr float cohesionWeight = 0.125f;    // Desire to compress the flock size.
    static constexpr float speedWeight = 0.0625f;       // Desire to move at full speed. Helps with separation.

    // Non-static members
    Vector position, velocity;

    // Methods
    [[nodiscard]] Vector steer(Vector vec) const;
};
