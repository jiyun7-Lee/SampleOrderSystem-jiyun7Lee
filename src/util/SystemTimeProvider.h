#pragma once

#include "ITimeProvider.h"

class SystemTimeProvider : public ITimeProvider {
public:
    std::chrono::system_clock::time_point Now() const override {
        return std::chrono::system_clock::now();
    }
};
