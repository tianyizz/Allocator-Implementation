/*
Tianyi Zhang
ECE2230-001
Fall
Assignment 2
*/

#define _GNU_SOURCE

#include <fcntl.h>
#include <stdlib.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/mman.h>
#include <string.h>
#include "list.h"


//24bytes
struct list_node{
  list_data data;
  struct list_node* rightLink;
  struct list_node* leftLink;
};

//24bytes
struct list{
  aNode* head;
  aNode* tail;
  aNode* current;
};

int fd;

/** Allocates and initializes a new empty list.
*
*  Returns a new empty list or NULL if there was an error.
*/
list list_init(void){
  fd=open("/dev/zero",O_RDONLY);
  list newLI=mmap(NULL,STRUCTSIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE,fd,0);


  if(newLI==NULL)return NULL;

  //INITIALIZE EVERYTHING TO NULL
  newLI->head=NULL;
  newLI->tail=NULL;
  newLI->current=NULL;

  return newLI;
}
/* Destroys the list and frees its memory.
*
*  Frees all memory allocated for the list. List data is not freed.
*/
void list_finalize(list lst){
	  while( lst->head!=NULL) {
	      aNode *hold = lst->head->rightLink;
	      munmap(lst->head,STRUCTSIZE);
	      lst->head = hold;
	  }
	  munmap(lst,STRUCTSIZE);
}
/** Inserts at the head of the list.
*
*  DATA becomes the new head of the list and DATA's successor is the
*  previous head of the list. DATA must not be NULL.
*
*  The current item is set to DATA.
*
*  On error the list is not modified.
*
*  Returns DATA or NULL on error.
*/
list_data list_insert(list lst, list_data data){
  if(data==NULL) return NULL;
  else{
    aNode* newNode=mmap(NULL,STRUCTSIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE,fd,0);
    newNode->data=data;
    //SETUP NEW NODE AS A NEW HEAD
    newNode->rightLink=lst->head;
    newNode->leftLink=NULL;
    //CHECK IF HEAD EXIST, SETUP THE OLD HEAD NODE
    if(lst->head!=NULL)lst->head->leftLink=newNode;
    lst->head=newNode;
    lst->current=newNode;
    //CHECK IF ONE ELEMENT, SET TO TAIL
    if(newNode->rightLink==NULL)lst->tail=newNode;
  }

  //printf("done with list operation\n");
  //printf("inserted value %d\n",*(int*)newNode->data);

  return data;
}
/** Appends to the tail of the list.
*
*  DATA becomes the new tail and DATA's predecessor is the previous
*  tail of the list. DATA must not be NULL.
*
*  The current item is set to DATA.
*
*  On error the list is not modified.
*
*  Returns DATA or NULL on error.
*/
list_data list_append(list lst, list_data data){
  if(data==NULL) return NULL;
  else{
    aNode* newNode=mmap(NULL,STRUCTSIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE,fd,0);


    //SETUP A NEW TAIL NODE
    newNode->data=data;
    newNode->rightLink=NULL;

    //CHANGE TAIL IF EXISTED
    if(lst->tail!=NULL)
      lst->tail->rightLink=newNode;

    //FINISH SETTIING UP AND MOVE CURRENT POINTER
    newNode->leftLink=lst->tail;
    lst->tail=newNode;
    lst->current=newNode;
    if(newNode->leftLink==NULL)lst->head=newNode;
  }
  return data;
}
/** Inserts before the current item.
*
*  DATA's predecessor will be the current item's predecessor. DATA's
*  successor will be the current item. DATA must not be NULL.
*
*  The current item becomes DATA.
*
*  It is an error if the current item is NULL. On error the list is
*  not modified.
*
*  Returns DATA or NULL on error.
*/
list_data list_insert_before(list lst, list_data data){
  aNode* temp=lst->current;
  //CHECK FOR NULLS
  if(lst->current==NULL||data==NULL) return NULL;
  else{
    aNode* newNode=mmap(NULL,STRUCTSIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE,fd,0);
/*
    if (newNode==NULL)
    {
      fprintf(stderr, "unsuccessful initializes\n");
    }
    else
      fprintf(stderr, "good initializes\n");
*/
    newNode->data=data;

    //SETUP NEW NODE DATA BEFORE MODIFYING CURRENT NODE
    newNode->rightLink=temp;
    newNode->leftLink=temp->leftLink;

    //SETUP NEIGHBOUR NODE
    temp=lst->current->leftLink;
    if(temp!=NULL)
    temp->rightLink=newNode;

    //CHANGE CURRENT
    lst->current->leftLink=newNode;
    lst->current=newNode;
    if(newNode->leftLink==NULL)lst->head=newNode;
    else if(newNode->rightLink==NULL)lst->tail=newNode;
  }

  return data;
}
/** Inserts after the current item.
*
*  DATA's predecessor will be the current item. DATA's successor will
*  be the current item's successor.  DATA must not be NULL.
*
*  The current item becomes DATA.
*
*  It is an error if the current item is NULL. On error the list is
*  not modified.
*
*  Returns DATA or NULL on error.
*/
list_data list_insert_after(list lst, list_data data){
  aNode* temp=lst->current;
  if(lst->current==NULL||data==NULL) return NULL;
  else{

    //SAME THINGS AS BEFORE BUT WITH DIFFERENT DIRECTION
    aNode* newNode=mmap(NULL,STRUCTSIZE,PROT_READ|PROT_WRITE|PROT_EXEC,MAP_PRIVATE,fd,0);
    newNode->data=data;
    newNode->rightLink=temp->rightLink;
    newNode->leftLink=temp;

    //CHECK RIGHT NODE OF THE CURRENT NODE INSTEAD OF LEFT
    temp=lst->current->rightLink;
    if(temp!=NULL)
    temp->leftLink=newNode;
    lst->current->rightLink=newNode;
    lst->current=newNode;
    if(newNode->leftLink==NULL)lst->head=newNode;
    else if(newNode->rightLink==NULL)lst->tail=newNode;
  }

  return data;
}
/** Removes the current item from the list.
*
*  Sets the new current item to the successor of the removed current
*  item. Does nothing if the current item is NULL.
*
*  Returns the removed item (may be NULL).
*/
list_data list_remove(list lst){
  if(lst->current==NULL) return NULL;
  else{
    list_data tempData=lst->current->data;

    //IF THE NODE IS A HEAD
    if(lst->current!=lst->head)
      lst->current->leftLink->rightLink=lst->current->rightLink;
    else{
      lst->head=lst->current->rightLink;
      if(lst->head!=NULL)
      lst->head->leftLink=NULL;
    }

    //IF THE NODE IS A HEAD
    if(lst->current!=lst->tail)
      lst->current->rightLink->leftLink=lst->current->leftLink;
    else{
      lst->tail=lst->current->leftLink;
      if(lst->tail!=NULL)
      lst->tail->rightLink=NULL;
    }

    aNode* tempNode=lst->current;
    lst->current=lst->current->rightLink;
    munmap(tempNode,STRUCTSIZE);
    return tempData;
  }
}
/** Returns the list head.
*
*  Sets the current item to the list head or NULL.
*
*  Returns the list head or NULL if the list is empty.
*/
list_data list_first(list lst){
  lst->current=lst->head;
  if(lst->head==NULL) return NULL;
  return lst->current->data;
}
/** Moves the current item forward in the list.
*
*  Sets the new current item to the successor of the current item. If
*  the current item is the list tail or the current item is NULL then
*  the new current item is NULL.
*
*  Returns the new current item.
*/
list_data list_next(list lst){
  if(lst->current!=NULL){
    lst->current=lst->current->rightLink;
    if(lst->current!=NULL)
    return lst->current->data;
  }

  return NULL;
}
/** Moves the current item backward in the list.
*
*  Sets the new current item to the predecessor of the current
*  item. If the current item is the list head or the current item is
*  NULL then the new current item is NULL.
*
*  Returns the new current item.
*/
list_data list_prev(list lst){
  if(lst->current!=NULL){
    lst->current=lst->current->leftLink;
    if(lst->current!=NULL)
    return lst->current->data;
  }
  return NULL;
}
/** Returns the list tail.
*
*  Sets the current item to the list tail or NULL.
*
*  Returns the list tail or NULL if the list is empty.
*/
list_data list_last(list lst){
  lst->current=lst->tail;
  if(lst->current==NULL) return NULL;
  return lst->current->data;
}
/** Returns the current item of the list.
*
*  Returns the current item of LST (may return NULL).
*/
list_data list_curr(list lst){
  if(lst->current==NULL) return NULL;
  return lst->current->data;
}
