Name:  Jack Lu
Last Update: 11/14/2020
Email:  jianlu2001@yahoo.com
C++ standard: C++17
GitHub full source codes: https://github.com/jacklu2030/TradingEngine-Ultra-Low-Latency

Attempt to Implement Ultra-Low Latency Trading Engine using C++17

Please refer to the "Trading_Order_Book_Architecture_Design_PDF.pdf" for detail architecture design

1.  Performances 

The number n below is the number of currently active limit prices in the market (it should be fairly small)

A)  execute an order with brand new Limit price:  O(log(n)) 
B)  execute an order whose limit price currently exists in the order book:  O(1)
C)  update an order:  O(1)
D). Cancel an order:  O(1)
E). Perform order book analysis:   O(log(n))

2.  Please run the project as you specified:

     cat sample_input.txt | ./run.sh

     Docker container of Debian:10 will be created by the command above, then c++ codes are compiled and deployed, and run.

Again, please refer to the "Trading_Order_Book_Architecture_Design_PDF.pdf" for detail architecture design of thread, memory layout, data structure, matching logic, etc.

