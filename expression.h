#pragma once
#include <locale.h>
#include <string>
#include <set>
#include <map>
#include <memory>
#include <stdexcept>

template<typename T>
class Expression {
public:
    enum class Kind { Const, Variable, Add, Sub, Mul, Div };

private:
    Kind kind_;
    T value_;
    std::string name_;
    std::unique_ptr<Expression> left_;
    std::unique_ptr<Expression> right_;

    Expression(Kind kind, T value, std::string name,
               std::unique_ptr<Expression> left, std::unique_ptr<Expression> right)
        : kind_(kind), value_(value), name_(name),
          left_(std::move(left)), right_(std::move(right)) {}

public:
    static std::unique_ptr<Expression> constant(T value) {
        return std::unique_ptr<Expression>(new Expression(Kind::Const, value, "", nullptr, nullptr));
    }

    static std::unique_ptr<Expression> variable(std::string name) {
        if (name.empty()) {
            throw std::invalid_argument("Имя переменной не может быть пустым");
        }
        return std::unique_ptr<Expression>(new Expression(Kind::Variable, T(), name, nullptr, nullptr));
    }

    static std::unique_ptr<Expression> binary(Kind kind,
                                                std::unique_ptr<Expression> left,
                                                std::unique_ptr<Expression> right) {
        if (left == nullptr || right == nullptr) {
            throw std::invalid_argument("Бинарная операция требует ровно два операнда");
        }
        return std::unique_ptr<Expression>(new Expression(kind, T(), "", std::move(left), std::move(right)));
    }

    T evaluate(const std::map<std::string, T>& vars) const {
        switch (kind_) {
            case Kind::Const:
                return value_;
            case Kind::Variable: {
                auto it = vars.find(name_);
                if (it == vars.end()) {
                    throw std::runtime_error("Неизвестная переменная: " + name_);
                }
                return it->second;
            }
            case Kind::Add:
                return left_->evaluate(vars) + right_->evaluate(vars);
            case Kind::Sub:
                return left_->evaluate(vars) - right_->evaluate(vars);
            case Kind::Mul:
                return left_->evaluate(vars) * right_->evaluate(vars);
            case Kind::Div: {
                T divisor = right_->evaluate(vars);
                if (divisor == T(0)) {
                    throw std::runtime_error("Деление на ноль");
                }
                return left_->evaluate(vars) / divisor;
            }
        }
        throw std::logic_error("Некорректный узел выражения");
    }

    std::string toString() const {
        switch (kind_) {
            case Kind::Const:
                return std::to_string(value_);
            case Kind::Variable:
                return name_;
            case Kind::Add:
                return "(" + left_->toString() + " + " + right_->toString() + ")";
            case Kind::Sub:
                return "(" + left_->toString() + " - " + right_->toString() + ")";
            case Kind::Mul:
                return "(" + left_->toString() + " * " + right_->toString() + ")";
            case Kind::Div:
                return "(" + left_->toString() + " / " + right_->toString() + ")";
        }
        return "";
    }

    std::set<std::string> variables() const {
        std::set<std::string> result;
        collectVariables(result);
        return result;
    }

    int depth() const {
        if (kind_ == Kind::Const || kind_ == Kind::Variable) {
            return 1;
        }
        int leftDepth = left_->depth();
        int rightDepth = right_->depth();
        return 1 + (leftDepth > rightDepth ? leftDepth : rightDepth);
    }

    int nodeCount() const {
        if (kind_ == Kind::Const || kind_ == Kind::Variable) {
            return 1;
        }
        return 1 + left_->nodeCount() + right_->nodeCount();
    }

    std::unique_ptr<Expression> simplify() const {
        if (kind_ == Kind::Const) {
            return constant(value_);
        }
        if (kind_ == Kind::Variable) {
            return variable(name_);
        }
        std::unique_ptr<Expression> newLeft = left_->simplify();
        std::unique_ptr<Expression> newRight = right_->simplify();
        bool bothConst = newLeft->kind_ == Kind::Const && newRight->kind_ == Kind::Const;
        if (bothConst && kind_ != Kind::Div) {
            std::map<std::string, T> empty;
            return constant(Expression(kind_, T(), "", std::move(newLeft), std::move(newRight)).evaluate(empty));
        }
        if (bothConst && kind_ == Kind::Div && newRight->value_ != T(0)) {
            return constant(newLeft->value_ / newRight->value_);
        }
        return binary(kind_, std::move(newLeft), std::move(newRight));
    }

private:
    void collectVariables(std::set<std::string>& result) const {
        if (kind_ == Kind::Variable) {
            result.insert(name_);
            return;
        }
        if (kind_ == Kind::Const) {
            return;
        }
        left_->collectVariables(result);
        right_->collectVariables(result);
    }
};
