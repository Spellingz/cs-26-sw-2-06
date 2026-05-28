cmake_minimum_required(VERSION 4.0)
project(MazeGenServer C)
set(CMAKE_C_STANDARD 23)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)


set(SOURCES
        src/ServerSide/main.c
        src/ServerSide/RequestManager/uploadstringToDataStruct.c
        src/ServerSide/MazeAlteration/changeMaze.c
        src/ServerSide/MazeGeneration/generateMaze.c
        src/ServerSide/Maze/maze.c
        src/ServerSide/Heatmap/heatmapGen.c
)

add_executable(MazeGenServer ${SOURCES})

set_target_properties(MazeGenServer PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}"
)

target_link_libraries(MazeGenServer PRIVATE microhttpd)
IF (WIN32)
target_link_libraries(MazeGenServer PRIVATE ws2_32)
ELSE()
target_link_libraries(MazeGenServer PRIVATE m)
ENDIF()