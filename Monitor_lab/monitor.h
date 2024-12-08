#ifndef MONITOR_H
#define MONITOR_H

#include <mutex>
#include <condition_variable>
#include <iostream>

// Класс для синхронизации потоков producer и consumer
class Monitor {
public:
    // Генерирует событие и уведомляет потребителя
    void produce();

    // Ожидает и обрабатывает событие
    void consume();

private:
    std::mutex mutex_; // Мьютекс для синхронизации доступа
    std::condition_variable cond_var_; // Условная переменная для уведомления
    bool event_ready_ = false; // Флаг готовности события
};

#endif // MONITOR_H
