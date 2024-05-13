#include "Object.h"
#include "RandomAccessFile.h"
#include "oop.h"
#include "stddef.h"
#include "oop.h"
#include "string.h"
#include "StringBuffer.h"
#include <strings.h>
#include <assert.h>
#include "String.h"
#include "primitive/Integer.h"
#include "List.h"
#include "ByteBuffer.h"
#include "UInt8List.h"
#include "Exception.h"
#include "IOException.h"
#include "primitive/StringRef.h"
#include <Closeable.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/stat.h>
#include <errno.h>

#define Super() Object_vtable()
#define Self RandomAccessFile

IMPLEMENT_OPERATOR_NEW()
IMPLEMENT_SELF_DOWNCASTS(ENUMERATE_RANDOM_ACCESS_FILE_PARENTS)
ENUMERATE_RANDOM_ACCESS_FILE_METHODS(IMPLEMENT_SELF_VIRTUAL_METHOD)

IMPLEMENT_SELF_METHOD(void, flush, THROWS) {
    int res = fflush(this.data->file);
    if (res == EOF) {
        THROW(IOException$make_fromErrno())
    }
}

IMPLEMENT_SELF_METHOD(size_t, length, THROWS)           {
    struct stat st;
    int fd = fileno(this.data->file);
    if (fd < 0) {
        THROW(IOException$make_fromErrno(), 0);
    }
    int res = fstat (fd, &st);
    if (res != 0) {
        THROW(IOException$make_fromErrno(), 0);
    }
    size_t size = st.st_size;
    return size;
}

