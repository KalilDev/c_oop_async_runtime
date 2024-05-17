#include "Object.h"
#include "primitive/Integer.h"
#include <stdio.h>
#include "foreach.h"
#include "downcast.h"
#include "autoclean.h"
#include "String.h"
#include "primitive/StringRef.h"
#include "primitive/Double.h"
#include "primitive/Bool.h"
#include "src/oop.h"
#include "EventLoop.h"
#include "Future.h"
#include "ServerSocket.h"
#include "Completer.h"
#include <assert.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#define WITH_OOP_MAIN
#include "main.h"
#include "Socket.h"


#define ENUMERATE_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)

IMPLEMENT_LAMBDA(OnData, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Object data = va_arg(args, Object);
    THROWS = va_arg(args, Throwable*);

    Socket connection = Socket$$fromObject(data);

    connection


    return null;
}
IMPLEMENT_LAMBDA(OnError, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Completer completer = DOWNCAST(this, Lambda_OnError).data->completer;
    Throwable error = va_arg(args, Throwable);
    THROWS = va_arg(args, Throwable*);
    fprintf(stderr, "error arrived\n");
    Completer_completeException(completer, error);
    return null;
}
IMPLEMENT_LAMBDA(OnDone, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Completer completer = DOWNCAST(this, Lambda_OnDone).data->completer;
    THROWS = va_arg(args, Throwable*);
    printf("done arrived\n");
    Completer_complete(completer, Integer$box(0).asObject);
    return null;
}


#define ENUMERATE_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer) \
    CAPTURE(ServerSocket, server)

IMPLEMENT_LAMBDA(JoinThreadAsync, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer, ServerSocket server) {
    ServerSocket server = DOWNCAST(this, Lambda_JoinThreadAsync).data->server;
    THROWS = va_arg(args, Throwable*);
    if (Object_isNull(server.data->subs.asObject)) {
        Completer completer = DOWNCAST(this, Lambda_JoinThreadAsync).data->completer;
        return Completer_future(completer).asObject;
    }
    thrd_sleep(&(struct timespec){.tv_nsec=16*1000000}, NULL); // sleep 16 msec
    thrd_yield();
    return Future_computation(this).asObject;
}


Object Main(List arguments, THROWS) {
    struct sockaddr_in addr;
    addr.sin_addr.s_addr = inet_addr("127.0.0.1");
    addr.sin_port = 3000;
    addr.sin_family = AF_INET;
    ServerSocket server = ServerSocket_bindSync(
    AF_INET,
    (const struct sockaddr*)&addr,
    sizeof(addr),
    EXCEPTION
    );
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    Completer completer = Completer$make_new();
    Stream_listen(
            ServerSocket_as_Stream(server),
            Lambda_OnData$make_new(completer).asFunction,
            Lambda_OnError$make_new(completer).asFunction,
            Lambda_OnDone$make_new(completer).asFunction,
            Bool$box(true)
    ).asObject;
    printf("Listening on 127.0.0.1:3000...\n");
    return null;
}