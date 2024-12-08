#include "server.h"
#include <iostream>
#include <cstring>
#include <csignal>
#include <fcntl.h>
#include <algorithm>
#include <errno.h>

volatile sig_atomic_t Server::was_signal_ = 0; // Определяем статическую переменную

// Конструктор
Server::Server(int port) : port_(port), server_fd_(-1) {
    setupSignalHandling();
    setupSocket();
}

// Деструктор
Server::~Server() {
    for (auto& client : clients_) {
        close(client.connfd);
    }
    if (server_fd_ != -1) {
        close(server_fd_);
    }
}

// Настройка обработки сигналов
void Server::setupSignalHandling() {
    struct sigaction sa{};
    sa.sa_handler = [](int) { Server::was_signal_ = 1; };
    sigaction(SIGHUP, &sa, nullptr);

    sigemptyset(&blocked_mask_);
    sigaddset(&blocked_mask_, SIGHUP);
    sigprocmask(SIG_BLOCK, &blocked_mask_, nullptr);
}

// Настройка сокета
void Server::setupSocket() {
    server_fd_ = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd_ == -1) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    int opt = 1;
    setsockopt(server_fd_, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    struct sockaddr_in address{};
    address.sin_family = AF_INET;
    address.sin_addr.s_addr = htonl(INADDR_ANY);
    address.sin_port = htons(port_);

    if (bind(server_fd_, (struct sockaddr*)&address, sizeof(address)) == -1) {
        perror("Bind failed");
        exit(EXIT_FAILURE);
    }

    if (listen(server_fd_, MAX_CLIENTS) == -1) {
        perror("Listen failed");
        exit(EXIT_FAILURE);
    }

    std::cout << "Server listening on port " << port_ << std::endl;
}

// Основной цикл работы сервера
void Server::run() {
    fd_set read_fds;

    while (true) {
        FD_ZERO(&read_fds);
        FD_SET(server_fd_, &read_fds);

        int max_fd = server_fd_;
        for (const auto& client : clients_) {
            FD_SET(client.connfd, &read_fds);
            max_fd = std::max(max_fd, client.connfd);
        }

        sigset_t empty_mask;
        sigemptyset(&empty_mask);

        int ready = pselect(max_fd + 1, &read_fds, nullptr, nullptr, nullptr, &empty_mask);
        if (ready == -1) {
            if (errno == EINTR && was_signal_) {
                handleSignal();
                was_signal_ = 0;
            }
            continue;
        }

        // Новое соединение
        if (FD_ISSET(server_fd_, &read_fds)) {
            handleNewConnection();
        }

        // Данные от клиентов
        for (auto it = clients_.begin(); it != clients_.end();) {
            if (FD_ISSET(it->connfd, &read_fds)) {
                handleClientData(*it);
                if (errno == ECONNRESET || errno == EPIPE) {
                    closeClient(*it);
                    it = clients_.erase(it);
                    continue;
                }
            }
            ++it;
        }
    }
}

// Обработка нового соединения
void Server::handleNewConnection() {
    struct sockaddr_in client_addr{};
    socklen_t client_len = sizeof(client_addr);
    int connfd = accept(server_fd_, (struct sockaddr*)&client_addr, &client_len);

    if (connfd == -1) {
        perror("Accept failed");
        return;
    }

    if (clients_.size() < MAX_CLIENTS) {
        clients_.push_back({connfd, client_addr});
        printClientAddress(clients_.back());
        std::cout << " Connected!\n";
    } else {
        std::cout << "Rejecting additional client connection\n";
        close(connfd);
    }
}

// Обработка данных клиента
void Server::handleClientData(Client& client) {
    char buffer[1024] = {0};
    ssize_t bytes = recv(client.connfd, buffer, sizeof(buffer), 0);

    if (bytes > 0) {
        buffer[bytes - 1] = 0;
        printClientAddress(client);
        std::cout << " " << buffer << std::endl;
    } else if (bytes == 0) {
        closeClient(client);
        std::cout << " Connection closed\n";
    } else {
        perror("recv failed");
    }
}

// Закрытие соединения
void Server::closeClient(Client& client) {
    close(client.connfd);
}

// Вывод информации о клиенте
void Server::printClientAddress(const Client& client) {
    std::cout << "[" << inet_ntoa(client.addr.sin_addr) << ":" << ntohs(client.addr.sin_port) << "]";
}

// Обработка сигнала
void Server::handleSignal() {
    std::cout << "Received SIGHUP signal. Connected clients:\n";
    for (const auto& client : clients_) {
        printClientAddress(client);
        std::cout << "\n";
    }
}
