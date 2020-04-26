#pragma once
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include<conio.h>

#define BLOCK_SIZE_COUNT 12
#define BITMASK_LENGTH 16
#define BITMASK_DEFAULT_NUM 64
#define BLOCK_SIZE 1024

/**
* struktura koja reprezentuje blok unutar segmenta na particionisanom delu heap-a
*/
typedef struct BlockSegment {				
	unsigned short avail[BITMASK_DEFAULT_NUM];				
	int segmentLocation;
	int numberOfBlocks;
} TBlockSegment;

/**
* struktura koja reprezentuje blok unutar dela heap-a za velike objekte
*/
typedef struct BigDataBlock {
	int size;
	int location;
	BigDataBlock* prev;
	BigDataBlock* next;
}TBigDataBlock;

/**
* struktura koja za svaku vrstu bloka pamti njegovu velicinu, broj kreiranih segmenata i sadrzi pokazivac na te segmente
*/
typedef struct SegmentSizePartition {
	int size;
	int numberOfSegments;
	TBlockSegment* blockSegments;
}TSegmentSizePartition;

/**
* struktura koja reprezentuje particionisani deo heap-a
* sadrzi niz od 12 elemenata, jer toliko ima vrsta blokova (16B, 32B, ..., 32kB)
*/
typedef struct PartitionHeapManager {
	TSegmentSizePartition segmentsSize[12];
} TPartitionHeapManager;

/**
* struktura koja reprezentuje deo heap-a za velike objekte
* sadrzi pokazivac na pocetak tih blokova, pocetnu i krajnju lokaciju particije za velike objekte
*/
typedef struct BigDataHeapManager {
	BigDataBlock* bigDataBlocks;
	int startLocation;
	int endLocation;
} TBigDataHeapManager;

/**
* struktura koja reprezentuje master pokazivac na heap
* sadrzi pokazivace na oba dela heap-a
*/
typedef struct MasterHeapManager {
	TPartitionHeapManager* partitionHeapManager;
	TBigDataHeapManager* bigDataHeapManager;
}TMasterHeapManager;

