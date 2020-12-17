// Created by Jian Lu on 9/29/20.

#ifndef TEST_CIRCULAR_BUFFER_H
#define TEST_CIRCULAR_BUFFER_H

#include <cstddef>
#include <mutex>
#include <iostream>

/*
 * What LMAX Disruptor boils down to though, is a ring buffer - a fixed-size array which behaves as if it were connected end to end.
 * There is a head and a tail index, items are inserted at the tail and removed at the head, each time advancing the relevant index.
 * When one of the indexes reaches the end of the array, they simply wrap around to 0 again. When the tail catches up with the head,
 * the buffer is full. When the head catches up with the tail, the buffer is empty.
 *
 * What Disruptor actually has is a pair of indexes for the head, and a pair for the tail. When a producer thread wants to insert an item
 * in to the queue, it uses an RMW operation to read the first tail index and then advance it by 1. This allows the producer to "claim"
 * its write index. When it has completed the write, it advances the second tail index. The same goes for reads - first a consumer thread
 * acquires its read index, reads the value, and then advances the second read index.
 */
template <typename T>
class lmax_style_ring_buffer {
private:
    T * m_data;
    std::size_t m_size = 100;
    std::atomic<int> m_head_1;
    std::atomic<int> m_head_2;
    std::atomic<int> m_tail_1;
    std::atomic<int> m_tail_2;
public:
    lmax_style_ring_buffer(size_t _size);
    ~lmax_style_ring_buffer();
    bool try_enqueue( const T& value );
    bool try_dequeue( T& out );
};

template <typename T>
lmax_style_ring_buffer<T>::lmax_style_ring_buffer(size_t _size) : m_size(_size) { m_data = new T[m_size]; }  // , m_data(new T[_size])

template <typename T>
lmax_style_ring_buffer<T>::~lmax_style_ring_buffer() { delete [] m_data; }

template <typename T>
bool lmax_style_ring_buffer<T>::try_enqueue( const T& value ) {
    const uint64_t head = m_head_2.load( std::memory_order_relaxed );
    int tail = m_tail_1.load( std::memory_order_relaxed );
    //uint64_t tail = m_tail_1.load( std::memory_order_relaxed );   // original
    const std::uint64_t count = tail - head;                        // Jack's choice

    if( count >= m_size )
    {
        // queue is full
        return false;
    }

    if( !m_tail_1.compare_exchange_strong( tail, tail + 1, std::memory_order_relaxed ) )
    {
        return false;
    }
    m_data[tail % m_size] = value;

    while( m_tail_2.load( std::memory_order_relaxed ) != tail ) {}

    std::atomic_thread_fence( std::memory_order_release );
    m_tail_2.store( tail + 1, std::memory_order_relaxed );

    return true;
}

template <typename T>
bool lmax_style_ring_buffer<T>::try_dequeue( T& out )
{
    const uint64_t tail = m_tail_2.load( std::memory_order_relaxed );
    //uint64_t head = m_head_1.load( std::memory_order_relaxed );           // original design
    int head = m_head_1.load( std::memory_order_relaxed );              // Jack's choice

    if( head >= tail )
    {
        return false;
    }

    if( !m_head_1.compare_exchange_strong( head, head + 1, std::memory_order_relaxed ) )
    {
        return false;
    }

    std::atomic_thread_fence( std::memory_order_acquire );
    out = m_data[head % m_size];

    while( m_head_2.load( std::memory_order_relaxed ) != head ) {}

    std::atomic_thread_fence( std::memory_order_release );
    m_head_2.store( head + 1, std::memory_order_relaxed );

    return true;
}

#endif //TEST_CIRCULAR_BUFFER_H
