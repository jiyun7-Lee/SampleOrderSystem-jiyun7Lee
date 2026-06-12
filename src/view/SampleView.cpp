#include "SampleView.h"
#include "ViewUtil.h"
#include <iostream>
#include <iomanip>
#include <sstream>
#include <limits>

// 컬럼 표시 너비 (display columns)
static constexpr int kColSampleId   =  8;
static constexpr int kColName       = 20;
static constexpr int kColAvgTime    = 16;
static constexpr int kColYield      =  6;
static constexpr int kSeparatorLen  = kColSampleId + kColName + kColAvgTime + kColYield + 9;
//  9 = " | " x 3

void SampleView::ShowMenu() const {
    std::cout << "\n[1] 시료 등록\n"
              << "[2] 시료 목록 조회\n"
              << "[3] 시료 검색\n"
              << "[0] 돌아가기\n"
              << "선택: ";
}

void SampleView::ShowSampleList(const std::vector<Sample>& samples) const {
    using namespace ViewUtil;
    std::cout << "\n"
              << PadRight("시료ID",      kColSampleId) << " | "
              << PadRight("시료명",      kColName)     << " | "
              << PadLeft ("평균생산시간(분)", kColAvgTime) << " | "
              << PadLeft ("수율",        kColYield)
              << "\n" << std::string(kSeparatorLen, '-') << "\n";
    for (const auto& s : samples) {
        ShowSample(s);
    }
    if (samples.empty()) {
        std::cout << "등록된 시료가 없습니다.\n";
    }
}

void SampleView::ShowSample(const Sample& sample) const {
    using namespace ViewUtil;

    std::ostringstream avgTime, yieldRate;
    avgTime   << std::fixed << std::setprecision(1) << sample.averageProductionTime;
    yieldRate << std::fixed << std::setprecision(2) << sample.yieldRate;

    std::cout << PadRight(sample.sampleId,  kColSampleId) << " | "
              << PadRight(sample.name,       kColName)     << " | "
              << PadLeft (avgTime.str(),     kColAvgTime)  << " | "
              << PadLeft (yieldRate.str(),   kColYield)
              << "\n";
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
