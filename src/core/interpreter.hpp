#ifndef INTERPRETER_H
#define INTERPRETER_H

#include "api.hpp"
#include "query.hpp"
#include <string>
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
    const char *DBMS_INFORMATION;
    void RunWithInputStream(istream &is, string environment);
    Query *ParseQuery(string command, ParseResult *parse_result, string environment);
public:
    Interpreter();
    ~Interpreter();
    void Init(API *api, const char *dbms_info);
    void Run();
    void Terminate();
};

#endif // INTERPRETER_H
