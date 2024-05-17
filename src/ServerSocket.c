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
#include "UInt8List.h"
#include <threads.h>
#include <netinet/in.h>

#define Super() Object_vtable()
#define Self ServerSocket
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_SERVER_SOCKET_PARENTS)

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

#define ENUMERATE_SSM_CAPTURES(CAPTURE) \
    CAPTURE(ServerSocket, myself)


IMPLEMENT_LAMBDA(ServerSocketMain, ENUMERATE_SSM_CAPTURES, NO_OWNED_CAPTURES, ServerSocket myself) {
    Lambda_ServerSocketMain self = DOWNCAST(this, Lambda_ServerSocketMain);
    ServerSocket myself = self.data->myself;
    int sockfd = myself.data->sockfd;
    StreamSubscription subscription = myself.data->subs;
    THROWS = va_arg(args, Throwable*);

    fprintf(stderr, "on thread\n");
    struct sockaddr clientAddr;
    size_t clientAddrLen;
    int clientFd;
    fprintf(stderr, "accept\n");
    while ((clientFd = accept(sockfd, &clientAddr, (socklen_t*)&clientAddrLen)) > 0) {
        fprintf(stderr, "accepted\n");
        Socket connection = Socket$make_new(clientFd, &clientAddr, clientAddrLen);
        StreamSubscription_handleData(subscription, connection.asObject);
    }
    Throwable error = IOException$make_fromErrno().asThrowable;
    StreamSubscription_handleError(subscription, error);
    StreamSubscription_handleDone(subscription);
}

#define MAX_CONNECTIONS 200
void startServerSocket(ServerSocket this) {
    int sockfd = this.data->sockfd;
    StreamSubscription  subscription = this.data->subs;
    fprintf(stderr, "listen\n");
    Throwable EXCEPTION = DOWNCAST(null, Throwable);
    int success = listen(sockfd, MAX_CONNECTIONS);
    if (success < 0) {
        Throwable error = IOException$make_fromErrno().asThrowable;
        StreamSubscription_handleError(subscription, error);
        StreamSubscription_handleDone(subscription);
        return;
    }
    fprintf(stderr, "success\n");
    fprintf(stderr, "thread\n");
    Thread thread = Thread_spawnSync(Lambda_ServerSocketMain$make_new(this).asFunction, &EXCEPTION);
    if (!Object_isNull(EXCEPTION.asObject)) {
        Object_delete(thread.asObject);
        StreamSubscription_handleError(subscription, EXCEPTION);
        StreamSubscription_handleDone(subscription);
        return;
    }
    this.data->serverThread = thread;
}

#define ENUMERATE_CLOSE_CAPTURES(CAPTURE) \
    CAPTURE(ServerSocket, myself)

IMPLEMENT_LAMBDA(Close, ENUMERATE_CLOSE_CAPTURES, NO_OWNED_CAPTURES, ServerSocket myself) {
    Lambda_Close self = DOWNCAST(this, Lambda_Close);
    ServerSocket myself = self.data->myself;
    THROWS = va_arg(args, Throwable*);
    if (Object_isNull(myself.data->subs.asObject)) {
        return null;
    }
    Thread_kill(myself.data->serverThread, KillUrgency$immediate);
    StreamSubscription_handleDone(myself.data->subs);
    return null;
}

#define ENUMERATE_START_CAPTURES(CAPTURE) \
    CAPTURE(ServerSocket, myself)

IMPLEMENT_LAMBDA(Start, ENUMERATE_START_CAPTURES, NO_OWNED_CAPTURES, ServerSocket myself) {
    Lambda_Start self = DOWNCAST(this, Lambda_Start);
    ServerSocket myself = self.data->myself;
    THROWS = va_arg(args, Throwable*);
    startServerSocket(myself);
    return null;
}

IMPLEMENT_OVERRIDE_METHOD(StreamSubscription, Stream, listen, Function onData, Function onError, Function onDone, Bool cancelOnError) {
    Self self = DOWNCAST(this, Self);
    Function onCancel = Lambda_Close$make_new(self).asFunction;
    StreamSubscription subs = StreamSubscription$make_new(onData, onError, onDone, onCancel, cancelOnError);
    self.data->subs = subs;
    Function start = Lambda_Start$make_new(self).asFunction;
    Future_computation(start);
    return subs;
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
1,
        "Stream", offsetof(Socket_vtable_t , Stream_vtable)
        );
    // Socket
    // Stream
    Stream_vtable_t *stream_vtable = &vtable->Stream_vtable;
    initImplementedInterfaceVtable(
            (Interface_vtable_t*)stream_vtable,
            (Interface_vtable_t*)IOSink_vtable(),
            sizeof(*IOSink_vtable()),
            offsetof(struct Socket_vtable_t, IOSink_vtable)
    );
    stream_vtable->listen = _ServerSocket_listen_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    //object_vtable->delete = _List_delete_impl;
}

OBJECT_CAST_IMPL(Stream, ServerSocket)
INTERFACE_CAST_IMPL(ServerSocket, Stream, Object)

IMPLEMENT_CONSTRUCTOR(new, int domain, int sockfd, const struct sockaddr* addr, socklen_t addrlen) {
    this.data->domain = domain;
    this.data->sockfd = sockfd;
    this.data->addr = malloc(addrlen);
    memcpy((struct sockaddr*)this.data->addr, addr, addrlen);
    this.data->addrlen = addrlen;
    this.data->subs = DOWNCAST(null, StreamSubscription);
}

IMPLEMENT_STATIC_METHOD(ServerSocket, bindSync, int domain, const struct sockaddr* addr, socklen_t addrlen, THROWS) {
    int sockfd = socket(domain, SOCK_STREAM, 0);
    if (sockfd < 0) {
        THROW(IOException$make_fromErrno(), DOWNCAST(null, ServerSocket))
    }
    int success = bind(sockfd, addr, addrlen);
    if (success < 0) {
        int e = errno;
        success = close(sockfd);
        if (success < 0) {
            // fuuuck
            e = errno;
        }
        THROW(IOException$make_new(e), DOWNCAST(null, ServerSocket))
    }
    return ServerSocket$make_new(domain, sockfd, addr, addrlen);
}

#undef Super
#undef Self