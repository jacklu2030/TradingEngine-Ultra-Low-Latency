//
// Created by Jian Lu on 11/19/20.
//

#include "SpinLock.h"
void SpinLock::lock() {
    while (lck.test_and_set(std::memory_order_acquire)) {};
}

void SpinLock::unlock() {
    lck.clear(std::memory_order_release);
}


