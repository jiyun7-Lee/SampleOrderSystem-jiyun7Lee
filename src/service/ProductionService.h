#pragma once
#include <vector>
#include <memory>
#include <optional>
#include "../model/domain/ProductionJob.h"
#include "../model/domain/ProductionQueue.h"
#include "../model/repository/IProductionRepository.h"
#include "../model/repository/IInventoryRepository.h"
#include "../model/repository/IOrderRepository.h"
#include "../util/ITimeProvider.h"

class ProductionService {
public:
    ProductionService(
        IProductionRepository& prodRepo,
        IInventoryRepository& invRepo,
        IOrderRepository& orderRepo,
        ITimeProvider& timeProvider
    );

    void EnqueueJob(const ProductionJob& job);
    void CheckAndCompleteProduction();
    std::optional<ProductionJob> GetCurrentJob() const;
    std::vector<ProductionJob> GetWaitingJobs() const;
    bool HasPendingJobs() const;

    static int CalcActualProductionQty(int requiredQty, double yieldRate);
    static double CalcTotalProductionTime(double avgTime, int actualQty);

private:
    IProductionRepository& prodRepo_;
    IInventoryRepository& invRepo_;
    IOrderRepository& orderRepo_;
    ITimeProvider& timeProvider_;
    ProductionQueue queue_;

    void CompleteCurrentJob(const ProductionJob& job);
};
