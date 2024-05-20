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
#include "HttpRequest.h"
#include "UInt8List.h"
#include "autoclean.h"
#include <threads.h>
#include <netinet/in.h>

#define Super() Object_vtable()
#define Self HttpRequest

IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_HTTP_REQUEST_PARENTS)

ENUMERATE_HTTP_REQUEST_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
List self = DOWNCAST(this, List);
// Delete the array
Super()

->
delete(this);
}

#define CAPTURE_MYSELF(CAPTURE) \
    CAPTURE(HttpRequest, myself)


// returns the index of the first \n that is preceeded by an \r
static int findCrlf(const char *buffer, size_t bufferSize) {
    for (size_t i = 1; i < bufferSize; i++) {
        char at = buffer[i];
        if (at == '\0') {
            break;
        }
        if (at != '\n') {
            continue;
        }
        if (buffer[i - 1] != '\r') {
            continue;
        }
        return (int) i;
    }
    return -1;
}

// returns the index of the ' '
static int findSpace(const char *string, size_t length) {
    for (size_t i = 1; i < length; i++) {
        char at = string[i];
        if (at == '\0') {
            break;
        }
        if (at != ' ') {
            continue;
        }
        return (int) i;
    }
    return -1;
}

IMPLEMENT_SELF_METHOD(HttpRequestReceiveState, processLine, String line, HttpRequestReceiveState state) {
        switch (state) {
        case HttpRequestReceiveState$command:{
            const char *str = String_cStringView(line);
            size_t len = String_length(line);
            int segmentEnd;
            // First, decode the method
            {
                segmentEnd = findSpace(str, len);
                if (segmentEnd == -1 || segmentEnd == 0) {
                    Object_delete(line.asObject);
                    return HttpRequestReceiveState$error;
                }
                char *method = malloc(segmentEnd + 1);
                memcpy(method, str, segmentEnd);
                method[segmentEnd] = '\0';
                // TODO: Validate
                String methodString = String$make_own_len(method, segmentEnd);
                this.data->method = methodString;
            }

            // Advance
            {
                str += segmentEnd;
                len -= segmentEnd;

                if (len == 0) {
                    Object_delete(line.asObject);
                    return HttpRequestReceiveState$error;
                }

                str++;
                len--;
            }

            // Now, we are at the uri
            {
                segmentEnd = findSpace(str, len);
                if (segmentEnd == -1 || segmentEnd == 0) {
                    Object_delete(line.asObject);
                    return HttpRequestReceiveState$error;
                }

                char *uri = malloc(segmentEnd + 1);
                memcpy(uri, str, segmentEnd);
                uri[segmentEnd] = '\0';
                // TODO: Parse
                String uriString = String$make_own_len(uri, segmentEnd);
                this.data->uri = uriString;
            }

            // Advance
            {
                str += segmentEnd;
                len -= segmentEnd;

                if (len == 0) {
                    Object_delete(line.asObject);
                    return HttpRequestReceiveState$error;
                }

                str++;
                len--;
            }
            // Now, we are at the http version part
            {
                // there should be no more spaces
                segmentEnd = findSpace(str, len);
                if (segmentEnd != -1) {
                    Object_delete(line.asObject);
                    return HttpRequestReceiveState$error;
                }
                segmentEnd = (int) (len);
                if (segmentEnd == 0) {
                    Object_delete(line.asObject);
                    return HttpRequestReceiveState$error;
                }

                char *http = malloc(segmentEnd + 1);
                memcpy(http, str, segmentEnd);
                http[segmentEnd] = '\0';
                // TODO: Parse
                String httpString = String$make_own_len(http, segmentEnd);
                this.data->http = httpString;
            }
            // Done!!
            Object_delete(line.asObject);
            return  HttpRequestReceiveState$headers;
        }
        case HttpRequestReceiveState$headers:{
            if (String_length(line) == 0) {
                Object_delete(line.asObject);
                return HttpRequestReceiveState$content;
            }
            HttpHeaders headers = this.data->headers;
            HttpHeaders_decode(headers, line);
            return HttpRequestReceiveState$headers;
        }
        case HttpRequestReceiveState$lineAfterHeaders:{
            return HttpRequestReceiveState$content;
            }
        case HttpRequestReceiveState$content:{
            assert(false);
            return state;
        }
        case HttpRequestReceiveState$error:{
            return state;
        }
    }
}

IMPLEMENT_LAMBDA(HttpRequestOnError, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpRequest myself) {
    Lambda_HttpRequestOnError self = DOWNCAST(this, Lambda_HttpRequestOnError);
    HttpRequest myself = self.data->myself;
    HttpRequestReceiveState state = myself.data->state;

    Throwable error = va_arg(args, Throwable);
    THROWS = va_arg(args, Throwable *);
    switch (state) {
        case HttpRequestReceiveState$content: {
            StreamController bodyController = myself.data->bodyController;
            StreamController_addError(bodyController, error);
        }
        default: {
            Completer completer = myself.data->onReady;
            Completer_completeException(completer, error);
        }
    }
    return null;
}


