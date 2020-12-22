#include "circular_buffer.h"

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
lmax_style_ring_buffer<T>::lmax_style_ring_buffer(size_t _size) : m_size(_size) { m_data = new T[m_size]; }  // , m_data(new T[_size])

template <typename T>
lmax_style_ring_buffer<T>::~lmax_style_ring_buffer() { delete [] m_data; }

template <typename T>
bool lmax_style_ring_buffer<T>::try_enqueue( const T& value ) {
    const uint64_t head = m_head_2.load( std::memory_order_relaxed );
    int tail = m_tail_1.load( std::memory_order_relaxed );       // Jack's choice
    //uint64_t tail = m_tail_1.load( std::memory_order_relaxed );   // original
    const std::uint64_t count = tail - head;

    if( count >= m_size )
    {
        // queue is full
        return false;
    }

    // continuous busy-writing until success. This is not safe since queue can become full now
    // while ( !m_tail_1.compare_exchange_strong( tail, tail + 1, std::memory_order_relaxed ) );

    // try-one-time only writing
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
        return false;  // queue is empty
    }

    // continuous busy-read until success. This is not safe since queue can become empty now
    // while ( !m_head_1.compare_exchange_strong( head, head + 1, std::memory_order_relaxed ) );

    // try-one-time only read
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

// this must be defined at the bottom of the template .cpp file, and it works only with Order.
// If we want this template to work with any other type, please add extra line of definition below.
template class lmax_style_ring_buffer<Order>;

