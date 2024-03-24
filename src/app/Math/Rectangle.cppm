module;
#include "pch.hpp"
export module Rectangle;


export class Rectangle {
public:
    Rectangle() = default;

    Rectangle(const Vector c, const Vector s) : center(c), size(s) {}

    explicit Rectangle(const Vector s) : center(0.0f, 0.0f), size(s) {}

    Rectangle(Rectangle const &) = default;

    Rectangle(Rectangle &&) = default;

    Rectangle &operator=(Rectangle const &) = default;

    Rectangle &operator=(Rectangle &&) = default;

    Rectangle operator*(const float val) const {
        return {center, size * val};
    }

    [[nodiscard]] bool contains(const Vector p) const {
        return !(
            p.x < center.x - size.x ||
            p.x > center.x + size.x ||
            p.y < center.y - size.y ||
            p.y > center.y + size.y
        );
    }

    [[nodiscard]] bool intersects(const Rectangle &other) const {
        return !(
            other.center.x + other.size.x < center.x - size.x ||
            other.center.x - other.size.x > center.x + size.x ||
            other.center.y + other.size.y < center.y - size.y ||
            other.center.y - other.size.y > center.y + size.y
        );
    }

    [[nodiscard]] int quadrant(const Vector p) const {
        const bool x{p.x >= center.x};
        const bool y{p.y >= center.y};
        return (!y << 1) + (!x & y | x & !y) & 0b11;
    }

    Vector center{0, 0};
    Vector size{0, 0};
};

std::ostream& operator<<(std::ostream& os, Rectangle const& r) {
    os << "Rectangle(center: {"
    << r.center.x << ", " << r.center.y
    << "}, size: {" << r.size.x << ", " << r.size.y
    << "})";
    return os;
}

