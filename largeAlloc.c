#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <unistd.h>

#define NUMBUFS 21
int bufsizes[NUMBUFS] = {8,5,7,0,6,125,127,126,0,90,15,64,20,40,0,5000,2000,1025,0,1025,1025};

int main()
{

	uint8_t *bufs[NUMBUFS];

	void * firstbreak = sbrk(0);

	free(NULL); //just for kicks
	int count=1000;
	while(count--){
	for (int i=0; i < NUMBUFS; i++)
	{
		//allocate the next block
		bufs[i] = malloc(bufsizes[i]);
		if(bufsizes[i]!=0)
			assert(bufs[i] != NULL); //should never return NULL
		else
			assert(bufs[i]==NULL);

		//write some data into the buffer
		if(bufs[i]!=0)
		memset(bufs[i], i, bufsizes[i]);

		//free(bufs[i]);
	}

	for (int i=0; i < NUMBUFS; i++)
	{
		//check whether or not the memory is still intact
		fprintf(stderr, "at buff number %d\n", i);
		for (int b=0; b < bufsizes[i]; b++)
		{
			
			if(bufs[i][b]!=i)
						fprintf(stderr, "current index is %d\n",b);
			//printf("current bufsize is %d and the bufs is %d, the i is supposed to be %d\n",bufsizes[i],bufs[i][b],i);
			assert (bufs[i][b] == i);
		}

		free(bufs[i]);
		bufs[i]=malloc(bufsizes[i]);
		if(bufs[i]!=0)
		memset(bufs[i],i,bufsizes[i]);

		for (int b=0; b < bufsizes[i]; b++)
		{
			
			//printf("current bufsize is %d and the bufs is %d, the i is supposed to be %d\n",bufsizes[i],bufs[i][b],i);
			assert (bufs[i][b] == i);
		}

		//free(bufs[i]);
		//fprintf(stderr, "finish free\n");

	}
	}

	void * lastbreak = sbrk(0);

	//verify that the program break never moved up.
	assert (firstbreak == lastbreak);

	return 0;
}