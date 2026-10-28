#pragma once

#include "elements.h"
#include <vector>
#include <memory>
#include <iostream>

class Form {
    std::vector<std::unique_ptr<UIElement>> elements_;
    int focused_;

    UIElement* topElementAt(const Point2D& p, bool onlyEnabled) const {
        for (auto it = elements_.rbegin(); it != elements_.rend(); ++it) {
            if ((*it)->contains(p) && (!onlyEnabled || (*it)->isEnabled())) {
                return it->get();
            }
        }
        return nullptr;
    }

    int indexOf(const UIElement* element) const {
        for (size_t i = 0; i < elements_.size(); i++) {
            if (elements_[i].get() == element) {
                return static_cast<int>(i);
            }
        }
        return -1;
    }

public:
    Form() : focused_(-1) {}

    void add(std::unique_ptr<UIElement> element) {
        elements_.push_back(std::move(element));
    }

    bool remove(size_t index) {
        if (index >= elements_.size()) {
            return false;
        }
        if (static_cast<int>(index) == focused_) {
            focused_ = -1;
        }
        elements_.erase(elements_.begin() + index);
        return true;
    }

    bool setEnabled(size_t index, bool enabled) {
        if (index >= elements_.size()) {
            return false;
        }
        elements_[index]->setEnabled(enabled);
        if (!enabled && static_cast<int>(index) == focused_) {
            focused_ = -1;
        }
        return true;
    }

    UIElement* elementAt(const Point2D& p) const {
        return topElementAt(p, false);
    }

    void handleClick(const Point2D& p) {
        UIElement* element = topElementAt(p, true);
        if (element == nullptr) {
            return;
        }
        element->onClick();
        if (element->canTakeFocus()) {
            focused_ = indexOf(element);
        }
    }

    void sendText(const std::string& text) {
        if (focused_ >= 0) {
            elements_[focused_]->onTextInput(text);
        }
    }

    void focusNext() {
        if (elements_.empty()) {
            return;
        }
        for (size_t step = 1; step <= elements_.size(); step++) {
            int index = (focused_ + static_cast<int>(step)) % static_cast<int>(elements_.size());
            if (index < 0) {
                index += static_cast<int>(elements_.size());
            }
            if (elements_[index]->isEnabled() && elements_[index]->canTakeFocus()) {
                focused_ = index;
                return;
            }
        }
    }

    UIElement* focusedElement() const {
        if (focused_ < 0) {
            return nullptr;
        }
        return elements_[focused_].get();
    }

    size_t size() const { return elements_.size(); }

    void render() const {
        std::cout << "=== Состояние формы ===" << std::endl;
        for (size_t i = 0; i < elements_.size(); i++) {
            std::string mark = (static_cast<int>(i) == focused_) ? " (в фокусе)" : "";
            std::cout << i << ": " << elements_[i]->toString() << mark << std::endl;
        }
        std::cout << "=======================" << std::endl;
    }
};
