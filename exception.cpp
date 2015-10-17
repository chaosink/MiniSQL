#include <iostream>
#include "exception.h"
using namespace std;

Exception::Exception()
{

}

Exception::~Exception()
{

}
Exception::Exception(string message)
{
    message_ = message;
}

string Exception::message()
{
    return message_;
}

ExceptionCommand::ExceptionCommand()
{

}

ExceptionCommand::~ExceptionCommand()
{

}

ExceptionCommand::ExceptionCommand(string message)
{
    message_ = message;
}
