#ifndef Socket
#define Socket Socket

#include "Object.h"
#include "oop.h"
#include "Stream.h"
#include "StreamController.h"
#include "ByteBuffer.h"
#include "IOCoroutine.h"
#include "IOSink.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>

#define WITH_RTTI

#include "rtti.h"

#define Super Object
#define Self Socket

START_CLASS

#define ENUMERATE_SOCKET_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_Socket$new sockfd, addr, addrlen
#define ENUMERATE_SOCKET_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, int sockfd, const struct sockaddr* addr, socklen_t addrlen)

#define ENUMERATE_SOCKET_STATIC_METHODS(METHOD) \
    METHOD(Socket, connectSync, int domain, const struct sockaddr* addr, socklen_t addrlen, THROWS)

#define ENUMERATE_SOCKET_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Stream)                                 \
    IMPLEMENTS(IOSink)

#define ENUMERATE_SOCKET_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(int, sockfd)                                        \
    ATTRIBUTE(const struct sockaddr*, addr)                       \
    ATTRIBUTE(socklen_t, addrlen)              \
    ATTRIBUTE(ByteBuffer, queued)              \
    ATTRIBUTE(StreamController, streamController)                 \
    ATTRIBUTE(IOCoroutine, coroutine)


DEFINE_SELF_CLASS(
        ENUMERATE_SOCKET_PARENTS,
        ENUMERATE_SOCKET_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_SOCKET_ATTRIBUTES,
        ENUMERATE_SOCKET_CONSTRUCTORS,
        ENUMERATE_SOCKET_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_OBJECT_CAST(Stream, Socket)
DECLARE_INTERFACE_CAST(Socket, Stream)

DECLARE_OBJECT_CAST(IOSink, Socket)
DECLARE_INTERFACE_CAST(Socket, IOSink)

END_CLASS

#undef Self
#undef Super

#endif