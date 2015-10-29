#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include "buffer_manager.h"
#include "query.h"
#include "table.h"

class RecordManager
{
    BufferManager *buffer_manager_;
public:
    RecordManager();
    ~RecordManager();
    void Init(BufferManager *buffer_manager);
    void Terminate();
    void CreateTable(string table_name);
    void DropTable(string table_name);
    void InsertRecord(TableInfo *table_info, QueryInsert *query);
    void SelectRecord(TableInfo *table_info, QuerySelect *query);
    void AddOneBlock(string table_name);
    void WriteRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address);
    void ReadRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address);
    void DeleteRecord();
};

#endif // RECORDMANAGER_H
