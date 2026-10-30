#include "geometry.h"
#include "elements.h"
#include "form.h"
#include <iostream>
#include <locale.h>
#include <windows.h>


int passed = 0;
int failed = 0;

void check(bool condition, const std::string& name) {
    if (condition) {
        passed++;
        std::cout << "Тест пройден: " << name << std::endl;
    } else {
        failed++;
        std::cout << "Тест НЕ пройден: " << name << std::endl;
    }
}

void testGeometry() {
    Point2D a(2.0, 5.0);
    Vector2D v(3.0, 4.0);
    Point2D b = a + v;
    check(b == Point2D(4.0, 6.0), "Point2D + Vector2D");
    check((b - a) == v, "Point2D - Point2D = Vector2D");
    check((v + v) == Vector2D(6.0, 8.0), "Vector2D + Vector2D");
    check((v - v) == Vector2D(0.0, 0.0), "Vector2D - Vector2D");
    check((v * 2.0) == Vector2D(6.0, 8.0), "Vector2D * scalar");
    check((v / 2.0) == Vector2D(1.5, 2.0), "Vector2D / scalar");
    check(a != Point2D(0.0, 0.0), "Point2D !=");
    check(v != Vector2D(1.0, 1.0), "Vector2D !=");

    Rect rect(Point2D(0.0, 0.0), 10.0, 5.0);
    check(rect.contains(Point2D(0.0, 0.0)), "Rect содержит угол");
    check(rect.contains(Point2D(10.0, 5.0)), "Rect содержит противоположный угол");
    check(rect.contains(Point2D(5.0, 2.5)), "Rect содержит центр");
    check(!rect.contains(Point2D(11.0, 2.0)), "Rect не содержит точку снаружи");
}

void testElements() {
    Button button(Point2D(0.0, 0.0), Vector2D(10.0, 5.0), "ОК");
    button.onClick();
    button.onClick();
    check(button.clickCount() == 2, "Кнопка считает нажатия");
    button.setEnabled(false);
    button.onClick();
    check(button.clickCount() == 2, "Отключенная кнопка не реагирует");

    CheckBox box(Point2D(0.0, 0.0), Vector2D(5.0, 5.0));
    box.onClick();
    check(box.isChecked(), "Флажок включается");
    box.onClick();
    check(!box.isChecked(), "Флажок выключается");
    box.setEnabled(false);
    box.onClick();
    check(!box.isChecked(), "Отключенный флажок не меняется");

    TextField field(Point2D(0.0, 0.0), Vector2D(10.0, 5.0));
    check(field.canTakeFocus(), "Текстовое поле поддерживает фокус");
    check(!button.canTakeFocus(), "Кнопка не поддерживает фокус");
    field.onTextInput("Привет");
    check(field.text() == "Привет", "Текстовое поле принимает ввод");
    field.setEnabled(false);
    field.onTextInput("!");
    check(field.text() == "Привет", "Отключенное поле не принимает ввод");
}

void testForm() {
    Form form;

    form.add(std::make_unique<Button>(Point2D(0.0, 0.0), Vector2D(10.0, 5.0), "Нижняя"));
    form.add(std::make_unique<Button>(Point2D(0.0, 0.0), Vector2D(10.0, 5.0), "Верхняя"));
    form.add(std::make_unique<CheckBox>(Point2D(20.0, 0.0), Vector2D(5.0, 5.0)));
    form.add(std::make_unique<TextField>(Point2D(0.0, 10.0), Vector2D(30.0, 5.0)));

    form.handleClick(Point2D(5.0, 2.5));
    Button* top = dynamic_cast<Button*>(form.elementAt(Point2D(5.0, 2.5)));
    check(top != nullptr && top->clickCount() == 1, "Нажатие получает верхний элемент");

    form.handleClick(Point2D(22.0, 2.0));
    CheckBox* box = dynamic_cast<CheckBox*>(form.elementAt(Point2D(22.0, 2.0)));
    check(box != nullptr && box->isChecked(), "Нажатие на флажок переключает его");

    form.handleClick(Point2D(5.0, 12.0));
    check(form.focusedElement() != nullptr, "Нажатие на поле дает фокус");

    form.sendText("Alice");
    TextField* field = dynamic_cast<TextField*>(form.focusedElement());
    check(field != nullptr && field->text() == "Alice", "Ввод уходит элементу в фокусе");

    form.focusNext();
    check(form.focusedElement() == field, "Фокус переходит только на поле");
    form.focusNext();
    check(form.focusedElement() == field, "Фокус циклически возвращается на поле");

    form.setEnabled(3, false);
    form.focusNext();
    check(form.focusedElement() == nullptr || !form.focusedElement()->isEnabled(), "Отключенный элемент не получает фокус");

    form.handleClick(Point2D(5.0, 2.5));
    check(top->clickCount() == 2, "Нажатие по координатам работает после изменений");

    check(form.elementAt(Point2D(100.0, 100.0)) == nullptr, "Вне элементов ничего не найдено");

    check(form.remove(1), "Удаление элемента");
    check(form.size() == 3, "Размер формы после удаления");
    check(!form.remove(10), "Удаление по несуществующему индексу");

    form.render();
}

int main() {
    SetConsoleOutputCP(1251);
    setlocale(LC_ALL, "Russian");

    testGeometry();
    testElements();
    testForm();

    std::cout << std::endl;
    std::cout << "Итого пройдено: " << passed << ", не пройдено: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
