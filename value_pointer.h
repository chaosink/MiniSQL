#ifndef VALUE_POINTER_H
#define VALUE_POINTER_H

#include <cstring>

struct String {
    char content[256];
    String() {
        new (this) String("");
    }
    String(const char *content) {
        memset(this->content, 0, sizeof(this->content));
        strcpy(this->content, content);
    }
    bool operator==(const String &value) {
        return strcmp(this->content, value.content) == 0;
    }
    bool operator<(const String &value) {
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
        return this->num == pointer.num;
    }
};

#endif // VALUE_POINTER_H
