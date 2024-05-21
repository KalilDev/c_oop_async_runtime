#ifndef HttpHeaders
#define HttpHeaders HttpHeaders

#include "bits/Object.h"
#include "bits/oop.h"
#include "bits/Stream.h"
#include "bits/Sink.h"
#include "bits/Function.h"
#include "bits/StreamController.h"
#include "bits/ServerSocket.h"
#include "bits/StreamSubscription.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>

#define WITH_RTTI

#include "bits/rtti.h"

#define Super Object
#define Self HttpHeaders

START_CLASS

#define ENUMERATE_HTTP_HEADERS_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_HttpHeaders_decode line
#define PARAMS_INVOCATION_HttpHeaders_encodeTo sink
#define PARAMS_INVOCATION_HttpHeaders_add name, value
#define ENUMERATE_HTTP_HEADERS_METHODS(METHOD) \
    METHOD(void, decode, String line)          \
    METHOD(void, encodeTo, Sink sink)          \
    METHOD(Integer, contentLength)             \
    METHOD(void, add, String name, String value)

#define ENUMERATE_HTTP_HEADERS_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new)

#define ENUMERATE_HTTP_HEADERS_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(List, headers)


DEFINE_SELF_CLASS(
        ENUMERATE_HTTP_HEADERS_PARENTS,
        NO_IMPLEMENTS,
        ENUMERATE_HTTP_HEADERS_METHODS,
        ENUMERATE_HTTP_HEADERS_ATTRIBUTES,
        ENUMERATE_HTTP_HEADERS_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)


END_CLASS

#undef Self
#undef Super

#endif