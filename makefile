
all: build main.exe

#main.exe

#main.exe: main.o
#	g++ -std=c++17 -o main.exe main.o -pthread

#main.o: main.cpp
#	g++ -std=c++17 -c main.cpp -pthread

compile:

	g++ -std=c++17 -o active_order_consumer.o -c active_order_consumer.cpp
	g++ -std=c++17 -o Order.o -c Order.cpp
	g++ -std=c++17 -o SpinLock.o -c SpinLock.cpp
	g++ -std=c++17 -o circular_buffer.o -c circular_buffer.cpp
	g++ -std=c++17 -o Limit.o -c Limit.cpp
	g++ -std=c++17 -o OrderBook.o -c OrderBook.cpp
	g++ -std=c++17 -o ThreadSafeQueue.o -c ThreadSafeQueue.cpp
	g++ -std=c++17 -o OrderBookManager.o -c OrderBookManager.cpp
	g++ -std=c++17 -o MarketDataEngine.o -c MarketDataEngine.cpp
	g++ -std=c++17 -o MatchingEngine.o -c MatchingEngine.cpp
	g++ -std=c++17 -o OrderConsumer.o -c OrderConsumer.cpp -pthread
	g++ -std=c++17 -o PostTradeProcesser.o -c PostTradeProcesser.cpp
	g++ -std=c++17 -o main.o -c main.cpp -pthread

objects := active_order_consumer.o Order.o SpinLock.o circular_buffer.o Limit.o OrderBook.o ThreadSafeQueue.o OrderBookManager.o MarketDataEngine.o MatchingEngine.o OrderConsumer.o PostTradeProcesser.o main.o 

build:
	g++ -std=c++17 -o main.exe $(objects)

clean: rm main.o main.exe

