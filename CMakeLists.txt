cmake_minimum_required(VERSION 3.24)
project(snakemp C)

set(CMAKE_C_STANDARD 11)
set(PROJECT_HEADERS
        fmod.h
        )
set(PROJECT_SOURCES
        main.c
        )

add_executable(snakemp main.c)

target_link_libraries(snakemp PUBLIC
        ${GLFW_LIBRARIES}
        ${GLEW_LIBRARIES}
        ${OPENGL_LIBRARIES}
        winMM.Lib)

