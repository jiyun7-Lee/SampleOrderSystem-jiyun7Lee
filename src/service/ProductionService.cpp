#include "ProductionService.h"
#include "../model/domain/OrderStatus.h"
#include "../model/domain/Inventory.h"
#include <algorithm>
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
        prodRepo_.DeleteById(job.productionId);
        return;
    }

    auto& order = *orderOpt;
    if (order.status == OrderStatus::CONFIRMED) {
        prodRepo_.DeleteById(job.productionId);
        return;
    }

    // 생산 완료분 재고 추가 (레코드 없으면 신규 생성)
    auto invOpt = invRepo_.FindBySampleId(job.sampleId);
    Inventory inv = invOpt.value_or(Inventory{job.sampleId, 0, 0});
    inv.currentStock += job.actualProductionQuantity;
    invRepo_.Save(inv);

    order.status = OrderStatus::CONFIRMED;
    orderRepo_.Save(order);

    prodRepo_.DeleteById(job.productionId);
}

std::optional<ProductionJob> ProductionService::GetCurrentJob() const {
    auto jobs = prodRepo_.FindAll();
    if (jobs.empty()) {
        return std::nullopt;
    }
    return jobs.front();
}

std::optional<ProductionProgress> ProductionService::GetCurrentJobProgress() const {
    auto jobOpt = GetCurrentJob();
    if (!jobOpt) return std::nullopt;

    const auto& job = *jobOpt;
    auto now = timeProvider_.Now();

    double totalSec   = std::chrono::duration<double>(job.expectedFinishTime - job.startTime).count();
    double elapsedSec = std::chrono::duration<double>(now - job.startTime).count();

    double pct = (totalSec > 0.0)
        ? std::clamp(elapsedSec / totalSec * 100.0, 0.0, 100.0)
        : 100.0;

    return ProductionProgress{job, pct};
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
