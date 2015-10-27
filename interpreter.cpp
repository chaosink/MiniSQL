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
        bool is_command = false ;
        while(!is_command)
        {
            getline(cin, command_part);
            command += command_part;
            if(command.compare("quit;") == 0)
                is_quit_ = true;
            if(command_part[command_part.length() - 1] == ';')
            {
                while (command[0] == ' ')
                {
                    command.erase(0,1);
                }
                cout << command << endl;
                Grammar(command);
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


bool Interpreter::Grammar(string str)
{
     int n= str.find(' ',0);
     string s1=str.substr(0,n);
     str.erase(0,n+1);
     if (s1== 'insert')
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

}

void Interpreter::Terminate()
{

}

void Interpreter::Print(string information)
{
    cout << information;
}
