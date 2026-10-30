#ifndef EXPRESSION_H
#define EXPRESSION_H


#include <memory>
#include <string>
#include <map>
#include <set>
#include <sstream>
#include <stdexcept>
#include <algorithm>


template <typename T> class ConstantExpr;
template <typename T> class VariableExpr;
template <typename T> class BinaryExpr;

template <typename T>
class Expression {
public:
    using Ptr = std::shared_ptr<Expression<T>>;

    // Перечисление бинарных операций (включая Multiply)
    enum class Kind {
        Add,
        Subtract,
        Multiply,
        Divide
    };

    virtual ~Expression() = default;

    virtual T evaluate(const std::map<std::string, T>& env) const = 0;
    virtual std::string toString() const = 0;
    virtual Ptr simplify() const = 0;
    virtual int depth() const = 0;
    virtual int nodeCount() const = 0;
    virtual void collectVariables(std::set<std::string>& out) const = 0;
    virtual bool isConstant() const = 0;

    std::set<std::string> variablesUsed() const {
        std::set<std::string> vars;
        collectVariables(vars);
        return vars;
    }

    static Ptr constant(T val);
    static Ptr variable(const std::string& name);
    static Ptr binary(Kind kind, Ptr left, Ptr right);
};

template <typename T>
class ConstantExpr : public Expression<T> {
private:
    T value_;

public:
    explicit ConstantExpr(T val) : value_(val) {}

    T evaluate(const std::map<std::string, T>&) const override {
        return value_;
    }

    std::string toString() const override {
        std::ostringstream ss;
        ss << value_;
        return ss.str();
    }

    typename Expression<T>::Ptr simplify() const override {
        return std::make_shared<ConstantExpr<T>>(value_);
    }

    int depth() const override { return 1; }
    int nodeCount() const override { return 1; }
    void collectVariables(std::set<std::string>&) const override {}
    bool isConstant() const override { return true; }
};

template <typename T>
class VariableExpr : public Expression<T> {
private:
    std::string name_;

public:
    explicit VariableExpr(std::string name) : name_(std::move(name)) {}

    T evaluate(const std::map<std::string, T>& env) const override {
        auto it = env.find(name_);
        if (it == env.end()) {
            throw std::runtime_error("Переменная '" + name_ + "' не найдена");
        }
        return it->second;
    }

    std::string toString() const override {
        return name_;
    }

    typename Expression<T>::Ptr simplify() const override {
        return std::make_shared<VariableExpr<T>>(name_);
    }

    int depth() const override { return 1; }
    int nodeCount() const override { return 1; }

    void collectVariables(std::set<std::string>& out) const override {
        out.insert(name_);
    }

    bool isConstant() const override { return false; }
};

template <typename T>
class BinaryExpr : public Expression<T> {
private:
    typename Expression<T>::Kind kind_;
    typename Expression<T>::Ptr left_;
    typename Expression<T>::Ptr right_;

    static T applyOp(typename Expression<T>::Kind k, T l, T r) {
        switch (k) {
            case Expression<T>::Kind::Add:
                return l + r;
            case Expression<T>::Kind::Subtract:
                return l - r;
            case Expression<T>::Kind::Multiply:
                return l * r;
            case Expression<T>::Kind::Divide:
                if (std::abs(r) < 1e-9) {
                    throw std::runtime_error("Деление на ноль");
                }
                return l / r;
        }
        throw std::runtime_error("Неизвестный оператор");
    }

    static std::string opSymbol(typename Expression<T>::Kind k) {
        switch (k) {
            case Expression<T>::Kind::Add: return "+";
            case Expression<T>::Kind::Subtract: return "-";
            case Expression<T>::Kind::Multiply: return "*";
            case Expression<T>::Kind::Divide: return "/";
        }
        return "?";
    }

public:
    BinaryExpr(typename Expression<T>::Kind kind,
               typename Expression<T>::Ptr left,
               typename Expression<T>::Ptr right)
        : kind_(kind), left_(std::move(left)), right_(std::move(right)) {
        if (!left_ || !right_) {
            throw std::runtime_error("Операнды не могут быть нулевыми указателями");
        }
    }

    T evaluate(const std::map<std::string, T>& env) const override {
        T lVal = left_->evaluate(env);
        T rVal = right_->evaluate(env);
        return applyOp(kind_, lVal, rVal);
    }

    std::string toString() const override {
        return "(" + left_->toString() + " " + opSymbol(kind_) + " " + right_->toString() + ")";
    }

    typename Expression<T>::Ptr simplify() const override {
        auto simpLeft = left_->simplify();
        auto simpRight = right_->simplify();

        if (simpLeft->isConstant() && simpRight->isConstant()) {
            T res = applyOp(kind_, simpLeft->evaluate({}), simpRight->evaluate({}));
            return std::make_shared<ConstantExpr<T>>(res);
        }

        return std::make_shared<BinaryExpr<T>>(kind_, simpLeft, simpRight);
    }

    int depth() const override {
        return 1 + std::max(left_->depth(), right_->depth());
    }

    int nodeCount() const override {
        return 1 + left_->nodeCount() + right_->nodeCount();
    }

    void collectVariables(std::set<std::string>& out) const override {
        left_->collectVariables(out);
        right_->collectVariables(out);
    }

    bool isConstant() const override {
        return left_->isConstant() && right_->isConstant();
    }
};

template <typename T>
typename Expression<T>::Ptr Expression<T>::constant(T val) {
    return std::make_shared<ConstantExpr<T>>(val);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::variable(const std::string& name) {
    return std::make_shared<VariableExpr<T>>(name);
}

template <typename T>
typename Expression<T>::Ptr Expression<T>::binary(Kind kind, Ptr left, Ptr right) {
    return std::make_shared<BinaryExpr<T>>(kind, left, right);
}

#endif 
