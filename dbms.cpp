#include <string>
#include "dbms.h"
#include "interpreter.h"
#include "api.h"
#include "record_manager.h"
#include "index_manager.h"
#include "catalog_manager.h"
#include "buffer_manager.h"
#include "exception.h"
using namespace std;

DBMS::DBMS()
{
    DBMS_INFORMATION = _DBMS_INFORMATION;
}

DBMS::~DBMS()
{

}

void DBMS::Init()
{
    interpreter_.Init();
    api_.Init();
    record_manager_.Init();
    index_manager_.Init();
    catalog_manager_.Init();
    buffer_manager_.Init();
}

void DBMS::Run()
{
    PrintDBMSInformation();
    interpreter_.Run();
}

void DBMS::Terminate()
{
    buffer_manager_.Terminate();
    catalog_manager_.Terminate();
    index_manager_.Terminate();
    record_manager_.Terminate();
    api_.Terminate();
    interpreter_.Terminate();
}

void DBMS::PrintDBMSInformation()
{
    Print(DBMS_INFORMATION);
}

void DBMS::Print(string information)
{
    interpreter_.Print(information);
}