IMPLEMENT_SELF_METHOD(void, lock)           {
    flockfile(this.data->file);
}
IMPLEMENT_SELF_METHOD(bool, tryLock)           {
    return ftrylockfile(this.data->file) == 0;
}
IMPLEMENT_SELF_METHOD(size_t, position, THROWS)     {
    long res = ftell(this.data->file);
    if (res < 0) {
        THROW(IOException$make_fromErrno(), 0);
    }
    return res;
}
IMPLEMENT_SELF_METHOD(Integer, readByte, THROWS)     {
    int c = fgetc(this.data->file);
    int error = ferror(this.data->file);
    if (error != 0) {
        // we already dealt with the error
        clearerr(this.data->file);
        THROW(IOException$make_new(error), DOWNCAST(null, Integer));
    }
    if(feof(this.data->file)) {
        return DOWNCAST(null, Integer);
    }
    return Integer$box(c);
}
IMPLEMENT_SELF_METHOD(size_t, readInto, List buffer, size_t start, Integer end, THROWS)     {
    #define CHUNK 512
    size_t bufferLen = List_length(buffer);
    size_t end_ = Object_isNull(end.asObject) ? bufferLen : end.unwrap;
    size_t bufferWindowSize = end_ - start;
    size_t remaining = bufferWindowSize;
    size_t readingChunkSize = CHUNK;
    size_t read = 0;
    char chunk[CHUNK];
    do {
        readingChunkSize = readingChunkSize > remaining ? remaining : readingChunkSize;
        size_t currentRead = fread(chunk, sizeof(char), readingChunkSize, this.data->file);
        // uint8list fast path!
        if (buffer.vtable == UInt8List_vtable()) {
            unsigned char *bb = UInt8List_list(DOWNCAST(buffer, UInt8List));
            memcpy( bb + start + read, chunk, currentRead);
        } else {
            // bad use of setLength!
            List_setLength(buffer, bufferLen + currentRead, EXCEPTION);
            if (HAS_EXCEPTION) {
                RETHROW(read)
            }
            for (size_t ci = 0; ci<currentRead; ci++) {
                Integer boxed = Integer$box_c(chunk[ci]);
                List_setAt(buffer, start + ci + read, UPCAST(boxed, Object));
            }
        }

        read += currentRead;
        bufferLen += currentRead;
        remaining -= currentRead;

        int error = ferror(this.data->file);
        if (error != 0) {
            // we already dealt with the error
            clearerr(this.data->file);
            THROW(IOException$make_new(error), read);
        }
        if (currentRead != readingChunkSize) {
            return read;
        }
    } while (remaining > 0);
    return read;
    #undef CHUNK
}
IMPLEMENT_SELF_METHOD(List, read, size_t count, THROWS)          {
    #define CHUNK 512
    ByteBuffer buffer = ByteBuffer$make_new();
    ByteBuffer_ensure(buffer, count, EXCEPTION);
    if (HAS_EXCEPTION) {
        RETHROW(DOWNCAST(null, List))
    }
    size_t i = 0;

    size_t remaining = count;
    size_t readingChunkSize = CHUNK;
    unsigned char chunk[CHUNK];
    do {
        readingChunkSize = readingChunkSize >= remaining ? readingChunkSize : remaining;
        size_t currentRead = fread(chunk, sizeof(unsigned char), readingChunkSize, this.data->file);
        ByteBuffer_writeBuffer(buffer, chunk, currentRead, EXCEPTION);
        if (HAS_EXCEPTION) {
            RETHROW(DOWNCAST(null, List))
        }
        remaining -= currentRead;

        int error = ferror(this.data->file);
        if (error != 0) {
            // we already dealt with the error
            clearerr(this.data->file);
            Object_delete(buffer.asObject);
            THROW(IOException$make_new(error), DOWNCAST(null, List));
        }
        if (currentRead != readingChunkSize) {
            List out = ByteBuffer_releaseToBytes(buffer).asList;
            Object_delete(buffer.asObject);
            return out;
        }
    } while (remaining > 0);
    List out = ByteBuffer_releaseToBytes(buffer).asList;
    Object_delete(buffer.asObject);
    return out;
    #undef CHUNK
}
IMPLEMENT_SELF_METHOD(String, readString, size_t count, THROWS)          {
    #define CHUNK 512
    StringBuffer buffer = StringBuffer$make_new();
    StringBuffer_ensure(buffer, count);
    size_t remaining = count;
    size_t readingChunkSize = CHUNK;
    char chunk[CHUNK];
    do {
        readingChunkSize = remaining < readingChunkSize ? remaining : readingChunkSize;
        size_t currentRead = fread(chunk, sizeof(char), readingChunkSize, this.data->file);
        StringBuffer_writeBuffer(buffer, chunk, currentRead);
        remaining -= currentRead;
        int error = ferror(this.data->file);
        if (error != 0) {
            // we already dealt with the error
            clearerr(this.data->file);
            Object_delete(buffer.asObject);
            THROW(IOException$make_new(error), DOWNCAST(null, String));
        }
        if (currentRead != readingChunkSize) {
            String out = StringBuffer_releaseToString(buffer);
            Object_delete (buffer.asObject);
            return out;
        }
    } while (remaining > 0);
    String out = StringBuffer_releaseToString(buffer);
    Object_delete (buffer.asObject);
    return out;
    #undef CHUNK
}
IMPLEMENT_SELF_METHOD(void, setPosition, size_t position, THROWS) {
    int res = fseek(this.data->file, (long)position, SEEK_SET);
    if (res < 0) {
        THROW(IOException$make_fromErrno());
    }
}
IMPLEMENT_SELF_METHOD(void, truncate, size_t length, THROWS)           {
    int fd = fileno(this.data->file);
    if (fd < 0) {
        THROW(IOException$make_fromErrno());
    }
    int res = ftruncate (fd, (long)length);
    if (res < 0) {
        THROW(IOException$make_fromErrno());
    }
}
IMPLEMENT_SELF_METHOD(void, unlock)           {
    funlockfile(this.data->file);
}
IMPLEMENT_SELF_METHOD(void, writeByte, char byte, THROWS)           {
    int written = fputc(byte, this.data->file);
    if (written == EOF) {
        THROW(IOException$make_fromErrno());
    }
}
IMPLEMENT_SELF_METHOD(void, writeFrom, List buffer, size_t start, Integer end, THROWS)         {
    #define CHUNK 512
    size_t bufferLen = List_length(buffer);
    size_t end_ = Object_isNull(UPCAST(end, Object)) ? bufferLen : Integer_unbox_ll(end);
    size_t bufferWindow = end_ - start;
    size_t remaining = bufferWindow;
    size_t writingChunkSize = CHUNK;
    char chunk[CHUNK];
    while (remaining > 0) {
        writingChunkSize = writingChunkSize >= remaining ? writingChunkSize : remaining;
        // uint8list fast path!
        if (buffer.vtable == UInt8List_vtable()) {
            unsigned char *bb = UInt8List_list(DOWNCAST(buffer, UInt8List));
            memcpy(chunk, bb + start, writingChunkSize);
        } else {
            for (size_t ci = 0; ci<writingChunkSize; ci++) {
                Object at = List_at(buffer, ci + start);
                Integer boxed = DOWNCAST(at, Integer);
                chunk[ci] = Integer_unbox_c(boxed);
            }
        }
        start += writingChunkSize;
        size_t currentWritten = fwrite(chunk, sizeof(char), writingChunkSize, this.data->file);

        int error = ferror(this.data->file);
        if (error != 0) {
            // we already dealt with the error
            clearerr(this.data->file);
            THROW(IOException$make_new(error));
        }
        remaining -= currentWritten;
        if (currentWritten != writingChunkSize) {
            return;
        }
    }
    #undef CHUNK
}
IMPLEMENT_SELF_METHOD(void, writeString, String string, THROWS) {
    #define CHUNK 512
    const char *str = String_cStringView(string);
    size_t start = 0;
    size_t strLen = String_length(string);
    size_t remaining = strLen;
    size_t writingChunkSize = CHUNK;
    char chunk[CHUNK];
    while (remaining > 0) {
        writingChunkSize = writingChunkSize >= remaining ? writingChunkSize : remaining;
        memcpy(chunk, str + start, writingChunkSize);
        start += writingChunkSize;
        size_t currentWritten = fwrite(chunk, sizeof(char), writingChunkSize, this.data->file);
        int error = ferror(this.data->file);
        if (error != 0) {
            // we already dealt with the error
            clearerr(this.data->file);
            THROW(IOException$make_new(error));
        }
        remaining -= currentWritten;
        if (currentWritten != writingChunkSize) {
            return;
        }
    }
    #undef CHUNK
}
IMPLEMENT_OVERRIDE_METHOD(void, Closeable, close) {
    RandomAccessFile self = Closeable_as_RandomAccessFile(this);
    fclose(self.data->file);
    self.data->file = NULL;
}

