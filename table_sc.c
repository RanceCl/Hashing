/* table_sc.c                   
 * Roderick "Rance" White
 * roderiw
 * ECE 2230 Fall 2020
 * MP7
 *
 * Purpose: Hold the functions to construct, destroy, and manipulate a hash table with a probe
 * type of separate chaining. 
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



/* Function to allocate table and the Separate Chaining (sc) segment if using separate chaining */
table_t *Table_SC_Construct(int table_size, int probe_type)
{
	int i;
	table_t *T;

	/* Malloc space for table */
	T = (table_t *) malloc(sizeof(table_t));		// Allocate space for actual tag

	//Set values in structure
	T->size = table_size;
	T->probetype = probe_type;
	T->KeyNum = 0;
	T->ProbeNum = 0;

	//Allocate the Separate Chaining (sc) segment if using separate chaining
	T->sc = (sep_chain_t **) malloc(table_size * sizeof(sep_chain_t *));

	//Loop to set all keys to be empty
	for(i=0; i<table_size; i++) T->sc[i] = NULL;

	return T;
}




/* Function to rehash a separate chaining table */
table_t *Table_SC_Rehash (table_t *T, int new_table_size)
{
	int i;
	table_t *NewT;
	sep_chain_t *temp, *prev;

	NewT = table_construct(new_table_size, T->probetype);		//Construct new graph

	/* Loop to insert all old entries into new table */
	for(i=0; i<T->size; i++) {
		temp = T->sc[i];									//Place into temp to access data
		//Check if empty
		if(temp != NULL) {
			/* Loop to print each entry in chain */
			while(temp != NULL) {
				//Only insert if the key hasn't been removed
				if(temp->key != NULL) {
					table_insert(NewT, temp->key, temp->data_ptr);
				}
				prev=temp;
				temp = temp->next;							//Move to next entry in chain
				free(prev);
			}
		}
	}
	free(T->sc);											//Free separate chaining array
	free(T);													//Finally free table
	return NewT;
}


/* Function to destroy the hash table and free Separate Chaining (sc) segment */
void Table_SC_Destruct (table_t *T)
{
	int i;
	sep_chain_t *temp, *prev;

	//Free the Separate Chaining (sc) segment if using separate chaining
	//Loop through each section of the table
	for(i=0; i<T->size; i++) {
		temp = T->sc[i];
		//Free the elements until the end is reached
		while(temp != NULL) {
			prev = temp;
			temp = temp->next;

			//Skip when no key is present (NULL)
			if(prev->key != NULL) {
				free(prev->key);
				free(prev->data_ptr);
			}
			free(prev);
		}
	}
	free(T->sc);						//Free array after freeing allocated memory
	free(T);
}





/* Function to run operation for separate chaining inserting 
 * Returns 0 if inserted
 * Returns 1 if an entry was replaced
 */
int Table_SepChain_Insert (table_t *T, hashkey_t K, data_t I)
{
	int i;
	sep_chain_t *temp, *prev, *NewEnt;

	/* Initializations */
	i = HashFunc(K, T);									// Let i be the first hash location
	T->ProbeNum = T->ProbeNum + 1;						// Increment probe up by 1

	// Allocate space for new entry
	NewEnt = (sep_chain_t *) malloc(sizeof(sep_chain_t));	//Malloc the new entry
	NewEnt->key = K;
	NewEnt->data_ptr = I;
	NewEnt->next = NULL;
	temp = T->sc[i];

	//If initial entry is empty
	if(temp == NULL) T->sc[i]=NewEnt;						//Place in empty spot

	//If initial entry has matching key, replace entry spot
	else if(temp->key != NULL && strcmp(K,temp->key) == 0) {
		NewEnt->next = temp->next;
		/* Free old entry information before replacing with old */
		prev = T->sc[i];
		free(prev->key);
		free(prev->data_ptr);
		free(T->sc[i]);										//Free old data
		T->sc[i]=NewEnt;									//Replace with new
		return 1;
	}

	//Otherwise, go to the end of the list
	else {
		prev=temp;
		temp=temp->next;

		/* Loop to check if empty */
		while(prev != NULL && temp != NULL) {
			//If matching key is found, replace entry
			if(temp->key != NULL && strcmp(K,temp->key) == 0) {
				NewEnt->next = temp->next;
				/* Free old information */
				free(temp->key);
				free(temp->data_ptr);
				free(temp);									//Free temp data
				prev->next = NewEnt;						//Replace with new
				return 1;
			}
			prev=temp;	temp=temp->next;
		}
		// Place new data at the end of the chain
		prev->next=NewEnt;
	}
	T->KeyNum = T->KeyNum + 1;								// Increase entries by one
	return 0;												// Returns 0 for new entry
}


