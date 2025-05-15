cmake_minimum_required(VERSION 3.31)
project(json_writer C)

set(CMAKE_C_STANDARD 11)

add_library(_json_writer STATIC
        src/json/json_writer.c
)
