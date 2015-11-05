#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include "query.h"
using namespace std;

struct AttributeInfo {
	string name;
    int type;
	int char_length;
	bool is_unique;
};

struct IndexInfo {
    string index_name;
    string table_name;
    string attribute_name;
    int type;
    int char_length;
    int root;
    int block_num;
    int empty_block_num;
};

struct Index {
    string index_name;
    string attribute_name;
};

struct TableInfo {
	string table_name;
	int attribute_num;
	int index_num;
	int record_num;
	string primary_key;
	int record_num_per_block;
	int record_size;
	int block_num;
	vector<AttributeInfo> attribute_info;
    vector<Index> index;
};

#endif // TABLE_H
