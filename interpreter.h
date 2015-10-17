#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
using namespace std;

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
    void Terminate();
    void Print(string information);
};

#endif // INTERPRETER_H
