#include "OpenRelTable.h"
#include <cstring>
#include <stdlib.h>
#include <stdio.h>

OpenRelTableMetaInfo OpenRelTable::tableMetaInfo[MAX_OPEN];

OpenRelTable::OpenRelTable() {

  // initialize relCache and attrCache with nullptr
  for (int i = 0; i < MAX_OPEN; ++i) {
    RelCacheTable::relCache[i] = nullptr;
    AttrCacheTable::attrCache[i] = nullptr;
    tableMetaInfo[i].free=true;
  }

  /************ Setting up Relation Cache entries ************/
  // (we need to populate relation cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Relation Cache Table****/
  RecBuffer relCatBlock(RELCAT_BLOCK);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, RELCAT_SLOTNUM_FOR_RELCAT);

  struct RelCacheEntry relCacheEntry;
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry.relCatEntry);
  relCacheEntry.recId.block = RELCAT_BLOCK;
  relCacheEntry.recId.slot = RELCAT_SLOTNUM_FOR_RELCAT;

  // allocate this on the heap because we want it to persist outside this function
  RelCacheTable::relCache[RELCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[RELCAT_RELID]) = relCacheEntry;

  /**** setting up Attribute Catalog relation in the Relation Cache Table ****/
  RecBuffer relCatBlock2(RELCAT_BLOCK);

  Attribute relCatRecord2[RELCAT_NO_ATTRS];
  relCatBlock2.getRecord(relCatRecord2, RELCAT_SLOTNUM_FOR_ATTRCAT);

  struct RelCacheEntry relCacheEntry2;
  RelCacheTable::recordToRelCatEntry(relCatRecord2, &relCacheEntry2.relCatEntry);
  relCacheEntry2.recId.block=RELCAT_BLOCK;
  relCacheEntry2.recId.slot=RELCAT_SLOTNUM_FOR_ATTRCAT;

  // set up the relation cache entry for the attribute catalog similarly
  // from the record at RELCAT_SLOTNUM_FOR_ATTRCAT

  RelCacheTable::relCache[ATTRCAT_RELID] = (struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  *(RelCacheTable::relCache[ATTRCAT_RELID]) = relCacheEntry2;

  // set the value at RelCacheTable::relCache[ATTRCAT_RELID]

  /************ Setting up Attribute cache entries ************/


  // (we need to populate attribute cache with entries for the relation catalog
  //  and attribute catalog.)

  /**** setting up Relation Catalog relation in the Attribute Cache Table ****/
  RecBuffer attrCatBlock(ATTRCAT_BLOCK);

  Attribute attrCatRecord[ATTRCAT_NO_ATTRS];
  // iterate through all the attributes of the relation catalog and create a linked
  // list of AttrCacheEntry (slots 0 to 5)
  // for each of the entries, set
  //    attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  //    attrCacheEntry.recId.slot = i   (0 to 5)
  //    and attrCacheEntry.next appropriately
  // NOTE: allocate each entry dynamically using malloc

  AttrCacheEntry* head = nullptr;
  AttrCacheEntry* tail = nullptr;

  for (int i = 0; i < RELCAT_NO_ATTRS; i++) {
      AttrCacheEntry* entry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));

      attrCatBlock.getRecord(attrCatRecord, i);
      AttrCacheTable::recordToAttrCatEntry(
          attrCatRecord, &entry->attrCatEntry
      );

      entry->recId.block = ATTRCAT_BLOCK;
      entry->recId.slot = i;
      entry->next = nullptr;

      if (head == nullptr) {
          head = entry;
      } else {
          tail->next = entry;
      }

      tail = entry;
  }

  AttrCacheTable::attrCache[RELCAT_RELID] = head;

  /**** setting up Attribute Catalog relation in the Attribute Cache Table ****/
  // set up the attributes of the attribute cache similarly.
  // read slots 6-11 from attrCatBlock and initialise recId appropriately
  RecBuffer attrCatBlock2(ATTRCAT_BLOCK);

  Attribute attrCatRecord2[ATTRCAT_NO_ATTRS];
  // iterate through all the attributes of the relation catalog and create a linked
  // list of AttrCacheEntry (slots 0 to 5)
  // for each of the entries, set
  //    attrCacheEntry.recId.block = ATTRCAT_BLOCK;
  //    attrCacheEntry.recId.slot = i   (0 to 5)
  //    and attrCacheEntry.next appropriately
  // NOTE: allocate each entry dynamically using malloc

  
  AttrCacheEntry* head2 = nullptr;
  AttrCacheEntry* tail2 = nullptr;

  for (int i=RELCAT_NO_ATTRS;i<RELCAT_NO_ATTRS+ATTRCAT_NO_ATTRS;i++) {
      AttrCacheEntry* entry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));

      attrCatBlock2.getRecord(attrCatRecord2, i);
      AttrCacheTable::recordToAttrCatEntry(
          attrCatRecord2, &entry->attrCatEntry
      );

      entry->recId.block = ATTRCAT_BLOCK;
      entry->recId.slot = i;
      entry->next = nullptr;

      if (head2 == nullptr) {
          head2 = entry;
      } else {
          tail2->next = entry;
      }

      tail2 = entry;
  }

  AttrCacheTable::attrCache[ATTRCAT_RELID] = head2;



