#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include "buffer_manager.h"
#include "query.h"
#include "table.h"
#include "value_pointer.h"

struct IndexInfo {
    string index_name;
    string table_name;
    string attribute_name;
    int type;
    int char_length;
    int root;
    int block_num;
    int empty_block_num;
};

class IndexManager {
    BufferManager *buffer_manager_;
public:
    IndexManager();
    ~IndexManager();
    void Init(BufferManager *buffer_manager);
    IndexInfo *GetIndexInfo(string index_name);
    void UpdateIndexInfo(IndexInfo *index_info);
    bool CreateIndex(TableInfo *table_info, string &index_name, string &attr_name);
    vector<Pointer> FindIndex(vector<WhereIndex> &where_index);
    void Insert(string index_name, string attr_value, Pointer pointer);
    void InsertIndex(TableInfo *table_info, vector<string> &attr_value, Pointer pointer);
    void InsertAllIndex(TableInfo *table_info, string index_name);
    void Delete(string index_name, string attr_value);
    void DeleteIndex(TableInfo *table_info, vector<vector<string> > &record);
    bool DropIndex(string &index_name);
    bool DropAllIndex(vector<Index> &index);
    string GetIndexTableName(string &index_name);
    void Terminate();
};

#endif // INDEXMANAGER_H
