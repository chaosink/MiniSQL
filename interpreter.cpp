#include <iostream>
#include <string>
#include "interpreter.h"
#include "exception.h"
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
                cout << command << endl;
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
