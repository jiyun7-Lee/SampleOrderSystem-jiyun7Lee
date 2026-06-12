#pragma once
#include <memory>
#include "../service/SampleService.h"
#include "../view/SampleView.h"

class SampleController {
public:
    SampleController(std::shared_ptr<SampleService> service, std::shared_ptr<SampleView> view);
    void Run();

private:
    std::shared_ptr<SampleService> _service;
    std::shared_ptr<SampleView> _view;

    void RegisterSample();
    void ListAllSamples();
    void SearchSample();
};
