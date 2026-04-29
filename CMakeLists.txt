cmake_minimum_required(VERSION 4.0)
project(HTTP_test C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(SOURCES
        src/ServerSide/httpRequestTest.c
        src/ServerSide/MazeAlteration/changeMaze.c
        src/ServerSide/MazeGeneration/generateMaze.c
        src/ServerSide/RequestManager/fileUnpacker.c
        src/ServerSide/RequestManager/requestManager.c
        src/ServerSide/RequestManager/uploadstringToDataStruct.c
        src/ServerSide/Maze/maze.c
        src/ServerSide/Maze/maze.h
)

add_executable(HTTP_test ${SOURCES})
add_executable(Unnamed src/ServerSide/MazeAlteration/changeMaze.c src/ServerSide/Maze/maze.c)


target_link_libraries(HTTP_test PRIVATE microhttpd)
IF (WIN32)
target_link_libraries(HTTP_test PRIVATE ws2_32)
ELSE()
target_link_libraries(HTTP_test PRIVATE m)
ENDIF()