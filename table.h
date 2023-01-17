/* table.h 
 * Lab7: Hash Tables 
 * Roderick "Rance" White
 * roderiw
 * Version: 1
 * ECE 2230, Fall 2020
 */

/* constants used to indicate type of probing.  */
enum ProbeDec_t {LINEAR, DOUBLE, CHAIN};

typedef void *data_t;   /* pointer to the information, I, to be stored in the table */
typedef char *hashkey_t;   /* the key, K, for the pair (K, I) */

typedef struct sep_chain_tag {
    hashkey_t key;
    data_t data_ptr;
    struct sep_chain_tag *next;
} sep_chain_t;

typedef struct table_etag {
    hashkey_t key;
    data_t data_ptr;
    int deleted;
} table_entry_t;

typedef struct table_tag {
    // you need to fill in details, and you can change the names!
    int size;
    int probetype;
    int KeyNum;
    int ProbeNum;
    table_entry_t *oa;
    sep_chain_t **sc;
} table_t;

/* Table Operations */

table_t *table_construct(int table_size, int probe_type);		/* Construct hash table */
table_t *table_rehash(table_t * T, int new_table_size);			/* Rehash table */
void table_destruct(table_t *T);								/* Deallocates memory for T */

int table_entries(table_t *T);									/* Return number of entries */
int table_full(table_t *T);										/* Returns if table is full */
int table_deletekeys(table_t *T);								/* Returns number of removed keys */
int table_stats(table_t *T);									/* Prints stats of table */

int table_insert(table_t *T, hashkey_t K, data_t I);			/* Inserts keys into table */
data_t table_delete(table_t *T, hashkey_t K);					/* Deletes key with matching K */
data_t table_retrieve(table_t *T, hashkey_t K);					/* Retrieves key with matching K */
hashkey_t table_peek(table_t *T, int index, int list_position);

void table_debug_print(table_t *T);								/* Debug prints table */

/* Hashing functions */
unsigned int HashFunc(hashkey_t K, table_t *T);
unsigned int ProDecFunc(hashkey_t K, table_t *T);
unsigned int oat_hash(void *key, int len);

/* Open Addressing Functions */
table_t *Table_OA_Construct(int table_size, int probe_type);
table_t *Table_OA_Rehash (table_t *T, int new_table_size);
void Table_OA_Destruct (table_t *T);

int Table_OpAddr_Insert (table_t *T, hashkey_t K, data_t I);
data_t Table_OpAddr_Delete (table_t *T, hashkey_t K);
data_t Table_OpAddr_Retrieve (table_t *T, hashkey_t K);
hashkey_t Table_OA_Peek (table_t *T, int index);

void Table_OpAddr_Debug_Print (table_t *T);

/* Separate Chaining Functions */
table_t *Table_SC_Construct(int table_size, int probe_type);
table_t *Table_SC_Rehash (table_t *T, int new_table_size);
void Table_SC_Destruct (table_t *T);

int Table_SepChain_Insert (table_t *T, hashkey_t K, data_t I);
data_t Table_SepChain_Delete (table_t *T, hashkey_t K);
data_t Table_SepChain_Retrieve (table_t *T, hashkey_t K);
hashkey_t Table_SC_Peek (table_t *T, int index, int list_position);

void Table_SepChain_Debug_Print (table_t *T);









