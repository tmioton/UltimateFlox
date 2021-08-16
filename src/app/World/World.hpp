#pragma once

#include "pch.hpp"

static constexpr float worldBound = 200.0f;

using Vector = glm::vec2;

inline glm::vec2 calculateBounds(float aspect) {
    return {
        aspect >= 1.0f ? worldBound * aspect : worldBound,
        aspect < 1.0f ? worldBound * aspect : worldBound
    };
}
