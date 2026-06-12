#include "SampleView.h"
#include <iostream>

void SampleView::ShowMenu() const {
    std::cout << "\n[1] 시료 등록\n"
              << "[2] 시료 목록 조회\n"
              << "[3] 시료 검색\n"
              << "[0] 돌아가기\n"
              << "선택: ";
}

void SampleView::ShowSampleList(const std::vector<Sample>& samples) const {
    for (const auto& s : samples) {
        ShowSample(s);
    }
}

void SampleView::ShowSample(const Sample& sample) const {
    std::cout << "ID: " << sample.sampleId
              << ", 이름: " << sample.name
              << ", 평균생산시간: " << sample.averageProductionTime << "분"
              << ", 수율: " << sample.yieldRate
              << std::endl;
}

void SampleView::ShowMessage(const std::string& msg) const {
    std::cout << msg << std::endl;
}

std::string SampleView::InputString(const std::string& prompt) const {
    std::cout << prompt;
    std::string value;
    std::cin >> value;
    return value;
}

double SampleView::InputDouble(const std::string& prompt) const {
    std::cout << prompt;
    double value = 0.0;
    std::cin >> value;
    return value;
}
