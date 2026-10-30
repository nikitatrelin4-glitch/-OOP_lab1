#include "expression2.h"
#include <locale.h>
#include <iostream>
#include <map>
#include <set>
#include <string>
#include <cmath>
#include <stdexcept>

void check(bool condition, const std::string& message) {
    if (!condition) {
        throw std::runtime_error("Тест не пройден: " + message);
    }
    std::cout << "[OK] " << message << '\n';
}

void checkDouble(double actual, double expected, const std::string& message) {
    if (std::abs(actual - expected) > 1e-9) {
        throw std::runtime_error("Тест не пройден: " + message);
    }
    std::cout << "[OK] " << message << '\n';
}

template <typename T>
void printVariables(const std::set<T>& variables) {
    std::cout << "Переменные: ";
    if (variables.empty()) {
        std::cout << "нет";
    } else {
        bool first = true;
        for (const auto& variable : variables) {
            if (!first) std::cout << ", ";
            std::cout << variable;
            first = false;
        }
    }
    std::cout << '\n';
}

int main() {
    setlocale(LC_ALL, "Russian");
    using Expr = Expression<double>;

    Expr::Ptr constants = Expr::binary(Expr::Kind::Add,
        Expr::constant(2.0), Expr::constant(3.0));
    std::cout << "Константы: " << constants->toString()
              << " = " << constants->evaluate({}) << '\n';
    checkDouble(constants->evaluate({}), 5.0, "Вычисление выражения из констант");

    Expr::Ptr oneVariable = Expr::binary(Expr::Kind::Multiply,
        Expr::constant(4.0), Expr::variable("price"));
    std::cout << "Одна переменная: " << oneVariable->toString() << '\n';
    checkDouble(oneVariable->evaluate({{"price", 5.0}}), 20.0,
                "Вычисление выражения с одной переменной");

    Expr::Ptr severalVariables = Expr::binary(Expr::Kind::Add,
        Expr::binary(Expr::Kind::Multiply,
            Expr::variable("price"), Expr::variable("count")),
        Expr::variable("delivery"));
    std::cout << "Несколько переменных: " << severalVariables->toString() << '\n';
    checkDouble(severalVariables->evaluate({{"price", 10.0}, {"count", 3.0},
                                          {"delivery", 5.0}}), 35.0,
                "Вычисление выражения с несколькими переменными");
    checkDouble(severalVariables->evaluate({{"price", 2.0}, {"count", 4.0},
                                          {"delivery", 1.0}}), 9.0,
                "Повторное вычисление с другими значениями");

    Expr::Ptr nested = Expr::binary(Expr::Kind::Multiply,
        Expr::binary(Expr::Kind::Add, Expr::constant(2.0), Expr::constant(3.0)),
        Expr::binary(Expr::Kind::Subtract,
            Expr::variable("price"),
            Expr::binary(Expr::Kind::Divide,
                Expr::variable("delivery"), Expr::constant(2.0))));
    std::cout << "Вложенное выражение: " << nested->toString() << '\n';
    std::cout << "Глубина дерева: " << nested->depth() << '\n';
    std::cout << "Количество узлов: " << nested->nodeCount() << '\n';
    printVariables(nested->variablesUsed());

    Expr::Ptr simplified = nested->simplify();
    std::cout << "После упрощения: " << simplified->toString() << '\n';
    check(simplified->toString().find("(2 + 3)") == std::string::npos,
          "Постоянное подвыражение упрощается");
    check(nested->toString().find("(2 + 3)") != std::string::npos,
          "Исходное дерево не изменилось после упрощения");
    checkDouble(simplified->evaluate({{"price", 10.0}, {"delivery", 4.0}}), 40.0,
                "Вычисление упрощённого выражения");

    bool unknownVariableHandled = false;
    try {
        Expr::Ptr unknown = Expr::variable("unknown");
        unknown->evaluate({});
    } catch (const std::runtime_error&) {
        unknownVariableHandled = true;
    }
    check(unknownVariableHandled, "Неизвестная переменная обрабатывается явно");

    bool divisionByZeroHandled = false;
    try {
        Expr::Ptr division = Expr::binary(Expr::Kind::Divide,
            Expr::constant(5.0), Expr::constant(0.0));
        division->evaluate({});
    } catch (const std::runtime_error&) {
        divisionByZeroHandled = true;
    }
    check(divisionByZeroHandled, "Деление на ноль обрабатывается явно");

    bool simplifyDivisionByZeroHandled = false;
    try {
        Expr::Ptr division = Expr::binary(Expr::Kind::Divide,
            Expr::constant(5.0), Expr::constant(0.0));
        division->simplify();
    } catch (const std::runtime_error&) {
        simplifyDivisionByZeroHandled = true;
    }
    check(simplifyDivisionByZeroHandled,
          "Деление на ноль обрабатывается и при упрощении");

    std::cout << "Все проверки завершены.\n";
    return 0;
}
