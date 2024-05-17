#ifndef ServerSocket
#define ServerSocket ServerSocket

#include "Object.h"
#include "oop.h"
#include "Stream.h"
#include "Thread.h"
#include "StreamSubscription.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>

#define WITH_RTTI

#include "rtti.h"

#define Super Object
#define Self ServerSocket

START_CLASS

#define ENUMERATE_SERVER_SOCKET_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_ServerSocket$new domain, sockfd, addr, addrlen
#define ENUMERATE_SERVER_SOCKET_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, int domain, int sockfd, const struct sockaddr* addr, socklen_t addrlen)

#define ENUMERATE_SERVER_SOCKET_STATIC_METHODS(METHOD) \
    METHOD(ServerSocket, bindSync, int domain, const struct sockaddr* addr, socklen_t addrlen, THROWS)

#define ENUMERATE_SERVER_SOCKET_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Stream)

#define ENUMERATE_SERVER_SOCKET_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(int, domain)                                        \
    ATTRIBUTE(int, sockfd)                                        \
    ATTRIBUTE(const struct sockaddr*, addr)                       \
    ATTRIBUTE(socklen_t, addrlen)                     \
    ATTRIBUTE(Thread, serverThread)                   \
    ATTRIBUTE(StreamSubscription, subs)

DEFINE_SELF_CLASS(
        ENUMERATE_SERVER_SOCKET_PARENTS,
        ENUMERATE_SERVER_SOCKET_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_SERVER_SOCKET_ATTRIBUTES,
        ENUMERATE_SERVER_SOCKET_CONSTRUCTORS,
        ENUMERATE_SERVER_SOCKET_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_OBJECT_CAST(Stream, ServerSocket)
DECLARE_INTERFACE_CAST(ServerSocket, Stream)

END_CLASS

#undef Self
#undef Super

#endif