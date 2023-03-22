// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once

#include "pch.hpp"


constexpr std::size_t BoidColorCount = 5;
constexpr Color BoidColors[BoidColorCount] {
    Color {1.00000f, 1.00000f, 1.00000f, 1.0f},  // White
    Color {0.76471f, 0.04314f, 0.30588f, 1.0f},  // Pictoral Carmine
    Color {1.00000f, 0.00000f, 0.00000f, 1.0f},  // Red
    Color {0.94118f, 0.63529f, 0.00784f, 1.0f},  // Marigold
    Color {0.05098f, 0.19608f, 0.30196f, 1.0f},  // Prussian Blue
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
};

inline float fastInverseSqrt(const float d) {
    constexpr std::uint32_t magic = 0x5f3759df;
    // reinterpret_cast doesn't appear to have too much effect compared to *(long*)&d
    const std::uint32_t i = magic - (*reinterpret_cast<std::uint32_t*>(const_cast<float*>(&d)) >> 1);
    const float y = *reinterpret_cast<float*>(const_cast<std::uint32_t*>(&i));
    return y * (1.5f - (d * 0.5f * y * y));
}

inline Vector magnitude(const Vector vec, const float mag) {
    return vec * fastInverseSqrt(glm::dot(vec, vec)) * mag;
}

inline Vector truncate(const Vector vec, const float max) {
    const float i = max * fastInverseSqrt(glm::dot(vec, vec));
    const bool do_truncate = i < 1.0f;
    return vec * (i * FloatEnable[do_truncate] + FloatDisable[do_truncate]);
    //return vec * (i < 1.0f ? i : 1.0f);  ~7% slower with branch
}

inline Vector steer(const Vector vec, const Vector velocity) {
    return truncate(magnitude(vec, Boid::maxSpeed) - velocity, Boid::maxForce);
}
