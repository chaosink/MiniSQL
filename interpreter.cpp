#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include "interpreter.h"
#include "query.h"
#include "api.h"
using namespace std;

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

void Interpreter::Run()
{
    string command_part;
    string command = "";
    while(!is_quit_)
    {
        Print(PROMPT);
        bool is_command = false ;
        while(!is_command)
        {
            getline(cin, command_part);
            command += command_part;
            if(command_part[command_part.length() - 1] == ';')
            {
                command[command.length() - 1] = ' ';
                Query *query = Parse(command);
                api_->ProcessQuery(query);
                command = "";
                is_command = true;
            }
            if(!is_command) {
                command += " ";
                Print(PROMPT_PART);
            }
        }
    }
}

/*
bool Interpreter::Grammar(string str)
{
     int n= str.find(' ',0);
     string s1=str.substr(0,n);
     str.erase(0,n+1);
     if (s1 == "insert")
     {
         while (str[0]==' '){
                str.erase(0,1);
         }
         n=str.find(' ',0);
         string s2=str.substr(0,n);
         if (s2!='into') return false;
                 else
         {
                   str.erase(0,n+1);
                    string s3=str.substr(0,str.find(' ',0));
                    str.erase(0,str.find(' ',0)+1);
                    n=str.find('(',0);
                    string s4=str.substr(0,n);
                    while (s4[s4.length()-1]==' ')
                        s4.erase(s4.length()-1,1);
                    if (s4!='value') return false;
                    str.erase(0,n+1);
                    str.erase(str.end());
                    while (str[str.length()-1]!=')')
                    {
                        str.erase(str.end());
                    }
                    str.erase(str.end());
          }
     }
     else if (s1=='create')
     {
         str.erase(0,n+1);
         delete_space(str);
         n=str.find(' ',0);
         string s2=
     }

}*/

void Interpreter::Terminate()
{

}

void Interpreter::Print(string information)
{
    cout << information;
}

void ReplaceParenthesesWithSpace(string *str)
{
    int found = str->find_first_of("()");;
    while (found != -1)
    {
        (*str)[found]=' ';
        found = str->find_first_of("()", found + 1);
    }
}

void ParseAttribute(QueryCreateTable *query, string attr_str)
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
    } else {
        string primary_attr;
        iss >> primary_attr;
        iss >> primary_attr;
        query->primary_key = primary_attr;
    }
}

Query *Interpreter::Parse(string command)
{
    int start = command.find_first_not_of(' ');
    int end = command.find_first_of(" ;", start);
    string command_type = command.substr(start, end - start);
    if(command_type == "create") {
        start = command.find_first_not_of(' ', end);
        end = command.find_first_of(' ', start);
        command_type = command.substr(start, end - start);
        if(command_type == "table") {
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(" (", start);
            string table_name = command.substr(start, end - start);
            QueryCreateTable *query = new QueryCreateTable;
            query->table_name = table_name;
            start = command.find_first_of('(', end);
            end = start;
            while(end != -1) {
                end++;
                start = command.find_first_not_of(' ', end);
                end = command.find_first_of(',', start);
                int end_d;
                if(end == -1) end_d = command.find_first_of(")", start);
                else end_d = end;
                string attr_str = command.substr(start, end_d - start);
                ParseAttribute(query, attr_str);
            }
            return query;
        } else if(command_type == "index") {
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(' ', start);
            string index_name = command.substr(start, end - start);
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(' ', start);
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(' ', start);
            string table_name = command.substr(start, end - start);
            start = command.find_first_of('(', end);
            end = command.find_first_of(')', start);
            string attr_name = command.substr(start + 1, end - start - 1);
            QueryCreateIndex *query = new QueryCreateIndex;
            query->index_name = index_name;
            query->table_name = table_name;
            query->attribute_name = attr_name;
            return query;
        }
    } else if(command_type == "drop") {
        start = command.find_first_not_of(' ', end);
        end = command.find_first_of(' ', start);
        command_type = command.substr(start, end - start);
        if(command_type == "table") {
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(" ;", start);
            string table_name = command.substr(start, end - start);
            QueryDropTable *query = new QueryDropTable;
            query->table_name = table_name;
            return query;
        } else if(command_type == "index") {
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(" ;", start);
            string index_name = command.substr(start, end - start);
            QueryDropIndex *query = new QueryDropIndex;
            query->index_name = index_name;
            return query;
        }
    } else if(command_type == "select") {
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
        return query;
    } else if(command_type == "insert") {
        start = command.find_first_not_of(' ', end);
        end = command.find_first_of(' ', start);
        start = command.find_first_not_of(' ', end);
        end = command.find_first_of(' ', start);
        string table_name = command.substr(start, end - start);
        QueryInsert *query = new QueryInsert;
        query->table_name = table_name;
        start = command.find_first_not_of(' ', end);
        end = command.find_first_of(' ', start);
        start = command.find_first_of('(', end);
        end = start;
        while(end != -1) {
            end++;
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(',', start);
            int end_d;
            if(end == -1) end_d = command.find_first_of(")", start);
            else end_d = end;
            string attr_value = command.substr(start, end_d - start);
            query->attribute_value.push_back(attr_value);
        }
        return query;
    } else if(command_type == "delete") {
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
        return query;
    } else if(command_type == "quit") {
        is_quit_ = true;
        return NULL;
    } else if(command_type == "execfile") {
        start = command.find_first_not_of(' ', end);
        end = command.find_first_of(" ;", start);
        string file_name = command.substr(start, end - start);
        ifstream input(file_name.c_str());
        {
            string command_part;
            string command = "";
            while(!input.eof())
            {
                bool is_command = false ;
                while(!is_command)
                {
                    getline(input, command_part);
                    command += command_part;
                    if(command_part[command_part.length() - 1] == ';')
                    {
                        command[command.length() - 1] = ' ';
                        Query *query = Parse(command);
                        api_->ProcessQuery(query);
                        command = "";
                        is_command = true;
                    }
                    if(!is_command) {
                        command += " ";
                    }
                    if(input.eof()) break;
                }
            }
        }
        return NULL;
    }
    return NULL;
}