IMPLEMENT_LAMBDA(HttpRequestOnDone, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpRequest myself) {
    Lambda_HttpRequestOnDone self = DOWNCAST(this, Lambda_HttpRequestOnDone);
    HttpRequest myself = self.data->myself;
    HttpRequestReceiveState state = myself.data->state;
    THROWS = va_arg(args, Throwable *);
    switch (state) {
        case HttpRequestReceiveState$content: {
            StreamController bodyController = myself.data->bodyController;
            StreamController_close(bodyController);
            break;
        }
        default: {
            Completer completer = myself.data->onReady;
            Completer_completeException(completer,
                                        Exception$make_newCString("Connection closed too early").asThrowable);
        }
    }
    return null;
}

const char *stateString(HttpRequestReceiveState state) {
    switch (state) {
        case HttpRequestReceiveState$content:
            return "HttpRequestReceiveState.content";
        case HttpRequestReceiveState$error:
            return "HttpRequestReceiveState.error";
        case HttpRequestReceiveState$command:
            return "HttpRequestReceiveState.command";
        case HttpRequestReceiveState$headers:
            return "HttpRequestReceiveState.headers";
        case HttpRequestReceiveState$lineAfterHeaders:
            return "HttpRequestReceiveState.lineAfterHeaders";
    }
}
IMPLEMENT_SELF_METHOD(void, _addBytes, UInt8List bytes) {
    // todo: fishy that im getting data. i should shutdown the socket
    if (this.data->closedContentStream) {
        return;
    }
    size_t length = UInt8List_length(bytes);
    StreamController bodyController = this.data->bodyController;
    size_t *remainingContentBytes = &this.data->remainingContentBytes;
    size_t wantedBufferLen = *remainingContentBytes > length ? length : *remainingContentBytes;
    if (wantedBufferLen < length) {
        UInt8List list = UInt8List$make_fromBuffer(UInt8List_list(bytes), wantedBufferLen);
        TypedList_release(bytes.asTypedList);
        Object_delete(bytes.asObject);
        bytes = list;
    }
    StreamController_add(bodyController, bytes.asObject);
    (*remainingContentBytes) -= wantedBufferLen;
    if (*remainingContentBytes == 0) {
        StreamController_close(bodyController);
    }
}

IMPLEMENT_LAMBDA(HttpRequestOnData, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpRequest myself) {
    Lambda_HttpRequestOnData self = DOWNCAST(this, Lambda_HttpRequestOnData);
    HttpRequest myself = self.data->myself;
    HttpRequestReceiveState state = myself.data->state;
    List receivedBytes = List$$fromObject(va_arg(args, Object));
    {
        autoclean(String) str = Object_toString(receivedBytes.asObject);
    }
    StreamController bodyController = myself.data->bodyController;
    THROWS = va_arg(args, Throwable *);
    if (state == HttpRequestReceiveState$content) {
        StreamController_add(bodyController, receivedBytes.asObject);
        return null;
    }
    ByteBuffer scratchPad = myself.data->scratchpad;
    ByteBuffer_writeAll(scratchPad, receivedBytes, CRASH_ON_EXCEPTION);
    // We need to process the request by line
    while (ByteBuffer_length(scratchPad) > 1) {
        UInt8List bytes = ByteBuffer_toBytes(scratchPad);
        size_t length = UInt8List_length(bytes);
        const unsigned char *buffer = UInt8List_list(bytes);
        int endOfLine = findCrlf((const char *) buffer, length);
        if (endOfLine == -1) {
            break;
        }
        size_t lineLength = endOfLine + 1;
        char *line = malloc(lineLength + 1);
        ByteBuffer_consumeToBuffer(scratchPad, (unsigned char *) line, lineLength);
        // dont keep the \r and \n
        line[lineLength - 2] = '\0';
        String lineString = String$make_own_len(line, lineLength - 2);
        myself.data->state = state = HttpRequest_processLine(myself, lineString, state);
        if (state == HttpRequestReceiveState$content || state == HttpRequestReceiveState$error) {
            break;
        }
    }
    if (state == HttpRequestReceiveState$content) {
        Completer onReady = myself.data->onReady;
        Completer_complete(onReady, myself.asObject);
        HttpHeaders headers = myself.data->headers;
        Integer contentLength = HttpHeaders_contentLength(headers);
        myself.data->remainingContentBytes = Object_isNull(contentLength.asObject) ? 0 : contentLength.unwrap;
        UInt8List remainingBytes = ByteBuffer_releaseToBytes(scratchPad);
        HttpRequest__addBytes(myself, remainingBytes);
        return null;
    }
    if (state == HttpRequestReceiveState$error) {
        Completer onReady = myself.data->onReady;
        Completer_completeException(onReady, Exception$make_newCString("Invalid http request").asThrowable);
        return null;
    }
    return null;
}
IMPLEMENT_SELF_METHOD(HttpResponse, response) {
    if (!Object_isNull(this.data->response.asObject)) {
        return this.data->response;
    }
    HttpResponse response = HttpResponse$make_fromTcp(this.data->socket);
    this.data->socket = DOWNCAST(null, Socket);
    return this.data->response = response;
}
IMPLEMENT_SELF_METHOD(void, startListening) {
    Socket socket = this.data->socket;
    this.data->socketSubscription = Stream_listen(
            Socket_as_Stream(socket),
            Lambda_HttpRequestOnData$make_new(this).asFunction,
            Lambda_HttpRequestOnError$make_new(this).asFunction,
            Lambda_HttpRequestOnDone$make_new(this).asFunction,
            Bool$box(true)
    );
}

