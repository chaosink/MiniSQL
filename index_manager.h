#ifndef INDEXMANAGER_H
#define INDEXMANAGER_H

#include "buffer_manager.h"

class IndexManager
{
    BufferManager *buffer_manager_;
public:
    IndexManager();
    ~IndexManager();
    void Init(BufferManager *buffer_manager);
    void Terminate();
};

#endif // INDEXMANAGER_H
