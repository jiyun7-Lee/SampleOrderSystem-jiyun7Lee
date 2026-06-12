#pragma once
#include <memory>
#include "../service/MonitoringService.h"
#include "../view/MonitoringView.h"

class MonitoringController {
public:
    MonitoringController(std::shared_ptr<MonitoringService> service,
                         std::shared_ptr<MonitoringView> view);
    void HandleMonitoring();

private:
    std::shared_ptr<MonitoringService> _service;
    std::shared_ptr<MonitoringView>    _view;
};
