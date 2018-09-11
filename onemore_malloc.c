#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>
#include <time.h>

#define NUMBLOCKS 100  //change this as desired
#define MAXSIZE 5000
#define ASICSIZE 126

char *blocks[NUMBLOCKS]; //initialize all of these pointers to NULL
int sizes[NUMBLOCKS];
char blockdata[NUMBLOCKS];

int main(){
	int count=100000;
	int counter=0;
	int count_large_alloc=0;
	int large_alloc_amount=0;

	srand(time(0));

	for(;counter<NUMBLOCKS;counter++){
		blocks[counter]=NULL;
	}

	while(count--){
		//pick a random block, i
		int i=rand()%NUMBLOCKS;

		//pick a random size, s
		int s=rand()%MAXSIZE;

		//pick a random character, c
		char c=(char)rand()%ASICSIZE;

		if(s>1024){

			int a=s;

			do{
				large_alloc_amount+=4096;
				a-=4096;
			}while(a>0);

			if(s>4096)
				count_large_alloc++;
		}

		if (blocks[i] == NULL) {

			//if(s>1024){

			//fprintf(stderr, "dealing with block %d\n",i);
			//fprintf(stderr, "malloced size of %d\n",s);
			//}
			
			blocks[i] = calloc(1,s);

			//fprintf(stderr, "dealing with block %d\n",i);
			//fprintf(stderr, "malloced size of %d\n",s);
			if(s!=0)
						assert(blocks[i]!=NULL);
			else
						assert(blocks[i]==NULL);

			sizes[i] = s;
			blockdata[i] = c;
			memset(blocks[i], blockdata[i], sizes[i]);
			
			


		} else {
			//randomly select an operation
			int dice = rand() % 10;


			if (dice < 3) { 

				//free the block
				//check to make sure that blocks[i] contains blockdata[i], repeated sizes[i] times
				
				//if(s>1024)
				//fprintf(stderr, "dealing with block %d\n",i);
				
				int ss=0;
				for(;ss<sizes[i];ss++){
					//assert(blocks[i][ss]==blockdata[i]);

					if(blocks[i][ss]!=blockdata[i])
						fprintf(stderr, "current index is %d with buf location %d\n",ss,i);

						assert(blocks[i][ss]==blockdata[i]);
				}

				//fprintf(stderr,"In free->>>>>>>>>>>>> %d of size %d\n",i,sizes[i]);		
				free(blocks[i]);
				
				/**
				if(blocks[i]!=NULL){
					fprintf(stderr, "free failed\n");
				}
				**/
				if(sizes[i]>1024)
				large_alloc_amount-=sizes[i];
				
				sizes[i]=0;
				blockdata[i]='\0';
				blocks[i]=NULL;
				//assert(blocks[i]==NULL);

			} else {
				//realloc to new random size, update blockdata and sizes appropriately.
							//	if(s>1024){

			//	fprintf(stderr, "dealing with block %d\n",i);

			//	fprintf(stderr, "reallocing memory from %d to %d\n", sizes[i],s);
				//	}

				blocks[i]=realloc(blocks[i],s);
				sizes[i] = s;
				blockdata[i] = c;
				memset(blocks[i], blockdata[i], sizes[i]);
			}
		}
		//if(s>1024)
		fprintf(stderr,"End of the %d loop and large allocted is %d and amount is %d\n\n",count,count_large_alloc,large_alloc_amount);
	}



}
