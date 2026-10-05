#include <iostream>
#include <cstring>
#include "Buffer/StaticBuffer.h"
#include "Cache/OpenRelTable.h"
#include "Disk_Class/Disk.h"
#include "FrontendInterface/FrontendInterface.h"
#include "define/constants.h"

using namespace std;

int main(int argc, char *argv[]) {
  Disk disk_run;
  StaticBuffer buffer;
  OpenRelTable cache;

  return FrontendInterface::handleFrontend(argc,argv);
}













  // stage-2
  // //create objects for the relation catalog and attribute catalog
  // RecBuffer relCatBuffer(RELCAT_BLOCK);
  // RecBuffer attrCatBuffer(ATTRCAT_BLOCK);

  // HeadInfo relCatHeader;
  // HeadInfo attrCatHeader;

  // // load the headers of both the blocks into relCatHeader and attrCatHeader.
  // // (we will implement these functions later)
  // relCatBuffer.getHeader(&relCatHeader);
  // attrCatBuffer.getHeader(&attrCatHeader);

  // // for (int i=0;i<relCatHeader.numEntries;i++) {

  // //   Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog
  // //   relCatBuffer.getRecord(relCatRecord, i);
  // //   printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);
    
  // //     for(int j=0;j<attrCatHeader.numEntries;j++)/* j = 0 to number of entries in the attribute catalog */
  // //     {
  // //       Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  // //       attrCatBuffer.getRecord(attrCatRecord,j);
  // //       // declare attrCatRecord and load the attribute catalog entry into it
  // //       if (!strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)/* attribute catalog entry corresponds to the current relation */) {
  // //         const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
  // //         printf("  %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
  // //       }
  // //     }
  // //   printf("\n");
  // // }

  // for (int i=0;i<relCatHeader.numEntries;i++) {
  //   Attribute relCatRecord[RELCAT_NO_ATTRS]; // will store the record from the relation catalog
  //   relCatBuffer.getRecord(relCatRecord, i);
  //   printf("Relation: %s\n", relCatRecord[RELCAT_REL_NAME_INDEX].sVal);
  //   Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  //   int rblk=ATTRCAT_BLOCK; //attrCatHeader.rblock;
  //   while(rblk!=-1)
  //   {
  //     RecBuffer attrCatBuffer2(rblk);
  //     attrCatBuffer2.getHeader(&attrCatHeader);
  //     for(int j=0;j<attrCatHeader.numEntries;j++)/* j = 0 to number of entries in the attribute catalog */
  //     {
      
  //       attrCatBuffer2.getRecord(attrCatRecord,j);
  //       // declare attrCatRecord and load the attribute catalog entry into it
  //       // if(!strcmp(attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal,"Class") && !strcmp(relCatRecord[RELCAT_REL_NAME_INDEX].sVal,"Students")){
  //       //   unsigned char buffer[BLOCK_SIZE];
  //       //   Disk::readBlock(buffer,rblk);
  //       //   //union Attribute *ptr=buffer+HEADER_SIZE+attrCatHeader.numSlots+(j*ATTR_SIZE*attrCatHeader.numAttrs)+ATTR_SIZE;
  //       //   memcpy(buffer+HEADER_SIZE+attrCatHeader.numSlots+(j*ATTR_SIZE*attrCatHeader.numAttrs)+ATTR_SIZE,"Batch",ATTR_SIZE);
  //       //   Disk::writeBlock(buffer,rblk);
  //       // }
  //       attrCatBuffer2.getRecord(attrCatRecord,j);
  //       if (!strcmp(attrCatRecord[ATTRCAT_REL_NAME_INDEX].sVal,relCatRecord[RELCAT_REL_NAME_INDEX].sVal)/* attribute catalog entry corresponds to the current relation */) {
  //         const char *attrType = attrCatRecord[ATTRCAT_ATTR_TYPE_INDEX].nVal == NUMBER ? "NUM" : "STR";
  //         printf("  %s: %s\n", attrCatRecord[ATTRCAT_ATTR_NAME_INDEX].sVal, attrType);
  //       }
  //     }
  //     rblk=attrCatHeader.rblock;
  //   }
  //   printf("\n");
  // }

  // return 0;







  //stage-3
  
  // RelCatEntry relCatBuf;
  // AttrCatEntry attrCatBuf;

  // for(int i=0; i<MAX_OPEN; i++) {
  //   int ret=RelCacheTable::getRelCatEntry(i,&relCatBuf);
  //   if(ret!=SUCCESS){
  //     continue;
  //   }
  //   printf("Relation: %s\n", relCatBuf.relName);
  //   for(int j=0; j<relCatBuf.numAttrs; j++) {
  //     AttrCacheTable::getAttrCatEntry(i,j,&attrCatBuf);
  //     printf("  %s: %s\n", attrCatBuf.attrName, attrCatBuf.attrType==0? "NUM" : "STR");
  //   }
  // }
  
  // return 0;