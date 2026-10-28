#pragma once

class Vector2D {
    double x_;
    double y_;

public:
    Vector2D(double x, double y) : x_(x), y_(y) {}

    double x() const { return x_; }
    double y() const { return y_; }

    Vector2D operator+(const Vector2D& v) const { return Vector2D(x_ + v.x_, y_ + v.y_); }
    Vector2D operator-(const Vector2D& v) const { return Vector2D(x_ - v.x_, y_ - v.y_); }
    Vector2D operator*(double k) const { return Vector2D(x_ * k, y_ * k); }
    Vector2D operator/(double k) const { return Vector2D(x_ / k, y_ / k); }

    bool operator==(const Vector2D& v) const { return x_ == v.x_ && y_ == v.y_; }
    bool operator!=(const Vector2D& v) const { return !(*this == v); }
};

class Point2D {
    double x_;
    double y_;

public:
    Point2D(double x, double y) : x_(x), y_(y) {}

    double x() const { return x_; }
    double y() const { return y_; }

    Point2D operator+(const Vector2D& v) const { return Point2D(x_ + v.x(), y_ + v.y()); }
    Vector2D operator-(const Point2D& p) const { return Vector2D(x_ - p.x(), y_ - p.y()); }

    bool operator==(const Point2D& p) const { return x_ == p.x_ && y_ == p.y_; }
    bool operator!=(const Point2D& p) const { return !(*this == p); }
};

class Rect {
    Point2D topLeft_;
    double width_;
    double height_;

public:
    Rect(Point2D topLeft, double width, double height)
        : topLeft_(topLeft), width_(width), height_(height) {}

    Point2D topLeft() const { return topLeft_; }
    double width() const { return width_; }
    double height() const { return height_; }

    bool contains(const Point2D& p) const {
        return p.x() >= topLeft_.x() && p.x() <= topLeft_.x() + width_
            && p.y() >= topLeft_.y() && p.y() <= topLeft_.y() + height_;
    }
};
