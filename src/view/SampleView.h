#pragma once
#include <vector>
#include <string>
#include "../model/domain/Sample.h"

class SampleView {
public:
    void ShowMenu() const;
    void ShowSampleList(const std::vector<Sample>& samples) const;
    void ShowSample(const Sample& sample) const;
    void ShowMessage(const std::string& msg) const;
    std::string InputString(const std::string& prompt) const;
    double InputDouble(const std::string& prompt) const;
};
