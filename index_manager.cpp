#include <fstream>
#include "index_manager.h"
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
    ofstream ofs2((index_name + ".idxinfo").c_str());
    ofs2 << index_name << ' ' << table_info->table_name << ' ' << attr_name << ' ' << 0 << ' ' << -1 << endl;
    ofs2.close();
    table_info->index_num++;
    IndexInfo index_info = {index_name, attr_name};
    table_info->index_info.push_back(index_info);
    return true;
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

string IndexManager::GetIndexTableName(string &index_name) {
    string table_name;
    ifstream ifs((index_name + ".idxinfo").c_str());
    if(ifs.is_open())
        ifs >> table_name >> table_name;
    else
        table_name = "";
    ifs.close();
    return table_name;
}

void IndexManager::Terminate() {

}
