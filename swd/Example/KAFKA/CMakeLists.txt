cmake_minimum_required(VERSION 3.10)

project(producer)

add_executable(producer producer.cpp)

target_link_libraries(producer rdkafka++)

