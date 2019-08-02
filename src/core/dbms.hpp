#ifndef DBMS_H
#define DBMS_H

#include "interpreter.hpp"
#include "api.hpp"
#include "record_manager.hpp"
#include "index_manager.hpp"
#include "catalog_manager.hpp"
#include "buffer_manager.hpp"

#define _DBMS_INFORMATION "Welcome to MiniSQL!\nVersion: 0.1\n\n"

class DBMS {
    const char *DBMS_INFORMATION;
    Interpreter interpreter_;
    API api_;
    RecordManager record_manager_;
    IndexManager index_manager_;
    CatalogManager catalog_manager_;
    BufferManager buffer_manager_;
public:
    DBMS();
    ~DBMS();
    void Init();
    void Run();
    void Terminate();
};

#endif // DBMS_H
