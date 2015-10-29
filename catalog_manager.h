#ifndef CATALOGMANAGER_H
#define CATALOGMANAGER_H

#include "buffer_manager.h"
#include "query.h"
#include "table.h"

class CatalogManager
{
    BufferManager *buffer_manager_;
public:
    CatalogManager();
    ~CatalogManager();
    void Init(BufferManager *buffer_manager);
    void CreateCatalog(QueryCreateTable *query);
    void UpdateCatalog(TableInfo *table_info);
    void DropCatalog(string table_name);
    void Terminate();
    TableInfo *GetTableInfo(string table_name);
};

#endif // CATALOGMANAGER_H
