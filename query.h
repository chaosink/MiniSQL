#ifndef QUERY_H
#define QUERY_H

#include <vector>
#include <string>
using namespace std;

enum CommandType {CREATE_TABLE, DROP_TABLE, CREATE_INDEX, DROP_INDEX, SELECT, INSERT, DELETE};
enum AttributeType {CHAR, INT, FLOAT};
enum ComparisonType {EQUAL, NOT_EQUAL, LESS, GREATER, LESS_EQUAL, GREATER_EQUAL};

struct Attribute
{
	string name;
	AttributeType type;
	int char_length;
	bool is_unique;
};

struct Where
{
    string attribute_name;
    ComparisonType comparison;
    string attribute_value;
};

struct Query
{
	CommandType command;
};

struct QueryCreateTable : Query
{
    string table_name;
    vector<Attribute> attribute;
	string primary_key;
    QueryCreateTable()
    {
    	command = CREATE_TABLE;
    }
};

struct QueryDropTable : Query
{
    string table_name;
    QueryDropTable()
    {
    	command = DROP_TABLE;
    }
};

struct QueryCreateIndex : Query
{
    string index_name;
    string table_name;
    string attribute_name;
    QueryCreateIndex()
    {
        command = CREATE_INDEX;
    }
};

struct QueryDropIndex : Query
{
    string index_name;
    QueryDropIndex()
    {
    	command = DROP_INDEX;
    }
};

struct QuerySelect : Query
{
    string table_name;
    vector<string> attribute_name; // 如果为*则留空
    vector<Where> where;
    QuerySelect()
    {
    	command = SELECT;
    }
};

struct QueryInsert : Query
{
    string table_name;
    vector<string> attribute_value;
    QueryInsert()
    {
		command = INSERT;    	
    }
};

struct QueryDelete : Query
{
    string table_name;
    vector<Where> where;
    QueryDelete()
    {
    	command = DELETE;
    }
};

#endif // QUERY_H
