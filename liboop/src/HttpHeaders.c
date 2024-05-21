#include "liboop.h"
#include <assert.h>
#include <strings.h>
#include <string.h>

#define Super() Object_vtable()
#define Self HttpHeaders
IMPLEMENT_OPERATOR_NEW()

IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_HTTP_HEADERS_PARENTS)

ENUMERATE_HTTP_HEADERS_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

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

IMPLEMENT_SELF_METHOD(void, decode, String line) {
    List_add(this.data->headers, line.asObject, CRASH_ON_EXCEPTION);
}
IMPLEMENT_SELF_METHOD(void, add, String name, String value) {
    List_add(this.data->headers, String_format_c("{}: {}", name, value).asObject, CRASH_ON_EXCEPTION);
    Object_delete(name.asObject);
    Object_delete(value.asObject);
}
IMPLEMENT_SELF_METHOD(void, encodeTo, Sink sink) {
    foreach(String, line, List_as_Iterable(this.data->headers), {
        Sink_add(sink, line.asObject);
        Sink_add(sink, StringRef$wrap("\r\n").asObject);
    })
}
IMPLEMENT_SELF_METHOD(Integer, contentLength) {
    const char* content_length = "Content-Length: ";
    Integer result = DOWNCAST(null, Integer);
    foreach(String, line, List_as_Iterable(this.data->headers), {
        const char* lineCstring = String_cStringView(line);
        if (strncasecmp(content_length, lineCstring, strlen(content_length)) != 0) {
            continue;
        }
        size_t n = 0;
        sscanf(lineCstring, "Content-Length: %lu", &n);
        result = Integer$box_l(n);
        break;
    })
    return result;
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
        (Object_vtable_t*)vtable,
        (Object_vtable_t*)Super(),
        sizeof(*Super()),
        STR(Self),
        0
        );
    // HttpHeaders
    vtable->contentLength = _HttpHeaders_contentLength_impl;
    vtable->decode = _HttpHeaders_decode_impl;
    vtable->encodeTo = _HttpHeaders_encodeTo_impl;
    vtable->add = _HttpHeaders_add_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t*)vtable;
    //object_vtable->delete = _List_delete_impl;
}

IMPLEMENT_CONSTRUCTOR(new) {
    this.data->headers = List_new();
}


#undef Super
#undef Self