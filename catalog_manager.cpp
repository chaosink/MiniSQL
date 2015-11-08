#include "buffer_manager.h"
#include "catalog_manager.h"
#include <fstream>
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
        ofs << it->name << ' ' << it->type << ' ' << it->char_length << ' ' << (it->is_unique || it->name == query->primary_key) << endl;
    }
    ofs << query->primary_key << ' ' << query->primary_key << endl;
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
        ifs >> attr_info.name >> attr_info.type >> attr_info.char_length >> attr_info.is_unique;
        table_info->attribute_info.push_back(attr_info);
    }
    for(int i = 0; i < table_info->index_num; i++) {
        Index index;
        ifs >> index.index_name >> index.attribute_name;
        table_info->index.push_back(index);
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
        ofs << table_info->index[i].index_name << ' ' << table_info->index[i].attribute_name << endl;
    }
    ofs.close();
}

void CatalogManager::DropIndex(string &table_name, string &index_name) {
    TableInfo *table_info = GetTableInfo(table_name);
    for(int i = 0; i < table_info->index_num; i++)
        if(table_info->index[i].index_name == index_name) {
            table_info->index.erase(table_info->index.begin() + i);
            table_info->index_num--;
            break;
        }
    UpdateCatalog(table_info);
}
