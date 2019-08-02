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
    bool operator<(const Pointer &pointer) {
        return this->num < pointer.num; //|| (this->num == pointer.num && this->offset < pointer.offset);
    }
};

#endif // VALUE_POINTER_H
