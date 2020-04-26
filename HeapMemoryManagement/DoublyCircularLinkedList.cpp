#include "DoublyCircularLinkedList.h"
#include "Structs.h"

int numTo4(int num) {
	if (num % 4 == 0) {
		return num;
	}
	return (num + 4 - num % 4);
}

/**
* dodavanje cvora na kraj liste
*/
void insertEnd(BigDataBlock** start, int size, int location)
{
	// ako je lista prazna, samo se kreira novi cvor
	if (*start == NULL)
	{
		TBigDataBlock* new_node = (TBigDataBlock*)malloc(sizeof(TBigDataBlock));
		new_node->size = size;
		new_node->location = location;
		new_node->prev = new_node;
		new_node->next = new_node;
		*start = new_node;
		return;
	}

	// ako lista nije prazna 
	TBigDataBlock *last = (*start)->prev;			// nadji poslednji element

	// dinamicko kreiranje cvora
	TBigDataBlock* new_node = (TBigDataBlock*)malloc(sizeof(TBigDataBlock));
	new_node->size = size;
	new_node->location = location;
 
	new_node->next = *start;						// pocetni cvor je sledeci za novi cvor (kruzna)
	(*start)->prev = new_node;						// prethodni od pocetnog cvora je novi cvor
	new_node->prev = last;							// prethodni od novog cvora je stari zadnji cvor 
	last->next = new_node;							// sledeci od starog zadnjeg je novi cvor
}

/**
* dodavanje cvora na pocetak liste
*/
void insertBegin(TBigDataBlock** start, int size, int location)
{
	TBigDataBlock *last = (*start)->prev;			// pokazivac na poslednjeg u listi

	// kreiranje novog cvora
	TBigDataBlock* new_node = (TBigDataBlock*)malloc(sizeof(TBigDataBlock));
	new_node->size = size;
	new_node->location = location;

	new_node->next = *start;						// prethodni i sledeci novog cvora
	new_node->prev = last;

	last->next = new_node;							// menjanje vrednosti prethodnog i sledeceg pocetnog i krajnjeg cvora
	(*start)->prev = new_node;

	*start = new_node;								// update pocetnog cvora
}

/**
* dodavanje cvora na zeljenu poziciju u listi (posle location1)
*/
void insertAfter(TBigDataBlock** start, int location1, int size, int location2)
{
	// kreiranje novog cvora
	TBigDataBlock* new_node = (TBigDataBlock*)malloc(sizeof(TBigDataBlock));
	new_node->size = size; 
	new_node->location = location2;

	// nalazenje cvora sa poljem vrednosti location1 i njegovog sledeceg
	TBigDataBlock*temp = *start;
	while (temp->location != location1)
		temp = temp->next;
	TBigDataBlock*next = temp->next;

	// umetanje cvora izmedju temp i next 
	temp->next = new_node;
	new_node->prev = temp;
	new_node->next = next;
	next->prev = new_node;
}

int insertBlock(TBigDataHeapManager** start, int size)
{
	//AKO JE PRAZNO
	if ((*start)->bigDataBlocks == NULL)
	{
		TBigDataBlock* new_node = (TBigDataBlock*)malloc(sizeof(TBigDataBlock));
		new_node->size = size;
		new_node->location = (*start)->startLocation;
		new_node->prev = new_node;
		new_node->next = new_node;
		(*start)->bigDataBlocks = new_node;
		return (*start)->startLocation;
	}
	//AKO IMA SAMO JEDAN ELEMENT
	else if ((*start)->bigDataBlocks->next->location == (*start)->bigDataBlocks->location) {
		insertAfter(&(*start)->bigDataBlocks, (*start)->startLocation, size, (*start)->bigDataBlocks->location + numTo4((*start)->bigDataBlocks->size));
		return (*start)->bigDataBlocks->location + numTo4((*start)->bigDataBlocks->size);
	}
	else if ((*start)->bigDataBlocks->location - (*start)->startLocation >= numTo4(size)) {
		insertBegin(&(*start)->bigDataBlocks, size, (*start)->startLocation);
		return (*start)->startLocation;
	}

	TBigDataBlock* forward = (*start)->bigDataBlocks, *backward = (*start)->bigDataBlocks->prev;

	while (forward->prev != backward->next)
	{
		if (forward->next->location - (forward->location + numTo4(forward->size)) >= numTo4(size)) {
			insertAfter(&(*start)->bigDataBlocks, forward->location, size, forward->location + numTo4(forward->size));
			return forward->location + numTo4(forward->size);
		}
		else if (backward->location - (backward->prev->location + numTo4(backward->prev->size)) >= numTo4(size)) {
			insertAfter(&(*start)->bigDataBlocks, backward->prev->location, size, backward->prev->location + numTo4(backward->prev->size));
			return backward->prev->location + numTo4(backward->prev->size);
		}

		if (forward->next == backward) {
			break;
		}
		forward = forward->next;
		backward = backward->prev;
	}
	//AKO JE SVE ZAUZETO SAMO IMAMO SLOBODAN VELIKI CANK NA KRAJU
	if ((*start)->endLocation - ((*start)->bigDataBlocks->prev->location + numTo4((*start)->bigDataBlocks->prev->size)) >= numTo4(size)) {
		insertEnd(&(*start)->bigDataBlocks, size, (*start)->bigDataBlocks->prev->location + numTo4((*start)->bigDataBlocks->prev->size));
		 return (*start)->bigDataBlocks->prev->location;
	}
	printf("NEMA MESTA SLOBODNOG\n");

	return 0;
}

