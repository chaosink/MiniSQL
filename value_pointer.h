#ifndef VALUE_POINTER_H
#define VALUE_POINTER_H

#include <cstring>

struct String256 {
    char content[256];
    String256() {
        new (this) String256("");
    }
    String256(const char *content) {
        memset(this->content, 0, sizeof(this->content));
        strcpy(this->content, content);
    }
    bool operator==(const String256 &value) {
        return strcmp(this->content, value.content) == 0;
    }
    bool operator<(const String256 &value) {
        return strcmp(this->content, value.content) < 0;
    }
};

struct String128 {
    char content[128];
    String128() {
        new (this) String128("");
    }
    String128(const char *content) {
        memset(this->content, 0, sizeof(this->content));
        strcpy(this->content, content);
    }
    bool operator==(const String128 &value) {
        return strcmp(this->content, value.content) == 0;
    }
    bool operator<(const String128 &value) {
        return strcmp(this->content, value.content) < 0;
    }
};

struct String64 {
    char content[64];
    String64() {
        new (this) String64("");
    }
    String64(const char *content) {
        memset(this->content, 0, sizeof(this->content));
        strcpy(this->content, content);
    }
    bool operator==(const String64 &value) {
        return strcmp(this->content, value.content) == 0;
    }
    bool operator<(const String64 &value) {
        return strcmp(this->content, value.content) < 0;
    }
};

struct String32 {
    char content[32];
    String32() {
        new (this) String32("");
    }
    String32(const char *content) {
        memset(this->content, 0, sizeof(this->content));
        strcpy(this->content, content);
    }
    bool operator==(const String32 &value) {
        return strcmp(this->content, value.content) == 0;
    }
    bool operator<(const String32 &value) {
        return strcmp(this->content, value.content) < 0;
    }
};

struct String16 {
    char content[16];
    String16() {
        new (this) String16("");
    }
    String16(const char *content) {
        memset(this->content, 0, sizeof(this->content));
        strcpy(this->content, content);
    }
    bool operator==(const String16 &value) {
        return strcmp(this->content, value.content) == 0;
    }
    bool operator<(const String16 &value) {
        return strcmp(this->content, value.content) < 0;
    }
};

struct String8 {
    char content[8];
    String8() {
        new (this) String8("");
    }
    String8(const char *content) {
        memset(this->content, 0, sizeof(this->content));
        strcpy(this->content, content);
    }
    bool operator==(const String8 &value) {
        return strcmp(this->content, value.content) == 0;
    }
    bool operator<(const String8 &value) {
        return strcmp(this->content, value.content) < 0;
    }
};

struct Pointer {
    int num;
    int offset;
    Pointer() {
        new (this) Pointer(-1, 0);
    }
    Pointer(short int num, short int offset) {
        this->num = num;
        this->offset = offset;
    }
    bool operator==(const Pointer &pointer) {
        return this->num == pointer.num && this->offset == pointer.offset;
    }
};

#endif // VALUE_POINTER_H
