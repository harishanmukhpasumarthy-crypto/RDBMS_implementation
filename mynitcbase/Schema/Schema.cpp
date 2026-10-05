#include "Schema.h"
#include <stdio.h>
#include <cmath>
#include <cstring>

int Schema::openRel(char relName[ATTR_SIZE]) {
  int ret = OpenRelTable::openRel(relName);

  // the OpenRelTable::openRel() function returns the rel-id if successful
  // a valid rel-id will be within the range 0 <= relId < MAX_OPEN and any
  // error codes will be negative
  if(ret >= 0){
    return SUCCESS;
  }

  //otherwise it returns an error message
  return ret;
}

int Schema::closeRel(char relName[ATTR_SIZE]) {
  if (!strcmp(RELCAT_RELNAME,relName) || !strcmp(ATTRCAT_RELNAME,relName)) {
    return E_NOTPERMITTED;
  }

  // this function returns the rel-id of a relation if it is open or
  // E_RELNOTOPEN if it is not. we will implement this later.
  int relId = OpenRelTable::getRelId(relName);

  if (relId < 0) {
    return E_RELNOTOPEN;
  }

  return OpenRelTable::closeRel(relId);
}

int Schema::renameRel(char oldRelName[ATTR_SIZE], char newRelName[ATTR_SIZE]) {
    if(!strcmp(oldRelName,RELCAT_RELNAME) || !strcmp(oldRelName,ATTRCAT_RELNAME) || !strcmp(newRelName,RELCAT_RELNAME) || !strcmp(newRelName,ATTRCAT_RELNAME)){
          return E_NOTPERMITTED;
    }
    // if the relation is open
    //    (check if OpenRelTable::getRelId() returns E_RELNOTOPEN)
    //    return E_RELOPEN

    if(OpenRelTable::getRelId(oldRelName)!=E_RELNOTOPEN){
      return E_RELOPEN;
    }
    int retVal = BlockAccess::renameRelation(oldRelName, newRelName);
    return retVal;
}


int Schema::renameAttr(char *relName, char *oldAttrName, char *newAttrName) {
  if(!strcmp(oldAttrName,RELCAT_RELNAME) || !strcmp(oldAttrName,ATTRCAT_RELNAME) || !strcmp(newAttrName,RELCAT_RELNAME) || !strcmp(newAttrName,ATTRCAT_RELNAME)){
          return E_NOTPERMITTED;
    }
    // if the relation is open
        //    (check if OpenRelTable::getRelId() returns E_RELNOTOPEN)
        //    return E_RELOPEN
    if(OpenRelTable::getRelId(oldAttrName)!=E_RELNOTOPEN){
      return E_RELOPEN;
    }
    // Call BlockAccess::renameAttribute with appropriate arguments.
    int r= BlockAccess::renameAttribute(relName,oldAttrName,newAttrName);
    return r;
    // return the value returned by the above renameAttribute() call
}