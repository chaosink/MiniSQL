#ifndef API_H
#define API_H

#include "query.h"

class API
{
public:
    API();
    ~API();
    void Init();
    void ProcessQuery(Query *query);
    void Terminate();
};

#endif // API_H
