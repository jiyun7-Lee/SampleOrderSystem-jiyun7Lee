#include "ProductionService.h"
#include "../model/domain/OrderStatus.h"
#include <cmath>
#include <stdexcept>

ProductionService::ProductionService(
    IProductionRepository& prodRepo,
    IInventoryRepository& invRepo,
    IOrderRepository& orderRepo,
    ITimeProvider& timeProvider)
    : prodRepo_(prodRepo)
    , invRepo_(invRepo)
    , orderRepo_(orderRepo)
    , timeProvider_(timeProvider)
{
}

void ProductionService::EnqueueJob(const ProductionJob& job) {
    queue_.Enqueue(job);
    prodRepo_.Save(job);
}

void ProductionService::CheckAndCompleteProduction() {
    auto jobs = prodRepo_.FindAll();
    if (jobs.empty()) {
        return;
    }

    auto now = timeProvider_.Now();
    const ProductionJob& currentJob = jobs.front();

    if (now >= currentJob.expectedFinishTime) {
        CompleteCurrentJob(currentJob);
    }
}

void ProductionService::CompleteCurrentJob(const ProductionJob& job) {
    auto orderOpt = orderRepo_.FindById(job.orderId);
    if (!orderOpt) {
        return;
    }

    auto& order = *orderOpt;
    if (order.status == OrderStatus::CONFIRMED) {
        return;
    }

    if (auto invOpt = invRepo_.FindBySampleId(job.sampleId)) {
        auto inv = *invOpt;
        inv.currentStock += job.actualProductionQuantity;
        invRepo_.Save(inv);
    }

    order.status = OrderStatus::CONFIRMED;
    orderRepo_.Save(order);

    ProductionJob completedJob = job;
    completedJob.startTime = timeProvider_.Now();
    prodRepo_.Save(completedJob);

    queue_.Dequeue();
}

std::optional<ProductionJob> ProductionService::GetCurrentJob() const {
    auto jobs = prodRepo_.FindAll();
    if (jobs.empty()) {
        return std::nullopt;
    }
    return jobs.front();
}

std::vector<ProductionJob> ProductionService::GetWaitingJobs() const {
    auto jobs = prodRepo_.FindAll();
    if (jobs.size() <= 1) {
        return {};
    }
    return std::vector<ProductionJob>(jobs.begin() + 1, jobs.end());
}

bool ProductionService::HasPendingJobs() const {
    return !prodRepo_.FindAll().empty();
}

int ProductionService::CalcActualProductionQty(int requiredQty, double yieldRate) {
    if (requiredQty <= 0) {
        throw std::invalid_argument("requiredQty must be greater than 0");
    }
    if (yieldRate <= 0.0) {
        throw std::invalid_argument("yieldRate must be greater than 0");
    }
    return static_cast<int>(std::ceil(
        static_cast<double>(requiredQty) / (yieldRate * YIELD_SAFETY_FACTOR)
    ));
}

double ProductionService::CalcTotalProductionTime(double avgTime, int actualQty) {
    if (avgTime < 0.0) {
        throw std::invalid_argument("avgTime must be non-negative");
    }
    if (actualQty < 0) {
        throw std::invalid_argument("actualQty must be non-negative");
    }
    return avgTime * static_cast<double>(actualQty);
}
