#include <iostream>
#include <string>
#include "interpreter.h"
#include "query.h"
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

void Interpreter::Init()
{

}

void Interpreter::Run()
{
    string command_part;
    string command = "";
    while(!is_quit_)
    {
        Print(PROMPT);
        bool is_command = false;
        while(!is_command)
        {
            getline(cin, command_part);
            command += command_part;
            if(command.compare("quit;") == 0)
                is_quit_ = true;
            if(command_part[command_part.length() - 1] == ';')
            {
                Parse(command);
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
            QueryCreateTable query;
            query.table_name = table_name;
            return &query;
        }
    }
}
