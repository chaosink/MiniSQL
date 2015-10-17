#ifndef DBMS_H
#define DBMS_H

#include <string>
#include "exception.h"
#include "interpreter.h"
#include "api.h"
#include "record_manager.h"
#include "index_manager.h"
#include "catalog_manager.h"
#include "buffer_manager.h"
using namespace std;



class DBMS
{
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
    void PrintDBMSInformation();
    void Print(string information);
};

#endif // DBMS_H
