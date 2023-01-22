#include "pch.hpp"
#include "Boid.hpp"

float Boid::scale{5.0f};
float Boid::maxSpeed{100.0f};
float Boid::maxForce{3.0f};

// Have a trigger to update these when scale is changed.
float Boid::disruptiveRadius = Boid::scale + 5.0f;
float Boid::cohesiveRadius = 2.0f * Boid::disruptiveRadius;

Vector Boid::steer(Vector vec) const {
    const float maxF2 = Boid::maxForce * Boid::maxForce;
    Vector steering = magnitude(vec, Boid::maxSpeed) - velocity;
    const float l2 = glm::length2(steering);
    if (l2 > maxF2 && l2 != 0.0f) {
        return steering * glm::fastInverseSqrt(l2) * Boid::maxForce;
    } else {
        return steering;
    }
}
