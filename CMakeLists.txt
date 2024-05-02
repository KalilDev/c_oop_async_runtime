cmake_minimum_required(VERSION 3.27)
project(c_oop C)

set(CMAKE_C_STANDARD 23)

add_library(c_oop STATIC
        src/Object.c
        src/ListIterator.c
        src/List.c
        src/Iterator.c
        src/Iterable.c
        src/Interface.c
        src/Closeable.c
        src/String.c
        src/primitive/Bool.c
        src/primitive/Double.c
        src/primitive/Integer.c
        src/primitive/Number.c
        src/primitive/StringRef.c
)
target_link_libraries(c_oop PRIVATE m)

target_include_directories(c_oop PRIVATE PRIVATE include)


add_executable(
        oop_example
        main.c
)
target_link_libraries(oop_example PRIVATE c_oop)
target_include_directories(oop_example PRIVATE PRIVATE include)
