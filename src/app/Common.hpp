#pragma once

constexpr size_t MaxDepth {11};
constexpr size_t BucketSize {16};
constexpr std::array<float, 2> FloatEnable {0.0f, 1.0f};
constexpr std::array<float, 2> FloatDisable {1.0f, 0.0f};
constexpr float Epsilon {std::numeric_limits<float>::epsilon()};
using Vector = glm::vec2;
using Color = glm::vec3;
using Projection = glm::mat4;

template<int length, typename T, glm::qualifier Q>
std::ostream &operator<<(std::ostream &os, glm::vec<length, T, Q> vec) {
    os << '(';
    for (int i = 0; i < length; ++i) {
        if (i) { os << ", "; }
        os << vec[i];
    }
    os << ')';
    return os;
}
