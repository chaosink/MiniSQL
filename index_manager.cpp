#include <iostream>
#include <fstream>
#include <cstring>
#include <cstdlib>
#include <algorithm>
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
    Index index = {index_name, attr_name};
    table_info->index.push_back(index);
    ofstream ofs2((index_name + ".idxinfo").c_str());
    ofs2 << index_name << ' ' << table_info->table_name << ' ' << attr_name << ' ' << table_info->attribute_info[attr_num].type << ' ' << table_info->attribute_info[attr_num].char_length << ' ' << -1 << ' ' << 0 << ' ' << 0 << endl;
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

vector<Pointer> Intersection(vector<Pointer> &pointer1, vector<Pointer> &pointer2) {
    vector<Pointer> pointer;
    for(unsigned int i = 0; i < pointer1.size(); i++) {
        vector<Pointer>::iterator it = find(pointer2.begin(), pointer2.end(), pointer1[i]);
        if(it != pointer2.end())
            pointer.push_back(pointer1[i]);
    }
    return pointer;
}

vector<Pointer> IndexManager::FindIndex(vector<WhereIndex> &where_index) {
    // An optimization of the order of WhereIndex in where_index should be added here.
    vector<Pointer> pointer;
    for(unsigned int i = 0; i < where_index.size(); i++) {
        vector<Pointer> pointer_temp;
        IndexInfo *index_info = GetIndexInfo(where_index[i].index_name);
        if(where_index[i].comparison == EQUAL) {
            if(index_info->type == CHAR) {
                string s = where_index[i].attribute_value.substr(1, where_index[i].attribute_value.size() - 2);
                if(index_info->char_length > 127) {
                    BPlusTree<String256, Pointer> b_plus_tree = BPlusTree<String256, Pointer>(index_info, *buffer_manager_);
                    pointer_temp.push_back(b_plus_tree.Find(String256(s.c_str())));
                } else {
                    BPlusTree<String128, Pointer> b_plus_tree = BPlusTree<String128, Pointer>(index_info, *buffer_manager_);
                    pointer_temp.push_back(b_plus_tree.Find(String128(s.c_str())));
                }
            } else if(index_info->type == INT) {
                BPlusTree<int, Pointer> b_plus_tree = BPlusTree<int, Pointer>(index_info, *buffer_manager_);
                pointer_temp.push_back(b_plus_tree.Find(atoi(where_index[i].attribute_value.c_str())));
            } else if(index_info->type == FLOAT) {
                BPlusTree<float, Pointer> b_plus_tree = BPlusTree<float, Pointer>(index_info, *buffer_manager_);
                pointer_temp.push_back(b_plus_tree.Find(atof(where_index[i].attribute_value.c_str())));
            }
            if(pointer_temp.back().num == -1) pointer_temp.pop_back();
        } else if(where_index[i].comparison == GREATER || where_index[i].comparison == GREATER_EQUAL) {
            if(index_info->type == CHAR) {
                string s = where_index[i].attribute_value.substr(1, where_index[i].attribute_value.size() - 2);
                if(index_info->char_length > 127) {
                    BPlusTree<String256, Pointer> b_plus_tree = BPlusTree<String256, Pointer>(index_info, *buffer_manager_);
                    pointer_temp = b_plus_tree.FindFrom(String256(s.c_str()), where_index[i].comparison == GREATER_EQUAL);
                } else {
                    BPlusTree<String128, Pointer> b_plus_tree = BPlusTree<String128, Pointer>(index_info, *buffer_manager_);
                    pointer_temp = b_plus_tree.FindFrom(String128(s.c_str()), where_index[i].comparison == GREATER_EQUAL);
                }
            } else if(index_info->type == INT) {
                BPlusTree<int, Pointer> b_plus_tree = BPlusTree<int, Pointer>(index_info, *buffer_manager_);
                pointer_temp = b_plus_tree.FindFrom(atoi(where_index[i].attribute_value.c_str()), where_index[i].comparison == GREATER_EQUAL);
            } else if(index_info->type == FLOAT) {
                BPlusTree<float, Pointer> b_plus_tree = BPlusTree<float, Pointer>(index_info, *buffer_manager_);
                pointer_temp = b_plus_tree.FindFrom(atof(where_index[i].attribute_value.c_str()), where_index[i].comparison == GREATER_EQUAL);
            }
        } else if(where_index[i].comparison == LESS || where_index[i].comparison == LESS_EQUAL) {
            if(index_info->type == CHAR) {
                string s = where_index[i].attribute_value.substr(1, where_index[i].attribute_value.size() - 2);
                if(index_info->char_length > 127) {
                    BPlusTree<String256, Pointer> b_plus_tree = BPlusTree<String256, Pointer>(index_info, *buffer_manager_);
                    pointer_temp = b_plus_tree.FindTo(String256(s.c_str()), where_index[i].comparison == LESS_EQUAL);
                } else {
                    BPlusTree<String128, Pointer> b_plus_tree = BPlusTree<String128, Pointer>(index_info, *buffer_manager_);
                    pointer_temp = b_plus_tree.FindTo(String128(s.c_str()), where_index[i].comparison == LESS_EQUAL);
                }
            } else if(index_info->type == INT) {
                BPlusTree<int, Pointer> b_plus_tree = BPlusTree<int, Pointer>(index_info, *buffer_manager_);
                pointer_temp = b_plus_tree.FindTo(atoi(where_index[i].attribute_value.c_str()), where_index[i].comparison == LESS_EQUAL);
            } else if(index_info->type == FLOAT) {
                BPlusTree<float, Pointer> b_plus_tree = BPlusTree<float, Pointer>(index_info, *buffer_manager_);
                pointer_temp = b_plus_tree.FindTo(atof(where_index[i].attribute_value.c_str()), where_index[i].comparison == LESS_EQUAL);
            }
        }
        if(!i) pointer = pointer_temp;
        else pointer = Intersection(pointer, pointer_temp);
    }
    return pointer;
}

