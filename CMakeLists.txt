cmake_minimum_required(VERSION 4.2)
project(HTTP_test C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(SOURCES
        src/ServerSide/httpRequestTest.c
        src/ServerSide/MazeGeneration/generateMaze.h
        src/ServerSide/MazeGeneration/generateMaze.c
)

add_executable(HTTP_test ${SOURCES})

target_link_libraries(HTTP_test PRIVATE microhttpd)
target_link_libraries(HTTP_test PRIVATE ws2_32)