#include<stdlib.h>
#include<stdio.h>
#include "Structs.h"

void insertEnd(BigDataBlock** start, int size, int location);
void insertBegin(TBigDataBlock** start, int size, int location);
void insertAfter(TBigDataBlock** start, int location1, int size, int location2);
int insertBlock(TBigDataHeapManager** start, int size);
void removeBlock(TBigDataBlock**start, int location);
void deleteNode(TBigDataBlock**start, int location);	
void display(TBigDataBlock* start);