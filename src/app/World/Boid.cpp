#include "pch.hpp"
#include "Boid.hpp"

Vector magnitude(Vector const &vec, float mag) {
    const float l2 = glm::length2(vec);
    if (l2 != 0.0f) {
        return vec * glm::fastInverseSqrt(l2) * mag;
    } else {
        return vec;
    }
}

Vector Boid::steer(Vector const &vec) const {
    constexpr float maxF2 = Boid::maxForce * Boid::maxForce;
    Vector steering = magnitude(vec, Boid::maxSpeed) - velocity;
    const float l2 = glm::length2(steering);
    if (l2 > maxF2 && l2 != 0.0f) {
        return steering * glm::fastInverseSqrt(l2) * Boid::maxForce;
    } else {
        return steering;
    }
}
