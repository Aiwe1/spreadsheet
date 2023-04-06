#include "sheet.h"

#include "cell.h"
#include "common.h"

#include <algorithm>
#include <functional>
#include <iostream>
#include <optional>

using namespace std::literals;

Sheet::~Sheet() {}

void Sheet::SetCell(Position pos, std::string text) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid Position");
    }
    
    if (pos.row >= static_cast<int>(sheet_.size())) {
        sheet_.resize(pos.row + 1);
    }
    if (pos.col >= static_cast<int>(sheet_[pos.row].size())) {
        sheet_[pos.row].resize(pos.col + 1);
    }

    sheet_[pos.row][pos.col] = std::make_unique<Cell>(); 
    sheet_[pos.row][pos.col]->Set(std::move(text));
}

const CellInterface* Sheet::GetCell(Position pos) const {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }

    if (pos.row >= static_cast<int>(sheet_.size()) || pos.col >= static_cast<int>(sheet_[pos.row].size())) {
        return nullptr;
    }

    return sheet_[pos.row][pos.col].get();
}

CellInterface* Sheet::GetCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("Invalid position");
    }

    if (pos.row >= static_cast<int>(sheet_.size()) || pos.col >= static_cast<int>(sheet_[pos.row].size())) {
        return nullptr;
    }

    return sheet_[pos.row][pos.col].get();
}

void Sheet::ClearCell(Position pos) {
    if (!pos.IsValid()) {
        throw InvalidPositionException("invalid position");
    }
    if (pos.row < static_cast<int>(sheet_.size()) && pos.col < static_cast<int>(sheet_[pos.row].size())) {
        if (sheet_[pos.row][pos.col] != nullptr) {
            sheet_[pos.row][pos.col]->Clear();
            sheet_[pos.row][pos.col].reset();
        }
    }
}

Size Sheet::GetPrintableSize() const {
    Size size{ 0, 0 };

    for (int row = 0; row < static_cast<int>(sheet_.size()); ++row) {
        for (int col = static_cast<int>(sheet_[row].size()) - 1; col >= 0; --col) {
            if (sheet_[row][col] != nullptr) {
                if (!sheet_[row][col]->GetText().empty()) {
                    size.rows = std::max(size.rows, row + 1);
                    size.cols = std::max(size.cols, col + 1);
                    break;
                }
            }
        }
    }
    return size;
}

void Sheet::PrintValues(std::ostream& output) const {
    auto size = GetPrintableSize();

    for (int row = 0; row < size.rows; ++row) {
        for (int col = 0; col < size.cols; ++col) {
            if (col > 0) {
                output << '\t';
            }

            if (col < static_cast<int>(sheet_[row].size())) {
                if (sheet_[row][col]!= nullptr) {
                    std::visit([&output](auto&& value) { output << value; }, sheet_[row][col]->GetValue());
                }
            }
        }
        output << '\n';
    }
}
void Sheet::PrintTexts(std::ostream& output) const {
    auto size = GetPrintableSize();

    for (int row = 0; row < size.rows; ++row) {
        for (int col = 0; col < size.cols; ++col) {
            if (col > 0) {
                output << '\t';
            }

            if (col < static_cast<int>(sheet_[row].size())) {
                if (sheet_[row][col] != nullptr) {
                    output << sheet_[row][col]->GetText();
                }
            }
        }
        output << '\n';
    }
}

std::unique_ptr<SheetInterface> CreateSheet() {
    return std::make_unique<Sheet>();
}