#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include "buffer_manager.h"
#include "value_pointer.h"
#include "table.h"
#include "query.h"
#include <string>
#include <map>
using namespace std;

struct IndexInfo {
    string index_name;
    string table_name;
    string attribute_name;
    int type;
    int char_length;
    int root;
    int block_num;
    int empty_block_num;
    IndexInfo() {
        index_name = "";
    }
};

struct WhereIndex : Where{
    string index_name;
    WhereIndex(string &index_name, Where &where) {
        this->index_name = index_name;
        this->attribute_name = where.attribute_name;
        this->comparison = where.comparison;
        this->attribute_value = where.attribute_value;
    }
};

class IndexManager {
    BufferManager *buffer_manager_;
    map<string, IndexInfo> index_info_;
    void Insert(string index_name, string attr_value, Pointer pointer);
    void Delete(string index_name, string attr_value);
public:
    IndexManager();
    ~IndexManager();
    void Init(BufferManager *buffer_manager);
    bool CreateIndex(TableInfo *table_info, string &index_name, string &attr_name);
    IndexInfo *GetIndexInfo(string index_name);
    void UpdateIndexInfo(IndexInfo *index_info);
    vector<Pointer> FindIndex(vector<WhereIndex> &where_index);
    void InsertIndex(TableInfo *table_info, vector<string> &attr_value, Pointer pointer);
    void InsertAllIndex(TableInfo *table_info, string index_name);
    void DeleteIndex(TableInfo *table_info, vector<vector<string> > &record);
    bool DropIndex(string &index_name);
    bool DropAllIndex(vector<Index> &index);
    void Terminate();
};

#endif // INDEXMANAGER_H