int GetAttributeNum(vector<AttributeInfo> &attr_info, string &attribute_name) {
    for(unsigned int i = 0; i < attr_info.size(); i++)
        if(attr_info[i].name == attribute_name)
            return i;
    return -1;
}

void IndexManager::Insert(string index_name, string attr_value, Pointer pointer) {
    IndexInfo *index_info = GetIndexInfo(index_name);
    if(index_info->type == CHAR) {
        string s = attr_value.substr(1, attr_value.size() - 2);
        if(index_info->char_length >127) {
            BPlusTree<String256, Pointer> b_plus_tree = BPlusTree<String256, Pointer>(index_info, *buffer_manager_);
            b_plus_tree.Insert(String256(s.c_str()), pointer);
        } else {
            BPlusTree<String128, Pointer> b_plus_tree = BPlusTree<String128, Pointer>(index_info, *buffer_manager_);
            b_plus_tree.Insert(String128(s.c_str()), pointer);
        }
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

void IndexManager::InsertIndex(TableInfo *table_info, vector<string> &attr_value, Pointer pointer) {
    for(int i = 0; i < table_info->index_num; i++) {
        int attr_num = GetAttributeNum(table_info->attribute_info, table_info->index[i].attribute_name);
        Insert(table_info->index[i].index_name, attr_value[attr_num], pointer);
    }
}

void IndexManager::InsertAllIndex(TableInfo *table_info, string index_name) {
    IndexInfo *index_info = GetIndexInfo(index_name);
    int offset = 1;
    for(unsigned int i = 0; i < table_info->attribute_info.size(); i++) {
        if(table_info->attribute_info[i].name == index_info->attribute_name) break;
        if(table_info->attribute_info[i].type == CHAR) offset += table_info->attribute_info[i].char_length;
        else offset += 4;
    }
    if(index_info->type == CHAR) {
        if(index_info->char_length > 127) {
            BPlusTree<String256, Pointer> b_plus_tree = BPlusTree<String256, Pointer>(index_info, *buffer_manager_);
            for(int i = 0; i < table_info->block_num; i++) {
                char *block = buffer_manager_->GetFileBlock(table_info->table_name + ".db", i);
                for(int j = 0; j < table_info->record_num_per_block; j++)
                    if(block[j * table_info->record_size]) {
                        char s[256] = {0};
                        memcpy(s, block + j * table_info->record_size + offset, index_info->char_length);
                        b_plus_tree.Insert(String256(s), Pointer(i, j * table_info->record_size));
                    }
            }
        } else {
            BPlusTree<String128, Pointer> b_plus_tree = BPlusTree<String128, Pointer>(index_info, *buffer_manager_);
            for(int i = 0; i < table_info->block_num; i++) {
                char *block = buffer_manager_->GetFileBlock(table_info->table_name + ".db", i);
                for(int j = 0; j < table_info->record_num_per_block; j++)
                    if(block[j * table_info->record_size]) {
                        char s[128] = {0};
                        memcpy(s, block + j * table_info->record_size + offset, index_info->char_length);
                        b_plus_tree.Insert(String128(s), Pointer(i, j * table_info->record_size));
                    }
            }
        }
    } else if(index_info->type == INT) {
        BPlusTree<int, Pointer> b_plus_tree = BPlusTree<int, Pointer>(index_info, *buffer_manager_);
        for(int i = 0; i < table_info->block_num; i++) {
            char *block = buffer_manager_->GetFileBlock(table_info->table_name + ".db", i);
            for(int j = 0; j < table_info->record_num_per_block; j++)
                if(block[j * table_info->record_size])
                    b_plus_tree.Insert(*(int *)(block + j * table_info->record_size + offset), Pointer(i, j * table_info->record_size));
        }
    } else if(index_info->type == FLOAT) {
        BPlusTree<float, Pointer> b_plus_tree = BPlusTree<float, Pointer>(index_info, *buffer_manager_);
        for(int i = 0; i < table_info->block_num; i++) {
            char *block = buffer_manager_->GetFileBlock(table_info->table_name + ".db", i);
            for(int j = 0; j < table_info->record_num_per_block; j++)
                if(block[j * table_info->record_size])
                    b_plus_tree.Insert(*(float *)(block + j * table_info->record_size + offset), Pointer(i, j * table_info->record_size));
        }
    }
    UpdateIndexInfo(index_info);
    delete index_info;
}

void IndexManager::Delete(string index_name, string attr_value) {
    IndexInfo *index_info = GetIndexInfo(index_name);
    if(index_info->type == CHAR) {
        string s = attr_value;
        if(index_info->char_length >127) {
            BPlusTree<String256, Pointer> b_plus_tree = BPlusTree<String256, Pointer>(index_info, *buffer_manager_);
            b_plus_tree.Delete(String256(s.c_str()));
        } else {
            BPlusTree<String128, Pointer> b_plus_tree = BPlusTree<String128, Pointer>(index_info, *buffer_manager_);
            b_plus_tree.Delete(String128(s.c_str()));
        }
    } else if(index_info->type == INT) {
        BPlusTree<int, Pointer> b_plus_tree = BPlusTree<int, Pointer>(index_info, *buffer_manager_);
        b_plus_tree.Delete(atoi(attr_value.c_str()));
    } else if(index_info->type == FLOAT) {
        BPlusTree<float, Pointer> b_plus_tree = BPlusTree<float, Pointer>(index_info, *buffer_manager_);
        b_plus_tree.Delete(atof(attr_value.c_str()));
    }
    UpdateIndexInfo(index_info);
    delete index_info;
}

void IndexManager::DeleteIndex(TableInfo *table_info, vector<vector<string> > &record) {
    for(int i = 0; i < table_info->index_num; i++) {
        int attr_num = GetAttributeNum(table_info->attribute_info, table_info->index[i].attribute_name);
        for(unsigned int j = 0; j < record.size(); j++)
            Delete(table_info->index[i].index_name, record[j][attr_num]);
    }
}

bool IndexManager::DropIndex(string &index_name) {
    ifstream ifs((index_name + ".idx").c_str());
    if(ifs.is_open()) {
        ifs.close();
        remove((index_name + ".idx").c_str());
        remove((index_name + ".idxinfo").c_str());
        buffer_manager_->DeleteBlock(index_name + ".idx");
        return true;
    }
    ifs.close();
    return false;
}

bool IndexManager::DropAllIndex(vector<Index> &index) {
    for(unsigned int i = 0; i < index.size(); i++)
        if(!DropIndex(index[i].index_name)) return false;
    return true;
}

void IndexManager::Terminate() {

}
