#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#define BLOCK_SIZE 4096
#define DEFAULT_BLOCK_NUM 10

class BufferTable
{

};

class BufferManager
{
    void *buffer_;
public:
    BufferManager();
    ~BufferManager();
    void Init();
    void Init(int block_num);
    void Terminate();
};

#endif // BUFFERMANAGER_H
