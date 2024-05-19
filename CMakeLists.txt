cmake_minimum_required(VERSION 3.27)
project(c_oop C)

set(CMAKE_C_STANDARD 23)
#set(CMAKE_C_COMPILER clang)

add_library(c_oop STATIC
        src/primitive/Double.c
        src/primitive/Integer.c
        src/primitive/Number.c
        src/primitive/Bool.c
        src/primitive/StringRef.c
        src/primitive/OutOfMemoryException.c
        src/Object.c
        src/List.c
        src/Iterator.c
        src/Iterable.c
        src/ListIterator.c
        src/Task.c
        src/Completer.c
        src/Future.c
        src/EventLoop.c
        src/StreamSubscription.c
        src/Stream.c
        src/Closeable.c
        src/Interface.c
        src/String.c
        src/rtti.c
        src/StringIterator.c
        src/Comparable.c
        src/StringBuffer.c
        src/RandomAccessFile.c
        src/FileSystemEntity.c
        src/File.c
        src/Directory.c
        src/Link.c
        src/GrowableList.c
        src/TypedList.c
        src/UInt8List.c
        src/ByteBuffer.c
        src/Throwable.c
        src/Exception.c
        src/Error.c
        src/IOException.c
        src/TypeError.c
        src/Function.c
        src/Sink.c
        src/IOSink.c
        src/Socket.c
        src/ServerSocket.c
        include/Thread.h
        src/Thread.c
        src/ThreadChildren.c
        src/StreamController.c
        src/AsyncEvent.c
        src/HttpServer.c
        src/HttpRequest.c
        src/HttpHeaders.c
        src/HttpResponse.c
        src/IOCoroutine.c
)
target_link_libraries(c_oop PRIVATE m)

target_include_directories(c_oop PRIVATE PRIVATE include)


add_executable(
        oop_example
        main.c
)
target_link_libraries(oop_example PRIVATE c_oop)
target_include_directories(oop_example PRIVATE PRIVATE include)

add_executable(
        client
        client.c
)
target_link_libraries(client PRIVATE c_oop)
target_include_directories(client PRIVATE PRIVATE include)