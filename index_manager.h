#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include "buffer_manager.h"
#include "query.h"
#include "table.h"

class IndexManager {
    BufferManager *buffer_manager_;
public:
    IndexManager();
    ~IndexManager();
    void Init(BufferManager *buffer_manager);
    bool CreateIndex(TableInfo *table_info, string &index_name, string &attr_name);
    bool DropIndex(string &index_name);
    bool DropAllIndex(vector<IndexInfo> &index_info);
    string GetIndexTableName(string &index_name);
    void Terminate();
};

#endif // INDEXMANAGER_H
