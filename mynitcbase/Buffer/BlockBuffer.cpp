#include "BlockBuffer.h"
#include "../Disk_Class/Disk.h"
#include <cstdlib>
#include <cstring>
#include <stdio.h>

BlockBuffer::BlockBuffer(int blockNum) {
  // initialise this.blockNum with the argument
  this->blockNum=blockNum;
}

BlockBuffer::BlockBuffer(char blockType){
    // allocate a block on the disk and a buffer in memory to hold the new block of
    // given type using getFreeBlock function and get the return error codes if any.
    int btype;
    if(blockType=='R'){
      btype=REC;
    }
    else if(blockType=='I'){
      btype=IND_INTERNAL;
    }
    else if(blockType=='L'){
      btype=IND_LEAF;
    }
    else btype=UNUSED_BLK;
    int freeblk=getFreeBlock(btype);
    this->blockNum=freeblk;
    if(freeblk<0) return;
    // set the blockNum field of the object to that of the allocated block
    // number if the method returned a valid block number,
    // otherwise set the error code returned as the block number.

    // (The caller must check if the constructor allocatted block successfully
    // by checking the value of block number field.)
}

// calls the parent class constructor
RecBuffer::RecBuffer(int blockNum) : BlockBuffer::BlockBuffer(blockNum) {}
// load the block header into the argument pointer

RecBuffer::RecBuffer() : BlockBuffer('R'){}
// call parent non-default constructor with 'R' denoting record block.


int BlockBuffer::getBlockNum()
{
    // return corresponding block number.
    return this->blockNum;
}

int BlockBuffer::getHeader(struct HeadInfo *head) {
  unsigned char *bufferPtr;
  int ret=loadBlockAndGetBufferPtr(&bufferPtr);
  if(ret!=SUCCESS){
    return ret;
  }
  // populate the numEntries, numAttrs and numSlots fields in *head
  //Disk::readBlock(buffer,this->blockNum);
  memcpy(&head->numSlots, bufferPtr + 24, 4);
  memcpy(&head->numEntries, bufferPtr + 16, 4);
  memcpy(&head->numAttrs, bufferPtr + 20, 4);
  memcpy(&head->rblock, bufferPtr + 12, 4);
  memcpy(&head->lblock, bufferPtr + 8, 4);

  return SUCCESS;
}

int BlockBuffer::setHeader(struct HeadInfo *head){

    unsigned char *bufferPtr;
    // get the starting address of the buffer containing the block using
    // loadBlockAndGetBufferPtr(&bufferPtr).
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS){
      return ret;
    }
    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.

    // cast bufferPtr to type HeadInfo*
    struct HeadInfo *bufferHeader = (struct HeadInfo *)bufferPtr;

    // copy the fields of the HeadInfo pointed to by head (except reserved) to
    // the header of the block (pointed to by bufferHeader)
    //(hint: bufferHeader->numSlots = head->numSlots )
    bufferHeader->blockType=head->blockType;
    bufferHeader->lblock=head->lblock;
    bufferHeader->numAttrs=head->numAttrs;
    bufferHeader->numEntries=head->numEntries;
    bufferHeader->numSlots=head->numSlots;
    bufferHeader->pblock=head->pblock;
    bufferHeader->rblock=head->rblock;
    
    ret= StaticBuffer::setDirtyBit(this->blockNum);
    if(ret!=SUCCESS) return ret;
    // update dirty bit by calling StaticBuffer::setDirtyBit()
    // if setDirtyBit() failed, return the error code

    return SUCCESS;
}


// load the record at slotNum into the argument pointer
int RecBuffer::getRecord(union Attribute *rec, int slotNum) {
  struct HeadInfo head;
  int rel=this->getHeader(&head);
  if(rel!=SUCCESS){
    return rel;
  }
  if(slotNum>=head.numSlots){
    return E_OUTOFBOUND;
  }
  int attrCount = head.numAttrs;
  int slotCount = head.numSlots;

  unsigned char* bufferPtr;
  int ret=loadBlockAndGetBufferPtr(&bufferPtr);
  if(ret!=SUCCESS){
    return ret;
  }
  //Disk::readBlock(buffer,this->blockNum);// read the block at this.blockNum into a buffer
  /* record at slotNum will be at offset HEADER_SIZE + slotMapSize + (recordSize * slotNum)
     - each record will have size attrCount * ATTR_SIZE
     - slotMap will be of size slotCount
  */
  int recordSize = attrCount * ATTR_SIZE;
  unsigned char *slotPointer = bufferPtr+HEADER_SIZE+slotCount+(recordSize*slotNum);
  // load the record into the rec data structure
  memcpy(rec, slotPointer, recordSize);
  return SUCCESS;
}



