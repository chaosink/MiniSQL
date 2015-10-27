#ifndef QUERY_H
#define QUERY_H

#include <vector>
#include <string>
using namespace std;

enum CommandType {CreateTable, DropTable, CreateIndex, DropIndex, Select, Insert, Delete};
enum AttributeType {Char, Int, Float};
enum ComparisonType {Equal, NotEqual, Less, Greater, LessEqual, GreaterEqual};

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
    string value;
};

struct Query
{
	CommandType command;
};

struct QueryCreateTable : Query
{
    string table_name;
    std::vector<Attribute> attribute;
	string primary_key;
};

struct QueryDropTable : Query
{
    string table_name;
};

struct QueryCreateIndex : Query
{
    string index_name;
    string table_name;
    string attribute_name;
};

struct QueryDropIndex : Query
{
    string index_name;
};

struct QuerySelect : Query
{
    string table_name;
    std::vector<string> attribute_name;
    std::vector<Where> where;
};

struct QueryDelete : Query
{
    string table_name;
    std::vector<Where> where;
};

#endif // QUERY_H
