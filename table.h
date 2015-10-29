#ifndef TABLE_H
#define TABLE_H

#include <vector>
#include "query.h"
using namespace std;

struct AttributeInfo
{
	string name;
	AttributeType type;
	int char_length;
	bool is_unique;
};

struct TableInfo
{
	string table_name;
	int attribute_num;
	int record_num;
	string primary_key;
	int record_num_per_block;
	int record_size;
	int block_num;
	vector<AttributeInfo> attribute_info;
};

#endif // TABLE_H
