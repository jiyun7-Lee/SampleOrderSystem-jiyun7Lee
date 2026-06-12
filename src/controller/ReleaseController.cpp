#include "ReleaseController.h"
#include <stdexcept>

ReleaseController::ReleaseController(std::shared_ptr<ReleaseService> service,
                                     std::shared_ptr<ReleaseView> view)
    : _service(std::move(service)), _view(std::move(view)) {}

void ReleaseController::HandleRelease() {
    auto confirmed = _service->GetConfirmedOrders();
    if (confirmed.empty()) {
        _view->ShowMessage("출고 가능한 주문이 없습니다.");
        return;
    }

    _view->ShowConfirmedOrders(confirmed);

    int choice = _view->InputInt("출고할 번호 > ");
    if (choice < 1 || choice > static_cast<int>(confirmed.size())) {
        _view->ShowMessage("올바른 번호를 입력하세요.");
        return;
    }

    const std::string& orderId = confirmed[choice - 1].orderId;

    try {
        _service->ReleaseOrder(orderId);
        auto released = _service->GetConfirmedOrders();
        // 출고된 주문을 직접 조회할 수 없으므로 변경 전 정보를 복사해서 상태만 교체
        Order result = confirmed[choice - 1];
        result.status = OrderStatus::RELEASE;
        _view->ShowReleaseResult(result);
    } catch (const std::exception& e) {
        _view->ShowMessage(std::string("오류: ") + e.what());
    }
}
