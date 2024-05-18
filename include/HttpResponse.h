#ifndef HttpResponse
#define HttpResponse HttpResponse

#include "Object.h"
#include "oop.h"
#include "Socket.h"
#include "String.h"
#include "primitive/Integer.h"
#include "Sink.h"
#include "Function.h"
#include "StreamController.h"
#include "ServerSocket.h"
#include "HttpHeaders.h"
#include "StreamSubscription.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>

#define WITH_RTTI

#include "rtti.h"
#include "IOSink.h"

#define Super Object
#define Self HttpResponse

START_CLASS

#define ENUMERATE_HTTP_RESPONSE_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_HttpResponse$fromTcp socket
#define ENUMERATE_HTTP_RESPONSE_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(fromTcp, Socket socket)

#define PARAMS_INVOCATION_HttpResponse_setStatusCode status
#define PARAMS_INVOCATION_HttpResponse_setStatusReason reason
#define PARAMS_INVOCATION_HttpResponse_setHttp http
#define ENUMERATE_HTTP_RESPONSE_METHODS(METHOD) \
    METHOD(Future, send) \
    METHOD(void, setStatusCode, Integer status) \
    METHOD(void, setStatusReason, String reason) \
    METHOD(void, setHttp, String http)



#define ENUMERATE_HTTP_RESPONSE_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(IOSink)

#define ENUMERATE_HTTP_RESPONSE_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(Integer, statusCode)                \
    ATTRIBUTE(String, statusReason)                    \
    ATTRIBUTE(HttpHeaders, headers)                    \
    ATTRIBUTE(String, http)

#define ENUMERATE_HTTP_RESPONSE_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(Socket, socket) \
    ATTRIBUTE(String, statusReason)                           \
    ATTRIBUTE(Integer, statusCode)                           \
    ATTRIBUTE(String, http)\
    ATTRIBUTE(HttpHeaders, headers)


DEFINE_SELF_CLASS(
        ENUMERATE_HTTP_RESPONSE_PARENTS,
        ENUMERATE_HTTP_RESPONSE_IMPLEMENTS,
        ENUMERATE_HTTP_RESPONSE_METHODS,
        ENUMERATE_HTTP_RESPONSE_ATTRIBUTES,
        ENUMERATE_HTTP_RESPONSE_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        ENUMERATE_HTTP_RESPONSE_GETTERS
)

DECLARE_OBJECT_CAST(IOSink, HttpResponse)
DECLARE_INTERFACE_CAST(HttpResponse, IOSink)

DECLARE_OBJECT_CAST(Sink, HttpResponse)
DECLARE_INTERFACE_CAST(HttpResponse, Sink)

END_CLASS

#undef Self
#undef Super

#endif