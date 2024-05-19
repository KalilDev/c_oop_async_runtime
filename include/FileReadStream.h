#ifndef FileReadStream
#define FileReadStream FileReadStream

#include "Object.h"
#include "oop.h"
#include "Stream.h"
#include "File.h"
#include "StreamSubscription.h"
#include "StreamController.h"
#include "IOCoroutine.h"
#include <stdlib.h>
#include <stdbool.h>
#include <stddef.h>
#include <sys/socket.h>
#include <threads.h>

#define WITH_RTTI

#include "rtti.h"

#define Super Object
#define Self FileReadStream

START_CLASS

#define ENUMERATE_FILE_READ_STREAM_PARENTS(PARENT) \
    ENUMERATE_OBJECT_PARENTS(PARENT)                                     \
    PARENT(Object)

FORWARD_DECL_CLASS(String)
FORWARD_DECL_THROWABLE

#define PARAMS_INVOCATION_FileReadStream$new file
#define ENUMERATE_FILE_READ_STREAM_CONSTRUCTORS(CONSTRUCTOR) \
    CONSTRUCTOR(new, File file)

#define ENUMERATE_FILE_READ_STREAM_IMPLEMENTS(IMPLEMENTS) \
    IMPLEMENTS(Stream)

#define ENUMERATE_FILE_READ_STREAM_ATTRIBUTES(ATTRIBUTE) \
    ATTRIBUTE(int, fd)                                        \
    ATTRIBUTE(File, file)                                        \
    ATTRIBUTE(StreamController, streamController)                 \
    ATTRIBUTE(IOCoroutine, coroutine)

DEFINE_SELF_CLASS(
        ENUMERATE_FILE_READ_STREAM_PARENTS,
        ENUMERATE_FILE_READ_STREAM_IMPLEMENTS,
        NO_METHODS,
        ENUMERATE_FILE_READ_STREAM_ATTRIBUTES,
        ENUMERATE_FILE_READ_STREAM_CONSTRUCTORS,
        NO_STATIC_METHODS,
        NO_STATIC_ATTRIBUTES,
        NO_GETTERS
)

DECLARE_OBJECT_CAST(Stream, FileReadStream)
DECLARE_INTERFACE_CAST(FileReadStream, Stream)

END_CLASS

#undef Self
#undef Super

#endif