#include "OrderController.h"
#include <stdexcept>

OrderController::OrderController(std::shared_ptr<OrderService> service, std::shared_ptr<OrderView> view)
    : _service(std::move(service)), _view(std::move(view)) {}

void OrderController::PlaceOrder() {
    _view->ShowOrderMenu();
    std::string sampleId    = _view->InputString("시료 ID: ");
    std::string customerName = _view->InputString("고객명: ");
    int quantity             = _view->InputInt("수량: ");

    try {
        _service->PlaceOrder(sampleId, customerName, quantity);
        _view->ShowMessage("주문이 접수되었습니다.");
    } catch (const std::exception& e) {
        _view->ShowMessage(std::string("오류: ") + e.what());
    }
}

void OrderController::ProcessApproval() {
    _view->ShowApprovalMenu();

    auto reserved = _service->GetReservedOrders();
    if (reserved.empty()) {
        _view->ShowMessage("처리할 주문이 없습니다.");
        return;
    }

    _view->ShowOrderList(reserved);

    int no = _view->InputInt("\n처리할 번호 (취소: 0): ");
    if (no == 0) return;
    if (no < 1 || no > static_cast<int>(reserved.size())) {
        _view->ShowMessage("올바른 번호를 입력하세요.");
        return;
    }

    const std::string& orderId = reserved[no - 1].orderId;
    int choice = _view->InputInt("[1] 승인  [2] 거절  선택: ");

    try {
        if (choice == 1) {
            auto preview = _service->PreviewApproval(orderId);
            if (!preview.sufficientStock) {
                _view->ShowShortageInfo(preview.shortage,
                                        preview.actualProductionQty,
                                        preview.totalProductionTime,
                                        preview.avgProductionTime);
                int confirm = _view->InputInt("생산 후 승인하시겠습니까? [1] 예  [2] 아니오  선택: ");
                if (confirm != 1) {
                    _view->ShowMessage("승인이 취소되었습니다.");
                    return;
                }
            }
            _service->ApproveOrder(orderId);
            _view->ShowMessage(preview.sufficientStock
                ? "주문이 승인되었습니다. (출고 대기)"
                : "생산 대기열에 등록되었습니다. (생산 중)");
        } else if (choice == 2) {
            _service->RejectOrder(orderId);
            _view->ShowMessage("주문이 거절되었습니다.");
        } else {
            _view->ShowMessage("올바른 번호를 입력하세요.");
        }
    } catch (const std::exception& e) {
        _view->ShowMessage(std::string("오류: ") + e.what());
    }
}
