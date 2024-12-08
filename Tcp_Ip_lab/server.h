#ifndef SERVER_H
#define SERVER_H

#include <signal.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <arpa/inet.h>

struct Client {
    int connfd;
    struct sockaddr_in addr;
};

class Server {
public:
    Server(int port);
    ~Server();

    void run(); // Основной цикл работы сервера

private:
    int port_;
    int server_fd_;
    std::vector<Client> clients_;
    sigset_t blocked_mask_;
    static volatile sig_atomic_t was_signal_; // Статическая переменная для сигналов
    static constexpr int MAX_CLIENTS = 2;     // Максимальное количество клиентов

    void setupSignalHandling();  // Настройка обработки сигналов
    void setupSocket();          // Настройка сокета

    void handleSignal();          // Обработка сигнала
    void handleNewConnection();   // Обработка нового подключения
    void handleClientData(Client& client); // Обработка данных клиента

    void closeClient(Client& client); // Закрытие соединения
    void printClientAddress(const Client& client); // Вывод информации о клиенте
};

#endif // SERVER_H
