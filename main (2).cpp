#include "expression.h"
#include <iostream>
#include <locale.h>

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

int main() {
    setlocale(LC_ALL, "Russian");
    setlocale(LC_ALL, ".UTF-8");

    std::cout << "=== Выражение только из констант ===" << std::endl;
    auto constExpr = Expression<double>::binary(
        Expression<double>::Kind::Add,
        Expression<double>::constant(2.0),
        Expression<double>::constant(3.0));
    std::map<std::string, double> vars;
    std::cout << constExpr->toString() << " = " << constExpr->evaluate(vars) << std::endl;
    check(constExpr->evaluate(vars) == 5.0, "Вычисление константного выражения");
    check(constExpr->variables().empty(), "Переменных нет");
    check(constExpr->depth() == 2, "Глубина константного выражения");
    check(constExpr->nodeCount() == 3, "Количество узлов");

    std::cout << std::endl << "=== Выражение с одной переменной ===" << std::endl;
    auto oneVar = Expression<double>::binary(
        Expression<double>::Kind::Mul,
        Expression<double>::constant(2.0),
        Expression<double>::variable("price"));
    std::cout << oneVar->toString() << std::endl;
    vars["price"] = 10.0;
    check(oneVar->evaluate(vars) == 20.0, "Вычисление с одной переменной");
    check(oneVar->variables().count("price") == 1 && oneVar->variables().size() == 1, "Список переменных");

    std::cout << std::endl << "=== Выражение с несколькими переменными ===" << std::endl;
    auto multiVar = Expression<double>::binary(
        Expression<double>::Kind::Add,
        Expression<double>::binary(
            Expression<double>::Kind::Mul,
            Expression<double>::variable("price"),
            Expression<double>::variable("count")),
        Expression<double>::variable("delivery"));
    std::cout << multiVar->toString() << std::endl;
    vars["price"] = 100.0;
    vars["count"] = 2.0;
    vars["delivery"] = 50.0;
    check(multiVar->evaluate(vars) == 250.0, "price * count + delivery = 250");
    vars["count"] = 3.0;
    check(multiVar->evaluate(vars) == 350.0, "Вычисление при других значениях");
    check(multiVar->variables().size() == 3, "Три переменных");
    check(multiVar->depth() == 3, "Глубина выражения");
    check(multiVar->nodeCount() == 5, "Количество узлов");

    std::cout << std::endl << "=== Глубокая вложенность и упрощение ===" << std::endl;
    auto nested = Expression<double>::binary(
        Expression<double>::Kind::Mul,
        Expression<double>::binary(
            Expression<double>::Kind::Add,
            Expression<double>::constant(2.0),
            Expression<double>::constant(3.0)),
        Expression<double>::variable("price"));
    std::cout << "Исходное: " << nested->toString() << std::endl;
    auto simplified = nested->simplify();
    std::cout << "Упрощенное: " << simplified->toString() << std::endl;
    check(simplified->toString() == "(5.000000 * price)", "Упрощение (2 + 3) * price");
    check(nested->toString() == "((2.000000 + 3.000000) * price)", "Исходное дерево не изменилось");
    check(simplified->depth() == 2, "Глубина упрощенного выражения");
    vars["price"] = 4.0;
    check(simplified->evaluate(vars) == 20.0, "Упрощенное выражение вычисляется");
    check(nested->evaluate(vars) == 20.0, "Исходное выражение вычисляется");

    auto deepConst = Expression<double>::binary(
        Expression<double>::Kind::Add,
        Expression<double>::binary(
            Expression<double>::Kind::Mul,
            Expression<double>::constant(1.0),
            Expression<double>::constant(2.0)),
        Expression<double>::binary(
            Expression<double>::Kind::Sub,
            Expression<double>::constant(10.0),
            Expression<double>::constant(4.0)));
    auto deepSimplified = deepConst->simplify();
    std::cout << "Исходное: " << deepConst->toString() << std::endl;
    std::cout << "Упрощенное: " << deepSimplified->toString() << std::endl;
    check(deepSimplified->toString() == "8.000000", "Полное упрощение констант");
    check(deepConst->nodeCount() == 7, "Количество узлов исходного дерева");

    std::cout << std::endl << "=== Обработка ошибок ===" << std::endl;
    try {
        Expression<double>::variable("unknown")->evaluate({});
        check(false, "Неизвестная переменная выбрасывает исключение");
    } catch (const std::runtime_error& e) {
        std::cout << "Поймано исключение: " << e.what() << std::endl;
        check(true, "Неизвестная переменная выбрасывает исключение");
    }

    try {
        Expression<double>::binary(
            Expression<double>::Kind::Div,
            Expression<double>::constant(1.0),
            Expression<double>::constant(0.0))->evaluate({});
        check(false, "Деление на ноль выбрасывает исключение");
    } catch (const std::runtime_error& e) {
        std::cout << "Поймано исключение: " << e.what() << std::endl;
        check(true, "Деление на ноль выбрасывает исключение");
    }

    try {
        Expression<double>::binary(
            Expression<double>::Kind::Add,
            Expression<double>::constant(1.0),
            nullptr);
        check(false, "Бинарная операция без двух операндов");
    } catch (const std::invalid_argument& e) {
        std::cout << "Поймано исключение: " << e.what() << std::endl;
        check(true, "Бинарная операция без двух операндов");
    }

    try {
        Expression<int>::variable("");
        check(false, "Пустое имя переменной");
    } catch (const std::invalid_argument& e) {
        std::cout << "Поймано исключение: " << e.what() << std::endl;
        check(true, "Пустое имя переменной");
    }

    std::cout << std::endl << "=== Тот же код с другим числовым типом (int) ===" << std::endl;
    auto intExpr = Expression<int>::binary(
        Expression<int>::Kind::Mul,
        Expression<int>::constant(6),
        Expression<int>::variable("count"));
    std::map<std::string, int> intVars;
    intVars["count"] = 7;
    std::cout << intExpr->toString() << " = " << intExpr->evaluate(intVars) << std::endl;
    check(intExpr->evaluate(intVars) == 42, "Шаблон работает с int");

    std::cout << std::endl;
    std::cout << "Итого пройдено: " << passed << ", не пройдено: " << failed << std::endl;

    return failed == 0 ? 0 : 1;
}
