#include <fstream>
#include <sstream>
#include <iostream>
#include <cstdio>
#include <cstdlib>
#include <cstring>
#include "record_manager.h"
#include "query.h"
#include "table.h"
using namespace std;

RecordManager::RecordManager()
{

}

RecordManager::~RecordManager()
{

}

void RecordManager::Init(BufferManager *buffer_manager)
{
    buffer_manager_ = buffer_manager;
}

void RecordManager::Terminate()
{

}

void RecordManager::CreateTable(string table_name)
{
    ofstream output((table_name + ".db").c_str());
    output.close();
}

void RecordManager::DropTable(string table_name)
{
    remove((table_name + ".db").c_str());
}

void RecordManager::AddOneBlock(string table_name)
{
    ofstream output((table_name + ".db").c_str(), ofstream::app | ofstream::binary);
    for(int i = 0; i < BLOCK_SIZE; i++) output.write("\0", 1);
    output.close();
}

void RecordManager::InsertRecord(TableInfo *table_info, QueryInsert *query)
{
    //printf("%s %d %d %s %d %d %d\n", table_info->table_name.c_str(), table_info->attribute_num, table_info->record_num, table_info->primary_key.c_str(), table_info->record_num_per_block, table_info->record_size, table_info->block_num);
    int table_block_num = table_info->block_num - 1;
    if(table_info->block_num * table_info->record_num_per_block == table_info->record_num) {
        table_block_num++;
        AddOneBlock(table_info->table_name);
        table_info->block_num++;
    }
    char *block = buffer_manager_->GetTableBlock(table_info->table_name, table_block_num);
    int record_num = table_info->record_num % table_info->record_num_per_block;
    WriteRecord(table_info->attribute_info, query->attribute_value, block + record_num * table_info->record_size);
    table_info->record_num++;
}

void RecordManager::WriteRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address)
{
    *address = 0;
    address++;
    for(int i = 0; i < (int)attr_info.size() ; i++) {
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
    }
    //cout << endl;
}

template <typename T>
bool SatisfyTemplate(T a, T b, ComparisonType comparison)
{
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
}

bool SatisfyWhere(vector<AttributeInfo> &attr_info, vector<string> &attr_value, vector<Where> &where) {
    for(int i = 0; i < where.size(); i++) {
        int attr_num;
        for(attr_num = 0; attr_num < attr_info.size(); attr_num++)
            if(where[i].attribute_name == attr_info[attr_num].name) break;
        if(!Satisfy(attr_info[attr_num].type, attr_value[attr_num], where[i])) return false;
    }
    return true;
}

void RecordManager::SelectRecord(TableInfo *table_info, QuerySelect *query)
{
    //printf("%s %d %d %s %d %d %d\n", table_info->table_name.c_str(), table_info->attribute_num, table_info->record_num, table_info->primary_key.c_str(), table_info->record_num_per_block, table_info->record_size, table_info->block_num);
    for(int i = 0; i < table_info->block_num; i++) {
        char *block = buffer_manager_->GetTableBlock(table_info->table_name, i);
        int record_num;
        if(i != table_info->block_num - 1) record_num = table_info->record_num_per_block;
        else record_num = (table_info->record_num - 1) % table_info->record_num_per_block + 1;
        for(int j = 0; j < record_num; j++) {
            vector<string> attribute_value;
            ReadRecord(table_info->attribute_info, attribute_value, block + j * table_info->record_size);
            if(SatisfyWhere(table_info->attribute_info, attribute_value, query->where)) {
                for(int i = 0; i < attribute_value.size(); i++) {
                    cout << attribute_value[i] << " ";
                }
                cout << endl;
            }
        }
    }
}

void RecordManager::ReadRecord(vector<AttributeInfo> &attr_info, vector<string> &attr_value, char *address)
{
    address++;
    for(int i = 0; i < (int)attr_info.size() ; i++) {
        switch(attr_info[i].type) {
            case CHAR: {
                char s[256] = {0};
                memcpy(s, address, attr_info[i].char_length);
                string str(s);
                attr_value.push_back(str);
                //cout << attr_info[i].type << " " << str << " ";
                address += attr_info[i].char_length;
                break;
            }
            case INT: {
                char s[256] = {0};
                sprintf(s, "%d", *((int *)address));
                string str(s);
                attr_value.push_back(str);
                //cout << attr_info[i].type << "! " << str << " ";
                address += 4;
                break;
            }
            case FLOAT: {
                char s[256] = {0};
                sprintf(s, "%f", *((float *)address));
                string str(s);
                attr_value.push_back(str);
                //cout << attr_info[i].type << "! " << str << " ";
                address += 4;
                break;
            }
        }
    }
    //cout << endl;
}

void RecordManager::DeleteRecord()
{

}
