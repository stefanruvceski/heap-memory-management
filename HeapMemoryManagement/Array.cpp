#include "Array.h"

/**
* funkcija za prikaz memorije u particionisanom delu heap-a
*/
void print3DArray(TPartitionHeapManager* heapHead) {
	system("cls");
	printf("Memory\n");
	for (int i = 0; i < BLOCK_SIZE_COUNT; i++)
	{
		for (int j = 0; j < heapHead->segmentsSize[i].numberOfSegments; j++)
		{
			int pom = 1;
			printf("\n=========================================\n");
			printf("Segment[%d] size %d[byte]\t\t\t\t |\nNumber of blocks  %d\t\t\t |\nSize of bloks %d\t\t\t |\nStart location %u\t\t\t |\n-----------------------------------------|",j+1, i + 1, heapHead->segmentsSize[i].blockSegments[j].numberOfBlocks, indexToSize(i), (unsigned)heapHead->segmentsSize[i].blockSegments[j].segmentLocation);
			for (int k = 0; k < BITMASK_DEFAULT_NUM; k++) {
				printf("\n\n%d", k+1);
				
				for (int p = 0; p < BITMASK_LENGTH; p++) {
					
					if (p % 8 == 0) {
						printf("\n");
					}

					if (getOneBit(heapHead->segmentsSize[i].blockSegments[j].avail[k], p) == 1) {
						printf("B %d [FREE]  |", pom);
					}
					else {
						printf("B %d [TAKEN] |", pom);
					}
					pom++;
				}
			}
			
			
			printf("\n=========================================\n");
		}
	}

	printf("\n");
}