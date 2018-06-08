cmake_minimum_required(VERSION 3.10.2)
project(dns_demo VERSION 0.0.1)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin")

add_executable(dns_demo
    dns_demo.c
)
