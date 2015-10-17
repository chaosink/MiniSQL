#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#define BUFFER_SIZE 4096

class Buffer
{
    char content_[BUFFER_SIZE];
};

class BufferManager
{
    char buffer_[];
public:
    BufferManager();
    ~BufferManager();
    void Init();
    void Init(int block_num);
    void Terminate();
};

#endif // BUFFERMANAGER_H