/************ Setting up tableMetaInfo entries ************/

for(int i=0;i<2;i++){
    tableMetaInfo[i].free=false;
    if(i==RELCAT_RELID)
    strcpy(tableMetaInfo[i].relName,RELCAT_RELNAME);
    if(i==ATTRCAT_RELID)
    strcpy(tableMetaInfo[i].relName,ATTRCAT_RELNAME);
}


}

/* This function will open a relation having name `relName`.
Since we are currently only working with the relation and attribute catalog, we
will just hardcode it. In subsequent stages, we will loop through all the relations
and open the appropriate one.
*/
int OpenRelTable::getRelId(char relName[ATTR_SIZE]) {

/* traverse through the tableMetaInfo array,
    find the entry in the Open Relation Table corresponding to relName.*/
    // if found return the relation id, else indicate that the relation do not have an entry in the Open Relation Table.
    for(int i=0;i<MAX_OPEN;i++){
        if(!tableMetaInfo[i].free && !strcmp(relName,tableMetaInfo[i].relName)){
            return i;
        }
    }
    return E_RELNOTOPEN;
}

int OpenRelTable::getFreeOpenRelTableEntry() {
  /* traverse through the tableMetaInfo array,
    find a free entry in the Open Relation Table.*/

    for(int i=0;i<MAX_OPEN;i++){
        if(tableMetaInfo[i].free==true){
            return i;
        }
    }
    return E_CACHEFULL;

  // if found return the relation id, else return E_CACHEFULL.
}


