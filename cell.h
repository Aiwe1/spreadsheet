#pragma once

#include <set>
#include <memory>

#include "common.h"
#include "formula.h"

//class Sheet;

class Cell : public CellInterface {
public:
    Cell(SheetInterface& sheet);
    ~Cell();

    void Set(std::string text);
    void Clear();

    Value GetValue() const override;
    std::string GetText() const override;
    std::vector<Position> GetReferencedCells() const override;

    //bool IsReferenced() const;

    void CheckCircle(std::set<const CellInterface*>& visited, const std::vector<Position>& ref_cells) const;

    void InvalidateCache(Cell* cell);

private:
    class Impl;
    class EmptyImpl;
    class TextImpl;
    class FormulaImpl;

    std::unique_ptr<Impl> impl_;

    SheetInterface& sheet_;

    std::set<Cell*> refs_;
    std::set<Cell*> depends_;
};
