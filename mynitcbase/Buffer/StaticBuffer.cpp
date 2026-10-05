#include "StaticBuffer.h"
#include <stdio.h>
#include <cstring>
// the declarations for this class can be found at "StaticBuffer.h"

unsigned char StaticBuffer::blocks[BUFFER_CAPACITY][BLOCK_SIZE];
struct BufferMetaInfo StaticBuffer::metainfo[BUFFER_CAPACITY];
unsigned char StaticBuffer::blockAllocMap[DISK_BLOCKS];

StaticBuffer::StaticBuffer() 
{
  unsigned char buffer[BLOCK_SIZE];
  for(int i=0;i<BLOCK_ALLOCATION_MAP_SIZE;i++)
  {
    Disk::readBlock(buffer,i);
    memcpy(blockAllocMap+i*BLOCK_SIZE,buffer,BLOCK_SIZE);
  }
  for (int i=0; i<BUFFER_CAPACITY; i++) 
  {
    metainfo[i].free=true;
    metainfo[i].dirty=false;
    metainfo[i].timeStamp=-1;
    metainfo[i].blockNum=-1;

  }
}

// write back all modified blocks on system exit
StaticBuffer::~StaticBuffer() {
  unsigned char buffer[BLOCK_SIZE];
  for(int i=0;i<BLOCK_ALLOCATION_MAP_SIZE;i++)
  {
    memcpy(buffer,blockAllocMap+i*BLOCK_SIZE,BLOCK_SIZE);
    Disk::writeBlock(buffer,i);
  }
  for(int i=0;i<BUFFER_CAPACITY;i++){
    if(!metainfo[i].free && metainfo[i].dirty){
      Disk::writeBlock(blocks[i],metainfo[i].blockNum);
    }
  }
}


int StaticBuffer::getFreeBuffer(int blockNum){
  if (blockNum < 0 || blockNum >= DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }
    // increase the timeStamp in metaInfo of all occupied buffers.
  for(int i=0;i<BUFFER_CAPACITY;i++){
    if(!metainfo[i].free){
      metainfo[i].timeStamp++;
    }
  }
    // let bufferNum be used to store the buffer number of the free/freed buffer.
    int bufferNum=E_OUTOFBOUND;

    // iterate through metainfo and check if there is any buffer free
    // if a free buffer is available, set bufferNum = index of that free buffer.
  for(int i=0;i<BUFFER_CAPACITY;i++){
    if(metainfo[i].free==true){
        bufferNum=i;
        break;
    }
  }
    // if a free buffer is not available,
    //     find the buffer with the largest timestamp
    //     IF IT IS DIRTY, write back to the disk using Disk::writeBlock()
    //     set bufferNum = index of this buffer
  if(bufferNum==E_OUTOFBOUND){
    int max=-1;
    for(int i=0;i<BUFFER_CAPACITY;i++){
      if(metainfo[i].timeStamp>max){
        max=metainfo[i].timeStamp;
        bufferNum=i;
      }
    }
    if(metainfo[bufferNum].dirty){
      Disk::writeBlock(blocks[bufferNum],metainfo[bufferNum].blockNum);
    }
  }
    // update the metaInfo entry corresponding to bufferNum with free:false, dirty:false, blockNum:the input block number, timeStamp:0.
    metainfo[bufferNum].free=false;
    metainfo[bufferNum].dirty=false;
    metainfo[bufferNum].timeStamp=0;
    metainfo[bufferNum].blockNum=blockNum;
    return bufferNum;
    // return the bufferNum.
}

/* Get the buffer index where a particular block is stored
   or E_BLOCKNOTINBUFFER otherwise
*/
int StaticBuffer::getBufferNum(int blockNum) {
  // Check if blockNum is valid (between zero and DISK_BLOCKS)
  // and return E_OUTOFBOUND if not valid.
  if(blockNum < 0 || blockNum >= DISK_BLOCKS) {
    return E_OUTOFBOUND;
  }
  // find and return the bufferIndex which corresponds to blockNum (check metainfo)
  for(int i=0; i<BUFFER_CAPACITY; i++){
    if(metainfo[i].blockNum==blockNum){
        return i;
    }
  }
  // if block is not in the buffer
  return E_BLOCKNOTINBUFFER;
}

int StaticBuffer::setDirtyBit(int blockNum){
    // find the buffer index corresponding to the block using getBufferNum().
  int bufferNum = getBufferNum(blockNum);
    // if block is not present in the buffer (bufferNum = E_BLOCKNOTINBUFFER)
    //     return E_BLOCKNOTINBUFFER
  
    // if blockNum is out of bound (bufferNum = E_OUTOFBOUND)
    //     return E_OUTOFBOUND
  if(bufferNum==E_BLOCKNOTINBUFFER){
    return E_BLOCKNOTINBUFFER;
  }
  if(bufferNum==E_OUTOFBOUND){
    return E_OUTOFBOUND;
  }
  else{
    metainfo[bufferNum].dirty=true;
  }
    // else
    //     (the bufferNum is valid)
    //     set the dirty bit of that buffer to true in metainfo

  return SUCCESS;
}























//changed in stage-6;

// int StaticBuffer::getFreeBuffer(int blockNum) {
//   if (blockNum < 0 || blockNum >= DISK_BLOCKS) {
//     return E_OUTOFBOUND;
//   }
//   int allocatedBuffer=E_OUTOFBOUND;

//   // iterate through all the blocks in the StaticBuffer
//   // find the first free block in the buffer (check metainfo)
//   // assign allocatedBuffer = index of the free block
//   for(int i=0; i<BUFFER_CAPACITY; i++){
//     if(metainfo[i].free==true){
//         allocatedBuffer=i;
//         break;
//     }
//   }
//   if (allocatedBuffer == E_OUTOFBOUND) {
//     return E_OUTOFBOUND;
//   }
//   metainfo[allocatedBuffer].free = false;
//   metainfo[allocatedBuffer].blockNum = blockNum;

//   return allocatedBuffer;
// }