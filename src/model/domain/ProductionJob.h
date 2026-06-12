#pragma once

#include <string>
#include <chrono>

struct ProductionJob {
    std::string productionId;
    std::string orderId;
    std::string sampleId;
    int requiredQuantity;
    int actualProductionQuantity;
    double totalProductionTime;
    std::chrono::system_clock::time_point startTime;
    std::chrono::system_clock::time_point expectedFinishTime;
};
