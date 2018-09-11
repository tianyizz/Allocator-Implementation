#define _GNU_SOURCE

//desctructor and constructor of the library
void __attribute__ ((constructor)) alloc_init(void);
void __attribute__ ((destructor)) alloc_cleanup(void);

#include <fcntl.h>
#include <stdlib.h>
#include <sys/mman.h>
#include <string.h>
#include <stdio.h>
#include <dlfcn.h>
#include <stdbool.h>

#define PAGESIZE 4096
#define MAXSIZE 1024
#define MAXARRAY 10

//24 bytes
typedef struct meta{
	int size;
	void* freeAddr;
	struct meta* rightLink;
}m;

typedef struct tag{
	bool isFree;
	void* nextAvail;
	m* header;
}t;

m* sizeBag[MAXARRAY]={NULL};
int fd;

//Power function looking for the closest 2's power
//(input cannot be 0) This is for roundup purposes only
int toTwoPower (int input){
	if(input==1){
		return 2;
	}

	int count=1;
	for(;count<input;count*=2);
	return count;
}

//must called with the return from toTwoPower
int toTwoExp(size_t input){
	int count=0;
	int i=(int)input;
	for(;i>1;i/=2){
		count++;
	}

	return count;
}	

//Get new page (smaller or equal to the max size)
m* getNewPage(int size){
	m* page=mmap(NULL,PAGESIZE,PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0);  
	page->size=size;
	page->freeAddr=(void*)page+sizeof(m);
	page->rightLink=NULL;

	//arranging the new page with tag and sections of memories
	int elemSize=sizeof(t)+size;
	int totalMemUsed=elemSize+sizeof(m);
	void* curLoc=page->freeAddr;

	while(true){
		t* newTag=curLoc;
		newTag->isFree=true;
		newTag->nextAvail=NULL;
		newTag->header=page;

		totalMemUsed+=elemSize;
		curLoc=curLoc+elemSize;

		if(totalMemUsed>PAGESIZE)
			break;

		newTag->nextAvail=curLoc;
	}


	return page;
}

//Get irregular large page
m* getNewLargePage(int size){
	m* page=mmap(NULL,size+sizeof(m)+sizeof(t),PROT_READ|PROT_WRITE,MAP_PRIVATE,fd,0);  
	page->size=size;
	page->freeAddr=(void*)page+sizeof(m);
	page->rightLink=NULL;

	t* newTag=page->freeAddr;
	newTag->isFree=false;
	newTag->nextAvail=NULL;
	newTag->header=page;

	return page;
}

//2's Power function can't take 0
int twoPower(int i){
	int count=1;
	while(i--){
		count*=2;
	}

	return count;
}

//Initializing library and take over the controls
void alloc_init(void){
  fd=open("/dev/zero",O_RDONLY);

  for(int i=0;i<MAXARRAY;i++){
  	sizeBag[i]=getNewPage(twoPower(i+1));
  }
}

//malloc
void *malloc(size_t c){
	
  //if malloced 0 size
  if(c==0){
    return NULL;
  }

  //malloc large page size
  if(c>MAXSIZE){
  	m*temp=getNewLargePage(c);
  	return ((void*)temp+sizeof(m)+sizeof(t));
  }

  //regular alloc
  c=toTwoPower(c);
  int array_id=toTwoExp(c)-1;
  void* returnAddr=NULL;
  m* temp=sizeBag[array_id];

  //check if any free space exists or take the next available position
  for(;temp!=NULL;temp=temp->rightLink){
  	if(temp->freeAddr!=NULL)
  		break;
  
  	if(temp->rightLink==NULL){
  		m* newPage=getNewPage(c);
  		temp->rightLink=newPage;
  	}
  }

  t* tempTag=temp->freeAddr;
  returnAddr=(void*)(temp->freeAddr)+sizeof(t);
  tempTag->isFree=false;
  temp->freeAddr=tempTag->nextAvail;
  return returnAddr;
}

//free function
void free (void *ptr){

	//if passed a null pointer
	if(ptr==NULL)return;

	t* tagTemp=ptr-sizeof(t);

	//if large size malloc
	if((tagTemp->header->size)>MAXSIZE){
		size_t sizeToFree=(size_t)(tagTemp->header->size)+sizeof(m)+sizeof(t);
		munmap(ptr-sizeof(t)-sizeof(m),sizeToFree);
		return;
	}

	tagTemp->isFree=true;
	tagTemp->nextAvail=tagTemp->header->freeAddr;
	tagTemp->header->freeAddr=tagTemp;
}

//realloc
void *realloc(void* ptr, size_t b){
	
	//if the size required is smaller than 0
	if(!(b>0)){
		return ptr;
	}

	if(ptr==NULL){
		return NULL;
	}

	//find the size of the ptr
	t* tempTag=ptr-sizeof(t);
	int newSize;
	int copySize=tempTag->header->size;

	//deciding new size
	if(!(b>MAXSIZE))
		newSize=toTwoPower(b);
	else
		newSize=b;

	//if the size is smaller or equal
	if((tempTag->header->size)>newSize){
		copySize=newSize;
	}

	//Alloc new memory and restore the memory from the original pointer
	void* returnAddr=malloc(newSize);
	memcpy(returnAddr,ptr,copySize);

	free(ptr);
	return returnAddr;
}

//calloc
void *calloc(size_t num,size_t a){
	size_t size=num*a;
	void* temp=malloc(size);

	//printf("passed malloc\n");
	if(temp==NULL){
		return NULL;
	}
	
	memset((char*)temp,'\0',size);

	return temp;
}

void alloc_cleanup(void){}
