#pragma once

#include <chrono>

class ITimeProvider {
public:
    virtual ~ITimeProvider() = default;

    virtual std::chrono::system_clock::time_point Now() const = 0;
};
