// Copyright (c) 2023 Tate "Xak Nitram" Mioton

#include "Rectangle.hpp"

Rectangle::Rectangle(Vector c, Vector s) : center(c), size(s) {}

Rectangle::Rectangle(Vector s) : center(0.0f, 0.0f), size(s) {}

bool Rectangle::contains(Vector p) const {
    return !(
        p.x < center.x - size.x ||
        p.x > center.x + size.x ||
        p.y < center.y - size.y ||
        p.y > center.y + size.y
    );
}

bool Rectangle::has_intersection_with(const Rectangle &other) const {
    return !(
        other.center.x + other.size.x < center.x - size.x ||
        other.center.x - other.size.x > center.x + size.x ||
        other.center.y + other.size.y < center.y - size.y ||
        other.center.y - other.size.y > center.y + size.y
    );
}

bool Rectangle::no_intersection_with(const Rectangle &other) const {
    return (
        other.center.x + other.size.x < center.x - size.x ||
        other.center.x - other.size.x > center.x + size.x ||
        other.center.y + other.size.y < center.y - size.y ||
        other.center.y - other.size.y > center.y + size.y
    );
}

int Rectangle::quadrant(Vector p) const {
    bool x{p.x >= center.x};
    bool y{p.y >= center.y};
    return ((!y << 1) + (((!x) & y) | (x & (!y)))) & 0b11;
}

Rectangle Rectangle::operator*(float val) const {
    return {center, size * val};
}

std::ostream& operator<<(std::ostream& os, Rectangle const& r) {
    os << "Rectangle(center: {"
    << r.center.x << ", " << r.center.y
    << "}, size: {" << r.size.x << ", " << r.size.y
    << "})";
    return os;
}
