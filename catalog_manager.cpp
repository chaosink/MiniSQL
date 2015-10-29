#include <fstream>
#include <iostream>
#include "catalog_manager.h"
#include "buffer_manager.h"
using namespace std;

CatalogManager::CatalogManager()
{

}

CatalogManager::~CatalogManager()
{

}

void CatalogManager::Init(BufferManager *buffer_manager)
{
    buffer_manager_ = buffer_manager;
}

void CatalogManager::Terminate()
{

}

void CatalogManager::CreateCatalog(QueryCreateTable *query)
{
    ofstream output((query->table_name + ".log").c_str());
    int record_size = 1;
    vector<Attribute>::iterator it;
    for(it = query->attribute.begin(); it != query->attribute.end(); it++)
    {
        if(it->type == CHAR)
            record_size += it->char_length;
        else
            record_size += 4;
    }
    int record_num_per_block = BLOCK_SIZE / record_size;
    output << query->table_name << ' ' << query->attribute.size() << ' ' << 0 << ' ' << query->primary_key << ' ' << record_num_per_block << ' ' << record_size << ' ' << 0 << endl;
    for(it = query->attribute.begin(); it != query->attribute.end(); it++)
    {
        output << it->name << ' ' << it->type << ' ' << it->char_length << ' ' << it->is_unique << endl;
    }
    output.close();
}

void CatalogManager::DropCatalog(string table_name)
{
    remove((table_name + ".log").c_str());
}

TableInfo *CatalogManager::GetTalbeInfo(string table_name)
{
    TableInfo *table_info = new TableInfo;
    ifstream input((table_name + ".log").c_str());
    input >> table_info->table_name >> table_info->attribute_num >> table_info->record_num >> table_info->primary_key >> table_info->record_num_per_block >> table_info->record_size >> table_info->block_num;
    for(int i = 0; i < table_info->attribute_num; i++) {
        AttributeInfo attr_info;
        int type;
        input >> attr_info.name >> type >> attr_info.char_length >> attr_info.is_unique;
        if(type == 0) attr_info.type = CHAR;
        else if(type == 1) attr_info.type = INT;
        else if(type == 2) attr_info.type = FLOAT;
        table_info->attribute_info.push_back(attr_info);
    }
    return table_info;
}

void CatalogManager::UpdateCatalog(TableInfo *table_info)
{
    ofstream output((table_info->table_name + ".log").c_str());
    output << table_info->table_name << ' ' << table_info->attribute_num << ' ' << table_info->record_num << ' ' << table_info->primary_key << ' ' << table_info->record_num_per_block << ' ' << table_info->record_size << ' ' << table_info->block_num << endl;
    vector<AttributeInfo>::iterator it;
    for(it = table_info->attribute_info.begin(); it != table_info->attribute_info.end(); it++)
    {
        output << it->name << ' ' << it->type << ' ' << it->char_length << ' ' << it->is_unique << endl;
    }
    output.close();
}
