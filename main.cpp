
#include <ctime>
#include "OrderConsumer.h"
#include "PostTradeProcesser.h"

void printTime(const std::string& msg) {
    time_t now;
    std::time(&now);
    std::cout << msg << asctime(localtime(&now)) << std::endl;
}

inline void processOrders() {
    std::string message = "";
    while (std::getline(std::cin, message)) {
        //std::cout << "Receive message = " << message  << std::endl;
        MarketDataEngine::parseOrder(message);
    }
}

int main() {

    printTime("\n***** Trades filled / partially-filled -- ");

    size_t consumer_thread_pool_size = 1;
    OrderConsumer::getInstance().startConsumerThreadPool(consumer_thread_pool_size);
    processOrders();
    OrderConsumer::getInstance().stop_thread_pool();
    OrderConsumer::getInstance().join_thread_pool();

    printTime("\n***** Dump of Order Book Content -- ");
    PostTradeProcesser::listOrderBook();
    std::cout << "\n";

    return 0;
}
