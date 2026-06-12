#pragma once

#include <chrono>
#include <cstdint>

namespace TimePointSerializer {

[[nodiscard]] inline int64_t ToEpoch(const std::chrono::system_clock::time_point& tp) {
    return std::chrono::duration_cast<std::chrono::seconds>(
        tp.time_since_epoch()).count();
}

[[nodiscard]] inline std::chrono::system_clock::time_point FromEpoch(int64_t sec) {
    return std::chrono::system_clock::time_point(std::chrono::seconds(sec));
}

} // namespace TimePointSerializer
