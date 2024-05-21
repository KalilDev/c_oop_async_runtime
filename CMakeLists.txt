cmake_minimum_required(VERSION 3.27)
project(c_oop C)

set(CMAKE_C_STANDARD 23)

add_subdirectory(liboop)

add_executable(
        server
        main.c
)
target_link_libraries(server PRIVATE oop)
target_include_directories(server PRIVATE liboop/include)

add_executable(
        client
        client.c
)
target_link_libraries(client PRIVATE oop)
target_include_directories(client PRIVATE liboop/include)