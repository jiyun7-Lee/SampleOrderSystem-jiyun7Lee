#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../domain/Order.h"
#include "../domain/OrderStatus.h"

// ISP: 주문 도메인은 상태 전이만 하며 삭제 시나리오가 없으므로 DeleteById를 인터페이스에서 제거.
// 삭제가 필요한 구현체는 구체 클래스 수준에서 별도 제공한다.
class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;

    virtual void Save(const Order& order) = 0;
    virtual std::optional<Order> FindById(const std::string& id) const = 0;
    virtual std::vector<Order> FindAll() const = 0;
    virtual std::vector<Order> FindByStatus(OrderStatus status) const = 0;
    virtual bool ExistsById(const std::string& id) const = 0;
};
