#include <iostream>
#include <iomanip>
#include <string>
#include <fstream>
#include <sstream>
#include "interpreter.h"
#include "query.h"
#include "api.h"
#include "result.h"
using namespace std;

#define MAX(a, b) ((a) > (b) ? (a) : (b))

Interpreter::Interpreter()
{
    is_quit_ = false;
    PROMPT = _PROMPT;
    PROMPT_PART = _PROMPT_PART;
}

Interpreter::~Interpreter()
{

}

void Interpreter::Init(API *api)
{
    api_ = api;
}

void Trim(string &command)
{
    int pos = command.find_first_not_of(" \t\n");
    command.erase(0, pos);
    pos = command.find_last_not_of(" \t\n") + 1;
    command.erase(pos, command.length() - pos);
}

void PrintSeparator(vector<int> &column_length) {
    cout << "+";
    for(int i = 0; i < column_length.size(); i++) {
        for(int j = 0; j < column_length[i] + 2; j++)
            cout << "-";
        cout << "+";
    }
    cout << endl;
}

void PrintRecord(vector<string> record, vector<int> &column_length) {
    cout << "|";
    for(int i = 0; i < record.size(); i++)
        cout << setw(column_length[i] + 1) << record[i] << " |";
    cout << endl;
}

void PrintForm(ResultSelect *r) {
    vector<int> column_length;
    for(int i = 0; i < r->attribute_name.size(); i++)
        column_length.push_back(r->attribute_name[i].length());
    for(int i = 0; i < r->attribute_name.size(); i++)
        for(int j = 0; j < r->record.size(); j++)
            column_length[i] = MAX(column_length[i], r->record[j][i].length());

    PrintSeparator(column_length);
    PrintRecord(r->attribute_name, column_length);
    PrintSeparator(column_length);
    for(int i = 0; i < r->record.size(); i++) {
        PrintRecord(r->record[i], column_length);
        PrintSeparator(column_length);
    }
}

void Interpreter::RunWithInputStream(bool is_cmd, istream &is) {
    bool is_command = true;
    string command;
    if(is_cmd) Print(PROMPT);
    while(!is_quit_) {
        char ch;
        while(((ch = is.get()) == ' ' || ch == '\t') && is_command);
        if(is.eof()) break;
        if(ch == '\n') {
            if(!is_command) command += ch;
            if(is_cmd) {
                if(is_command) Print(PROMPT);
                else Print(PROMPT_PART);
            }
        } else if(ch == ';') {
            Trim(command);
            //cout << "'" << command << "'" << endl;
            Result *result = NULL;
            ParseResult parse_result;
            Query *query = ParseQuery(command, &parse_result);
            if(!parse_result.is_failed && query) {
                result = api_->ProcessQuery(query);
                cout << result->message << endl;
                if(!result->is_failed && result->type == SELECT) {
                    ResultSelect *r = (ResultSelect *)result;
                    if(!r->record.empty()) PrintForm(r);
                }
                delete result;
                delete query;
            } else
                cout << parse_result.message << endl;
            command.clear();
            is_command = true;
        } else {
            if(ch == '(' || ch == ')' || ch == ',') {
                command += " ";
                command += ch;
                command += " ";
            } else command += ch;
            is_command = false;
        }
    }
}

void Interpreter::Run()
{
    RunWithInputStream(true, cin);
}

void Interpreter::Terminate()
{

}

void Interpreter::Print(string information)
{
    cout << information;
}

void ReplaceParenthesesWithSpace(string *str)
{
    int found = str->find_first_of("()");
    while (found != -1)
    {
        (*str)[found]=' ';
        found = str->find_first_of("()", found + 1);
    }
}

bool ParseAttribute(QueryCreateTable *query, string &attr_str)
{
    ReplaceParenthesesWithSpace(&attr_str);
    istringstream iss(attr_str);
    string attr_name;
    iss >> attr_name;
    if(attr_name != "primary") {
        Attribute attribute;
        attribute.name = attr_name;
        string attr_type;
        iss >> attr_type;
        if(attr_type == "int") {
            attribute.type = INT;
            attribute.char_length = 0;
        } else if(attr_type == "float") {
            attribute.type = FLOAT;
            attribute.char_length = 0;
        } else {
            attribute.type = CHAR;
            int attr_char_length;
            iss >> attr_char_length;
            attribute.char_length = attr_char_length;
        }
        string attr_unique;
        iss >> attr_unique;
        if(!attr_unique.empty()) attribute.is_unique = true;
        else attribute.is_unique = false;
        query->attribute.push_back(attribute);
        return true;
    } else {
        string primary_attr;
        iss >> primary_attr;
        iss >> primary_attr;
        query->primary_key = primary_attr;
        return true;
    }
    return false;
}

