#include "ProductionController.h"

ProductionController::ProductionController(
    std::shared_ptr<ProductionService> service,
    std::shared_ptr<ProductionView> view)
    : _service(std::move(service)), _view(std::move(view)) {}

void ProductionController::ShowProductionLine() {
    _service->CheckAndCompleteProduction();
    auto currentJob  = _service->GetCurrentJob();
    auto waitingJobs = _service->GetWaitingJobs();
    _view->ShowProductionLine(currentJob, waitingJobs);
}

void ProductionController::CheckProduction() {
    _service->CheckAndCompleteProduction();
}