/* Function to run operation for separate chaining removing 
 * Returns the information of the key, if found
 */
data_t Table_SepChain_Delete (table_t *T, hashkey_t K)
{
	int i, *ip;
	sep_chain_t *temp, *prev;


	/* Separate Chaining Operations */
	/* Initialize values */
	i = HashFunc(K, T);						// i is hash function value
	temp = T->sc[i];

	//If initial entry is the matching key
	if(temp != NULL && strcmp(K,temp->key) == 0) {
		T->sc[i] = temp->next;						// Shift chain pointer to the right
		T->KeyNum = T->KeyNum - 1;					// Decrement the number of keys
		ip = temp->data_ptr;						// Info to be returned
		free(temp->key); free(temp);				// Free deleted entry
		return ip;									// Return pointer to the info
	}

	/* Find matching key */
	while(temp!=NULL) {
		T->ProbeNum = T->ProbeNum + 1;				// Increment probe up by 1
		if(temp->key != NULL && strcmp(K,temp->key) == 0) break;	//Loop ends if K is found
		/* Shift upwards */
		prev = temp;								// Used to replace old data
		temp = temp->next;
	}

	/* Determine success or failure */
	if(temp == NULL || temp->key == NULL) {
		return NULL;								// Return NULL if K was not found
	} else {
		prev->next = temp->next;					// Shift chain pointer to the right
		ip = temp->data_ptr;						// Info to be returned
		free(temp->key); free(temp);				// Free deleted array
		T->KeyNum = T->KeyNum - 1;					// Decrement the number of keys
		return ip;									// Return pointer to the information
	}
	return NULL;
}


/* Function to run operation for separate chaining retrieval 
 * Returns the information of the key, if found
 */
data_t Table_SepChain_Retrieve (table_t *T, hashkey_t K)
{
	int i;
	sep_chain_t *temp;

	/* Initialize values */
	i = HashFunc(K, T);							//i is hash function value
	temp = T->sc[i];

	/* Find matching key */
	while(temp!=NULL) {
		T->ProbeNum = T->ProbeNum + 1;			// Increment probe up by 1
		if(temp->key != NULL && strcmp(K,temp->key) == 0) break;	//Loop ends if K is found
		temp = temp->next;
	}

	/* Determine success or failure */
	if(temp == NULL || temp->key == NULL) {
		return NULL;							// Return NULL to signify that K was not found
	} else {
		return temp->data_ptr;					// Return index to the matching key
	}
}


/* This function to return the key at the index sent. 
 * Returns 0 if not found
 */
hashkey_t Table_SC_Peek (table_t *T, int index, int list_position)
{
	int i=0;
	sep_chain_t *temp;

	temp = T->sc[index];						//Place into temp to access data

	if(temp == NULL) return 0;					//Check if intially empty

	//Otherwise, check all entries in the chain
	else {
		/* Move to entry in chain */
		// If the entry is NULL, then the position doesn't exist
		while(temp != NULL && i < list_position)
		{
			i++;								//Increment i up
			temp = temp->next;
		}
		/* Check if there is a key present */
		//return 0 if there is no entry at the position or if the key has been removed
		if(temp == NULL || temp->key == NULL) return 0;

		//Otherwise return the key
		else return temp->key;
	}
	return 0;
}





/* Function to print entries in a separate chaining table 
 */
void Table_SepChain_Debug_Print (table_t *T)
{
	int i, *ip;
	sep_chain_t *temp;

	/* Print table */
	printf("\n%5c ->( Key, Info )\n", 'I');
	//Loop to print information
	for(i=0; i<T->size; i++) {
		temp = T->sc[i];			//Place into temp to access data
		printf("%5d ", i);			//Print index
		//Check if intially empty
		if(temp == NULL) printf("->( NULL )");

		//Otherwise, print all entries in the chain
		else {
			/* Loop to print each entry in chain */
			while(temp != NULL) {
				//Print information
				ip = temp->data_ptr;
				printf("->(%8s,%5d)", temp->key, *ip);
				temp = temp->next;			//Move to next entry in chain
			}
		}
		printf("\n");
	}
}








