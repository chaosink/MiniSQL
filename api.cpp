#include "api.h"
#include "record_manager.h"
#include "index_manager.h"
#include "catalog_manager.h"
#include "query.h"
#include "table.h"
#include "result.h"
#include <iostream>
#include <cstdio>
#include <algorithm>
using namespace std;

API::API() {

}

API::~API() {

}

void API::Init(RecordManager *record_manager, IndexManager *index_manager, CatalogManager *catalog_manager) {
    record_manager_ = record_manager;
    index_manager_ = index_manager;
    catalog_manager_ = catalog_manager;
}

bool VerifyType(int type, string &attr_value) {
    switch(type) {
        case CHAR:
            if(attr_value[0] != '\'') return false;
            break;
        case INT:
            if(attr_value[0] == '\'' || attr_value.find_first_of('.') != string::npos) return false;
            break;
        case FLOAT:
            if(attr_value[0] == '\'') return false;
            break;
    }
    return true;
}

bool VerifyRecordType(vector<AttributeInfo> &attribute_info, vector<string> &attr_value) {
    for(unsigned int i = 0; i < attribute_info.size(); i++)
        if(!VerifyType(attribute_info[i].type, attr_value[i])) return false;
    return true;
}

bool VerifyWhereType(vector<AttributeInfo> &attribute_info, vector<Where> &where) {
    for(unsigned int i = 0; i < where.size(); i++) {
        unsigned int j;
        for(j = 0; j < attribute_info.size(); j++)
            if(attribute_info[j].name == where[i].attribute_name) {
                if(!VerifyType(attribute_info[j].type, where[i].attribute_value)) return false;
                break;
            }
        if(j == attribute_info.size()) return false;
    }
    return true;
}

int IsUnique(vector<AttributeInfo> &attribute_info, string attr_name) {
    for(unsigned int i = 0; i < attribute_info.size(); i++)
        if(attribute_info[i].name == attr_name)
            return attribute_info[i].is_unique;
    return -1;
}

string GetIndexName(vector<Index> &index, string &attr_name) {
    for(unsigned int i = 0; i < index.size(); i++)
        if(index[i].index_name == attr_name)
            return index[i].index_name;
    return "";
}

bool API::VerifyUnique(TableInfo *table_info, vector<string> &attribute_value) {
    for(int i = 0; i < table_info->attribute_num; i++)
        if(table_info->attribute_info[i].is_unique) {
            Where where;
            where.attribute_name = table_info->attribute_info[i].name;
            where.comparison = EQUAL;
            where.attribute_value = attribute_value[i];
            string index_name = GetIndexName(table_info->index, table_info->attribute_info[i].name);
            if(index_name.empty()) {
                vector<Where> where_nonindex;
                where_nonindex.push_back(where);
                ResultSelect result;
                record_manager_->SelectRecord(table_info, where_nonindex, &result);
                if(!result.record.empty()) return false;
            } else {
                vector<WhereIndex> where_index;
                where_index.push_back(WhereIndex(index_name, where));
                vector<Pointer> pointer = index_manager_->FindIndex(where_index);
                if(!pointer.empty()) return false;
            }
        }
    return true;
}

void GetWhereApart(TableInfo *table_info, vector<Where> &where, vector<Where> &where_nonindex, vector<WhereIndex> &where_index) {
    for(unsigned int i = 0; i < where.size(); i++) {
        if(where[i].comparison == NOT_EQUAL) {
            where_nonindex.push_back(where[i]);
            continue;
        }
        unsigned int j;
        for(j = 0; j < table_info->index.size(); j++)
            if(where[i].attribute_name == table_info->index[j].attribute_name) {
                where_index.push_back(WhereIndex(table_info->index[j].index_name, where[i]));
                break;
            }
        if(j == table_info->index.size())
            where_nonindex.push_back(where[i]);
    }
}

