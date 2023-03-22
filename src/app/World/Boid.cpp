// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "pch.hpp"
#include "Boid.hpp"


float Boid::scale {5.0f};
float Boid::maxSpeed {100.0f};
float Boid::maxForce {3.0f};

// Have a trigger to update these when scale is changed.
float Boid::disruptiveRadius = Boid::scale + 5.0f;
float Boid::cohesiveRadius = 2.0f * Boid::disruptiveRadius;
