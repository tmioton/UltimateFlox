#pragma once

#include "pch.hpp"

struct Boid;

inline Vector magnitude(Vector vec, float mag);

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

    //static constexpr glm::vec3 color {1.00000f, 0.00000f, 0.00000f};  // Red
    //static constexpr glm::vec3 color {1.00000f, 1.00000f, 1.00000f};  // White
    static constexpr glm::vec3 color {0.05098f, 0.19608f, 0.30196f};  // Prussian Blue
    //static constexpr glm::vec3 color {0.00000f, 0.29412f, 0.65882f};  // Cobalt Blue
    //static constexpr glm::vec3 color {0.24706f, 0.65490f, 0.83922f};  // Cerulean Crayola
    //static constexpr glm::vec3 color{0.76471f, 0.04314f, 0.30588f};  // Pictoral Carmine
    //static constexpr glm::vec3 color {0.17647f, 0.18824f, 0.27843f};  // Space Cadet
    //static constexpr glm::vec3 color {1.00000f, 0.87843f, 0.40000f};  // Naples Yellow

    // Non-static members
    Vector position, velocity;

    // Methods
    [[nodiscard]] Vector steer(Vector vec) const;
};