void removeBlock(TBigDataBlock**start, int location) {
	// ako je lista prazna
	unsigned i = (unsigned)location;

	if (*start == NULL)
		return;

	TBigDataBlock* forward = *start, *backward = (*start)->prev, *curr = NULL, *prev = NULL;

	while (forward->prev != backward->next)
	{
		if (forward->location == location) {
			curr = forward;
			prev = forward->prev;
			break;
		}
		else if (backward->location == location) {
			curr = backward;
			prev = backward->prev;
			break;
		}

		if (forward->next == backward) {
			break;
		}
		forward = forward->next;
		backward = backward->prev;
	}

	if (curr == NULL)
		return;

	// provera da li je cvor jedini u listi
	if (curr->next == *start && prev == NULL)
	{
		(*start) = NULL;
		free(curr);
		return;
	}

	// ako lista ima viser od jednog elementa, proveriti da li je to prvi u listi
	if (curr == *start)
	{
		prev = (*start)->prev;
		*start = (*start)->next;
		prev->next = *start;
		(*start)->prev = prev;
		free(curr);
	}
	// provera da li je zadnji
	else if (curr->next == *start)
	{
		// Adjust the pointers of prev_1 and start node 
		prev->next = *start;
		(*start)->prev = prev;
		free(curr);
	}
	else
	{
		// novi pokazivac, pokazuje na sledeci od trenutnog
		TBigDataBlock*temp = curr->next;
		// prevezivanje pokazivaca 
		prev->next = temp;
		temp->prev = prev;
		free(curr);
	}
}

// brisanje zeljenog cvora iz liste
void deleteNode(TBigDataBlock** start, int location)
{
	// provera da li je lista prazna 
	if (*start == NULL)
		return;

	// trazenje zeljenog cvora
	TBigDataBlock*curr = *start, *prev_1 = NULL;
	while (curr->location != location)
	{
		// ako cvor ne postoji u listi 
		if (curr->next == *start)
		{
			printf("\nList doesn't have node with location = %d", location);
			return;
		}

		prev_1 = curr;
		curr = curr->next;
	}

	// provera da li je cvor jedini u listi
	if (curr->next == *start && prev_1 == NULL)
	{
		(*start) = NULL;
		free(curr);
		return;
	}

	// ako lista ima vise od jednog elementa, proveriti da li je prvi
	if (curr == *start)
	{
		prev_1 = (*start)->prev; 
		*start = (*start)->next;
		// prevezivanje pokazivaca
		prev_1->next = *start;
		(*start)->prev = prev_1;
		free(curr);
	}
	// provera da li je zadnji
	else if (curr->next == *start)
	{
		// prevezivanje pokazivaca
		prev_1->next = *start;
		(*start)->prev = prev_1;
		free(curr);
	}
	else
	{
		// novi pokazivac, pokazuje na sledeci od trenutnog 
		TBigDataBlock* temp = curr->next;
		// prevezivanje pokazivaca
		prev_1->next = temp;
		temp->prev = prev_1;
		free(curr);
	}
}

void display(TBigDataBlock* start)
{
	TBigDataBlock *temp = start;

	int i = 0;

	printf("\nList in forward direction \n");
	while (temp->next != start)
	{
		printf("%d ", temp->location);
		temp = temp->next;
		i++;
	}
	printf("%d ", temp->location);

	printf("\n Number of allocated blocks: %d\n ", i+1);
}