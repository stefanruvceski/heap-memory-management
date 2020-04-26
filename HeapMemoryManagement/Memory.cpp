#include "Memory.h"
#include "Array.h"
#include "DoublyCircularLinkedList.h"

TPartitionHeapManager* partitionHeapHead;
TBigDataHeapManager* bigDataHeapHead;

/**
* funkcija koja inicijalizuje particionisani deo heap
*/
TPartitionHeapManager* initializePartitionHeap(TPartitionHeapManager* partitionHeapManager) {
	partitionHeapManager = (TPartitionHeapManager*)malloc(sizeof(TPartitionHeapManager));
	partitionHeapHead = partitionHeapManager;
	initSegments();

	return partitionHeapManager;
}

/**
* funkcija koja inicijalizuje deo heap-a za velike objekte
*/
TBigDataHeapManager* initializeBigDataHeap(int stepOf2, TBigDataHeapManager* bigDataHeapManager) {
	bigDataHeapHead = (TBigDataHeapManager*)malloc(sizeof(TBigDataHeapManager));
	bigDataHeapHead->bigDataBlocks = NULL;
	bigDataHeapHead->startLocation = (unsigned)malloc((unsigned)pow(2, stepOf2));
	bigDataHeapHead->endLocation = bigDataHeapHead->startLocation + (int)pow(2, stepOf2);

	return bigDataHeapHead;
}

/**
* funkcija koja inicijalizuje memoriju za segmente
*/
TPartitionHeapManager* initSegments() {
	for (int i = 0; i < BLOCK_SIZE_COUNT; i++)
	{
		partitionHeapHead->segmentsSize[i].numberOfSegments = 1;
		partitionHeapHead->segmentsSize[i].blockSegments = (TBlockSegment*)malloc(sizeof(TBlockSegment));

		partitionHeapHead->segmentsSize[i].blockSegments[0].segmentLocation = (int)malloc(BLOCK_SIZE * (int)pow(2, 4 + i));		// rezervisanje memorije za svaki od segmenata i smestanje adrese pocetka bloka
		partitionHeapHead->segmentsSize[i].blockSegments[0].numberOfBlocks = BLOCK_SIZE;
		partitionHeapHead->segmentsSize[i].size = (int)pow(2, 4 + i);

		for (int j = 0; j < BITMASK_DEFAULT_NUM; j++) {
			partitionHeapHead->segmentsSize[i].blockSegments[0].avail[j] = 0xFFFF;			// inicijalno su svi blokovi slobodni
		}
	}

	return partitionHeapHead;
}

/**
* funkcija koja alocira memoriju, parametar je velicina zeljene memorije
*/
void* allocateMemory(int size) {
	int index = sizeToIndex(nextPowerOf2(size));
	void* retVal = NULL;

	if (size <= pow(2, 15)) {
		int i = 0;
		
		while (1) {
			for (int j = 0; j < BITMASK_DEFAULT_NUM; j++) {
				for (int k = 0; k < BITMASK_LENGTH; k++) {
					if (getOneBit(partitionHeapHead->segmentsSize[index].blockSegments[i].avail[j], k) == 1) {
						partitionHeapHead->segmentsSize[index].blockSegments[i].avail[j] = clearOneBit(partitionHeapHead->segmentsSize[index].blockSegments[i].avail[j], k);
						retVal = (void*)fromStartAddressToAddress(partitionHeapHead->segmentsSize[index].blockSegments[i].segmentLocation, j * BITMASK_LENGTH + k, indexToSize(index));
						return retVal;
					}
					else if (j * BITMASK_LENGTH + k == partitionHeapHead->segmentsSize[index].blockSegments[i].numberOfBlocks-1 && partitionHeapHead->segmentsSize[index].numberOfSegments == i + 1) {
						createNewSegment(&partitionHeapHead->segmentsSize[index], i + 1);
						j = -1;
						break;
					}
				}
				if (j == -1) {
					break;
				}
			}

			i++;
		}
	}
	else {
		retVal = (void*)insertBlock(&bigDataHeapHead, size);
	}


	return retVal;
}

/**
* funkcija kreira novi segment, poziva se iz allocateMemory u slucaju da je prethodni segment popunjen
*/
void createNewSegment(TSegmentSizePartition* segmentSize, int index) {
	TBlockSegment* newBlocks = (TBlockSegment*)malloc(sizeof(TBlockSegment)*(index + 1));

	for (int i = 0; i < index; i++)
	{
		newBlocks[i] = segmentSize->blockSegments[i];
	}
	newBlocks[index].segmentLocation = (int)malloc(BLOCK_SIZE * segmentSize->size);		// rezervisanje memorije za svaki od segmenata i smestanje adrese pocetka bloka
	newBlocks[index].numberOfBlocks = BLOCK_SIZE;

	for (int i = 0; i < BITMASK_DEFAULT_NUM; i++) {
		newBlocks[index].avail[i] = 0xFFFF;
	}

	segmentSize->numberOfSegments++;
	free(segmentSize->blockSegments);
	segmentSize->blockSegments = newBlocks;
}