/* NOTE: This function will NOT check if the block has been initialised as a
   record or an index block. It will copy whatever content is there in that
   disk block to the buffer.
   Also ensure that all the methods accessing and updating the block's data
   should call the loadBlockAndGetBufferPtr() function before the access or
   update is done. This is because the block might not be present in the
   buffer due to LRU buffer replacement. So, it will need to be bought back
   to the buffer before any operations can be done.
 */
int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char ** buffPtr) {
    /* check whether the block is already present in the buffer
       using StaticBuffer.getBufferNum() */
    int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

    if(bufferNum!=E_BLOCKNOTINBUFFER){
      for(int i=0;i<BUFFER_CAPACITY;i++){
        StaticBuffer::metainfo[i].timeStamp++;
      }
      StaticBuffer::metainfo[bufferNum].timeStamp=0;
    }
    else{
      bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);
      if (bufferNum == E_OUTOFBOUND) {
        return E_OUTOFBOUND;
      }
      Disk::readBlock(StaticBuffer::blocks[bufferNum],this->blockNum);
    }
    *buffPtr=(StaticBuffer::blocks[bufferNum]);
        // get a free buffer using StaticBuffer.getFreeBuffer()
        // if the call returns E_OUTOFBOUND, return E_OUTOFBOUND here as
        // the blockNum is invalid
        // Read the block into the free buffer using readBlock()
    // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr

    return SUCCESS;
}


/* used to get the slotmap from a record block
NOTE: this function expects the caller to allocate memory for `*slotMap`
*/
int RecBuffer::getSlotMap(unsigned char *slotMap) {
  unsigned char *bufferPtr;

  // get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr().
  int ret = loadBlockAndGetBufferPtr(&bufferPtr);
  if (ret != SUCCESS) {
    return ret;
  }

  struct HeadInfo head;
  // get the header of the block using getHeader() function
  getHeader(&head);

  int slotCount = head.numSlots;/* number of slots in block from header */

  // get a pointer to the beginning of the slotmap in memory by offsetting HEADER_SIZE
  unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;

  // copy the values from `slotMapInBuffer` to `slotMap` (size is `slotCount`)
  for(int i=0;i<slotCount;i++){
    slotMap[i]=*slotMapInBuffer;
    slotMapInBuffer++;
  }
  return SUCCESS;
}

int RecBuffer::setSlotMap(unsigned char *slotMap)
{
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block using
       loadBlockAndGetBufferPtr(&bufferPtr). */
    int ret = loadBlockAndGetBufferPtr(&bufferPtr);

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
    // return the value returned by the call.
    if (ret != SUCCESS)
    {
        return ret;
    }

    // get the header of the block using the getHeader() function
    struct HeadInfo head;
    this->getHeader(&head);

    int numSlots = head.numSlots;

    // the slotmap starts at bufferPtr + HEADER_SIZE. Copy the contents of the
    // argument `slotMap` to the buffer replacing the existing slotmap.
    // Note that size of slotmap is `numSlots`
    unsigned char *slotMapInBuffer = bufferPtr + HEADER_SIZE;
    memcpy(slotMapInBuffer, slotMap, numSlots);

    // update dirty bit using StaticBuffer::setDirtyBit
    // if setDirtyBit failed, return the value returned by the call
    return StaticBuffer::setDirtyBit(this->blockNum);
}

int compareAttrs(union Attribute attr1, union Attribute attr2, int attrType) {

    double diff;
    if(attrType == STRING){
      diff=strcmp(attr1.sVal,attr2.sVal);
    }
    else diff=attr1.nVal-attr2.nVal;

    if(diff>0) return 1;
    else if(diff<0)return -1;
    else if(diff==0) return 0;
}

