#pragma once

#include <string>
#include <vector>
#include <optional>
#include "../domain/Order.h"
#include "../domain/OrderStatus.h"

class IOrderRepository {
public:
    virtual ~IOrderRepository() = default;

    virtual void Save(const Order& order) = 0;
    virtual std::optional<Order> FindById(const std::string& id) const = 0;
    virtual std::vector<Order> FindAll() const = 0;
    virtual std::vector<Order> FindByStatus(OrderStatus status) const = 0;
    virtual bool ExistsById(const std::string& id) const = 0;
    virtual void DeleteById(const std::string& id) = 0;
};
