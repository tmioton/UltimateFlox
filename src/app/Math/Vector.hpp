#pragma once
#include "pch.hpp"

struct Vector {
    float x = 0.0f;
    float y = 0.0f;

    [[nodiscard]] float magnitude() const;
    void magnitude(float value);

    // Operators
    Vector& operator+=(const Vector& other);
    Vector& operator+=(float scalar);

    Vector& operator-=(const Vector& other);
    Vector& operator-=(float scalar);

    Vector& operator*=(const Vector& other);
    Vector& operator*=(float scalar);

    Vector& operator/=(const Vector& other);
    Vector& operator/=(float scalar);

    Vector operator+(const Vector& other) const;
    Vector operator+(float scalar) const;

    Vector operator-(const Vector& other) const;
    Vector operator-(float scalar) const;

    Vector operator*(const Vector& other) const;
    Vector operator*(float scalar) const;

    Vector operator/(const Vector& other) const;
    Vector operator/(float scalar) const;

    friend Vector operator+(float lhs, const Vector& rhs);
    friend Vector operator-(float lhs, const Vector& rhs);
    friend Vector operator*(float lhs, const Vector& rhs);
    friend Vector operator/(float lhs, const Vector& rhs);

    friend std::ostream& operator<<(std::ostream& os, const Vector& vec);
};
