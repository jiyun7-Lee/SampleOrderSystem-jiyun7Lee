#include "SampleController.h"
#include "../view/SampleView.h"
#include <iostream>
#include <stdexcept>

SampleController::SampleController(std::shared_ptr<SampleService> service)
    : _service(std::move(service)) {}

void SampleController::Run() {
    int choice = -1;
    while (true) {
        std::cout << "\n[1] 시료 등록\n"
                  << "[2] 시료 목록 조회\n"
                  << "[3] 시료 검색\n"
                  << "[0] 돌아가기\n"
                  << "선택: ";
        std::cin >> choice;
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
    std::string id, name;
    double avgTime = 0.0, yieldRate = 0.0;

    std::cout << "시료 ID: ";
    std::cin >> id;
    std::cout << "이름: ";
    std::cin >> name;
    std::cout << "평균 생산시간(분): ";
    std::cin >> avgTime;
    std::cout << "수율(0.0~1.0): ";
    std::cin >> yieldRate;

    Sample sample;
    sample.sampleId = id;
    sample.name = name;
    sample.averageProductionTime = avgTime;
    sample.yieldRate = yieldRate;

    try {
        _service->RegisterSample(sample);
        std::cout << "시료가 등록되었습니다." << std::endl;
    } catch (const std::exception& e) {
        std::cout << "오류: " << e.what() << std::endl;
    }
}

void SampleController::ListAllSamples() {
    auto samples = _service->GetAllSamples();
    if (samples.empty()) {
        std::cout << "등록된 시료가 없습니다." << std::endl;
        return;
    }
    for (const auto& s : samples) {
        std::cout << "ID: " << s.sampleId
                  << ", 이름: " << s.name
                  << ", 평균생산시간: " << s.averageProductionTime << "분"
                  << ", 수율: " << s.yieldRate
                  << std::endl;
    }
}

void SampleController::SearchSample() {
    std::string keyword;
    std::cout << "검색어: ";
    std::cin >> keyword;

    auto results = _service->SearchByName(keyword);
    if (results.empty()) {
        std::cout << "검색 결과가 없습니다." << std::endl;
        return;
    }
    for (const auto& s : results) {
        std::cout << "ID: " << s.sampleId
                  << ", 이름: " << s.name
                  << ", 평균생산시간: " << s.averageProductionTime << "분"
                  << ", 수율: " << s.yieldRate
                  << std::endl;
    }
}
