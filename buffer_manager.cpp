#include <iostream>
#include <fstream>
#include <time.h>
#include "buffer_manager.h"
#include "table.h"
using namespace std;

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
    block_num_ = block_num;
    buffer_ = (char (*)[BLOCK_SIZE])new char[block_num_ * BLOCK_SIZE];
    buffer_block_info_ = new BufferBlockInfo[block_num_];
    for(int i = 0; i < block_num_; i++)
    {
    	buffer_block_info_[i].time = MAX_TIME;
    }
}

char *BufferManager::GetTableBlock(string table_name, int table_block_num)
{
    for(int i = 0; i < block_num_; i++)
    {
        if(buffer_block_info_[i].table_name == table_name && buffer_block_info_[i].table_block_num == table_block_num)
        {
            return buffer_[i];
        }
    }
    int block_num = GetAnAvailableBufferBlock();
    ReadTableBlock(table_name, table_block_num, block_num);
    return buffer_[block_num];
}

int BufferManager::GetAnAvailableBufferBlock()
{
	int block_num;
    long time = 0;
    for(int i = 0; i < block_num_; i++)
	{
        if(buffer_block_info_[i].time > time)
		{
            time = buffer_block_info_[i].time;
            block_num = i;
		}
	}
    WriteTableBlock(buffer_block_info_[block_num].table_name, buffer_block_info_[block_num].table_block_num, block_num);
    return block_num;
}

void BufferManager::ReadTableBlock(string table_name, int table_block_num, int block_num)
{
    ifstream input((table_name + ".db").c_str());
    input.seekg(table_block_num * BLOCK_SIZE, input.beg);
    input.read(buffer_[block_num], BLOCK_SIZE);
    input.close();
    buffer_block_info_[block_num].table_name = table_name;
    buffer_block_info_[block_num].table_block_num = table_block_num;
    buffer_block_info_[block_num].time = time(NULL);
    buffer_block_info_[block_num].is_modified = 0;
    buffer_block_info_[block_num].is_pined = 0;
}

void BufferManager::WriteTableBlock(string table_name, int table_block_num, int block_num)
{
    ofstream output((table_name + ".db").c_str(), ofstream::in | ofstream::out);
    output.seekp(table_block_num * BLOCK_SIZE, output.beg);
    output.write(buffer_[block_num], BLOCK_SIZE);
    output.close();
}

void BufferManager::Terminate()
{
    for(int i = 0; i < block_num_; i++) {
        if(!buffer_block_info_[i].table_name.empty()) {
            //cout << i << endl;
            //cout << buffer_block_info_[i].table_name << endl;
            WriteTableBlock(buffer_block_info_[i].table_name, buffer_block_info_[i].table_block_num, i);
        }
    }
}
