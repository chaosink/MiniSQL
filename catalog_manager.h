#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include "buffer_manager.h"
#include "table.h"
#include "query.h"

class CatalogManager {
    BufferManager *buffer_manager_;
public:
    CatalogManager();
    ~CatalogManager();
    void Init(BufferManager *buffer_manager);
    void CreateCatalog(QueryCreateTable *query);
    TableInfo *GetTableInfo(string table_name);
    void UpdateCatalog(TableInfo *table_info);
    void DropCatalog(string table_name);
    void DropIndex(string &table_name, string &index_name);
    void Terminate();
};

#endif // CATALOGMANAGER_H
