#include "SampleController.h"
#include <stdexcept>

SampleController::SampleController(std::shared_ptr<SampleService> service, std::shared_ptr<SampleView> view)
    : _service(std::move(service)), _view(std::move(view)) {}

void SampleController::Run() {
    while (true) {
        _view->ShowMenu();
        int choice = _view->InputInt("");
        if (choice == -1) {
            _view->ShowMessage("올바른 번호를 입력하세요.");
            continue;
        }
        if (choice == 1) {
            RegisterSample();
        } else if (choice == 2) {
            ListAllSamples();
        } else if (choice == 3) {
            SearchSample();
        } else if (choice == 0) {
            return;
        }
    }
}

void SampleController::RegisterSample() {
    std::string id = _view->InputString("시료 ID: ");
    std::string name = _view->InputString("이름: ");
    double avgTime = _view->InputDouble("평균 생산시간(분): ");
    double yieldRate = _view->InputDouble("수율(0.0~1.0): ");

    Sample sample;
    sample.sampleId = id;
    sample.name = name;
    sample.averageProductionTime = avgTime;
    sample.yieldRate = yieldRate;

    try {
        _service->RegisterSample(sample);
        _view->ShowMessage("시료가 등록되었습니다.");
    } catch (const std::exception& e) {
        _view->ShowMessage(std::string("오류: ") + e.what());
    }
}

void SampleController::ListAllSamples() {
    auto samples = _service->GetAllSamples();
    if (samples.empty()) {
        _view->ShowMessage("등록된 시료가 없습니다.");
        return;
    }
    _view->ShowSampleList(samples);
}

void SampleController::SearchSample() {
    std::string keyword = _view->InputString("검색어: ");

    auto results = _service->SearchByName(keyword);
    if (results.empty()) {
        _view->ShowMessage("검색 결과가 없습니다.");
        return;
    }
    _view->ShowSampleList(results);
}