IMPLEMENT_OVERRIDE_METHOD(void, Object, delete) {
    RandomAccessFile self = DOWNCAST(this, RandomAccessFile);
    if (self.data->file != NULL) {
        fclose(self.data->file);
        self.data->file = NULL;
    }
    Object_delete(self.data->path.asObject);
    Super()->delete(this);
}

IMPLEMENT_SELF_VTABLE() {
    initVtable(
            (Object_vtable_t *) vtable,
            (Object_vtable_t *) Super(),
            sizeof(*Super()),
            STR(Self),
            1,
            "Closeable", offsetof(RandomAccessFile_vtable_t, Closeable_vtable));
    // RandomAccessFile
    vtable->flush = _RandomAccessFile_flush_impl;
    vtable->length = _RandomAccessFile_length_impl;
    vtable->lock = _RandomAccessFile_lock_impl;
    vtable->tryLock = _RandomAccessFile_tryLock_impl;
    vtable->position = _RandomAccessFile_position_impl;
    vtable->readByte = _RandomAccessFile_readByte_impl;
    vtable->readInto = _RandomAccessFile_readInto_impl;
    vtable->read = _RandomAccessFile_read_impl;
    vtable->readString = _RandomAccessFile_readString_impl;
    vtable->setPosition = _RandomAccessFile_setPosition_impl;
    vtable->truncate = _RandomAccessFile_truncate_impl;
    vtable->unlock = _RandomAccessFile_unlock_impl;
    vtable->writeByte = _RandomAccessFile_writeByte_impl;
    vtable->writeFrom = _RandomAccessFile_writeFrom_impl;
    vtable->writeString = _RandomAccessFile_writeString_impl;
    // Closeable
    Closeable_vtable_t *closeable_vtable = &vtable->Closeable_vtable;
    initImplementedInterfaceVtable(
            (Interface_vtable_t *) closeable_vtable,
            (Interface_vtable_t *) Iterable_vtable(),
            sizeof(*Iterable_vtable()),
            offsetof(RandomAccessFile_vtable_t, Closeable_vtable)
    );
    closeable_vtable->close = _RandomAccessFile_close_impl;
    // Object
    Object_vtable_t *object_vtable = (Object_vtable_t *) vtable;
    object_vtable->delete = _RandomAccessFile_delete_impl;
}

OBJECT_CAST_IMPL(Closeable, RandomAccessFile)

INTERFACE_CAST_IMPL(RandomAccessFile, Closeable, Object)


RandomAccessFile RandomAccessFile$make_open (String path, const char *modes, THROWS) {
    RandomAccessFile this = RandomAccessFile_operator_new();
    if (Object_isNull(this.asObject)) { return this; }
    FILE* file = this.data->file = fopen(String_cStringView(path), modes);
    if (file == NULL) {
        this.data->path = DOWNCAST(null, String);
        Object_delete(this.asObject);
        THROW(IOException$make_fromErrno(), DOWNCAST(null, RandomAccessFile))
    }
    this.data->path = Object_toString(path.asObject);
    return this;
}

IMPLEMENT_SELF_GETTER(String, path) {
    return this.data->path;
}

IMPLEMENT_SELF_GETTER(FILE *, unwrap) {
    return this.data->file;
}

#undef Super
#undef Self