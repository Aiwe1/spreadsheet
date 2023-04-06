#include "cell.h"

#include <cassert>
#include <iostream>
#include <string>
#include <optional>

using namespace std::literals;

class Cell::Impl {
public:
	virtual Value GetValue() const = 0;
	virtual std::string GetText() const = 0;
};

class Cell::EmptyImpl : public Impl {
public:
    Value GetValue() const override {
        return ""s;
    }

    std::string GetText() const override {
        return ""s;
    }
};

class Cell::TextImpl : public Impl {
public:
    TextImpl(std::string text) : text_(std::move(text)) {}

    Value GetValue() const override {
        if (text_[0] == ESCAPE_SIGN) {
            return text_.substr(1);
        }
        return text_;
    }

    std::string GetText() const override {
        return text_;
    }

private:
    std::string text_;
};

class Cell::FormulaImpl : public Impl {
public:
    FormulaImpl(std::string text) {
        formula_ = ParseFormula(text.substr(1));
    }

    Value GetValue() const override {
        auto res = formula_->Evaluate();

        return std::visit([](const auto& x) { return Value(x); }, res);
    }

    std::string GetText() const override {
        return FORMULA_SIGN + formula_->GetExpression();
    }

private:
    std::unique_ptr<FormulaInterface> formula_;
    FormulaInterface::Value value_;
};

// Реализуйте следующие методы
Cell::Cell() : impl_(std::make_unique<EmptyImpl>()) {}

Cell::~Cell() {}

void Cell::Set(std::string text) {
    if (text.empty()) {
        impl_ = std::make_unique<EmptyImpl>();
    }
    else if (text.size() > 1 && text[0] == FORMULA_SIGN) {
        impl_ = std::make_unique<FormulaImpl>(std::move(text));
    }
    else {
        impl_ = std::make_unique<TextImpl>(std::move(text));
    }
}

void Cell::Clear() {
    impl_ = std::make_unique<EmptyImpl>();
}


Cell::Value Cell::GetValue() const {
    return impl_->GetValue();
}

std::string Cell::GetText() const {
    return impl_->GetText();
}