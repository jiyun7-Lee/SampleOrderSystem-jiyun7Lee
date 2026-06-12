#pragma once

#include <string>

struct Inventory {
    std::string sampleId;
    int currentStock;
    int reservedStock;

    int availableStock() const {
        return currentStock - reservedStock;
    }
};
