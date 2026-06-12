#include "SampleView.h"
#include <iostream>
#include <limits>

void SampleView::ShowMenu() const {
    std::cout << "\n[1] 시료 등록\n"
              << "[2] 시료 목록 조회\n"
              << "[3] 시료 검색\n"
              << "[0] 돌아가기\n"
              << "선택: ";
}

void SampleView::ShowSampleList(const std::vector<Sample>& samples) const {
    std::cout << "ID | 시료명 | 평균생산시간(분) | 수율\n"
              << "-------------------------------------------\n";
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
    std::getline(std::cin, value);
    return value;
}

double SampleView::InputDouble(const std::string& prompt) const {
    std::cout << prompt;
    double value = 0.0;
    while (!(std::cin >> value)) {
        std::cin.clear();
        std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
        std::cout << "숫자를 입력하세요: ";
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}

int SampleView::InputInt(const std::string& prompt) const {
    std::cout << prompt;
    int value = -1;
    std::cin >> value;
    if (std::cin.fail()) {
        std::cin.clear();
    }
    std::cin.ignore(std::numeric_limits<std::streamsize>::max(), '\n');
    return value;
}
