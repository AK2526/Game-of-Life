cmake_minimum_required(VERSION 3.28.0)

project(GameOfLife)


include_directories(
    ${CMAKE_SOURCE_DIR}/libraries/include
)

# List of directories to copy
set(DIRECTORIES_TO_COPY
    resources
    shaders
)

# Copy each directory to the build directory
foreach(DIR ${DIRECTORIES_TO_COPY})
    file(COPY ${CMAKE_SOURCE_DIR}/${DIR} DESTINATION ${CMAKE_BINARY_DIR})
endforeach()


add_executable(${PROJECT_NAME} src/main.cpp src/glad.c src/stb.cpp src/input.cpp src/cellupdates.cpp src/cells.cpp)

target_link_libraries(${PROJECT_NAME}
    ${CMAKE_SOURCE_DIR}/libraries/lib/glfw3.lib
)