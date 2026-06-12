#pragma once
#include <string>
#include <vector>
#include <optional>
#include <chrono>
#include "../model/domain/ProductionJob.h"

class ProductionView {
public:
    void ShowProductionLine(
        const std::optional<ProductionJob>& currentJob,
        const std::vector<ProductionJob>& waitingJobs) const;
    void ShowMessage(const std::string& msg) const;

private:
    static std::string FormatTimePoint(
        const std::chrono::system_clock::time_point& tp);
    void PrintCurrentJob(const ProductionJob& job) const;
    void PrintWaitingJobs(const std::vector<ProductionJob>& jobs) const;
};
