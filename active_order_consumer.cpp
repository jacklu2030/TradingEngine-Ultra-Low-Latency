//
// Created by Jian Lu on 12/21/20.
//

#include <atomic>
#include <thread>
#include <iostream>
#include "active_order_consumer.h"

struct active_order_consumer {
    template <typename FN>
    active_order_consumer(FN fn) : thread([this,fn](){ while(alive) fn(); }) {};
    ~active_order_consumer() { std::cout << "active consumer going out of scope...!" << std::endl; alive = false; thread.join(); }

    active_order_consumer(const active_order_consumer&) = delete; // copy constructor
    active_order_consumer( active_order_consumer&& ) = delete;    // move constructor
    active_order_consumer& operator=(active_order_consumer) = delete;  // assignment operator

private:
    std::atomic<bool> alive = true;
    std::thread thread;
};
