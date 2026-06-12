#pragma once
#include "ProductionJob.h"
#include <queue>
#include <vector>
#include <optional>

class ProductionQueue {
public:
    void Enqueue(const ProductionJob& job);
    std::optional<ProductionJob> Peek() const;
    void Dequeue();
    std::vector<ProductionJob> GetWaitingJobs() const;
    bool IsEmpty() const;
    int Size() const;
private:
    std::queue<ProductionJob> queue_;
};

inline void ProductionQueue::Enqueue(const ProductionJob& job) {
    queue_.push(job);
}

inline std::optional<ProductionJob> ProductionQueue::Peek() const {
    if (queue_.empty()) {
        return std::nullopt;
    }
    return queue_.front();
}

inline void ProductionQueue::Dequeue() {
    if (!queue_.empty()) {
        queue_.pop();
    }
}

inline std::vector<ProductionJob> ProductionQueue::GetWaitingJobs() const {
    std::vector<ProductionJob> result;
    std::queue<ProductionJob> temp = queue_;
    while (!temp.empty()) {
        result.push_back(temp.front());
        temp.pop();
    }
    return result;
}

inline bool ProductionQueue::IsEmpty() const {
    return queue_.empty();
}

inline int ProductionQueue::Size() const {
    return static_cast<int>(queue_.size());
}