Query *Interpreter::ParseQuery(string command, ParseResult *parse_result)
{
    parse_result->is_failed = true;
    istringstream iss(command);

    string word;
    iss >> word;
    if(word.empty()) {
        parse_result->message = "Error: No query specified!";
        return NULL;
    }

    if(word == "create") {
        iss >> word;
        if(word == "table") {
            string table_name;
            iss >> table_name;
            if(table_name.empty()) {
                parse_result->message = "Error: No table name in CREATE TABLE!";
                return NULL;
            }
            iss >> word;
            if(word != "(") {
                parse_result->message = "Error: Invalid systax in CREATE TABLE!";
                return NULL;
            }
            QueryCreateTable *query = new QueryCreateTable;
            query->table_name = table_name;
            int start = command.find_first_of('(');
            int end = start;
            while(end != -1) {
                end++;
                start = command.find_first_not_of(' ', end);
                end = command.find_first_of(',', start);
                int end_d = end;
                if(end == -1) end_d = command.find_last_of(")");
                string attr_str = command.substr(start, end_d - start);
                if(!ParseAttribute(query, attr_str)) {
                    parse_result->message = "Error: Invalid systax in table information of CREATE TABLE!";
                    delete query;
                    return NULL;
                }
            }
            if(query->primary_key.empty()) {
                parse_result->message = "Error: No primary key specified in CREATE TABLE!";
                delete query;
                return NULL;
            }
            parse_result->is_failed = false;
            return query;
        } else if(word == "index") {
            string index_name;
            iss >> index_name;
            iss >> word;
            string table_name;
            iss >> table_name;
            iss >> word;
            string attr_name;
            iss >> attr_name;
            QueryCreateIndex *query = new QueryCreateIndex;
            query = new QueryCreateIndex;
            query->index_name = index_name;
            query->table_name = table_name;
            query->attribute_name = attr_name;
            parse_result->is_failed = false;
            return query;
        }
        parse_result->message = "Error: Invalid CREATE command type!";
        return NULL;
    } else if(word == "drop") {
        iss >> word;
        if(word == "table") {
            string table_name;
            iss >> table_name;
            QueryDropTable *query = new QueryDropTable;
            query->table_name = table_name;
            parse_result->is_failed = false;
            return query;
        } else if(word == "index") {
            string index_name;
            iss >> index_name;
            QueryDropIndex *query = new QueryDropIndex;
            query->index_name = index_name;
            parse_result->is_failed = false;
            return query;
        }
    } else if(word == "select") {
        istringstream iss(command);
        string word;
        iss >> word;
        iss >> word;
        iss >> word;
        string table_name;
        iss >> table_name;
        QuerySelect *query = new QuerySelect;
        query->table_name = table_name;
        iss >> word;
        if(word == "where") {
             while(true) {
                string attr_name;
                string comparison;
                string value;
                iss >> attr_name >> comparison >> value;
                Where where;
                where.attribute_name = attr_name;
                if     (comparison == "=" ) where.comparison = EQUAL;
                else if(comparison == "<>") where.comparison = NOT_EQUAL;
                else if(comparison == "<" ) where.comparison = LESS;
                else if(comparison == ">" ) where.comparison = GREATER;
                else if(comparison == "<=") where.comparison = LESS_EQUAL;
                else if(comparison == ">=") where.comparison = GREATER_EQUAL;
                where.attribute_value = value;
                query->where.push_back(where);
                word.clear();
                iss >> word;
                if(word.empty()) break;
            }
        }
        parse_result->is_failed = false;
        return query;
    } else if(word == "insert") {
        iss >> word;
        string table_name;
        iss >> table_name;
        QueryInsert *query = new QueryInsert;
        query->table_name = table_name;
        int start = command.find_first_of('(');
        int end = start;
        while(end != -1) {
            end++;
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(',', start);
            int end_d;
            if(end == -1) end_d = command.find_first_of(")", start);
            else end_d = end;
            string attr_value = command.substr(start, end_d - start);
            Trim(attr_value);
            query->attribute_value.push_back(attr_value);
        }
        parse_result->is_failed = false;
        return query;
    } else if(word == "delete") {
        istringstream iss(command);
        string word;
        iss >> word;
        iss >> word;
        string table_name;
        iss >> table_name;
        QueryDelete *query = new QueryDelete;
        query->table_name = table_name;
        iss >> word;
        if(word == "where") {
            while(true) {
               string attr_name;
               string comparison;
               string attr_value;
               iss >> attr_name >> comparison >> attr_value;
               Where where;
               where.attribute_name = attr_name;
               if     (comparison == "=" ) where.comparison = EQUAL;
               else if(comparison == "<>") where.comparison = NOT_EQUAL;
               else if(comparison == "<" ) where.comparison = LESS;
               else if(comparison == ">" ) where.comparison = GREATER;
               else if(comparison == "<=") where.comparison = LESS_EQUAL;
               else if(comparison == ">=") where.comparison = GREATER_EQUAL;
               where.attribute_value = attr_value;
               query->where.push_back(where);
               word.clear();
               iss >> word;
               if(word.empty()) break;
           }
        }
        parse_result->is_failed = false;
        return query;
    } else if(word == "quit") {
        is_quit_ = true;
        parse_result->message = "Bye!";
        return NULL;
    } else if(word == "execfile") {
        string file_name;
        iss >> file_name;
        ifstream ifs(file_name.c_str());
        RunWithInputStream(false, ifs);
        ifs.close();
        parse_result->message = "Execfile completed!";
        return NULL;
    }
    parse_result->message = "Error: Invalid SQL command type!";
    return NULL;
}
