#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "record_manager.h"
#include "query.h"
#include "table.h"
#include "result.h"
using namespace std;

RecordManager::RecordManager() {

}

RecordManager::~RecordManager() {

}

void RecordManager::Init(BufferManager *buffer_manager) {
    buffer_manager_ = buffer_manager;
}

void RecordManager::Terminate() {

}

bool RecordManager::CreateTable(string table_name) {
    ifstream ifs((table_name + ".db").c_str());
    if(ifs.is_open()) {
        ifs.close();
        return false;
    }
    ifs.close();
    ofstream ofs((table_name + ".db").c_str());
    ofs.close();
    return true;
}

bool RecordManager::DropTable(string table_name) {
    ifstream ifs((table_name + ".db").c_str());
    if(ifs.is_open()) {
        ifs.close();
        remove((table_name + ".db").c_str());
        buffer_manager_->DeleteBlock(table_name + ".db");
        return true;
    }
    ifs.close();
    return false;
}

void RecordManager::AddOneBlock(string table_name) {
    static char empty_block[BLOCK_SIZE] ={0};
    ofstream output((table_name + ".db").c_str(), ofstream::app | ofstream::binary);
    output.write(empty_block, BLOCK_SIZE);
    output.close();
}

Pointer RecordManager::InsertRecord(TableInfo *table_info, QueryInsert *query) {
    //printf("%s %d %d %s %d %d %d\n", table_info->table_name.c_str(), table_info->attribute_num, table_info->record_num, table_info->primary_key.c_str(), table_info->record_num_per_block, table_info->record_size, table_info->block_num);
    if(table_info->block_num * table_info->record_num_per_block == table_info->record_num) {
        AddOneBlock(table_info->table_name);
        char *block = buffer_manager_->GetFileBlock(table_info->table_name + ".db", table_info->block_num);
        table_info->block_num++;
        WriteRecord(table_info->attribute_info, query->attribute_value, block);
        table_info->record_num++;
        return Pointer(table_info->block_num - 1, 0);
    }
    for(int i = 0; i < table_info->block_num; i++) {
        char *block = buffer_manager_->GetFileBlock(table_info->table_name + ".db", i);
        for(int j = 0; j < table_info->record_num_per_block; j++)
            if(!block[j * table_info->record_size]) {
                WriteRecord(table_info->attribute_info, query->attribute_value, block + j * table_info->record_size);
                table_info->record_num++;
                return Pointer(i, j * table_info->record_size);
            }
    }
    return Pointer(-1, 0);
}

void RecordManager::WriteRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address) {
    *address = 1;
    address++;
    for(int i = 0; i < (int)attr_info.size() ; i++)
        switch(attr_info[i].type) {
            case CHAR:
                //cout << attr_value[i] << " ";
                memcpy(address, attr_value[i].c_str() + 1, attr_value[i].size() - 2);
                memset(address + attr_value[i].size() - 2, 0, attr_info[i].char_length - attr_value[i].size() + 2);
                address += attr_info[i].char_length;
                break;
            case INT:
                //cout << attr_value[i] << " ";
                *(int *)address = atoi(attr_value[i].c_str());
                address += 4;
                break;
            case FLOAT:
                //cout << attr_value[i] << " ";
                *(float *)address = atof(attr_value[i].c_str());
                address += 4;
                break;
        }
    //cout << endl;
}

template <typename T>
bool SatisfyTemplate(T a, T b, ComparisonType comparison) {
    switch(comparison) {
        case EQUAL:
            if(a == b) return true;
            else return false;
            break;
        case NOT_EQUAL:
            if(a != b) return true;
            else return false;
            break;
        case LESS:
            if(a < b) return true;
            else return false;
            break;
        case GREATER:
            if(a > b) return true;
            else return false;
            break;
        case LESS_EQUAL:
            if(a <= b) return true;
            else return false;
            break;
        case GREATER_EQUAL:
            if(a >= b) return true;
            else return false;
            break;
    }
    return false;
}

bool Satisfy(AttributeType &type, string &attr_value, Where &where) {
    //cout << attr_value << " " << where.attribute_value << endl;
    if(type == CHAR) {
        string a = "'" + attr_value + "'";
        string b = where.attribute_value;
        return SatisfyTemplate(a, b, where.comparison);
    } else if(type == INT) {
        int a = atoi(attr_value.c_str());
        int b = atoi(where.attribute_value.c_str());
        return SatisfyTemplate(a, b, where.comparison);
    } else if(type == FLOAT) {
        float a = atof(attr_value.c_str());
        float b = atof(where.attribute_value.c_str());
        return SatisfyTemplate(a, b, where.comparison);
    }
    return false;
}

bool SatisfyWhere(vector<AttributeInfo> &attr_info, vector<string> &attr_value, vector<Where> &where) {
    for(int i = 0; i < (int)where.size(); i++) {
        int attr_num;
        for(attr_num = 0; attr_num < (int)attr_info.size(); attr_num++)
            if(where[i].attribute_name == attr_info[attr_num].name) break;
        if(!Satisfy(attr_info[attr_num].type, attr_value[attr_num], where[i])) return false;
    }
    return true;
}

void RecordManager::SelectRecord(TableInfo *table_info, QuerySelect *query, ResultSelect *result) {
    //printf("%s %d %d %s %d %d %d\n", table_info->table_name.c_str(), table_info->attribute_num, table_info->record_num, table_info->primary_key.c_str(), table_info->record_num_per_block, table_info->record_size, table_info->block_num);
    for(int i = 0; i < table_info->block_num; i++) {
        char *block = buffer_manager_->GetFileBlock(table_info->table_name + ".db", i);
        for(int j = 0; j < table_info->record_num_per_block; j++)
            if(block[j * table_info->record_size]) {
                vector<string> attribute_value;
                ReadRecord(table_info->attribute_info, attribute_value, block + j * table_info->record_size);
                if(SatisfyWhere(table_info->attribute_info, attribute_value, query->where)) {
                    result->record.push_back(attribute_value);
                }
            }
    }
}

void RecordManager::ReadRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address) {
    address++;
    for(int i = 0; i < (int)attr_info.size() ; i++) {
        switch(attr_info[i].type) {
            case CHAR: {
                char s[256] = {0};
                memcpy(s, address, attr_info[i].char_length);
                string str(s);
                attr_value.push_back(str);
                //cout << str << " ";
                address += attr_info[i].char_length;
                break;
            }
            case INT: {
                char s[256] = {0};
                sprintf(s, "%d", *((int *)address));
                string str(s);
                attr_value.push_back(str);
                //cout  << str << " ";
                address += 4;
                break;
            }
            case FLOAT: {
                char s[256] = {0};
                sprintf(s, "%f", *((float *)address));
                string str(s);
                attr_value.push_back(str);
                //cout << str << " ";
                address += 4;
                break;
            }
        }
    }
    //cout << endl;
}

int RecordManager::DeleteRecord(TableInfo *table_info, QueryDelete *query) {
    int delete_num = 0;
    for(int i = 0; i < table_info->block_num; i++) {
        char *block = buffer_manager_->GetFileBlock(table_info->table_name + ".db", i);
        for(int j = 0; j < table_info->record_num_per_block; j++)
            if(block[j * table_info->record_size]) {
                vector<string> attribute_value;
                ReadRecord(table_info->attribute_info, attribute_value, block + j * table_info->record_size);
                if(SatisfyWhere(table_info->attribute_info, attribute_value, query->where)) {
                    block[j * table_info->record_size] = 0;
                    table_info->record_num--;
                    delete_num++;
                }
            }
    }
    return delete_num;
}
