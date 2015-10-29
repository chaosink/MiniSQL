#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <string>
using namespace std;

#define BLOCK_SIZE 4096
#define DEFAULT_BLOCK_NUM 10
#define MAX_TIME 2147483647

struct BufferBlockInfo
{
    string table_name;
    int table_block_num;
    long time;
    bool is_modified;
    bool is_pined;
};

class BufferManager
{
    int block_num_;
    char (*buffer_)[BLOCK_SIZE];
    BufferBlockInfo *buffer_block_info_;
public:
    BufferManager();
    ~BufferManager();
    void Init();
    void Init(int block_num);
    char *GetTableBlock(string table_name, int table_record_num);
    void ReadTableBlock(string table_name, int table_block_num, int block_num);
    void WriteTableBlock(string table_name, int table_block_num, int block_num);
    int GetAnAvailableBufferBlock();
    void Terminate();
};

#endif // BUFFERMANAGER_H
