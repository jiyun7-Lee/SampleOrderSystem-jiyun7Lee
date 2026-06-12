#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "../model/domain/ProductionJob.h"
#include "../model/repository/IProductionRepository.h"
#include "../model/repository/IInventoryRepository.h"
#include "../model/repository/IOrderRepository.h"
#include "../util/ITimeProvider.h"

struct ProductionProgress {
    ProductionJob job;
    double progressPercent;   // 0.0 ~ 100.0
};

class ProductionService {
public:
    // 수율 오차 보정 계수: 실 생산량 = ceil(부족분 / (수율 * YIELD_SAFETY_FACTOR))
    static constexpr double YIELD_SAFETY_FACTOR = 0.9;

    ProductionService(
        IProductionRepository& prodRepo,
        IInventoryRepository& invRepo,
        IOrderRepository& orderRepo,
        ITimeProvider& timeProvider
    );

    void EnqueueJob(const ProductionJob& job);
    void CheckAndCompleteProduction();
    std::optional<ProductionJob> GetCurrentJob() const;
    std::optional<ProductionProgress> GetCurrentJobProgress() const;
    std::vector<ProductionJob> GetWaitingJobs() const;
    bool HasPendingJobs() const;

    // requiredQty <= 0 또는 yieldRate <= 0 이면 std::invalid_argument 예외
    static int CalcActualProductionQty(int requiredQty, double yieldRate);
    // avgTime < 0 또는 actualQty < 0 이면 std::invalid_argument 예외
    static double CalcTotalProductionTime(double avgTime, int actualQty);

private:
    IProductionRepository& prodRepo_;
    IInventoryRepository& invRepo_;
    IOrderRepository& orderRepo_;
    ITimeProvider& timeProvider_;

    void CompleteCurrentJob(const ProductionJob& job);
};
