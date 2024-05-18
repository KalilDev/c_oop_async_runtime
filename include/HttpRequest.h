#ifndef HttpRequest
#define HttpRequest HttpRequest

#include "Object.h"
#include "oop.h"
#include "Stream.h"
#include "Sink.h"
#include "Function.h"
#include "StreamController.h"
#include "ServerSocket.h"
#include "HttpHeaders.h"
#include "StreamSubscription.h"
#include <stdlib.h>
#include "HttpResponse.h"
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>

#define WITH_RTTI

#include "rtti.h"
#include "IOSink.h"

#define Super Object
#define Self HttpRequest

START_CLASS

#define ENUMERATE_HTTP_REQUEST_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_HttpRequest$fromTcp socket
#define ENUMERATE_HTTP_REQUEST_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(fromTcp, Socket socket)

#define PARAMS_INVOCATION_HttpRequest_processLine line, state
#define ENUMERATE_HTTP_REQUEST_METHODS(METHOD) \
    METHOD(HttpRequestReceiveState, processLine, String line, HttpRequestReceiveState state) \
    METHOD(void, startListening)               \
    METHOD(HttpResponse, response)

#define ENUMERATE_HTTP_REQUEST_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Stream)

#define ENUMERATE_HTTP_REQUEST_GETTERS(ATTRIBUTE) \
    ATTRIBUTE(Future, onReady)                    \
    ATTRIBUTE(String, method)\
    ATTRIBUTE(String, http)\
    ATTRIBUTE(String, uri)

typedef enum HttpRequestReceiveState {
    HttpRequestReceiveState$command = 0,
    HttpRequestReceiveState$headers = 1,
    HttpRequestReceiveState$lineAfterHeaders = 2,
    HttpRequestReceiveState$content = 3,
    HttpRequestReceiveState$error = 4,
} HttpRequestReceiveState;

#define ENUMERATE_HTTP_REQUEST_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(Socket, socket)                        \
    ATTRIBUTE(ByteBuffer, scratchpad)                \
    ATTRIBUTE(String, method)                        \
    ATTRIBUTE(String, uri)                           \
    ATTRIBUTE(String, http)\
    ATTRIBUTE(HttpHeaders, headers)                \
    ATTRIBUTE(StreamController, bodyController) \
    ATTRIBUTE(StreamSubscription, socketSubscription)\
    ATTRIBUTE(Completer, onReady)                    \
    ATTRIBUTE(HttpRequestReceiveState, state) \
    ATTRIBUTE(HttpResponse, response)


DEFINE_SELF_CLASS(
    ENUMERATE_HTTP_REQUEST_PARENTS,
    ENUMERATE_HTTP_REQUEST_IMPLEMENTS,
    ENUMERATE_HTTP_REQUEST_METHODS,
    ENUMERATE_HTTP_REQUEST_ATTRIBUTES,
    ENUMERATE_HTTP_REQUEST_CONSTRUCTORS,
    NO_STATIC_METHODS,
    NO_STATIC_ATTRIBUTES,
    ENUMERATE_HTTP_REQUEST_GETTERS
)

DECLARE_OBJECT_CAST(Stream, HttpRequest)
DECLARE_INTERFACE_CAST(HttpRequest, Stream)

END_CLASS

#undef Self
#undef Super

#endif