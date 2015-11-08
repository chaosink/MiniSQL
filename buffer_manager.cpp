#include "buffer_manager.h"
#include <fstream>
#include <sys/time.h>
using namespace std;

BufferManager::BufferManager() {

}

BufferManager::~BufferManager() {

}

void BufferManager::Init() {
    Init(DEFAULT_BLOCK_NUM);
}

void BufferManager::Init(int block_num) {
    block_num_ = block_num;
    block_ = (char (*)[BLOCK_SIZE])new char[block_num_ * BLOCK_SIZE];
    block_info_ = new BlockInfo[block_num_];
}

int BufferManager::block_num() {
    return block_num_;
}

char *BufferManager::GetFileBlock(string file_name, int file_block_num) {
    for(int i = 0; i < block_num_; i++)
        if(block_info_[i].file_name == file_name && block_info_[i].file_block_num == file_block_num) {
            struct timeval time;
            gettimeofday(&time, NULL);
            block_info_[i].time = time.tv_sec * 1000000 + time.tv_usec;
            return block_[i];
        }
    int block_num = GetAnAvailableBufferBlock();
    ReadFileBlock(file_name, file_block_num, block_num);
    return block_[block_num];
}

void BufferManager::Pin(char *block_address) {
    block_info_[(block_address - (char *)block_) / BLOCK_SIZE].is_pined = 1;
}

void BufferManager::Unpin(char *block_address) {
    block_info_[(block_address - (char *)block_) / BLOCK_SIZE].is_pined = 0;
}

void BufferManager::SetModified(char *block_address) {
    block_info_[(block_address - (char *)block_) / BLOCK_SIZE].is_modified = 1;
}

int BufferManager::GetAnAvailableBufferBlock() {
	int block_num;
    long time = MAX_TIME;
    for(int i = 0; i < block_num_; i++) {
        if(!block_info_[i].is_pined && block_info_[i].time < time) {
            time = block_info_[i].time;
            block_num = i;
		}
	}
    if(block_info_[block_num].is_modified)
        WriteFileBlock(block_info_[block_num].file_name, block_info_[block_num].file_block_num, block_num);
    return block_num;
}

void BufferManager::ReadFileBlock(string file_name, int file_block_num, int block_num) {
    ifstream input((file_name).c_str());
    input.seekg(file_block_num * BLOCK_SIZE, input.beg);
    input.read(block_[block_num], BLOCK_SIZE);
    input.close();
    block_info_[block_num].file_name = file_name;
    block_info_[block_num].file_block_num = file_block_num;
    struct timeval time;
    gettimeofday(&time, NULL);
    block_info_[block_num].time = time.tv_sec * 1000000 + time.tv_usec;
    block_info_[block_num].is_modified = 0;
    block_info_[block_num].is_pined = 0;
}

void BufferManager::WriteFileBlock(string file_name, int file_block_num, int block_num) {
    ofstream output((file_name).c_str(), ofstream::in | ofstream::out);
    output.seekp(file_block_num * BLOCK_SIZE, output.beg);
    output.write(block_[block_num], BLOCK_SIZE);
    output.close();
}

void BufferManager::DeleteBlock(string file_name) {
    for(int i = 0; i < block_num_; i++)
        if(block_info_[i].file_name == file_name)
            block_info_[i].file_name.clear();
}

void BufferManager::Terminate() {
    for(int i = 0; i < block_num_; i++)
        if(!block_info_[i].file_name.empty() && block_info_[i].is_modified)
            WriteFileBlock(block_info_[i].file_name, block_info_[i].file_block_num, i);
}
