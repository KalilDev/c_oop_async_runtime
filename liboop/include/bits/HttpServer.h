#ifndef HttpServer
#define HttpServer HttpServer

#include "Object.h"
#include "oop.h"
#include "Stream.h"
#include "Sink.h"
#include "Function.h"
#include "StreamController.h"
#include "ServerSocket.h"
#include "StreamSubscription.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>
#include <netinet/in.h>

#define WITH_RTTI

#include "rtti.h"

#define Super Object
#define Self HttpServer

START_CLASS

#define ENUMERATE_HTTP_SERVER_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_HttpServer$fromTcp serverSocket
#define ENUMERATE_HTTP_SERVER_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(fromTcp, ServerSocket serverSocket)

#define ENUMERATE_HTTP_SERVER_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Stream)

#define ENUMERATE_HTTP_SERVER_STATIC_METHODS(METHOD) \
    METHOD(HttpServer, bindSync, String address, in_port_t port, THROWS) \
    METHOD(HttpServer, bind6Sync, String address, in_port_t port, THROWS)

#define ENUMERATE_HTTP_SERVER_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(ServerSocket, serverSocket) \
    ATTRIBUTE(StreamController, requestsController) \
    ATTRIBUTE(StreamSubscription, serverSubscription)


DEFINE_SELF_CLASS(
        ENUMERATE_HTTP_SERVER_PARENTS,
        ENUMERATE_HTTP_SERVER_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_HTTP_SERVER_ATTRIBUTES,
        ENUMERATE_HTTP_SERVER_CONSTRUCTORS,
        ENUMERATE_HTTP_SERVER_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_OBJECT_CAST(Stream, HttpServer)
DECLARE_INTERFACE_CAST(HttpServer, Stream)

END_CLASS

#undef Self
#undef Super

#endif