#pragma once

#include "elements.h"
#include <vector>
#include <memory>
#include <iostream>
#include <algorithm>

class Form {
    std::vector<std::unique_ptr<UIElement>> elements_;
    int focused_;

    // Единый механизм поиска верхнего элемента под точкой.
    // Порядок в векторе = порядок наложения: ищем с конца.
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

    void setFocus(UIElement* element) {
        UIElement* current = focusedElement();
        if (current == element) {
            return;
        }
        if (current != nullptr) {
            current->onFocusLost();
        }
        focused_ = indexOf(element);
        if (element != nullptr) {
            element->onFocusGained();
        }
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
            elements_[index]->onFocusLost();
            focused_ = -1;
        }
        return true;
    }

    // Верхний элемент под точкой (включая отключённые — для идентификации).
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
            setFocus(element);
        }
    }

    void sendText(const std::string& text) {
        UIElement* element = focusedElement();
        if (element != nullptr && element->isEnabled()) {
            element->onTextInput(text);
        }
    }

    // Переход фокуса определяется только полиморфным canTakeFocus()
    // и isEnabled() — без условных проверок конкретного типа.
    void focusNext() {
        if (elements_.empty()) {
            return;
        }
        for (size_t step = 1; step <= elements_.size(); step++) {
            int index = (focused_ + static_cast<int>(step))
                % static_cast<int>(elements_.size());
            if (index < 0) {
                index += static_cast<int>(elements_.size());
            }
            if (elements_[index]->isEnabled() && elements_[index]->canTakeFocus()) {
                setFocus(elements_[index].get());
                return;
            }
        }
        setFocus(nullptr);
    }

    UIElement* focusedElement() const {
        if (focused_ < 0) {
            return nullptr;
        }
        return elements_[focused_].get();
    }

    size_t size() const { return elements_.size(); }

    // Отображение рисунка формы в консоли:
    // рамки элементов по их областям, '#' — элемент в фокусе.
    void render() const {
        std::vector<std::string> grid(12, std::string(60, ' '));

        auto drawRect = [&](const Rect& r, char border) {
            int x0 = std::max(0, static_cast<int>(r.topLeft.x));
            int y0 = std::max(0, static_cast<int>(r.topLeft.y));
            int x1 = std::min(59, static_cast<int>(r.bottomRight().x));
            int y1 = std::min(11, static_cast<int>(r.bottomRight().y));
            for (int y = y0; y <= y1; y++) {
                for (int x = x0; x <= x1; x++) {
                    bool edge = (y == y0 || y == y1 || x == x0 || x == x1);
                    grid[y][x] = edge ? border : ' ';
                }
            }
        };

        for (size_t i = 0; i < elements_.size(); i++) {
            char border = (static_cast<int>(i) == focused_) ? '#' : '+';
            drawRect(elements_[i]->bounds(), border);
        }

        for (const auto& line : grid) {
            std::cout << line << std::endl;
        }
        for (size_t i = 0; i < elements_.size(); i++) {
            std::string mark = (static_cast<int>(i) == focused_) ? " (в фокусе)" : "";
            std::cout << i << ": " << elements_[i]->toString() << mark << std::endl;
        }
    }
};
