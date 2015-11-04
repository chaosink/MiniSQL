#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <string>
#include "api.h"
#include "query.h"
using namespace std;

#define _PROMPT "MiniSQL> "
#define _PROMPT_PART "      -> "

struct ParseResult {
    bool is_failed;
    string message;
};

class Interpreter {
    const char *PROMPT;
    const char *PROMPT_PART;
    bool is_quit_;
    API *api_;
public:
    Interpreter();
    ~Interpreter();
    void Init(API *api);
    void Run();
    void RunWithInputStream(bool is_cmd, istream &is, string environment);
    Query *ParseQuery(string command, ParseResult *parse_result, string environment);
    void Terminate();
    void Print(string information);
    void delete_space(string str);
};

#endif // INTERPRETER_H
