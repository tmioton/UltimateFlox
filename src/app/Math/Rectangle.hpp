#pragma once

class Rectangle {
public:
    Rectangle() = default;

    Rectangle(Vector center, Vector size);

    explicit Rectangle(Vector size);

    Rectangle(Rectangle const &) = default;

    Rectangle(Rectangle &&) = default;

    Rectangle &operator=(Rectangle const &) = default;

    Rectangle &operator=(Rectangle &&) = default;

    Rectangle operator*(float) const;

    [[nodiscard]] bool contains(Vector point) const;

    [[nodiscard]] bool intersects(Rectangle &) const;

    [[nodiscard]] int quadrant(Vector point) const;

    Vector center{0, 0};
    Vector size{0, 0};
};
