#include "dbms.h"

int main()
{
    DBMS dbms;

    dbms.Init();
    dbms.Run();
    dbms.Terminate();

    return 0;
}
