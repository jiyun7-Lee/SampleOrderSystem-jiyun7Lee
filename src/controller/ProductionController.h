#pragma once
#include <memory>
#include "../service/ProductionService.h"
#include "../view/ProductionView.h"

class ProductionController {
public:
    ProductionController(
        std::shared_ptr<ProductionService> service,
        std::shared_ptr<ProductionView> view);
    void ShowProductionLine();
    void CheckProduction();
private:
    std::shared_ptr<ProductionService> _service;
    std::shared_ptr<ProductionView> _view;
};
