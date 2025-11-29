cmake_minimum_required(VERSION 4.2.0)

project(Server LANGUAGES C)

add_executable(Server src/main.c src/Message.c src/log.c)

target_compile_options(Server PRIVATE -Wall -Wextra -Wpedantic)

target_include_directories(Server PRIVATE include/)
