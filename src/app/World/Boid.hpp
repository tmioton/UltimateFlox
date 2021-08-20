#pragma once

#include "pch.hpp"
#include "World.hpp"

struct Boid;

using BoidArray = std::unique_ptr<Boid[]>;

Vector magnitude(Vector const& vec, float mag);

struct Boid {
    static constexpr float scale = 5.0f;

    static constexpr float maxSpeed = 100.0f;
    static constexpr float maxForce = 3.0f;

    static constexpr float disruptiveRadius = 1.75f * scale;
    static constexpr float cohesiveRadius = 2.0f * disruptiveRadius;

    // These weights get normalized, so they do not have to add to 1.
    static constexpr float primadonnaWeight = 0.4f;  // Desire to be on-screen. Must be stronger than alignment.
    static constexpr float alignmentWeight = 0.375f;   // Desire to move in the same direction as other boids.
    static constexpr float separationWeight = 0.5f;  // Desire to have personal space.
    static constexpr float cohesionWeight = 0.125f;    // Desire to compress the flock size.
    static constexpr float speedWeight = 0.0625f;       // Desire to move at full speed. Helps with separation.

    // Non-static members
    Vector position, velocity;

    // Methods
    [[nodiscard]] Vector steer(Vector const& vec) const;
};
