// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#pragma once

#ifndef NDEBUG
#define FLOX_SHOW_DEBUG_INFO
#endif

typedef unsigned long long int UnsignedInt;
typedef signed long long int SignedInt;
constexpr size_t MaxDepth {11};
constexpr size_t BucketSize {16};
constexpr std::array<float, 2> FloatEnable {0.0f, 1.0f};
constexpr std::array<float, 2> FloatDisable {1.0f, 0.0f};
constexpr float Epsilon {std::numeric_limits<float>::epsilon()};
using Vector = glm::vec<2, float, glm::defaultp>;
using Color = glm::vec<4, float, glm::defaultp>;
using Vec4 = glm::vec<4, float, glm::defaultp>;
using Projection = glm::mat<4, 4, float, glm::defaultp>;

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
