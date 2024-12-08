#include "monitor.h"

void Monitor::produce() {
    {
        // Блокируем мьютекс и устанавливаем флаг готовности события
        std::lock_guard<std::mutex> lock(mutex_);
        event_ready_ = true;
        std::cout << "Producer: Event generated.\n";
    }
    cond_var_.notify_one();
}

void Monitor::consume() {
    // Блокируем мьютекс и ждем, пока флаг готовности события не будет установлен
    std::unique_lock<std::mutex> lock(mutex_);
    cond_var_.wait(lock, [this] { return event_ready_; });

    std::cout << "Consumer: Event received and processed.\n";
    event_ready_ = false;
}
