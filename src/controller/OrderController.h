#pragma once
#include <memory>
#include "../service/OrderService.h"
#include "../view/OrderView.h"

class OrderController {
public:
    OrderController(std::shared_ptr<OrderService> service, std::shared_ptr<OrderView> view);

    void PlaceOrder();
    void ProcessApproval();

private:
    std::shared_ptr<OrderService> _service;
    std::shared_ptr<OrderView>    _view;
};
