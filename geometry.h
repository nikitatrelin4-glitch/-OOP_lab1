#pragma once

struct Vector2D {
    const double x;
    const double y;

    Vector2D(double x, double y) : x(x), y(y) {}

    Vector2D operator+(const Vector2D& v) const { return Vector2D(x + v.x, y + v.y); }
    Vector2D operator-(const Vector2D& v) const { return Vector2D(x - v.x, y - v.y); }
    Vector2D operator*(double k) const { return Vector2D(x * k, y * k); }
    Vector2D operator/(double k) const { return Vector2D(x / k, y / k); }

    bool operator==(const Vector2D& v) const { return x == v.x && y == v.y; }
    bool operator!=(const Vector2D& v) const { return !(*this == v); }
};

struct Point2D {
    const double x;
    const double y;

    Point2D(double x, double y) : x(x), y(y) {}

    Point2D operator+(const Vector2D& v) const { return Point2D(x + v.x, y + v.y); }
    Vector2D operator-(const Point2D& p) const { return Vector2D(x - p.x, y - p.y); }

    bool operator==(const Point2D& p) const { return x == p.x && y == p.y; }
    bool operator!=(const Point2D& p) const { return !(*this == p); }
};

struct Rect {
    const Point2D topLeft;
    const double width;
    const double height;

    Rect(Point2D topLeft, double width, double height)
        : topLeft(topLeft), width(width), height(height) {}

    Point2D bottomRight() const { return Point2D(topLeft.x + width, topLeft.y + height); }

    bool contains(const Point2D& p) const {
        return p.x >= topLeft.x && p.x <= topLeft.x + width
            && p.y >= topLeft.y && p.y <= topLeft.y + height;
    }

    bool operator==(const Rect& r) const {
        return topLeft == r.topLeft && width == r.width && height == r.height;
    }
    bool operator!=(const Rect& r) const { return !(*this == r); }
};
