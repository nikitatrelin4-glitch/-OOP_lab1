#pragma once

#include "geometry.h"
#include <string>

class UIElement {
protected:
    Point2D position_;
    Vector2D size_;
    bool enabled_;

public:
    UIElement(Point2D position, Vector2D size)
        : position_(position), size_(size), enabled_(true) {}

    virtual ~UIElement() = default;

    Rect bounds() const { return Rect(position_, size_.x(), size_.y()); }

    bool contains(const Point2D& p) const { return bounds().contains(p); }

    bool isEnabled() const { return enabled_; }

    void setEnabled(bool enabled) { enabled_ = enabled; }

    virtual void onClick() = 0;

    virtual bool canTakeFocus() const { return false; }

    virtual void onTextInput(const std::string& text) {}

    virtual std::string toString() const = 0;
};

class Button : public UIElement {
    std::string caption_;
    int clickCount_;

public:
    Button(Point2D position, Vector2D size, std::string caption)
        : UIElement(position, size), caption_(caption), clickCount_(0) {}

    void onClick() override {
        if (enabled_) {
            clickCount_++;
        }
    }

    int clickCount() const { return clickCount_; }

    std::string toString() const override {
        std::string state = enabled_ ? "включена" : "отключена";
        return "Кнопка \"" + caption_ + "\", нажатий: " + std::to_string(clickCount_)
            + ", состояние: " + state;
    }
};

class CheckBox : public UIElement {
    bool checked_;

public:
    CheckBox(Point2D position, Vector2D size)
        : UIElement(position, size), checked_(false) {}

    void onClick() override {
        if (enabled_) {
            checked_ = !checked_;
        }
    }

    bool isChecked() const { return checked_; }

    std::string toString() const override {
        std::string mark = checked_ ? "[x]" : "[ ]";
        std::string state = enabled_ ? "включен" : "отключен";
        return "Флажок " + mark + ", состояние: " + state;
    }
};

class TextField : public UIElement {
    std::string text_;

public:
    TextField(Point2D position, Vector2D size)
        : UIElement(position, size) {}

    bool canTakeFocus() const override { return true; }

    void onClick() override {}

    void onTextInput(const std::string& text) override {
        if (enabled_) {
            text_ += text;
        }
    }

    const std::string& text() const { return text_; }

    std::string toString() const override {
        std::string state = enabled_ ? "включено" : "отключено";
        return "Текстовое поле, текст: \"" + text_ + "\", состояние: " + state;
    }
};
