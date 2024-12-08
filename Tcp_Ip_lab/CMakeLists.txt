cmake_minimum_required(VERSION 3.10)
project(TcpSignalServer)

set(CMAKE_CXX_STANDARD 17)

# Добавляем библиотеку
add_library(server_lib server.cpp)
target_include_directories(server_lib PUBLIC include)

# Основной исполняемый файл
add_executable(main main.cpp)
target_link_libraries(main server_lib)
