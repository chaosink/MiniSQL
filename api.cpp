#include <iostream>
#include <cstdio>
#include "api.h"
#include "record_manager.h"
#include "index_manager.h"
#include "catalog_manager.h"
#include "query.h"
#include "table.h"
#include "result.h"
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

bool VerifyType(AttributeType type, string &attr_value) {
    switch(type) {
        case CHAR:
            if(attr_value[0] != '\'') return false;
            break;
        case INT:
            if(attr_value[0] == '\'' || attr_value.find_first_of('.') != -1) return false;
            break;
        case FLOAT:
            if(attr_value[0] == '\'') return false;
            break;
    }
    return true;
}

bool VerifyRecordType(vector<AttributeInfo> &attribute_info, vector<string> &attr_value) {
    for(int i = 0; i < attribute_info.size(); i++)
        if(!VerifyType(attribute_info[i].type, attr_value[i])) return false;
    return true;
}

bool VerifyWhereType(vector<AttributeInfo> &attribute_info, vector<Where> &where) {
    for(int i = 0; i < where.size(); i++) {
        int j;
        for(j = 0; j < attribute_info.size(); j++)
            if(attribute_info[j].name == where[i].attribute_name) {
                if(!VerifyType(attribute_info[j].type, where[i].attribute_value)) return false;
                break;
            }
        if(j == attribute_info.size()) return false;
    }
    return true;
}

Result *API::ProcessQuery(Query *query)
{
    if(!query) return NULL;
    switch(query->type) {
        case CREATE_TABLE: {
            ResultCreateTable *result = new ResultCreateTable;
            QueryCreateTable *q = (QueryCreateTable *)query;
            if(record_manager_->CreateTable(q->table_name)) {
                catalog_manager_->CreateCatalog(q);
                result->is_failed = false;
                result->message = "Table '" + q->table_name + "' created successfully!";
            } else {
                result->is_failed = true;
                result->message = "Error: Table '" + q->table_name + "' already exists!";
            }
            return result;
            break;
        }
        case DROP_TABLE: {
            ResultDropTable *result = new ResultDropTable;
            QueryDropTable *q = (QueryDropTable *)query;
            if(record_manager_->DropTable(q->table_name)) {
                catalog_manager_->DropCatalog(q->table_name);
                result->is_failed = false;
                result->message = "Table '" + q->table_name + "' droped successfully!";
            } else {
                result->is_failed = true;
                result->message = "Error: Table '" + q->table_name + "' doesn't exist!";
            }
            return result;
            break;
        }
        case CREATE_INDEX: {
            ResultCreateIndex *result = new ResultCreateIndex;
            QueryCreateIndex *q = (QueryCreateIndex *)query;
            return result;
            break;
        }
        case DROP_INDEX: {
            ResultDropIndex *result = new ResultDropIndex;
            QueryDropIndex *q = (QueryDropIndex *)query;
            return result;
            break;
        }
        case SELECT: {
            ResultSelect *result = new ResultSelect;
            QuerySelect *q = (QuerySelect *)query;
            TableInfo *table_info = catalog_manager_->GetTableInfo(q->table_name);
            if(table_info) {
                if(!VerifyWhereType(table_info->attribute_info, q->where)) {
                    result->is_failed = true;
                    result->message = "Error: Type mismatch between table '" + q->table_name + "' and WHERE value in SELETE!";
                    return result;
                }
                record_manager_->SelectRecord(table_info, q, result);
                if(result->record.empty()) {
                    result->message = "Empty set!";
                } else {
                    for(int i = 0; i < table_info->attribute_info.size(); i++)
                        result->attribute_name.push_back(table_info->attribute_info[i].name);
                    result->message = "Select result:";
                }
                result->is_failed = false;
            } else {
                result->is_failed = true;
                result->message = "Error: Table '" + q->table_name + "' doesn't exist!";
            }
            delete table_info;
            return result;
            break;
        }
        case INSERT: {
            ResultInset *result = new ResultInset;
            QueryInsert *q = (QueryInsert *)query;
            TableInfo *table_info = catalog_manager_->GetTableInfo(q->table_name);
            if(table_info) {
                if(!VerifyRecordType(table_info->attribute_info, q->attribute_value)) {
                    result->is_failed = true;
                    result->message = "Error: Type mismatch between table '" + q->table_name + "' and INSERT value!";
                    return result;
                }
                record_manager_->InsertRecord(table_info, q);
                catalog_manager_->UpdateCatalog(table_info);
                result->is_failed = false;
                result->message = "Record inserted successfully!";
            } else {
                result->is_failed = true;
                result->message = "Error: Table '" + q->table_name + "' doesn't exist!";
            }
            return result;
            break;
        }
        case DELETE: {
            ResultDelete *result = new ResultDelete;
            QueryDelete *q = (QueryDelete *)query;
            TableInfo *table_info = catalog_manager_->GetTableInfo(q->table_name);
            if(table_info) {
                if(!VerifyWhereType(table_info->attribute_info, q->where)) {
                    result->is_failed = true;
                    result->message = "Error: Type mismatch between table '" + q->table_name + "' and WHERE value in DELETE!";
                    return result;
                }
                record_manager_->DeleteRecord(table_info, q);
                catalog_manager_->UpdateCatalog(table_info);
                result->is_failed = false;
                result->message = "Record deleted successfully!";
            } else {
                result->is_failed = true;
                result->message = "Error: Table '" + q->table_name + "' doesn't exist!";
            }
            return result;
            break;
        }
    }
}

void API::Terminate()
{

}
