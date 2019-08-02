#ifndef RECORDMANAGER_H
#define RECORDMANAGER_H

#include "buffer_manager.hpp"
#include "value_pointer.hpp"
#include "result.hpp"
#include "table.hpp"
#include "query.hpp"

class RecordManager {
    BufferManager *buffer_manager_;
    void AddOneBlock(string table_name);
    void WriteRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address);
    void ReadRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address);
public:
    RecordManager();
    ~RecordManager();
    void Init(BufferManager *buffer_manager);
    bool CreateTable(string table_name);
    bool DropTable(string table_name);
    Pointer InsertRecord(TableInfo *table_info, QueryInsert *query);
    void SelectRecord(TableInfo *table_info, vector<Where> &where, ResultSelect *result);
    void SelectRecordWithPointer(TableInfo *table_info, vector<Pointer> &pointer, vector<Where> &where_nonindex, ResultSelect *result);
    int DeleteRecord(TableInfo *table_info, QueryDelete *query, vector<vector<string> > &record);
    int DeleteRecordWithPointer(TableInfo *table_info, vector<Pointer> &pointer, vector<Where> &where_nonindex, vector<vector<string> > &record);
    void Terminate();
};

#endif // RECORDMANAGER_H
