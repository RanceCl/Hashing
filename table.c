/* table.c                   
 * Roderick "Rance" White
 * roderiw
 * ECE 2230 Fall 2020
 * MP7
 *
 * Purpose: Hold the functions to construct, destroy, and manipulate a hash table with a probe
 * type of either linear or double open addressing or separate chaining. 
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


/*  The empty table is created.  The table must be dynamically allocated and
 *  have a total size of table_size.  The maximum number of (K, I) entries
 *  that can be stored in the table is table_size-1.  For open addressing, 
 *  the table is filled with a special empty key distinct from all other 
 *  nonempty keys (e.g., NULL).  
 *
 *  the probe_type must be one of {LINEAR, DOUBLE, CHAIN}
 *
 *  Do not "correct" the table_size or probe decrement if there is a chance
 *  that the combinaion of table size or probe decrement will not cover
 *  all entries in the table.  Instead we will experiment to determine under
 *  what conditions an incorrect choice of table size and probe decrement
 *  results in poor performance.
 */
table_t *table_construct(int table_size, int probe_type)
{
	/* First, make sure that probe_type is one of the possible types  */
	assert(probe_type == LINEAR || probe_type == DOUBLE || probe_type == CHAIN);

	/* Open Address Operations */
	if(probe_type==LINEAR || probe_type==DOUBLE) 
		return Table_OA_Construct(table_size, probe_type);

	/* Separate Chaining Operations */
	else 
		return Table_SC_Construct(table_size, probe_type);
}




/* Sequentially remove each table entry (K, I) and insert into a new
 * empty table with size new_table_size.  Free the memory for the old table
 * and return the pointer to the new table.  The probe type should remain
 * the same.
 *
 * Do not rehash the table during an insert or delete function call.  Instead
 * use drivers to verify under what conditions rehashing is required, and
 * call the rehash function in the driver to show how the performance
 * can be improved.
 */
table_t *table_rehash(table_t *T, int new_table_size)
{
	/* Open Address Operations */
	if(T->probetype != CHAIN) 
		return Table_OA_Rehash(T, new_table_size);

	/* Separate Chaining Operations */
	else 
		return Table_SC_Rehash(T, new_table_size);
}



/* Free all information in the table, the table itself, and any additional
 * headers or other supporting data structures.  
 */
void table_destruct(table_t *T)
{
	/* Open Address Operations */
	//Free the Open Addressing (oa) segment if using linear or double
	if(T->probetype == LINEAR || T->probetype == DOUBLE) 
		Table_OA_Destruct (T);

	/* Separate Chaining Operations */
	//Free the Separate Chaining (sc) segment if using separate chaining
	else 
		Table_SC_Destruct (T);
}




/* returns number of entries in the table */
int table_entries(table_t *T)
{
		return T->KeyNum;
}


/* returns 1 if table is full and 0 if not full. 
 * For separate chaining the table is never full
 */
int table_full(table_t *T)
{
	// If the table is using separate chaining, the table is never full.
	if(T->probetype == CHAIN) 
		return 0;

	//If table is full, return 1
	//Since max entries is one less than the size, table is full if entries equal one less than size
	if(T->KeyNum == T->size-1)
		return 1;

	//Otherwise
	return 0;
}


/* returns the number of table entries marked as deleted */
int table_deletekeys(table_t *T)
{
	int i, DelCount=0;

	/* Open Address Operations */
	if(T->probetype != CHAIN) {
		for(i=0; i<T->size; i++) {
			/* Count up if the key was deleted */
			if(T->oa[i].deleted == 1) DelCount++;
		}
	}

	/* Separate Chaining Operations */
	else return 0;		//Since no deleted entries are held

	return DelCount;
}


   
/* Insert a new table entry (K, I) into the table provided the table is not
 * already full.  
 * Return:
 *      0 if (K, I) is inserted, 
 *      1 if an older (K, I) is already in the table (update with the new I), or 
 *     -1 if the (K, I) pair cannot be inserted.
 *
 * Note that both K and I are pointers to memory blocks created by malloc()
 *
 * Most of the code uses Standish examples for the basic makeup
 *
 */
int table_insert(table_t *T, hashkey_t K, data_t I)
{
	T->ProbeNum = 0;							//Reset to 0

	/* Check for invalid values */
	if(K == NULL || I == NULL || K < 0 || I < 0) return -1;

	/* Open Address Operations */
	if(T->probetype != CHAIN) return Table_OpAddr_Insert(T, K, I);

	/* Separate Chaining Operations */
	else return Table_SepChain_Insert(T, K, I);
}


