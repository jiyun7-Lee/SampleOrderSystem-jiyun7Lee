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
    if (!orderOpt.has_value()) {
        return;
    }

    auto order = orderOpt.value();
    if (order.status == OrderStatus::CONFIRMED) {
        return;
    }

    auto invOpt = invRepo_.FindBySampleId(job.sampleId);
    if (invOpt.has_value()) {
        auto inv = invOpt.value();
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
    return static_cast<int>(std::ceil(requiredQty / (yieldRate * 0.9)));
}

double ProductionService::CalcTotalProductionTime(double avgTime, int actualQty) {
    return avgTime * actualQty;
}
