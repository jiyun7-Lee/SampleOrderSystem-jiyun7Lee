#pragma once
#include <memory>
#include "../service/ReleaseService.h"
#include "../view/ReleaseView.h"

class ReleaseController {
public:
    ReleaseController(std::shared_ptr<ReleaseService> service,
                      std::shared_ptr<ReleaseView> view);
    void HandleRelease();

private:
    std::shared_ptr<ReleaseService> _service;
    std::shared_ptr<ReleaseView>    _view;
};
