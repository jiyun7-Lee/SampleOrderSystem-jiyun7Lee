#pragma once
#include <memory>
#include "../service/SampleService.h"

class SampleView;

class SampleController {
public:
    explicit SampleController(std::shared_ptr<SampleService> service);
    void Run();

private:
    std::shared_ptr<SampleService> _service;

    void RegisterSample();
    void ListAllSamples();
    void SearchSample();
};
