//
// Created by Jian Lu on 12/19/20.
//

#include "ThreadSafeQueue.h"

template <typename T>
T* ThreadSafeQueue<T>::front() {
    spinLock_producer_consumer.lock();
    T* item = nullptr;
    if (!queue.empty()) {
        item = &queue.front();
        queue.pop();
    }
    spinLock_producer_consumer.unlock();
    return item;
}

template <typename T>
void ThreadSafeQueue<T>::push(T& item) {
    spinLock_producer_consumer.lock();
    queue.push(std::move(item));
    spinLock_producer_consumer.unlock();
}

template <typename T>
int ThreadSafeQueue<T>::size() {
    return queue.size();
}