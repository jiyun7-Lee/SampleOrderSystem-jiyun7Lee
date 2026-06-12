#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../domain/ProductionJob.h"

class IProductionRepository {
public:
    virtual ~IProductionRepository() = default;

    virtual void Save(const ProductionJob& job) = 0;
    virtual std::optional<ProductionJob> FindById(const std::string& id) const = 0;
    virtual std::vector<ProductionJob> FindByOrderId(const std::string& orderId) const = 0;
};
