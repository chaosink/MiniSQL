#include <fstream>
#include <iostream>
#include "catalog_manager.h"
#include "buffer_manager.h"
using namespace std;

CatalogManager::CatalogManager() {

}

CatalogManager::~CatalogManager() {

}

void CatalogManager::Init(BufferManager *buffer_manager) {
    buffer_manager_ = buffer_manager;
}

void CatalogManager::Terminate() {

}

void CatalogManager::CreateCatalog(QueryCreateTable *query) {
    ofstream ofs((query->table_name + ".log").c_str());
    int record_size = 1;
    vector<Attribute>::iterator it;
    for(it = query->attribute.begin(); it != query->attribute.end(); it++) {
        if(it->type == CHAR)
            record_size += it->char_length;
        else
            record_size += 4;
    }
    int record_num_per_block = BLOCK_SIZE / record_size;
    ofs << query->table_name << ' ' << query->attribute.size() << ' ' << 1 << ' ' << 0 << ' ' << query->primary_key << ' ' << record_num_per_block << ' ' << record_size << ' ' << 0 << endl;
    for(it = query->attribute.begin(); it != query->attribute.end(); it++) {
        ofs << it->name << ' ' << it->type << ' ' << it->char_length << ' ' << it->is_unique << endl;
    }
    int primary_key_num;
    for(unsigned int i = 0; i < query->attribute.size(); i++)
        if(query->attribute[i].name == query->primary_key) {
            primary_key_num = i;
            break;
        }
    ofs << query->primary_key << ' ' << query->primary_key << ' ' << query->attribute[primary_key_num].type << ' ' << query->attribute[primary_key_num].char_length << ' ' << -1 << ' ' << 0 << ' ' << 0 << endl;
    ofs.close();
}

void CatalogManager::DropCatalog(string table_name) {
    remove((table_name + ".log").c_str());
}

TableInfo *CatalogManager::GetTableInfo(string table_name) {
    ifstream ifs((table_name + ".log").c_str());
    if(!ifs.is_open()) {
        return NULL;
    }
    TableInfo *table_info = new TableInfo;
    ifs >> table_info->table_name >> table_info->attribute_num >> table_info->index_num >> table_info->record_num >> table_info->primary_key >> table_info->record_num_per_block >> table_info->record_size >> table_info->block_num;
    for(int i = 0; i < table_info->attribute_num; i++) {
        AttributeInfo attr_info;
        int type;
        ifs >> attr_info.name >> type >> attr_info.char_length >> attr_info.is_unique;
        if(type == 0) attr_info.type = CHAR;
        else if(type == 1) attr_info.type = INT;
        else if(type == 2) attr_info.type = FLOAT;
        table_info->attribute_info.push_back(attr_info);
    }
    for(int i = 0; i < table_info->index_num; i++) {
        IndexInfo index_info;
        ifs >> index_info.index_name >> index_info.attribute_name >> index_info.type >> index_info.char_length >> index_info.root >> index_info.block_num >> index_info.empty_block_num;
        index_info.table_name = table_name;
        table_info->index_info.push_back(index_info);
    }
    return table_info;
}

void CatalogManager::UpdateCatalog(TableInfo *table_info) {
    ofstream ofs((table_info->table_name + ".log").c_str());
    ofs << table_info->table_name << ' ' << table_info->attribute_num << ' ' << table_info->index_num << ' ' << table_info->record_num << ' ' << table_info->primary_key << ' ' << table_info->record_num_per_block << ' ' << table_info->record_size << ' ' << table_info->block_num << endl;
    vector<AttributeInfo>::iterator it;
    for(it = table_info->attribute_info.begin(); it != table_info->attribute_info.end(); it++) {
        ofs << it->name << ' ' << it->type << ' ' << it->char_length << ' ' << it->is_unique << endl;
    }
    for(int i = 0; i < table_info->index_num; i++) {
        ofs << table_info->index_info[i].index_name << ' ' << table_info->index_info[i].attribute_name << ' ' << table_info->index_info[i].type << ' ' << table_info->index_info[i].char_length << ' ' << table_info->index_info[i].root << ' ' << table_info->index_info[i].block_num << ' ' << table_info->index_info[i].empty_block_num << endl;
    }
    ofs.close();
}

void CatalogManager::DropIndex(string &table_name, string &index_name) {
    TableInfo *table_info = GetTableInfo(table_name);
    for(int i = 0; i < table_info->index_num; i++)
        if(table_info->index_info[i].index_name == index_name) {
            table_info->index_info.erase(table_info->index_info.begin() + i);
            table_info->index_num--;
            break;
        }
    UpdateCatalog(table_info);
}
