#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include "query.h"
using namespace std;

#define _PROMPT "MiniSQL> "
#define _PROMPT_PART "      -> "

class Interpreter
{
    const char *PROMPT;
    const char *PROMPT_PART;
    bool is_quit_;
public:
    Interpreter();
    ~Interpreter();
    void Init();
    void Run();
    Query *Parse(string command);
    void Terminate();
    void Print(string information);
    bool Grammar(string str);
    void delete_space(string str);
};

#endif // INTERPRETER_H
