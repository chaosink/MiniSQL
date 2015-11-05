#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include "index_manager.h"
#include "b_plus_tree.h"
using namespace std;

IndexManager::IndexManager() {

}

IndexManager::~IndexManager() {

}

void IndexManager::Init(BufferManager *buffer_manager) {
    buffer_manager_ = buffer_manager;
}

bool IndexManager::CreateIndex(TableInfo *table_info, string &index_name, string &attr_name) {
    ifstream ifs((index_name + ".idx").c_str());
    if(ifs.is_open()) {
        ifs.close();
        return false;
    }
    ifs.close();
    ofstream ofs1((index_name + ".idx").c_str());
    ofs1.close();

    int attr_num;
    for(unsigned int i = 0; i < table_info->attribute_info.size(); i++)
        if(table_info->attribute_info[i].name == attr_name) {
            attr_num = i;
            break;
        }
    table_info->index_num++;
    IndexInfo index_info = {index_name, table_info->table_name, attr_name, table_info->attribute_info[attr_num].type, table_info->attribute_info[attr_num].char_length, -1, 0, 0};
    table_info->index_info.push_back(index_info);
    ofstream ofs2((index_name + ".idxinfo").c_str());
    ofs2 << index_info.index_name << ' ' << index_info.table_name << ' ' << index_info.attribute_name << ' ' << index_info.type << ' ' << index_info.char_length << ' ' << index_info.root << ' ' << index_info.block_num << ' ' << index_info.empty_block_num << endl;
    ofs2.close();
    return true;
}

IndexInfo *IndexManager::GetIndexInfo(string index_name) {
    ifstream ifs((index_name + ".idxinfo").c_str());
    if(!ifs.is_open()) {
        return NULL;
    }
    IndexInfo *index_info = new IndexInfo;
    ifs >> index_info->index_name >> index_info->table_name >> index_info->attribute_name >> index_info->type >> index_info->char_length >> index_info->root >> index_info->block_num >> index_info->empty_block_num;
    return index_info;
}

void IndexManager::UpdateIndexInfo(IndexInfo *index_info) {
    ofstream ofs((index_info->index_name + ".idxinfo").c_str());
    ofs << index_info->index_name << ' ' << index_info->table_name << ' ' << index_info->attribute_name << ' ' << index_info->type << ' ' << index_info->char_length << ' ' << index_info->root << ' ' << index_info->block_num << ' ' << index_info->empty_block_num;
    ofs.close();
}

void IndexManager::InsertIndex(string index_name, string attr_value, Pointer pointer) {
    IndexInfo *index_info = GetIndexInfo(index_name);
    if(index_info->type == CHAR) {
        BPlusTree<String, Pointer> b_plus_tree = BPlusTree<String, Pointer>(index_info, *buffer_manager_);
        string s = attr_value.substr(1, attr_value.size() - 2);
        b_plus_tree.Insert(String(s.c_str()), pointer);
    } else if(index_info->type == INT) {
        BPlusTree<int, Pointer> b_plus_tree = BPlusTree<int, Pointer>(index_info, *buffer_manager_);
        b_plus_tree.Insert(atoi(attr_value.c_str()), pointer);
    } else if(index_info->type == FLOAT) {
        BPlusTree<float, Pointer> b_plus_tree = BPlusTree<float, Pointer>(index_info, *buffer_manager_);
        b_plus_tree.Insert(atof(attr_value.c_str()), pointer);
    }
    UpdateIndexInfo(index_info);
    delete index_info;
}

bool IndexManager::DropIndex(string &index_name) {
    ifstream ifs((index_name + ".idx").c_str());
    if(ifs.is_open()) {
        ifs.close();
        remove((index_name + ".idx").c_str());
        remove((index_name + ".idxinfo").c_str());
        return true;
    }
    ifs.close();
    return false;
}

bool IndexManager::DropAllIndex(vector<IndexInfo> &index_info) {
    for(unsigned int i = 0; i < index_info.size(); i++)
        if(!DropIndex(index_info[i].index_name)) return false;
    return true;
}

void IndexManager::Terminate() {

}
