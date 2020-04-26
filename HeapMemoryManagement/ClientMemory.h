/**
* Server moze da koristi samo funkcije koje mu izlaze ClientMemory.h
*/
#pragma once
#include<windows.h>
#include "Structs.h"

void* _alloc(int size);
void _free(void* address);
void _initMasterHeap(TMasterHeapManager** masterHeapManager, int stepOf2);
void _deinitMasterHeap(TMasterHeapManager* masterHeapManager);
void _print(TMasterHeapManager* masterHeapManager);
float _calculateAllocatedMemory();
