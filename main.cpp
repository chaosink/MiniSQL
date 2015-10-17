#include "dbms.h"

int main()
{
    DBMS dbms;

    try
    {
        dbms.Init();
        dbms.Run();
        dbms.Terminate();
    }
    catch(Exception exception)
    {
        dbms.Print(exception.message());
    }

    return 0;
}
