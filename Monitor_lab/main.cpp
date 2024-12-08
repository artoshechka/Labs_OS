#include <thread>
#include <atomic>
#include <chrono>
#include "monitor.h"

// Функция для потока производителя
void producerThread(Monitor& monitor, std::atomic<bool>& stop_flag) {
    while (!stop_flag) {
        std::this_thread::sleep_for(std::chrono::seconds(1));
        monitor.produce();
    }
}

// Функция для потока потребителя
void consumerThread(Monitor& monitor, std::atomic<bool>& stop_flag) {
    while (!stop_flag) {
        monitor.consume();
    }
}

int main() {
    Monitor monitor; // Объект для синхронизации потоков
    std::atomic<bool> stop_flag(false); // Флаг завершения работы потоков

    // Запускаем потоки производителя и потребителя
    std::thread producer(producerThread, std::ref(monitor), std::ref(stop_flag));
    std::thread consumer(consumerThread, std::ref(monitor), std::ref(stop_flag));

    std::this_thread::sleep_for(std::chrono::seconds(10));
    stop_flag = true;

    producer.join();
    consumer.join();

    std::cout << "Program finished.\n";
    return 0;
}
