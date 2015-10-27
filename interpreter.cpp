#include <iostream>
#include <string>
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
            if(command == "quit;")
                is_quit_ = true;
            if(command_part[command_part.length() - 1] == ';')
            {
                //   while (command[0] == ' ')
             //   {
              //      command.erase(0,1);
            //    }
             //  Grammar(command);
                cout << command << endl;
                Query *query = Parse(command);
                api_->ProcessQuery(query);
                command = "";
                is_command = true;
            }
            if(!is_command) {
                command += "\n";
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

Query * Interpreter::Parse(string command)
{
    int start = command.find_first_not_of(' ');
    int end = command.find_first_of(' ', start);
    string command_type = command.substr(start, end - start);
    if(command_type == "create") {
        start = command.find_first_not_of(' ', end);
        end = command.find_first_of(' ', start);
        command_type = command.substr(start, end - start);
        if(command_type == "table") {
            start = command.find_first_not_of(' ', end);
            end = command.find_first_of(';', start);
            string table_name = command.substr(start, end - start);
            QueryCreateTable *query = new QueryCreateTable;
            query->table_name = table_name;
            return query;
        }
    }
    return NULL;
}
