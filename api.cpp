#include <iostream>
#include <cstdio>
#include "api.h"
#include "record_manager.h"
#include "index_manager.h"
#include "catalog_manager.h"
#include "query.h"
#include "table.h"
using namespace std;

API::API()
{

}

API::~API()
{

}

void API::Init(RecordManager *record_manager, IndexManager *index_manager, CatalogManager *catalog_manager)
{
    record_manager_ = record_manager;
    index_manager_ = index_manager;
    catalog_manager_ = catalog_manager;
}

void API::ProcessQuery(Query *query)
{
    if(!query) return;
    switch(query->command) {
        case CREATE_TABLE: {
            QueryCreateTable *q = (QueryCreateTable *)query;
            record_manager_->CreateTable(q->table_name);
            catalog_manager_->CreateCatalog(q);
            break;
        }
        case DROP_TABLE: {
            QueryDropTable *q = (QueryDropTable *)query;
            record_manager_->DropTable(q->table_name);
            catalog_manager_->DropCatalog(q->table_name);
            break;
        }
        case CREATE_INDEX: {
            QueryCreateIndex *q = (QueryCreateIndex *)query;

            break;
        }
        case DROP_INDEX: {
            QueryDropIndex *q = (QueryDropIndex *)query;

            break;
        }
        case SELECT: {
            QuerySelect *q = (QuerySelect *)query;
            TableInfo *table_info = catalog_manager_->GetTalbeInfo(q->table_name);
            record_manager_->SelectRecord(table_info, q);
            break;
        }
        case INSERT: {
            QueryInsert *q = (QueryInsert *)query;
            TableInfo *table_info = catalog_manager_->GetTalbeInfo(q->table_name);
            record_manager_->InsertRecord(table_info, q);
            catalog_manager_->UpdateCatalog(table_info);
            break;
        }
        case DELETE: {
            QueryDelete *q = (QueryDelete *)query;
            break;
        }
    }
}

void API::Terminate()
{

}
