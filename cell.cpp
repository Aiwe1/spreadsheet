#include <cassert>
#include <iostream>
#include <string>
#include <optional>

#include "cell.h"
#include "common.h"

using namespace std::literals;

class Cell::Impl {
public:
	virtual Value GetValue() const = 0;
	virtual std::string GetText() const = 0;
    virtual std::vector<Position> GetReferencedCells() const {
        return {};
    }
    virtual void InvalidateCache() {}
    virtual std::optional<FormulaInterface::Value> GetCache() const {
        return {};
    }
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
    FormulaImpl(std::string text, const SheetInterface& sheet) : sheet_(sheet) {
        formula_ = ParseFormula(text.substr(1));
    }

    Value GetValue() const override {
        if (!cache_) {
            cache_ = formula_->Evaluate(sheet_);
        }

        return std::visit([](const auto& x) { return Value(x); }, *cache_);
    }

    std::string GetText() const override {
        return FORMULA_SIGN + formula_->GetExpression();
    }

    std::vector<Position> GetReferencedCells() const {
        return formula_->GetReferencedCells();
    }

    std::optional<FormulaInterface::Value> GetCache() const override {
        return cache_;
    }

    void InvalidateCache() override {
        cache_.reset();
    }

private:
    const SheetInterface& sheet_;
    std::unique_ptr<FormulaInterface> formula_;
    mutable std::optional<FormulaInterface::Value> cache_;
};

// Реализуйте следующие методы
Cell::Cell(SheetInterface& sheet) : impl_(std::make_unique<EmptyImpl>()), sheet_(sheet) {}

Cell::~Cell() {}

std::vector<Position> Cell::GetReferencedCells() const {
    return impl_->GetReferencedCells();
}

void Cell::Set(std::string text) {
    if (text.empty()) {
        impl_ = std::make_unique<EmptyImpl>();
    }
    else if (text.size() > 1 && text[0] == FORMULA_SIGN) {
        impl_ = std::make_unique<FormulaImpl>(std::move(text), sheet_);
    }
    else {
        impl_ = std::make_unique<TextImpl>(std::move(text));
    }

    std::set<const CellInterface*> visited;
    CheckCircle(visited, impl_->GetReferencedCells());
    impl_ = std::move(impl_);

    for (Cell* cell : depends_) {
        cell->refs_.erase(this);
    }
    refs_.clear();

    for (const auto& pos : impl_->GetReferencedCells()) {
        CellInterface* ref_cell = sheet_.GetCell(pos);

        if (!ref_cell) {
            sheet_.SetCell(pos, "");
            ref_cell = sheet_.GetCell(pos);
        }
        refs_.insert(static_cast<Cell*>(ref_cell));
        static_cast<Cell*>(ref_cell)->depends_.insert(this);
    }
    InvalidateCache(this);
}

void Cell::CheckCircle(std::set<const CellInterface*>& visited, const std::vector<Position>& ref_cells) const {
    for (const auto& pos : ref_cells) {
        CellInterface* cell = sheet_.GetCell(pos);
        if (cell == this) {
            throw CircularDependencyException("");
        }
        if (cell && !visited.count(cell)) {
            const auto ref_cells = cell->GetReferencedCells();
            if (!ref_cells.empty())
                CheckCircle(visited, ref_cells);
            visited.insert(cell);
        }
    }
}

void Cell::InvalidateCache(Cell* cell) {
    for (Cell* dep_cell : cell->depends_) {
        if (dep_cell->impl_->GetCache()) {
            InvalidateCache(dep_cell);
            dep_cell->impl_->InvalidateCache();
        }
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
