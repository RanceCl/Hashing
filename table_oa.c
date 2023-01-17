/* table_oa.c                   
 * Roderick "Rance" White
 * roderiw
 * ECE 2230 Fall 2020
 * MP7
 *
 * Purpose: Hold the functions to construct, destroy, and manipulate a hash table with a probe
 * type of either linear or double open addressing. 
 *
 */



#include <stdlib.h>
#include <stdio.h>
#include <assert.h>
#include <math.h>
#include <string.h>
#include <ctype.h>
#include <time.h>
#include <unistd.h>

#include "table.h"



/* Local Functions */
int table_index_search(table_t *T, hashkey_t K);




//Function to allocate the hash table and Open Addressing (oa) segment if using linear or double
table_t *Table_OA_Construct(int table_size, int probe_type)
{
	int i;
	table_t *T;

	/* First, make sure that probe_type is one of the possible types  */
	assert(probe_type == LINEAR || probe_type == DOUBLE || probe_type == CHAIN);

	/* Malloc space for table */
	T = (table_t *) malloc(sizeof(table_t));		// Allocate space for actual tag

	//Set values in structure
	T->size = table_size;
	T->probetype = probe_type;
	T->KeyNum = 0;
	T->ProbeNum = 0;

	//Allocate the Open Addressing (oa) segment if using linear or double
	T->oa = (table_entry_t *) malloc(table_size * sizeof(table_entry_t));

	//Loop to set all keys to be empty
	for(i=0; i<table_size; i++) {
		T->oa[i].key = NULL;
		T->oa[i].deleted = -1;
	}

	return T;
}


/* Function to rehash an open address table */
table_t *Table_OA_Rehash (table_t *T, int new_table_size)
{
	int i;
	table_t *NewT;

	NewT = table_construct(new_table_size, T->probetype);		//Construct new graph

	/* Loop to insert all old entries into new table */
	for(i=0; i<T->size; i++) {
		//Insert all entries that aren't empty
		if(T->oa[i].key != NULL) 
			table_insert(NewT, T->oa[i].key, T->oa[i].data_ptr);
	}
	free(T->oa);												//Free the open addressing array
	free(T);													//Finally free table

	return NewT;
}


/* Function to destroy the hash table and free Open Addressing (oa) segment */
void Table_OA_Destruct (table_t *T)
{
	int i;

	/* Free the Info that is in the table first, since it was allocated */
	for(i=0; i<T->size; i++)
	{
		//Skip when no key is present (NULL)
		if(T->oa[i].key != NULL){
			free(T->oa[i].key);
			free(T->oa[i].data_ptr);
		}
	}
	free(T->oa);						//Free array after freeing allocated memory
	free(T);
}



/* Function to run operation for open addressing inserting 
 * Returns 0 is successful
 * Returns 1 if replacing an old entry
 * Returns -1 if unsuccessful
 */
int Table_OpAddr_Insert (table_t *T, hashkey_t K, data_t I)
{
	int i, RemIndex=-1, ProbeDecrement, LoopCheck;

	/* Initializations */
	i = HashFunc(K, T);									// Let i be the first hash location
	ProbeDecrement = ProDecFunc(K, T);					// Compute probe decrement
	T->ProbeNum = T->ProbeNum + 1;						// Increment probe up by 1
	LoopCheck = i;

	/* Loop to search for an empty location */
	// If the deleted value is -1, that means no entry has ever been present
	while (T->oa[i].deleted != -1) {
		//Check if a removed key has been found
		// Doesn't break loop in case a matching key exists later
		if(T->oa[i].key == NULL && RemIndex == -1) RemIndex = i;

		//Replace old entry if keys match
		else if(T->oa[i].key != NULL && strcmp(K, T->oa[i].key) == 0) {
			free(K);							// Free the K sent
			free(T->oa[i].data_ptr);			// Free the old information
			T->oa[i].data_ptr = I;				// Replace with new information
			T->oa[i].deleted = 0;				// Change delete to 0
			return 1;
		}

		i -= ProbeDecrement;							// Compute next probe location
		T->ProbeNum = T->ProbeNum + 1;					// Increment probe up by 1
		if(i < 0) {
			i += T->size;								// Wrap around if needed
		}

		//If the loop reaches the beginning again, break loop.
		if(i == LoopCheck) break;
	}
	//If a removed entry was found, change i to the index
	if (RemIndex > -1) i = RemIndex;
	if (T->oa[i].key != NULL) return -1;		// If not empty, then no node was available
	T->oa[i].key = K;							// Insert new key K into table T
	T->oa[i].data_ptr = I;						// Insert new info I into table T
	T->oa[i].deleted = 0;						// Change delete to 0
	T->KeyNum = T->KeyNum + 1;					// Increase entries by one
	return 0;									// Returns 0 for new entry
}