IMPLEMENT_OVERRIDE_METHOD(StreamSubscription, Stream, listen, Function onData, Function onError, Function onDone,
                          Bool cancelOnError) {
    Self self = Stream_as_HttpRequest(this);
    return Stream_listen (StreamController_as_Stream(self.data->bodyController), onData, onError, onDone, cancelOnError);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
    (Object_vtable_t *) vtable,
    (Object_vtable_t *) Super(),
    sizeof(*Super()),
    STR(Self),
    1,
    "Stream", offsetof(HttpRequest_vtable_t, Stream_vtable)
    );
    // HttpRequest
    vtable->processLine = _HttpRequest_processLine_impl;
    vtable->startListening = _HttpRequest_startListening_impl;
    vtable->response = _HttpRequest_response_impl;
    vtable->_addBytes = _HttpRequest__addBytes_impl;
    // Stream
    Stream_vtable_t *stream_vtable = &vtable->Stream_vtable;
    initImplementedInterfaceVtable(
    (Interface_vtable_t *) stream_vtable,
    (Interface_vtable_t *) Stream_vtable(),
    sizeof(*Stream_vtable()),
    offsetof(struct HttpRequest_vtable_t, Stream_vtable)
    );
    stream_vtable->listen = _HttpRequest_listen_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t *) vtable;
    //object_vtable->delete = _List_delete_impl;
}


INTERFACE_CAST_IMPL(HttpRequest, Stream, Object)
OBJECT_CAST_IMPL(Stream, HttpRequest)


IMPLEMENT_LAMBDA(OnListen, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpRequest myself) {
    Lambda_OnListen self = DOWNCAST(this, Lambda_OnListen);
    HttpRequest myself = self.data->myself;
    // do nothing
    fprintf(stderr, "Started reading the body\n");
}

IMPLEMENT_LAMBDA(OnCancelSubscription, CAPTURE_MYSELF, NO_OWNED_CAPTURES, HttpRequest myself) {
    Lambda_OnListen self = DOWNCAST(this, Lambda_OnListen);
    HttpRequest myself = self.data->myself;
    StreamSubscription socketSubscription = myself.data->socketSubscription;
    StreamSubscription_cancel(socketSubscription);
}

IMPLEMENT_CONSTRUCTOR(fromTcp, Socket socket) {
this.data->
onReady = Completer$make_new();
this.data->
state = HttpRequestReceiveState$command;
this.data->
scratchpad = ByteBuffer$make_new();
this.data->
headers = HttpHeaders$make_new();
this.data->
method = DOWNCAST(null, String);
this.data->
http = DOWNCAST(null, String);
this.data->
uri = DOWNCAST(null, String);
this.data->
socket = socket;
this.data->
socketSubscription = DOWNCAST(null, StreamSubscription);
this.data->response = DOWNCAST(null, HttpResponse);
this.data->remainingContentBytes = 0;
this.data->
bodyController = StreamController$make_new(
        Lambda_OnListen$make_new(this).asFunction,
        Lambda_OnCancelSubscription$make_new(this).asFunction
);
HttpRequest_startListening(this);
}

IMPLEMENT_STATIC_METHOD(ServerSocket, bindSync, int domain, const struct sockaddr *addr, socklen_t addrlen, THROWS) {
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

IMPLEMENT_SELF_GETTER(Future, onReady) {
    return Completer_future(this.data->onReady);
}

IMPLEMENT_SELF_GETTER(String, method) {
    return this.data->method;
}

IMPLEMENT_SELF_GETTER(String, http) {
    return this.data->http;
}

IMPLEMENT_SELF_GETTER(String, uri) {
    return this.data->uri;
}

IMPLEMENT_SELF_GETTER(HttpHeaders, headers) {
    return this.data->headers;
}
#undef Super
#undef Self