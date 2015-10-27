#include <iostream>
#include "api.h"
using namespace std;

API::API()
{

}

API::~API()
{

}

void API::Init()
{

}

void API::ProcessQuery(Query *query)
{
    if(!query) return;
    cout << query->command << endl;
    cout << ((QueryCreateTable *)query)->table_name << endl;
}

void API::Terminate()
{

}
