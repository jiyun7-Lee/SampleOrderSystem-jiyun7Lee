#pragma once
#include <string>
#include <vector>
#include <optional>
#include "../model/domain/ProductionJob.h"

class ProductionView {
public:
    void ShowProductionLine(
        const std::optional<ProductionJob>& currentJob,
        const std::vector<ProductionJob>& waitingJobs) const;
    void ShowMessage(const std::string& msg) const;
};
