#ifndef BUFFERMANAGER_H
#define BUFFERMANAGER_H

#include <string>
using namespace std;

#define BLOCK_SIZE 4096
#define DEFAULT_BLOCK_NUM 1000
#define MAX_TIME 9223372036854775807

struct BlockInfo {
    string file_name;
    int file_block_num;
    long time;
    bool is_modified;
    bool is_pined;
    BlockInfo() {
        file_name = "";
        file_block_num = -1;
        time = 0;
        is_modified = 0;
        is_pined = 0;
    }
};

class BufferManager {
    int block_num_;
    char (*block_)[BLOCK_SIZE];
    BlockInfo *block_info_;
    void ReadFileBlock(string file_name, int file_block_num, int block_num);
    void WriteFileBlock(string file_name, int file_block_num, int block_num);
    int GetAnAvailableBufferBlock();
public:
    BufferManager();
    ~BufferManager();
    void Init();
    void Init(int block_num);
    int block_num();
    void Pin(char *block_address);
    void Unpin(char *block_address);
    void SetModified(char *block_address);
    char *GetFileBlock(string file_name, int file_block_num);
    void DeleteBlock(string file_name);
    void Terminate();
};

#endif // BUFFERMANAGER_H
