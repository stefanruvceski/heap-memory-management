/**
* Implementacija metoda iz ClientMemory.h
*/
#include"ClientMemory.h"
#include"Memory.h"

HANDLE semaphore1;

/**
* thread safe alokacija memorije, koristi funkciju allocateMemory iz Memory.h
*/
void* _alloc(int size) {
	WaitForSingleObject(semaphore1, INFINITE);
	void* ret = allocateMemory(size);
	ReleaseSemaphore(semaphore1, 1, NULL);
	return ret;
}

/**
* thread safe dealokacija memorije, koristi funkciju freeMemory iz Memory.h
*/
void _free(void* address) {
	WaitForSingleObject(semaphore1, INFINITE);
	freeMemory(address);
	ReleaseSemaphore(semaphore1, 1, NULL);
}

/**
* inicijalizacija heap-a, prosludjuje se pokazivac na master, pa se odatle inicijalizuju oba dela heap-a
*/
void _initMasterHeap(TMasterHeapManager** masterHeapManager, int stepOf2) {
	semaphore1 = CreateSemaphore(NULL, 1, 1, NULL);
	*masterHeapManager = (TMasterHeapManager*)malloc(sizeof(TMasterHeapManager));
	
	// Inicijalizacija oba dela heap-a
	(*masterHeapManager)->partitionHeapManager = initializePartitionHeap((*masterHeapManager)->partitionHeapManager);
	(*masterHeapManager)->bigDataHeapManager = initializeBigDataHeap(stepOf2, (*masterHeapManager)->bigDataHeapManager); //Staviti odgovarajuci broj za odredjeni test
}

/**
* deinicijalizacija heap-a, koristi se funkcija deinitializeHeap iz Memory.h
*/
void _deinitMasterHeap(TMasterHeapManager* masterHeapManager) {
	deinitializeHeap(masterHeapManager);
}

/**
* prikaz stanja particionisanog dela heap-a, koristi se funkcija print3DArray iz Memory.h
*/
void _print(TMasterHeapManager* masterHeapManager) {
	print3DArray(masterHeapManager->partitionHeapManager);
}

/**
* racunanje kolicine zauzete memorije, moze da se koristi da se proveri postoje li curenja memorije
*/
float _calculateAllocatedMemory() {
	return calculateAllocatedMemory();
}