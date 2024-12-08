cmake_minimum_required(VERSION 3.10)
project(MonitorApp)

# Установка стандартов C++
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# Добавляем исходные файлы проекта
add_library(Monitor monitor.cpp monitor.h)
add_executable(MonitorApp main.cpp)

# Линкуем библиотеку Monitor с исполняемым файлом
target_link_libraries(MonitorApp Monitor)
