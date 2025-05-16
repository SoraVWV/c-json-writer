cmake_minimum_required(VERSION 3.31)
project(jsonwriter C)

set(CMAKE_C_STANDARD 11)

add_library(jsonwriter STATIC
        src/json/json_writer.c
)
