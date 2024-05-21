#include "Object.h"
#include "Socket.h"
#include "ServerSocket.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include "StringBuffer.h"
#include <strings.h>
#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include "String.h"
#include "primitive/StringRef.h"
#include "foreach.h"
#include "GrowableList.h"
#include "Throwable.h"
#include "Future.h"
#include "IOException.h"
#include "Function.h"
#include "primitive/Bool.h"
#include "StreamSubscription.h"
#include "HttpResponse.h"
#include "UInt8List.h"
#include "autoclean.h"
#include <threads.h>
#include <netinet/in.h>

#define Super() Object_vtable()
#define Self HttpResponse

IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_HTTP_RESPONSE_PARENTS)

ENUMERATE_HTTP_RESPONSE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
List self = DOWNCAST(this, List);
// Delete the array
Super()

->
delete(this);
}

#define ENUMERATE_HTTP_RESPONSE_METHODS(METHOD) \
    METHOD(void, setStatusCode, Integer status) \
    METHOD(void, setStatusReason, String reason) \
    METHOD(void, setHttp, String http)

IMPLEMENT_SELF_METHOD(Future, send) {
    IOSink iosink = HttpResponse_as_IOSink(this);
    Sink sink = iosink.asSink;
    String responseCommand = String_format_c("{} {} {}\r\n", HttpResponse_http(this), HttpResponse_statusCode(this), HttpResponse_statusReason(this));
    String emptyLine = StringRef$wrap("\r\n").asString;

    Sink_add(sink, responseCommand.asObject);
    HttpHeaders_encodeTo(HttpResponse_headers(this), sink);
    Sink_add(sink, emptyLine.asObject);
    return IOSink_flush(iosink);
}

IMPLEMENT_SELF_METHOD(void, setStatusCode, Integer status) {
    this.data->statusCode = status;
}

IMPLEMENT_SELF_METHOD(void, setStatusReason, String reason) {
    this.data->statusReason = reason;
}

IMPLEMENT_SELF_METHOD(void, setHttp, String http) {
    this.data->http = http;
}


IMPLEMENT_OVERRIDE_METHOD(void, Sink, close) {
    Self self = Sink_as_HttpResponse(this);
    Sink_close(Socket_as_IOSink(self.data->socket).asSink);
}

IMPLEMENT_OVERRIDE_METHOD(Future, IOSink, flush) {
    Self self = IOSink_as_HttpResponse(this);
    return IOSink_flush(Socket_as_IOSink(self.data->socket));
}

IMPLEMENT_OVERRIDE_METHOD(Future, IOSink, addStream, Stream stream) {
    Self self = IOSink_as_HttpResponse(this);
    return IOSink_addStream(Socket_as_IOSink(self.data->socket), stream);
}

IMPLEMENT_OVERRIDE_METHOD(void, Sink, add, Object value) {
    Self self = Sink_as_HttpResponse(this);
    if (IS_OBJECT_ASSIGNABLE(value, String)) {
        String v = DOWNCAST(value, String);
        UInt8List stringBytes = UInt8List$make_fromBuffer((unsigned char *)String_cStringView(v), String_length(v));
        // TODO: Free string
        value = stringBytes.asObject;
    }
    Sink_add(Socket_as_IOSink(self.data->socket).asSink, value);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
    (Object_vtable_t *) vtable,
    (Object_vtable_t *) Super(),
    sizeof(*Super()),
    STR(Self),
    2,
    "IOSink", offsetof(HttpResponse_vtable_t, IOSink_vtable),
    "Sink", offsetof(HttpResponse_vtable_t, IOSink_vtable)
    );
    // HttpResponse
    vtable->send = _HttpResponse_send_impl;
    vtable->setHttp = _HttpResponse_setHttp_impl;
    vtable->setStatusCode = _HttpResponse_setStatusCode_impl;
    vtable->setStatusReason = _HttpResponse_setStatusReason_impl;
    // IOSink
    IOSink_vtable_t *iosink_vtable = &vtable->IOSink_vtable;
    initImplementedInterfaceVtable(
        (Interface_vtable_t *) iosink_vtable,
        (Interface_vtable_t *) IOSink_vtable(),
        sizeof(*IOSink_vtable()),
        offsetof(struct HttpResponse_vtable_t, IOSink_vtable)
    );
    iosink_vtable->flush = _HttpResponse_flush_impl;
    iosink_vtable->addStream = _HttpResponse_addStream_impl;
    // Sink
    Sink_vtable_t *sink_vtable = (Sink_vtable_t *)iosink_vtable;
    sink_vtable->add = _HttpResponse_add_impl;
    sink_vtable->close = _HttpResponse_close_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t *) vtable;
    //object_vtable->delete = _List_delete_impl;
}

INTERFACE_CAST_IMPL(HttpResponse, IOSink, Object)
OBJECT_CAST_IMPL(IOSink, HttpResponse)

Sink HttpResponse_as_Sink(HttpResponse this) {
    return HttpResponse_as_IOSink(this).asSink;
}

HttpResponse Sink_as_HttpResponse(Sink this) {
    return IOSink_as_HttpResponse(DOWNCAST(this, IOSink));
}

IMPLEMENT_CONSTRUCTOR(fromTcp, Socket socket) {
    this.data->socket = socket;
    this.data->statusReason = DOWNCAST(null, String);
    this.data->statusCode = DOWNCAST(null, Integer);
    this.data->http = DOWNCAST(null, String);
    this.data->headers = HttpHeaders$make_new();
}

IMPLEMENT_SELF_GETTER(Integer, statusCode) {
    return this.data->statusCode;
}

IMPLEMENT_SELF_GETTER(String, statusReason) {
    return this.data->statusReason;
}

IMPLEMENT_SELF_GETTER(String, http) {
    return this.data->http;
}

IMPLEMENT_SELF_GETTER(HttpHeaders, headers) {
    return this.data->headers;
}

#undef Super
#undef Self