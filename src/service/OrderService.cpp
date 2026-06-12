#include "OrderService.h"
#include "../model/domain/ProductionJob.h"
#include "../model/domain/OrderStatus.h"
#include <stdexcept>
#include <cmath>
#include <sstream>
#include <iomanip>
#include <ctime>
#include <algorithm>

OrderService::OrderService(
    IOrderRepository& orderRepo,
    ISampleRepository& sampleRepo,
    InventoryService& inventoryService,
    IProductionRepository& productionRepo,
    ITimeProvider& timeProvider)
    : orderRepo_(orderRepo)
    , sampleRepo_(sampleRepo)
    , inventoryService_(inventoryService)
    , productionRepo_(productionRepo)
    , timeProvider_(timeProvider)
    , orderCounter_(0)
{
    // 재시작 후 기존 주문 중 최대 일련번호로 카운터 초기화 (중복 방지)
    for (const auto& order : orderRepo_.FindAll()) {
        // orderId 형식: ORD-YYYYMMDD-NNNN
        auto pos = order.orderId.rfind('-');
        if (pos != std::string::npos) {
            try {
                int n = std::stoi(order.orderId.substr(pos + 1));
                if (n > orderCounter_) orderCounter_ = n;
            } catch (...) {}
        }
    }
}

void OrderService::PlaceOrder(const std::string& sampleId,
                              const std::string& customerName,
                              int quantity)
{
    if (sampleId.empty()) {
        throw std::invalid_argument("sampleId must not be empty");
    }
    auto isBlank = [](const std::string& s) {
        return std::all_of(s.begin(), s.end(), [](unsigned char c){ return std::isspace(c); });
    };
    if (customerName.empty() || isBlank(customerName)) {
        throw std::invalid_argument("customerName must not be empty");
    }
    if (quantity <= 0) {
        throw std::invalid_argument("quantity must be greater than 0");
    }
    if (!sampleRepo_.ExistsById(sampleId)) {
        throw std::runtime_error("Sample not found: " + sampleId);
    }

    auto now = timeProvider_.Now();
    std::time_t t = std::chrono::system_clock::to_time_t(now);
    std::tm tm{};
#ifdef _WIN32
    localtime_s(&tm, &t);
#else
    localtime_r(&t, &tm);
#endif

    ++orderCounter_;

    std::ostringstream oss;
    oss << "ORD-"
        << std::setw(4) << std::setfill('0') << (tm.tm_year + 1900)
        << std::setw(2) << std::setfill('0') << (tm.tm_mon + 1)
        << std::setw(2) << std::setfill('0') << tm.tm_mday
        << "-"
        << std::setw(4) << std::setfill('0') << orderCounter_;

    Order order;
    order.orderId      = oss.str();
    order.sampleId     = sampleId;
    order.customerName = customerName;
    order.quantity     = quantity;
    order.status       = OrderStatus::RESERVED;
    order.createdAt    = now;

    orderRepo_.Save(order);
}

void OrderService::ApproveOrder(const std::string& orderId)
{
    auto opt = orderRepo_.FindById(orderId);
    if (!opt.has_value()) {
        throw std::runtime_error("Order not found: " + orderId);
    }

    Order order = opt.value();
    if (order.status != OrderStatus::RESERVED) {
        throw std::logic_error("Order is not in RESERVED status: " + orderId);
    }

    int availableStock = inventoryService_.GetAvailableStock(order.sampleId);

    if (availableStock >= order.quantity) {
        inventoryService_.AllocateStock(order.sampleId, order.quantity);
        order.status = OrderStatus::CONFIRMED;
        orderRepo_.Save(order);
    } else {
        auto sampleOpt = sampleRepo_.FindById(order.sampleId);
        if (!sampleOpt.has_value()) {
            throw std::runtime_error("Sample not found: " + order.sampleId);
        }
        const Sample& sample = sampleOpt.value();

        int shortage = order.quantity - availableStock;
        int actualProductionQty =
            static_cast<int>(std::ceil(static_cast<double>(shortage) / (sample.yieldRate * 0.9)));
        double totalProductionTime = sample.averageProductionTime * actualProductionQty;

        auto now = timeProvider_.Now();
        auto finishTime = now + std::chrono::seconds(
            static_cast<long long>(totalProductionTime * 60));

        ProductionJob job;
        job.productionId             = "PJ-" + order.orderId;
        job.orderId                  = order.orderId;
        job.sampleId                 = order.sampleId;
        job.requiredQuantity         = order.quantity;
        job.actualProductionQuantity = actualProductionQty;
        job.totalProductionTime      = totalProductionTime;
        job.startTime                = now;
        job.expectedFinishTime       = finishTime;

        productionRepo_.Save(job);

        order.status = OrderStatus::PRODUCING;
        orderRepo_.Save(order);
    }
}

void OrderService::RejectOrder(const std::string& orderId)
{
    auto opt = orderRepo_.FindById(orderId);
    if (!opt.has_value()) {
        throw std::runtime_error("Order not found: " + orderId);
    }

    Order order = opt.value();
    if (order.status != OrderStatus::RESERVED) {
        throw std::logic_error("Order is not in RESERVED status: " + orderId);
    }

    order.status = OrderStatus::REJECTED;
    orderRepo_.Save(order);
}

std::vector<Order> OrderService::GetReservedOrders()
{
    return orderRepo_.FindByStatus(OrderStatus::RESERVED);
}

ApprovalPreview OrderService::PreviewApproval(const std::string& orderId) const
{
    auto opt = orderRepo_.FindById(orderId);
    if (!opt.has_value()) throw std::runtime_error("Order not found: " + orderId);

    const Order& order = opt.value();
    int avail    = inventoryService_.GetAvailableStock(order.sampleId);
    int shortage = std::max(0, order.quantity - avail);

    ApprovalPreview preview{};
    preview.availableStock  = avail;
    preview.requiredQty     = order.quantity;
    preview.shortage        = shortage;
    preview.sufficientStock = (shortage == 0);

    if (!preview.sufficientStock) {
        auto sampleOpt = sampleRepo_.FindById(order.sampleId);
        if (!sampleOpt.has_value()) throw std::runtime_error("Sample not found: " + order.sampleId);
        const Sample& sample = sampleOpt.value();
        preview.avgProductionTime    = sample.averageProductionTime;
        preview.actualProductionQty  = static_cast<int>(
            std::ceil(static_cast<double>(shortage) / (sample.yieldRate * 0.9)));
        preview.totalProductionTime  = sample.averageProductionTime * preview.actualProductionQty;
    }
    return preview;
}