/**
* funkcija koja dealociran memoriju, parametar je pocetna adresa
*/
void freeMemory(void* address) {

	if ((unsigned)bigDataHeapHead->startLocation <= (unsigned)address && (unsigned)bigDataHeapHead->endLocation >= (unsigned)address) {
		removeBlock(&bigDataHeapHead->bigDataBlocks, (int)address);
		return;
	}

	for (int i = 0; i < BLOCK_SIZE_COUNT; i++) {
		for (int j = 0; j < partitionHeapHead->segmentsSize[i].numberOfSegments; j++) {
			if ((int)address >= partitionHeapHead->segmentsSize[i].blockSegments[j].segmentLocation &&
				(int)address <= partitionHeapHead->segmentsSize[i].blockSegments[j].segmentLocation + partitionHeapHead->segmentsSize[i].blockSegments[j].numberOfBlocks * partitionHeapHead->segmentsSize[i].size) {

				int offset = ((int)address - partitionHeapHead->segmentsSize[i].blockSegments[j].segmentLocation)/ (partitionHeapHead->segmentsSize[i].size) ;
				int bitmaskIndex = offset / BITMASK_LENGTH;
				int bitIndex = offset % BITMASK_LENGTH;

				partitionHeapHead->segmentsSize[i].blockSegments[j].avail[bitmaskIndex] = setOneBit(partitionHeapHead->segmentsSize[i].blockSegments[j].avail[bitmaskIndex], bitIndex);
				return;
			}
		}
	}
}

/**
* funkcija deinicijalizuje ceo heap, radi free sve zauzete memorije
*/
void deinitializeHeap(TMasterHeapManager* masterHeapManager) {
	for (int i = 0; i < BLOCK_SIZE_COUNT; i++)
	{
		for (int j = 0; j < partitionHeapHead->segmentsSize[i].numberOfSegments; j++)
		{
			free((void*)partitionHeapHead->segmentsSize[i].blockSegments[j].segmentLocation);
		}
		free(partitionHeapHead->segmentsSize[i].blockSegments);
		
	}

	free(partitionHeapHead->segmentsSize);

	free((void*)bigDataHeapHead->startLocation);
	free(bigDataHeapHead);
	//free(partitionHeapHead);
	free(masterHeapManager);
	masterHeapManager = NULL;
}

/**
* funkcija koja konvertuje indeks u velicinu bloka (16B, 32B, ..., 32kB)
*/
int indexToSize(int index) {
	return (int)pow(2, index + 4);
}

/**
* funkcija koja konvertuje velicinu bloka u indeks
*/
int sizeToIndex(int size) {
	return (int)log2(size) - 4;
}

/**
* funkcija racuna adresu bloka, na osnovu pocetne adrese segmenta, udaljenosti i velicine bloka
*/
int fromStartAddressToAddress(int startAddress, int offset, int size) {
	return startAddress + offset * size;
}

/**
* funkcija pretvara zeljenu velicinu segmenta u najblizi veci stepen dvojke
*/
static inline unsigned nextPowerOf2(unsigned size)
{
	size -= 1;
	size |= (size >> 1);
	size |= (size >> 2);
	size |= (size >> 4);
	size |= (size >> 8);
	size |= (size >> 16);
	size++;

	if (size < 16) {
		return 16;
	}

	return size;
}

/**
* funkcija preuzima vrednost bita proslednjenog broja sa zeljene pozicije
*/
bool getOneBit(short number, short index) {
	short mask = 0x0001 << index;
	return number & mask;
}

/**
* funkcija postavlja vrednost bita u prosledjenom broju na odgovarajucoj poziciji
*/
short setOneBit(short number, short index) {
	short mask = 0x0001 << index;
	return (short)(number | mask);
}

/**
* funkcija brise vrednost bita u prosledjenom broju na odgovarajucoj poziciji
*/
short clearOneBit(short number, short index) {
	short mask = ~(0x0001 << index);
	return (short)(number & mask);
}

/**
* funkcija koja broji zauzete blokove na heap-u
*/
int countTakenBlocks(unsigned short block) {
	int retVal = 0;

	for (int i = 0; i < BITMASK_LENGTH; i++) {
		if (getOneBit(block, i) == 0) {
			retVal++;
		}
	}

	return retVal;
}

/**
* funkcija racuna kolicinu zauzete memorije na heap-u
*/
float calculateAllocatedMemory() {
	int takenBlocks = 0;
	float retVal = 0;

	for (int i = 0; i < BLOCK_SIZE_COUNT; i++) {
		for (int j = 0; j < partitionHeapHead->segmentsSize[i].numberOfSegments; j++) {
			for (int k = 0; k < BITMASK_DEFAULT_NUM; k++) {
				takenBlocks += countTakenBlocks(partitionHeapHead->segmentsSize[i].blockSegments[j].avail[k]);
				retVal += ((float)takenBlocks * (float)partitionHeapHead->segmentsSize[i].size) / 1048576;			// u MB
			}
		}
	}

	return retVal;
}