/* Delete the table entry (K, I) from the table.  Free the key in the table.
 * Return:
 *     pointer to I, or
 *     null if (K, I) is not found in the table.  
 *
 * Be sure to free(K)
 *
 * See the note on page 490 in Standish's book about marking table entries for
 * deletions when using open addressing.
 */
data_t table_delete(table_t *T, hashkey_t K)
{
	T->ProbeNum = 0;								//Reset to 0

	/* Open Address Operations */
	if(T->probetype != CHAIN) return Table_OpAddr_Delete(T, K);

	/* Separate Chaining Operations */
	else return Table_SepChain_Delete(T, K);
}




/* Given a key, K, retrieve the pointer to the information, I, from the table,
 * but do not remove (K, I) from the table.  Return NULL if the key is not
 * found.
 */
data_t table_retrieve(table_t *T, hashkey_t K)
{
	T->ProbeNum = 0;								//Reset to 0

	/* Open Address Operations */
	if(T->probetype != CHAIN) return Table_OpAddr_Retrieve(T, K);

	/* Separate Chaining Operations */
	else return Table_SepChain_Retrieve(T, K);

	return NULL;
}




/* The number of probes for the most recent call to table_retrieve,
 * table_insert, or table_delete 
 */
int table_stats(table_t *T)
{
	return T->ProbeNum;
}




/* This function is for testing purposes only.  Given an index position into
 * the hash table return the value of the key if data is stored in this 
 * index position.  If the index position does not contain data, then the
 * return value must be zero.  Make the first
 * lines of this function 
 *       assert(0 <= index && index < table_size); 
 *       assert(0 <= list_position); 
 */
hashkey_t table_peek(table_t *T, int index, int list_position)
{
	assert(0 <= index && index < T->size);
	assert(0 <= list_position);

	/* Open Address Operations */
	// Find key at index */
	if(T->probetype != CHAIN) return Table_OA_Peek (T, index);

	/* Separate Chaining Operations */
	else return Table_SC_Peek (T, index, list_position);
}


/* Print the table position and keys in a easily readable and compact format.
 * Only useful when the table is small.
 */
void table_debug_print(table_t *T)
{
	/* Assertions */
	if(T->probetype != CHAIN)
		assert(T->KeyNum <= T->size-1);			//Assert keys don't exceed size if open address
	assert(T->probetype == LINEAR || T->probetype == DOUBLE || T->probetype == CHAIN);

	/* Print Information about table */
	//Print table type
	switch(T->probetype){
		case LINEAR: printf("\nA linear probe table ");			break;
		case DOUBLE: printf("\nA double hashing table ");		break;
		case CHAIN: printf("\nA separate chaining ");			break;
	}
	printf("holding %d entries with a size of %d entries.\n",T->KeyNum,T->size);
	printf("The number of most recent probes is %d.\n", T->ProbeNum);

	/* Open Address Operations */
	if(T->probetype != CHAIN) Table_OpAddr_Debug_Print(T);

	/* Separate Chaining Operations */
	else Table_SepChain_Debug_Print(T);

	printf("\n");
}



/* Function for hash value */
unsigned int HashFunc(hashkey_t K, table_t *T)
{
	return oat_hash(K, strlen(K)) % T->size;
}

/* Function for probe decrement value */
unsigned int ProDecFunc(hashkey_t K, table_t *T)
{
	unsigned int HashMax = (oat_hash(K, strlen(K))/T->size) % T->size;

	//Return 1 if 1 is larger than the value calculated for hash max or probe type is linear
	if(T->probetype == LINEAR || 1 > HashMax)
		return 1;

	return HashMax;
}

/* The One-at-a-Time hash
 * From: https://eternallyconfuzzled.com/hashing-c-introduction-to-hashing
 */
//unsigned oat_hash(hashkey_t key, int len)
//unsigned oat_hash(unsigned key, int len)
unsigned int oat_hash(void *key, int len)
{
    unsigned char *p = key;
    unsigned int h = 0;
    int i;

    for (i = 0; i < len; i++)
    {
        h += p[i];
        h += (h << 10);
        h ^= (h >> 6);
    }

    h += (h << 3);
    h ^= (h >> 11);
    h += (h << 15);

    return h;
}








