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
#include "HttpRequest.h"
#include "HttpServer.h"
#include "UInt8List.h"
#include "StringBuffer.h"


#define ENUMERATE_CAPTURES(CAPTURE) \
    CAPTURE(Completer, completer)

String buildResponse() {
    StringBuffer buffer = StringBuffer$make_new();
#define Ln(line) StringBuffer_writeLn(buffer, StringRef$wrap(line).asObject);
    Ln("<!DOCTYPE html>");
    Ln("<html lang=\"en\">");
    Ln("  <head>");
    Ln("    <meta charset=\"ASCII\">");
    Ln("    <meta name=\"viewport\" content=\"width=device-width, initial-scale=1.0\">");
    Ln("    <meta http-equiv=\"X-UA-Compatible\" content=\"ie=edge\">")
    Ln("    <title>Hello</title>");
    //Ln("    <link rel=\"stylesheet\" href=\"style.css\">");
    Ln("  </head>");
    Ln("  <body>");
    Ln("    <h1>Hello</h1>");
    Ln("  </body>");
    Ln("</html>");
#undef Ln
    String res = StringBuffer_releaseToString(buffer);
    Object_delete(buffer.asObject);
    return res;
}

IMPLEMENT_LAMBDA(OnData, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    HttpRequest request = va_arg(args, HttpRequest);
    THROWS = va_arg(args, Throwable*);
    fprintf(stdout, "Request!\n");
    fprintf(stdout, "Method: %s\n", String_cStringView(HttpRequest_method(request)));
    fprintf(stdout, "Uri: %s\n", String_cStringView(HttpRequest_uri(request)));
    fprintf(stdout, "Http: %s\n", String_cStringView(HttpRequest_http(request)));


    String responseBody = buildResponse();
    Integer contentLength = Integer$box_l((long)String_length(responseBody));

    HttpResponse response = HttpRequest_response(request);
    Sink requestSink = HttpResponse_as_Sink(response);

    HttpResponse_setStatusCode(response, Integer$box_l(200));
    HttpResponse_setHttp(response, StringRef$wrap("HTTP/1.1").asString);
    HttpResponse_setStatusReason(response, StringRef$wrap("Okay!!!").asString);

    HttpHeaders responseHeaders = HttpResponse_headers(response);
    HttpHeaders_add(responseHeaders, StringRef$wrap("Date").asString, StringRef$wrap("Sun, 01 Oct 2000 23:25:17 GMT").asString);
    HttpHeaders_add(responseHeaders, StringRef$wrap("Server").asString, StringRef$wrap("Poor man's dart HttpServer").asString);
    HttpHeaders_add(responseHeaders, StringRef$wrap("Last-modified").asString, StringRef$wrap("Tue, 04 Jul 2000 09:46:21 GMT").asString);
    HttpHeaders_add(responseHeaders, StringRef$wrap("Content-length").asString, Object_toString(contentLength.asObject));
    HttpHeaders_add(responseHeaders, StringRef$wrap("Content-type").asString, StringRef$wrap("text/html").asString);

    HttpResponse_send(response);

    Sink_add(requestSink, responseBody.asObject);
    Sink_close(requestSink);


    printf("Sent response!\n");
    Object_delete(request.asObject);
    return null;
}
IMPLEMENT_LAMBDA(OnError, ENUMERATE_CAPTURES, NO_OWNED_CAPTURES, Completer completer) {
    Completer completer = DOWNCAST(this, Lambda_OnError).data->completer;
    Throwable error = va_arg(args, Throwable);
    THROWS = va_arg(args, Throwable*);
    autoclean(String) errorStr = Object_toString(error.asObject);
    fprintf(stderr, "error arrived %s\n", String_cStringView(errorStr));
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



Object Main(List arguments, THROWS) {
    HttpServer server = HttpServer_bindSync(
            StringRef$wrap("0.0.0.0").asString,
            3000,
            EXCEPTION
    );
    if (HAS_EXCEPTION) {
        RETHROW(null)
    }
    Completer completer = Completer$make_new();
    Stream_listen(
            HttpServer_as_Stream(server),
            Lambda_OnData$make_new(completer).asFunction,
            Lambda_OnError$make_new(completer).asFunction,
            Lambda_OnDone$make_new(completer).asFunction,
            Bool$box(true)
    ).asObject;
    printf("Listening on 127.0.0.1:3000...\n");
    return null;
}