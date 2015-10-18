#include "buffer_manager.h"

BufferManager::BufferManager()
{

}

BufferManager::~BufferManager()
{

}

void BufferManager::Init()
{
    Init(DEFAULT_BLOCK_NUM);
}

void BufferManager::Init(int block_num)
{
    buffer_ = new char[block_num * BLOCK_SIZE];
}

void BufferManager::Terminate()
{

}
