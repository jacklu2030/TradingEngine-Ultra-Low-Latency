Name:  Jack Lu
Last Update: 11/14/2020
Email:  jianlu2001@yahoo.com
C++ standard: C++17

Attempt to Implement Ultra-Low Latency Trading Engine using C++17

1.  Data Structure and order matching logic

Here are the data structure designs for the trading system. At the top level is a map with key=ccyPair, value=Limit object. 
Each Limit object represents one specific limit price of either buying or selling.
Each Limit object holds a duque containing orders of the same limit price in sequence order.

An Order Book is created for each ccyPair. Within each order book, there are two priority queues: 
one with highest buying limit price on top, one with lowest selling limit price on top.  

Upon entry of each order from the market event pipe or stream, it’s stored in the data structure, then the highest top 
buying Limit object is mathced against the lowest limit selling price at the top of the buying/selling priority queues.
Orders within the top Limit objects are match against each in the sequence of entry, results are printed to console.

At the end, all unmatched orders or partially filled orders from all Limit objects are collected, 
sorted in entry sequence, and printed to console.

Note, a LMAX Disruptor style ring buffer is adopted to pass market events / orders from producer to consumers. 
LMAX ring buffer reuse pre-allocated memory, which is better than a queue where objects are created and thrown away once consumed.
 
2.  Here are the performances 

The number n below is the number of currently active limit prices in the market (it should be fairly small)

A)  execute an order with brand new Limit price:  O(log(n)) 
B)  execute an order whose limit price currently exists in the order book:  O(1)
C)  update an order:  O(1)
D). Cancel an order:  O(1)
E). Perform order book analysis:   O(log(n))

3. Thread safety

A)  Consumer thread pool is created, but it may race trade executions concurrently, trade execution may finish in 
    different order than their entry sequential order.  So, using single thread with thread affinity on CPU core may be a good option.

B)  A custom class “threadSafeQueue” is created to pass trade orders from producer to consumer.  
    Lockless algorithm is adopted instead of using thread wait/notify.

C). Spinlock is used instead of mutex lock on relatively short tasks for faster performance. With small thread pool 
    (or even single thread consumer), thread contention will be much less frequent

D)  A map of ccyPair / mutex is adopted, lock scope is limited to order execution per ccyPair, per Limit price.

E)  Memory false sharing is optimally avoided when accessing Order objects sequentially in ring buffer by a single consumer thread

F)  Below is how consumer thread pool size can be adjusted:

       size_t consumer_thread_pool_size = 1;
       OrderConsumer::getInstance().startConsumerThreadPool(consumer_thread_pool_size);

4.  Memory usage

This project tries to totally avoid allocating memory on the heap, it uses stack frame for fast performance.  
In real world, the performance of stack frame over heap is the highest design priority
of this project, memory availability on the stack frame is the secondary concern.

5.  Please run the project as you specified:

     cat sample_input.txt | ./run.sh

     Docker container of Debian:10 will be created by the command above, then c++ codes are compiled and deployed, and run.


