#include "MonitoringController.h"

MonitoringController::MonitoringController(std::shared_ptr<MonitoringService> service,
                                           std::shared_ptr<MonitoringView> view)
    : _service(std::move(service)), _view(std::move(view)) {}

void MonitoringController::HandleMonitoring() {
    while (true) {
        _view->ShowMenu();
        int choice = _view->InputInt("선택 > ");

        switch (choice) {
            case 1: {
                auto summary = _service->GetOrderSummary();
                _view->ShowOrderSummary(summary);
                break;
            }
            case 2: {
                auto statuses = _service->GetInventoryStatus();
                _view->ShowInventoryStatus(statuses);
                break;
            }
            case 0:
                return;
            default:
                _view->ShowMessage("올바른 번호를 입력하세요.");
                break;
        }
    }
}