int RecBuffer::setRecord(union Attribute *rec, int slotNum) {
    unsigned char *bufferPtr;
    /* get the starting address of the buffer containing the block
       using loadBlockAndGetBufferPtr(&bufferPtr). */

    // if loadBlockAndGetBufferPtr(&bufferPtr) != SUCCESS
        // return the value returned by the call.
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS){
      return ret;
    }
    /* get the header of the block using the getHeader() function */
    HeadInfo head;
    getHeader(&head);
    int attrs=head.numAttrs;
    int slots=head.numSlots;
    // get number of attributes in the block.
    // get the number of slots in the block.
    if(slotNum>=slots || slotNum<0){
      return E_OUTOFBOUND;
    }
    // if input slotNum is not in the permitted range return E_OUTOFBOUND.

    /* offset bufferPtr to point to the beginning of the record at required
       slot. the block contains the header, the slotmap, followed by all
       the records. so, for example,
       record at slot x will be at bufferPtr + HEADER_SIZE + (x*recordSize)
       copy the record from `rec` to buffer using memcpy
       (hint: a record will be of size ATTR_SIZE * numAttrs)
    */
   int recordsize=ATTR_SIZE*attrs;
    bufferPtr=bufferPtr+HEADER_SIZE+slots+slotNum*recordsize;
    memcpy(bufferPtr,rec,recordsize);
    // update dirty bit using setDirtyBit()
    StaticBuffer::setDirtyBit(this->blockNum);
    /* (the above function call should not fail since the block is already
       in buffer and the blockNum is valid. If the call does fail, there
       exists some other issue in the code) */
    return SUCCESS;
}


int BlockBuffer::setBlockType(int blockType){

    unsigned char *bufferPtr;
    //get the starting address of the buffer containing the block using loadBlockAndGetBufferPtr(&bufferPtr).
    int ret=loadBlockAndGetBufferPtr(&bufferPtr);
    if(ret!=SUCCESS){
      return ret;
    }
    // store the input block type in the first 4 bytes of the buffer.
    // (hint: cast bufferPtr to int32_t* and then assign it)
    // *((int32_t *)bufferPtr) = blockType;

    *(int32_t*)bufferPtr=blockType;
    StaticBuffer::blockAllocMap[this->blockNum]=blockType;
    // update the StaticBuffer::blockAllocMap entry corresponding to the object's block number to `blockType`.

    ret=StaticBuffer::setDirtyBit(this->blockNum);
    if(ret!=SUCCESS) return ret;
    // update dirty bit by calling StaticBuffer::setDirtyBit()
    // if setDirtyBit() failed
        // return the returned value from the call

    return SUCCESS;
}

int BlockBuffer::getFreeBlock(int blockType){

    // iterate through the StaticBuffer::blockAllocMap and find the block number of a free block in the disk.
    int bnum=E_DISKFULL;
    for(int i=0;i<DISK_BLOCKS;i++){
      if(StaticBuffer::blockAllocMap[i]==UNUSED_BLK){
        bnum=i;
        break;
      }
    }
    if(bnum==E_DISKFULL){   // if no block is free, return E_DISKFULL.
      return E_DISKFULL;
    }
    this->blockNum=bnum;            // set the object's blockNum to the block number of the free block.
    int freebuf=StaticBuffer::getFreeBuffer(bnum);         // find a free buffer using StaticBuffer::getFreeBuffer() .
    // initialize the header of the block passing a struct HeadInfo with values
    // pblock: -1, lblock: -1, rblock: -1, numEntries: 0, numAttrs: 0, numSlots: 0
    // to the setHeader() function.
    HeadInfo head;
    head.pblock=-1;
    head.lblock=-1;
    head.rblock=-1;
    head.numEntries=0;
    head.numAttrs=0;
    head.numSlots=0;
    int ret=this->setHeader(&head);
    if(ret!=SUCCESS) return ret;

    this->setBlockType(blockType);        // update the block type of the block to the input block type using setBlockType().
    return this->blockNum;
    // return block number of the free block.
}











//changed in stage-6
// int BlockBuffer::loadBlockAndGetBufferPtr(unsigned char **buffPtr) {
//   // check whether the block is already present in the buffer using StaticBuffer.getBufferNum()
//   int bufferNum = StaticBuffer::getBufferNum(this->blockNum);

//   if (bufferNum == E_BLOCKNOTINBUFFER) {
//     bufferNum = StaticBuffer::getFreeBuffer(this->blockNum);

//     if (bufferNum == E_OUTOFBOUND) {
//       return E_OUTOFBOUND;
//     }

//     Disk::readBlock(StaticBuffer::blocks[bufferNum], this->blockNum);
//   }

//   // store the pointer to this buffer (blocks[bufferNum]) in *buffPtr
//   *buffPtr = StaticBuffer::blocks[bufferNum];

//   return SUCCESS;
// }