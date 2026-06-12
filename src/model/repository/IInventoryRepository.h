#pragma once

#include <string>
#include <optional>
#include "../domain/Inventory.h"

class IInventoryRepository {
public:
    virtual ~IInventoryRepository() = default;

    virtual void Save(const Inventory& inventory) = 0;
    virtual std::optional<Inventory> FindBySampleId(const std::string& sampleId) const = 0;
    virtual bool ExistsBySampleId(const std::string& sampleId) const = 0;
};