int OpenRelTable::openRel(char relName[ATTR_SIZE]) {
  int r=OpenRelTable::getRelId(relName);
  if(r!=E_RELNOTOPEN){
    // (checked using OpenRelTable::getRelId())
    return r;
    // return that relation id;
  }

  /* find a free slot in the Open Relation Table
     using OpenRelTable::getFreeOpenRelTableEntry(). */
  int fs=OpenRelTable::getFreeOpenRelTableEntry();
  if (fs == E_CACHEFULL){
    return E_CACHEFULL;
  }

  // let relId be used to store the free slot.
  int relId;
  relId=fs;
  /****** Setting up Relation Cache entry for the relation ******/

  /* search for the entry with relation name, relName, in the Relation Catalog using
      BlockAccess::linearSearch().
      Care should be taken to reset the searchIndex of the relation RELCAT_RELID
      before calling linearSearch().*/
  // relcatRecId stores the rec-id of the relation `relName` in the Relation Catalog.
  RecId relcatRecId;
  RelCacheTable::resetSearchIndex(RELCAT_RELID);
  Attribute relNameAttr;
  strcpy(relNameAttr.sVal, relName);
  relcatRecId = BlockAccess::linearSearch(RELCAT_RELID, (char*)"RelName", relNameAttr, EQ);
  if (relcatRecId.block == -1) {
    // (the relation is not found in the Relation Catalog.)
    return E_RELNOTEXIST;
  }

  /* read the record entry corresponding to relcatRecId and create a relCacheEntry
      on it using RecBuffer::getRecord() and RelCacheTable::recordToRelCatEntry().
      update the recId field of this Relation Cache entry to relcatRecId.
      use the Relation Cache entry to set the relId-th entry of the RelCacheTable.
    NOTE: make sure to allocate memory for the RelCacheEntry using malloc()
  */
  RecBuffer relCatBlock(relcatRecId.block);

  Attribute relCatRecord[RELCAT_NO_ATTRS];
  relCatBlock.getRecord(relCatRecord, relcatRecId.slot);

  RelCacheEntry* relCacheEntry = (RelCacheEntry*)malloc(sizeof(RelCacheEntry));
  RelCacheTable::recordToRelCatEntry(relCatRecord, &relCacheEntry->relCatEntry);
  relCacheEntry->recId.block = relcatRecId.block;
  relCacheEntry->recId.slot = relcatRecId.slot;
  relCacheEntry->searchIndex = RecId{-1,-1};
  // allocate this on the heap because we want it to persist outside this function
  RelCacheTable::relCache[relId] = relCacheEntry;
  /****** Setting up Attribute Cache entry for the relation ******/
  //RecBuffer attrCatBlock(ATTRCAT_BLOCK);
  struct AttrCacheEntry *head=nullptr,*tail=nullptr;
  RelCacheTable::resetSearchIndex(ATTRCAT_RELID);

  /*iterate over all the entries in the Attribute Catalog corresponding to each
  attribute of the relation relName by multiple calls of BlockAccess::linearSearch()
  care should be taken to reset the searchIndex of the relation, ATTRCAT_RELID,
  corresponding to Attribute Catalog before the first call to linearSearch().*/

  while (true) {

    /* let attrcatRecId store a valid record id an entry of the relation, relName,
      in the Attribute Catalog.*/

      RecId attrcatRecId;

      /* read the record entry corresponding to attrcatRecId and create an
      Attribute Cache entry on it using RecBuffer::getRecord() and
      AttrCacheTable::recordToAttrCatEntry().
      update the recId field of this Attribute Cache entry to attrcatRecId.
      add the Attribute Cache entry to the linked list of listHead .*/
      // NOTE: make sure to allocate memory for the AttrCacheEntry using malloc()
    attrcatRecId = BlockAccess::linearSearch(ATTRCAT_RELID, (char*)"RelName", relNameAttr, EQ);
    if (attrcatRecId.block == -1) break;

      RecBuffer attrBuf(attrcatRecId.block);
      Attribute attrRec[ATTRCAT_NO_ATTRS];
      attrBuf.getRecord(attrRec, attrcatRecId.slot);


      AttrCacheEntry* entry = (AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
      AttrCacheTable::recordToAttrCatEntry(attrRec, &entry->attrCatEntry);
      entry->recId = attrcatRecId;
      entry->searchIndex = {-1, -1};
      entry->next = nullptr;
      if (!head) {
          head = tail = entry;
      } else {
          tail->next = entry;
          tail = entry;
      }
  }

  AttrCacheTable::attrCache[relId] = head;
  // set the relIdth entry of the AttrCacheTable to listHead.

  /****** Setting up metadata in the Open Relation Table for the relation******/

  // update the relIdth entry of the tableMetaInfo with free as false and
  // relName as the input.
  tableMetaInfo[relId].free = false;
  strcpy(tableMetaInfo[relId].relName, relName);

  return relId;
}

int OpenRelTable::closeRel(int relId) {
  if (relId==ATTRCAT_RELID) {
    return E_NOTPERMITTED;
  }

  if (relId==RELCAT_RELID) {
    return E_NOTPERMITTED;
  }

  if (tableMetaInfo[relId].free==true) {
    return E_RELNOTOPEN;
  }
  /****** Releasing the Relation Cache entry of the relation ******/
  
  if(RelCacheTable::relCache[relId]->dirty==true){
      /* Get the Relation Catalog entry from RelCacheTable::relCache
      Then convert it to a record using RelCacheTable::relCatEntryToRecord(). */
      Attribute attr[RELCAT_NO_ATTRS];
      RelCatEntry relcat=RelCacheTable::relCache[relId]->relCatEntry;
      RelCacheTable::relCatEntryToRecord(&relcat,attr);
      RecId recId=RelCacheTable::relCache[relId]->recId;

      // declaring an object of RecBuffer class to write back to the buffer
      RecBuffer relCatBlock(recId.block);

      // Write back to the buffer using relCatBlock.setRecord() with recId.slot
      relCatBlock.setRecord(attr,recId.slot);
  }

  free(RelCacheTable::relCache[relId]);
  RelCacheTable::relCache[relId]=NULL;

  /****** Releasing the Attribute Cache entry of the relation ******/

  // free the memory allocated in the attribute caches which was
  // allocated in the OpenRelTable::openRel() function

  // (because we are not modifying the attribute cache at this stage,
  // write-back is not required. We will do it in subsequent
  // stages when it becomes needed)

  AttrCacheEntry *curr = AttrCacheTable::attrCache[relId];
  while (curr != nullptr) {
    AttrCacheEntry *next = curr->next;
    free(curr);
    curr = next;
  }
  AttrCacheTable::attrCache[relId]=NULL;

  tableMetaInfo[relId].free=true;
  return SUCCESS;
}


OpenRelTable::~OpenRelTable() {
  // free all the memory that you allocated in the constructor
  // close all open relations (from rel-id = 2 onwards. Why?)
  for (int i = 2; i < MAX_OPEN; ++i) {
    if (!tableMetaInfo[i].free) {
      OpenRelTable::closeRel(i); // we will implement this function later
    }
  }

  // free the memory allocated for rel-id 0 and 1 in the caches

  free(RelCacheTable::relCache[RELCAT_RELID]);
  free(RelCacheTable::relCache[ATTRCAT_RELID]);
  RelCacheTable::relCache[RELCAT_RELID] = nullptr;
  RelCacheTable::relCache[ATTRCAT_RELID] = nullptr;

  AttrCacheEntry *curr = AttrCacheTable::attrCache[RELCAT_RELID];
  while (curr != nullptr) {
    AttrCacheEntry *next = curr->next;
    free(curr);
    curr = next;
  }
  AttrCacheTable::attrCache[RELCAT_RELID] = nullptr;
  curr = AttrCacheTable::attrCache[ATTRCAT_RELID];
  while (curr != nullptr) {
    AttrCacheEntry *next = curr->next;
    free(curr);
    curr = next;
  }
  AttrCacheTable::attrCache[ATTRCAT_RELID] = nullptr;  

}









//stage-3
//in constructor



// loading students into cache

// // find Students' record in the relation catalog
// int studentsSlot=-1;
// Attribute studentsRelRecord[RELCAT_NO_ATTRS];
// HeadInfo header;
// relCatBlock.getHeader(&header);
// for(int slot=0;slot<header.numSlots;slot++){
//     if(relCatBlock.getRecord(studentsRelRecord,slot)!=SUCCESS){
//         break;
//     }
//     if(strcmp(studentsRelRecord[0].sVal,"Students")==0){
//         studentsSlot=slot;
//         break;
//     }
// }
// if(studentsSlot==-1){
//   return;
// }

// // cache the relation catalog entry for Students
// struct RelCacheEntry studentsRelCacheEntry;
// RelCacheTable::recordToRelCatEntry(studentsRelRecord,&studentsRelCacheEntry.relCatEntry);
// studentsRelCacheEntry.recId.block=RELCAT_BLOCK;
// studentsRelCacheEntry.recId.slot=studentsSlot;

// RelCacheTable::relCache[studentsSlot]=(struct RelCacheEntry*)malloc(sizeof(RelCacheEntry));
// *(RelCacheTable::relCache[studentsSlot])=studentsRelCacheEntry;

// // cache the attribute catalog entries for Students
// int numAttrs=studentsRelCacheEntry.relCatEntry.numAttrs;
// int found=0;
// int slot=0;

// struct AttrCacheEntry *head3=nullptr;
// struct AttrCacheEntry *tail3=nullptr;

// while(found<numAttrs){
//     Attribute studentsAttrRecord[ATTRCAT_NO_ATTRS];

//     if(attrCatBlock.getRecord(studentsAttrRecord,slot)!=SUCCESS){
//         break;
//     }

//     if(strcmp(studentsAttrRecord[0].sVal,"Students")==0){
//         struct AttrCacheEntry *attr=(struct AttrCacheEntry*)malloc(sizeof(AttrCacheEntry));
//         attr->next=nullptr;
//         attr->dirty=false;
//         attr->searchIndex={-1,-1};
//         attr->recId.block=ATTRCAT_BLOCK;
//         attr->recId.slot=slot;
//         AttrCacheTable::recordToAttrCatEntry(studentsAttrRecord,&attr->attrCatEntry);

//         if(head3==nullptr){
//             head3=attr;
//             tail3=attr;
//         }
//         else{
//             tail3->next=attr;
//             tail3=attr;
//         }
//         found++;
//     }

//     slot++;
// }
// AttrCacheTable::attrCache[studentsSlot]=head3;


//getrelid before stage-5
  // // if relname is RELCAT_RELNAME, return RELCAT_RELID
  // // if relname is ATTRCAT_RELNAME, return ATTRCAT_RELID
  // if(!strcmp(relName,RELCAT_RELNAME))
  // return RELCAT_RELID;

  // if(!strcmp(relName,ATTRCAT_RELNAME))
  // return ATTRCAT_RELID;

  // RelCatEntry relCatBuf;
  // //AttrCatEntry attrCatBuf;

  // for(int i=0; i<MAX_OPEN; i++) {
  //   int ret=RelCacheTable::getRelCatEntry(i,&relCatBuf);
  //       if(ret!=SUCCESS){
  //       continue;
  //       }
  //       if(!strcmp(relName,relCatBuf.relName)){
  //           return i;
  //       }
    
  //   }
  //    return E_RELNOTOPEN;