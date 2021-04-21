#include "pch.hpp"
#include "Vector.hpp"

Vector &Vector::operator+=(const Vector &other) {
    x += other.x;
    y += other.y;
    return *this;
}

Vector &Vector::operator+=(float scalar) {
    x += scalar;
    y += scalar;
    return *this;
}

Vector &Vector::operator-=(const Vector &other) {
    x -= other.x;
    y -= other.y;
    return *this;
}

Vector &Vector::operator-=(float scalar) {
    x -= scalar;
    y -= scalar;
    return *this;
}

Vector &Vector::operator*=(const Vector &other) {
    x *= other.x;
    y *= other.y;
    return *this;
}

Vector &Vector::operator*=(float scalar) {
    x *= scalar;
    y *= scalar;
    return *this;
}

Vector &Vector::operator/=(const Vector &other) {
    x /= other.x;
    y /= other.y;
    return *this;
}

Vector &Vector::operator/=(float scalar) {
    x /= scalar;
    y /= scalar;
    return *this;
}

Vector Vector::operator+(const Vector &other) const {
    return {x + other.x, y + other.y};
}

Vector Vector::operator+(float scalar) const {
    return {x + scalar, y + scalar};
}

Vector Vector::operator-(const Vector &other) const {
    return {x - other.x, y - other.y};
}

Vector Vector::operator-(float scalar) const {
    return {x - scalar, y - scalar};
}

Vector Vector::operator*(const Vector &other) const {
    return {x * other.x, y * other.y};
}

Vector Vector::operator*(float scalar) const {
    return {x * scalar, y * scalar};
}

Vector Vector::operator/(const Vector &other) const {
    return {x / other.x, y / other.y};
}

Vector Vector::operator/(float scalar) const {
    return {x / scalar, y / scalar};
}

std::ostream &operator<<(std::ostream &os, const Vector &vec) {
    os << '(' << vec.x << ',' << vec.y << ')';
    return os;
}

Vector operator+(float lhs, const Vector &rhs) {
    return {lhs + rhs.x, lhs + rhs.y};
}

Vector operator-(float lhs, const Vector &rhs) {
    return {lhs - rhs.x, lhs - rhs.y};
}

Vector operator*(float lhs, const Vector &rhs) {
    return {lhs * rhs.x, lhs * rhs.y};
}

Vector operator/(float lhs, const Vector &rhs) {
    return {lhs / rhs.x, lhs / rhs.y};
}

float Vector::magnitude() const {
    return std::sqrtf(x * x + y * y);
}

void Vector::magnitude(float value) {
    const float mag = magnitude();
    if (mag == 0.0f) {
        return;
    }

    const float factor = value / mag;
    x *= factor;
    y *= factor;
}

float Vector::distance(Vector other) const {
    const float combX = other.x - x;
    const float combY = other.y - y;
    return sqrtf(combX * combX + combY * combY);
}
