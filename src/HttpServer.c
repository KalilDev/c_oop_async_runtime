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
#include "HttpServer.h"
#include "HttpRequest.h"
#include <threads.h>
#include <netinet/in.h>
#include "StreamSubscription.h"
#include <arpa/inet.h>

#define Super() Object_vtable()
#define Self HttpServer
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_HTTP_SERVER_PARENTS)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    List self = DOWNCAST(this, List);
    if (List_length(self) == 0) {
        Super()->delete(this);
        return;
    }
    // If there are objects, delete them
    foreach (Object, e, List_as_Iterable(self), {
        Object_delete(e);
    })

    // Delete the array
    Super()->delete(this);
}


OBJECT_CAST_IMPL(Stream, HttpServer)
INTERFACE_CAST_IMPL(HttpServer, Stream, Object)


#define CAPTURE_MYSELF(CAPTURE)\
    CAPTURE(HttpServer, myself)

IMPLEMENT_LAMBDA(OnHttpRequestReady, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpServer myself) {
    Lambda_OnHttpRequestReady self = DOWNCAST(this, Lambda_OnHttpRequestReady);
    HttpServer myself = self.data->myself;
    HttpRequest request = va_arg(args, HttpRequest);
    StreamController requestsController = myself.data->requestsController;
    StreamController_add(requestsController, request.asObject);
}

IMPLEMENT_LAMBDA(OnHttpRequestError, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpServer myself) {
    Lambda_OnHttpRequestReady self = DOWNCAST(this, Lambda_OnHttpRequestReady);
    HttpServer myself = self.data->myself;
    Throwable error = va_arg(args, Throwable);
    StreamController requestsController = myself.data->requestsController;
    StreamController_addError(requestsController, error);
}

IMPLEMENT_LAMBDA(OnTcpConnectionReceived, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpServer myself) {
    Lambda_OnTcpConnectionReceived self = DOWNCAST(this, Lambda_OnTcpConnectionReceived);
    HttpServer myself = self.data->myself;
    StreamController requestsController = myself.data->requestsController;
    Socket connection = va_arg(args, Socket);

    HttpRequest request = HttpRequest$make_fromTcp(connection);
    Future onHttpRequestReady = HttpRequest_onReady(request);
    onHttpRequestReady = Future_then(onHttpRequestReady, Lambda_OnHttpRequestReady$make_new(myself).asFunction);
    onHttpRequestReady = Future_catch(onHttpRequestReady, Lambda_OnHttpRequestError$make_new(myself).asFunction);
    return onHttpRequestReady.asObject;
}

IMPLEMENT_LAMBDA(OnTcpConnectionError, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpServer myself) {
    Lambda_OnTcpConnectionReceived self = DOWNCAST(this, Lambda_OnTcpConnectionReceived);
    HttpServer myself = self.data->myself;
    StreamController requestsController = myself.data->requestsController;
    Throwable error = va_arg(args, Throwable);
    StreamController_addError(requestsController, error);
    return null;
}

IMPLEMENT_LAMBDA(OnTcpConnectionDone, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpServer myself) {
    Lambda_OnTcpConnectionReceived self = DOWNCAST(this, Lambda_OnTcpConnectionReceived);
    HttpServer myself = self.data->myself;
    StreamController requestsController = myself.data->requestsController;
    StreamController_close(requestsController);
    return null;
}

IMPLEMENT_LAMBDA(OnListen, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpServer myself) {
    Lambda_OnListen self = DOWNCAST(this, Lambda_OnListen);
    HttpServer myself = self.data->myself;
    ServerSocket serverSocket = myself.data->serverSocket;
    StreamController streamController = va_arg(args, StreamController);
    StreamSubscription subs = va_arg(args, StreamSubscription);
    myself.data->serverSubscription = Stream_listen(
            ServerSocket_as_Stream(serverSocket),
            Lambda_OnTcpConnectionReceived$make_new(myself).asFunction,
            Lambda_OnTcpConnectionError$make_new(myself).asFunction,
            Lambda_OnTcpConnectionDone$make_new(myself).asFunction,
            Bool$box(StreamSubscription_cancelOnError(subs))
    );
}

IMPLEMENT_LAMBDA(OnCancelSubscription, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpServer myself) {
    Lambda_OnListen self = DOWNCAST(this, Lambda_OnListen);
    HttpServer myself = self.data->myself;
    StreamSubscription serverSubscription = myself.data->serverSubscription;
    StreamSubscription_cancel(serverSubscription);
}

IMPLEMENT_OVERRIDE_METHOD(StreamSubscription, Stream, listen, Function onData, Function onError, Function onDone, Bool cancelOnError) {
    Self self = Stream_as_HttpServer(this);
    return self.data->serverSubscription = Stream_listen(StreamController_as_Stream(self.data->requestsController), onData, onError, onDone, cancelOnError);
}
IMPLEMENT_SELF_VTABLE() {
    initVtable(
            (Object_vtable_t*)vtable,
            (Object_vtable_t*)Super(),
            sizeof(*Super()),
            STR(Self),
            1,
            "Stream", offsetof(ServerSocket_vtable_t , Stream_vtable)
    );
    // ServerSocket
    // Stream
    Stream_vtable_t *stream_vtable = &vtable->Stream_vtable;
    initImplementedInterfaceVtable(
            (Interface_vtable_t*)stream_vtable,
            (Interface_vtable_t*)Stream_vtable(),
            sizeof(*Stream_vtable()),
            offsetof(struct ServerSocket_vtable_t, Stream_vtable)
    );
    stream_vtable->listen = _HttpServer_listen_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    //object_vtable->delete = _List_delete_impl;
}


IMPLEMENT_CONSTRUCTOR(fromTcp, ServerSocket serverSocket) {
    this.data->serverSubscription = DOWNCAST(null, StreamSubscription);
    this.data->serverSocket = serverSocket;
    this.data->requestsController = StreamController$make_new(
        Lambda_OnListen$make_new(this).asFunction,
        Lambda_OnCancelSubscription$make_new(this).asFunction
    );
}


IMPLEMENT_STATIC_METHOD(HttpServer, bindSync, String address, in_port_t port, THROWS) {
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr(String_cStringView(address));
    addr.sin_port = htons(port);
    addr.sin_family = AF_INET;
    ServerSocket tcpServer = ServerSocket_bindSync(
            AF_INET,
            (const struct sockaddr*)&addr,
            sizeof(addr),
            EXCEPTION
    );
    if (HAS_EXCEPTION) {
        RETHROW(DOWNCAST(null, HttpServer))
    }
    return HttpServer$make_fromTcp(tcpServer);
}

IMPLEMENT_STATIC_METHOD(HttpServer, bind6Sync, String address, in_port_t port, THROWS) {
//    struct sockaddr_in6 addr;
//    addr.sin_addr.s_addr = inet_addr(String_cStringView(address));
//    addr.sin_port = port;
//    addr.sin_family = AF_INET;
//    ServerSocket tcpServer = ServerSocket_bindSync(
//            AF_INET,
//            (const struct sockaddr*)&addr,
//            sizeof(addr),
//            EXCEPTION
//    );
//    return HttpServer$make_fromTcp(tcpServer);
    return DOWNCAST(null, HttpServer);
}

#undef Super
#undef Self