/* Function to run operation for open addressing removing
 * Returns the information of the key, if found
 */
data_t Table_OpAddr_Delete (table_t *T, hashkey_t K)
{
	int MatchIndex;

	/* Find the index with the matching key, K */
	MatchIndex = table_index_search(T, K);

	/* Determine success or failure */
	if(MatchIndex == -1) {
		return NULL;							// Return NULL to signify that K was not found
	} else {
		free(T->oa[MatchIndex].key);
		T->oa[MatchIndex].key = NULL;
		T->oa[MatchIndex].deleted = 1;			// Change the deletion variable
		T->KeyNum = T->KeyNum - 1;				// Decrement the number of keys
		return T->oa[MatchIndex].data_ptr;		// Return pointer to the information I
	}
}



/* Function to run operation for open addressing retrieval
 * Returns the information of the key, if found
 */
data_t Table_OpAddr_Retrieve (table_t *T, hashkey_t K)
{
	int MatchIndex;

	/* Find the index with the matching key, K */
	MatchIndex = table_index_search(T, K);

	/* Determine success or failure */
	if(MatchIndex == -1) {
		return NULL;							// Return NULL to signify that K was not found
	} else {
		return T->oa[MatchIndex].data_ptr;		// Return pointer to the information I
	}

}



/* A function that find the index in the table with the matching key, K 
 * Returns -1 if no such key exists
 * Used for open addressing retrieval and delete operations
 */
int table_index_search(table_t *T, hashkey_t K)
{
	int i, ProbeDecrement, LoopCheck, IsDeleted;
	hashkey_t ProbeKey;

	T->ProbeNum = 0;									//Reset to 0

	/* Initializations */
	i = HashFunc(K, T);									// Let i be the first hash location
	ProbeDecrement = ProDecFunc(K, T);					// Compute probe decrement
	T->ProbeNum = T->ProbeNum + 1;						// Increment probe up by 1
	ProbeKey = T->oa[i].key;
	LoopCheck = i;
	IsDeleted = T->oa[i].deleted;						// Used to ensure deleted entries are skipped

	/* Search Loop */
	//Since empty entries had deleted status set to -1, this is a better way to check
	while (IsDeleted != -1) {

		//Check if matching key has been found
		//Due to errors in comparing for a deleted node, needed to separate from while loop.
		if((ProbeKey != NULL) && (strcmp(K, ProbeKey) == 0)) break;

		i -= ProbeDecrement;							// Compute next probe location
		T->ProbeNum = T->ProbeNum + 1;					// Increment probe up by 1
		if(i < 0) {
			i += T->size;								// Wrap around if needed
		}

		ProbeKey = T->oa[i].key;						// Extract next probe key
		IsDeleted = T->oa[i].deleted;					// Get deletion state of key

		//If the loop reaches the beginning again, the value can't be found.
		if(i == LoopCheck){
			return -1;
		}
	}

	/* Determine success or failure */
	if(ProbeKey == NULL) {
		return -1;										// Return -1 to signify that K was not found
	} else {
		return i;										// Return index to the matching key
	}
}



/* This function to return the key at the index sent. 
 * Returns 0 if not found
 */
hashkey_t Table_OA_Peek (table_t *T, int index)
{
	//Return 0 if the index has no data
	if(T->oa[index].key == NULL) return 0;

	//Returns the data if data is present
	else return T->oa[index].key;
}





/* Function to print entries in an open address table */
void Table_OpAddr_Debug_Print (table_t *T)
{
	int i, *ip;

	/* Print table */
	printf("\n|%6s%2c%10s%4c%10s%4c\n", "Index",'|', "Key",'|', "Info", '|');
	//Loop to print information
	for(i=0; i<T->size; i++) {
		//Print if a key has been removed
		if(T->oa[i].deleted == 1) {
			ip = T->oa[i].data_ptr;
			printf("|%6d%2c%10s%4c%10s%4c\n", i,'|', "REMOVED",'|', "REMOVED", '|');
		}

		//Print that a key is empty if it equals NULL
		else if(T->oa[i].key == NULL)
			printf("|%6d%2c%10s%4c%10s%4c\n", i,'|', "NULL",'|', "NULL", '|');

		//Otherwise print information
		else {
			ip = T->oa[i].data_ptr;
			printf("|%6d%2c%10s%4c%10d%4c\n", i,'|', T->oa[i].key,'|', *ip, '|');
		}
	}
}











