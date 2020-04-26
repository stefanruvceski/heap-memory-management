/**
* Sve potrebne funkcije za rad sa HeapManager-om
*/
#define _WINSOCK_DEPRECATED_NO_WARNINGS
#define _CRT_NONSTDC_NO_DEPRECATE
#define _CRT_SECURE_NO_WARNINGS
#define _CRT_SECURE_NO_DEPRECATE  

#include "Structs.h"
#include "Array.h"
#include "DoublyCircularLinkedList.h"

// funkcija koja inicijalizuje particionisani deo heap
TPartitionHeapManager* initializePartitionHeap(TPartitionHeapManager* partitionHeapManager);

// funkcija koja inicijalizuje deo heap-a za velike objekte
TBigDataHeapManager* initializeBigDataHeap(int stepOf2, TBigDataHeapManager* bigDataHeapManager);

// funkcija koja inicijalizuje memoriju za segmente
TPartitionHeapManager* initSegments();

// funkcija koja alocira memoriju, parametar je velicina zeljene memorije
void* allocateMemory(int size);

// funkcija koja dealociran memoriju, parametar je pocetna adresa
void freeMemory(void* address);

// funkcija koja konvertuje indeks u velicinu bloka (16B, 32B, ..., 32kB)
int indexToSize(int index);

// funkcija koja konvertuje velicinu bloka u indeks
int sizeToIndex(int Size);

// funkcija racuna adresu bloka, na osnovu pocetne adrese segmenta, udaljenosti i velicine bloka
int fromStartAddressToAddress(int startAddress, int offset, int size);

// funkcija pretvara zeljenu velicinu segmenta u najblizi veci stepen dvojke
static inline unsigned nextPowerOf2(unsigned size);

// funkcija kreira novi segment
void createNewSegment(TSegmentSizePartition* segmentSize, int index);

// funkcija preuzima vrednost bita proslednjenog broja sa zeljene pozicije
bool getOneBit(short number, short index);

// funkcija postavlja vrednost bita u prosledjenom broju na odgovarajucoj poziciji
short setOneBit(short number, short index);

// funkcija brise vrednost bita u prosledjenom broju na odgovarajucoj poziciji
short clearOneBit(short number, short index);

// funkcija deinicijalizuje ceo heap, radi free sve zauzete memorije
void deinitializeHeap(TMasterHeapManager* masterHeapManager);

// funkcija koja broji zauzete blokove na heap-u
int countTakenBlocks(unsigned short block);

// funkcija racuna kolicinu zauzete memorije na heap-u
float calculateAllocatedMemory();