Result *API::ProcessQuery(Query *query) {
    if(!query) return NULL;
    switch(query->type) {
        case CREATE_TABLE: {
            ResultCreateTable *result = new ResultCreateTable;
            QueryCreateTable *q = (QueryCreateTable *)query;
            if(record_manager_->CreateTable(q->table_name)) {
                catalog_manager_->CreateCatalog(q);
                TableInfo *table_info = catalog_manager_->GetTableInfo(q->table_name);
                index_manager_->CreateIndex(table_info, q->primary_key, q->primary_key);
                result->is_failed = false;
                result->message = "Table '" + q->table_name + "' created successfully";
            } else {
                result->is_failed = true;
                result->message = "ERROR: Table '" + q->table_name + "' already exists";
            }
            return result;
            break;
        }
        case DROP_TABLE: {
            ResultDropTable *result = new ResultDropTable;
            QueryDropTable *q = (QueryDropTable *)query;
            if(record_manager_->DropTable(q->table_name)) {
                TableInfo *table_info = catalog_manager_->GetTableInfo(q->table_name);
                index_manager_->DropAllIndex(table_info->index);
                catalog_manager_->DropCatalog(q->table_name);
                result->is_failed = false;
                result->message = "Table '" + q->table_name + "' droped successfully";
            } else {
                result->is_failed = true;
                result->message = "ERROR: Table '" + q->table_name + "' doesn't exist";
            }
            return result;
            break;
        }
        case CREATE_INDEX: {
            ResultCreateIndex *result = new ResultCreateIndex;
            QueryCreateIndex *q = (QueryCreateIndex *)query;
            TableInfo *table_info = catalog_manager_->GetTableInfo(q->table_name);
            if(table_info) {
                int is_unique = IsUnique(table_info->attribute_info, q->attribute_name);
                if(is_unique == -1) {
                    result->is_failed = true;
                    result->message = "ERROR: Attribute '" + q->attribute_name + "' doesn't exist in Table '" + q->table_name +"'";
                } else if(is_unique == 0) {
                    result->is_failed = true;
                    result->message = "ERROR: Attribute '" + q->attribute_name + "' in Table '" + q->table_name +"' is not unique";
                } else if(index_manager_->CreateIndex(table_info, q->index_name, q->attribute_name)){
                    catalog_manager_->UpdateCatalog(table_info);
                    index_manager_->InsertAllIndex(table_info, q->index_name);
                    result->is_failed = false;
                    result->message = "Index '" + q->index_name + "' created successfully";
                } else {
                    result->is_failed = true;
                    result->message = "ERROR: Index '" + q->index_name + "' already exists";
                }
            } else {
                result->is_failed = true;
                result->message = "ERROR: Table '" + q->table_name + "' doesn't exist";
            }
            return result;
            break;
        }
        case DROP_INDEX: {
            ResultDropIndex *result = new ResultDropIndex;
            QueryDropIndex *q = (QueryDropIndex *)query;
            IndexInfo *index_info = index_manager_->GetIndexInfo(q->index_name);
            if(index_info) {
                catalog_manager_->DropIndex(index_info->table_name, q->index_name);
                index_manager_->DropIndex(q->index_name);
                result->is_failed = false;
                result->message = "Index '" + q->index_name + "' droped successfully";
            } else {
                result->is_failed = true;
                result->message = "ERROR: Index '" + q->index_name + "' doexn't exist";
            }
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
                    result->message = "ERROR: Type mismatch between table '" + q->table_name + "' and WHERE value in SELETE";
                    return result;
                }
                vector<Where> where_nonindex;
                vector<WhereIndex> where_index;
                GetWhereApart(table_info, q->where, where_nonindex, where_index);
                if(where_index.size()) {
                    vector<Pointer> pointer = index_manager_->FindIndex(where_index);
                    sort(pointer.begin(), pointer.end());
                    record_manager_->SelectRecordWithPointer(table_info, pointer, where_nonindex, result);
                } else
                    record_manager_->SelectRecord(table_info, q->where, result);
                if(result->record.empty()) {
                    result->message = "Empty set";
                } else {
                    for(unsigned int i = 0; i < table_info->attribute_info.size(); i++)
                        result->attribute_name.push_back(table_info->attribute_info[i].name);
                    char buffer[256];
                    sprintf(buffer, "%lu", result->record.size());
                    if(result->record.size() == 1)
                        result->message = string(buffer) + " row in set";
                    else
                        result->message = string(buffer) + " rows in set";
                }
                result->is_failed = false;
            } else {
                result->is_failed = true;
                result->message = "ERROR: Table '" + q->table_name + "' doesn't exist";
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
                    result->message = "ERROR: Type mismatch between table '" + q->table_name + "' and INSERT value";
                    return result;
                }
                /*if(!VerifyUnique(table_info, q->attribute_value)) {
                    result->is_failed = true;
                    result->message = "ERROR: Try to insert a duplicate value of an unique attribute in Table '" + q->table_name + "'";
                    return result;
                }*/
                Pointer pointer = record_manager_->InsertRecord(table_info, q);
                index_manager_->InsertIndex(table_info, q->attribute_value, pointer);
                catalog_manager_->UpdateCatalog(table_info);
                result->is_failed = false;
                result->message = "Record inserted successfully";
            } else {
                result->is_failed = true;
                result->message = "ERROR: Table '" + q->table_name + "' doesn't exist";
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
                    result->message = "ERROR: Type mismatch between table '" + q->table_name + "' and WHERE value in DELETE";
                    return result;
                }
                vector<Where> where_nonindex;
                vector<WhereIndex> where_index;
                GetWhereApart(table_info, q->where, where_nonindex, where_index);
                int delete_num;
                vector<vector<string> > record;
                if(where_index.size()) {
                    vector<Pointer> pointer = index_manager_->FindIndex(where_index);
                    sort(pointer.begin(), pointer.end());
                    delete_num = record_manager_->DeleteRecordWithPointer(table_info, pointer, where_nonindex, record);
                } else
                    delete_num = record_manager_->DeleteRecord(table_info, q, record);
                index_manager_->DeleteIndex(table_info, record);
                catalog_manager_->UpdateCatalog(table_info);
                result->is_failed = false;
                char buffer[256];
                sprintf(buffer, "%d", delete_num);
                if(delete_num == 0)
                    result->message = "No record deleted";
                else if(delete_num == 1)
                    result->message = string(buffer) + " record deleted successfully";
                else
                    result->message = string(buffer) + " records deleted successfully";
            } else {
                result->is_failed = true;
                result->message = "ERROR: Table '" + q->table_name + "' doesn't exist";
            }
            return result;
            break;
        }
    }
    return NULL;
}

void API::Terminate() {

}
