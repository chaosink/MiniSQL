#ifndef API_H
#define API_H

#include "record_manager.hpp"
#include "index_manager.hpp"
#include "catalog_manager.hpp"

class API {
    RecordManager *record_manager_;
    IndexManager *index_manager_;
    CatalogManager *catalog_manager_;
    bool VerifyUnique(TableInfo *table_info, vector<string> &attribute_value);
public:
    API();
    ~API();
    void Init(RecordManager *record_manager, IndexManager *index_manager, CatalogManager *catalog_manager);
    Result *ProcessQuery(Query *query);
    void Terminate();
};

#endif // API_H
