#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include "buffer_manager.h"
#include "query.h"
#include "table.h"
#include "result.h"
#include "value_pointer.h"

class RecordManager {
    BufferManager *buffer_manager_;
public:
    RecordManager();
    ~RecordManager();
    void Init(BufferManager *buffer_manager);
    void Terminate();
    bool CreateTable(string table_name);
    bool DropTable(string table_name);
    Pointer InsertRecord(TableInfo *table_info, QueryInsert *query);
    void SelectRecord(TableInfo *table_info, QuerySelect *query, ResultSelect *result);
    int DeleteRecord(TableInfo *table_info, QueryDelete *query);
    void AddOneBlock(string table_name);
    void WriteRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address);
    void ReadRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address);
};

#endif // RECORDMANAGER_H
