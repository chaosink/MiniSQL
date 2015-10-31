#ifndef API_H
#define API_H

#include "query.h"
#include "record_manager.h"
#include "index_manager.h"
#include "catalog_manager.h"
#include "result.h"

class API
{
    RecordManager *record_manager_;
    IndexManager *index_manager_;
    CatalogManager *catalog_manager_;
public:
    API();
    ~API();
    void Init(RecordManager *record_manager, IndexManager *index_manager, CatalogManager *catalog_manager);
    Result *ProcessQuery(Query *query);
    void Terminate();
};

#endif // API_H
