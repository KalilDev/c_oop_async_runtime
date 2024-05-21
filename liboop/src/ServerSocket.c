#include <assert.h>
#include <unistd.h>
#include <errno.h>
#include "liboop.h"
#include <fcntl.h>
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

#define MAX_CONNECTIONS 200

#define CAPTURE_MYSELF(CAPTURE) \
    CAPTURE(ServerSocket, myself)

IMPLEMENT_LAMBDA(OnCancel, CAPTURE_MYSELF, NO_OWNED_CAPTURES, ServerSocket myself) {
    Lambda_OnCancel self = DOWNCAST(this, Lambda_OnCancel);
    StreamController controller = va_arg(args, StreamController);
    ServerSocket myself = self.data->myself;
    IOCoroutine coroutine = myself.data->coroutine;
    if (Object_isNull(coroutine.asObject)) {
        StreamController_close(controller);
        return null;
    }
    IOCoroutine_remove(coroutine);
    StreamController_close(controller);
    return null;
}

#define ENUMERATE_STEP_CAPTURES(CAPTURE) \
    CAPTURE(ServerSocket, myself)                \
    CAPTURE(StreamController, controller)


IMPLEMENT_LAMBDA(Step, ENUMERATE_STEP_CAPTURES, NO_OWNED_CAPTURES, ServerSocket myself, StreamController controller) {
    Lambda_Step looper = DOWNCAST(this, Lambda_Step);
    ServerSocket myself = looper.data->myself;
    StreamController controller = looper.data->controller;
    int sockfd = myself.data->sockfd;
    Throwable error = DOWNCAST(null, Throwable);
    Socket connection = DOWNCAST(null, Socket);
    THROWS = &error;
    {
        struct sockaddr clientAddr;
        size_t clientAddrLen;
        int clientFd = accept(sockfd, &clientAddr, (socklen_t*)&clientAddrLen);
        if (clientFd < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // We are not blocking, so just loop again
                goto loop;
            }
            *EXCEPTION = IOException$make_fromErrno().asThrowable;
            APPEND_STACK(error);
            goto thrown;
        }
        connection = Socket$make_new(clientFd, &clientAddr, clientAddrLen);
    }


    data:

    StreamController_add(controller, connection.asObject);

    goto loop;
    thrown:;
    Throwable _error = *EXCEPTION;
    *EXCEPTION = DOWNCAST(null, Throwable);
    StreamController_addError(controller, _error);
    loop:

    // Recurse again in the next microtask
    return True.asObject;
    done:

    StreamController_close(controller);
    return False.asObject;
}

IMPLEMENT_LAMBDA(OnListen, CAPTURE_MYSELF, NO_OWNED_CAPTURES, ServerSocket myself) {
    Lambda_OnListen self = DOWNCAST(this, Lambda_OnListen);
    ServerSocket myself = self.data->myself;
    int sockfd = myself.data->sockfd;
    THROWS = CRASH_ON_EXCEPTION;
    StreamController controller = va_arg(args, StreamController);
    StreamSubscription subs = va_arg(args, StreamSubscription);

    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        THROW(IOException$make_fromErrno(), null)
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) != 0) {
        THROW(IOException$make_fromErrno(), null)
    }
    int success = listen(sockfd, MAX_CONNECTIONS);
    if (success < 0) {
        THROW(IOException$make_fromErrno(), null)
    }
    Function stepper = Lambda_Step$make_new(myself, controller).asFunction;
    IOCoroutine coro = IOCoroutine$make_new(stepper, sockfd, -1);
    myself.data->coroutine = coro;
    return null;
}

IMPLEMENT_OVERRIDE_METHOD(StreamSubscription, Stream, listen, Function onData, Function onError, Function onDone, Bool cancelOnError) {
    Self self = Stream_as_ServerSocket(this);
    return Stream_listen(StreamController_as_Stream(self.data->streamController), onData, onError, onDone, cancelOnError);
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
    this.data->coroutine = DOWNCAST(null, IOCoroutine);
    this.data->streamController = StreamController$make_new(
            Lambda_OnListen$make_new(this).asFunction,
            Lambda_OnCancel$make_new(this).asFunction
    );
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