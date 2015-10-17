#ifndef EXCEPTION_H
#define EXCEPTION_H

#include <string>
using namespace std;

class Exception
{
protected:
    string message_;
public:
    Exception();
    ~Exception();
    Exception(string message);
    string message();
};

class ExceptionCommand : public Exception
{
public:
    ExceptionCommand();
    ~ExceptionCommand();
    ExceptionCommand(string message);
};

#endif // EXCEPTION_H
