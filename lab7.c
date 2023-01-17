/* lab7.c 
 * Lab7: Hash Tables 
 * Roderick "Rance" White
 * roderiw
 * ECE 2230, Fall 2020
 *
 * Version: 1
 *
 * This file contains drivers to test the Hash Table ADT package.
 *
 *   -m to set the table size
 *   -a to set the load factor
 *   -h to set the type of probe sequence {linear|double|chain}
 *
 * The -r driver builds a table using table_insert and then accesses
 * keys in the table using table_retrieve.  Use
 *   -r run the retrieve driver and specifiy the type of initial table keys 
 *      (rand|seq)
 *   -t to set the number of access trials 
 *
 * Another test driver tests random inserts and deletes.  This driver builds
 * an initial table with random keys, and then performs insertions and deletions
 * with equal probability.
 *   -e run the equilibrium driver
 *
 * To test using the rehash driver.  The table size must be at least 6
 *   -b
 *
 * Custom driver builds, mostly for testing unfctions. 
 *	 -u
 *
 * Bugs: 
 *    rehash driver (-b) does not check table stats or delete keys
 *    have not designed method to create worst case table in which many
 *       keys hash into same table index
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

/* constants used with Global variables */

enum TableType_t {RAND, SEQ};


#define TRUE 1
#define FALSE 0
#define MAXCHARS 8

/* Global variables for command line parameters.  */
int Verbose = FALSE;
static int TableSize = 11;
static int ProbeDec = LINEAR;
static double LoadFactor = 0.9;
static int TableType = RAND;
static int RetrieveTest = FALSE;
static int EquilibriumTest = FALSE;
static int RehashTest = FALSE;
static int Trials = 50000;
static int Seed = 147974267;

int UnitNumber = 0;

#define CLASSSIZE 47
char *classroll[] = {
"grega"  , "emarnol", "clbarcl", "jbjella", "jcboone", "jhbrant", "vjgb"   , 
"burrou5", "cpcahil", "ccasey2", "cecasey", "tdoolit", "jdunca5", "bedmund", 
"bformby", "crgrigg", "naiymh" , "cjenki8", "mklengs", "mrkraft", "yiweil" , 
"mjmarin", "maria9" , "mlmayer", "zmeinin", "hmorila", "cmyers9", "jdneigh", 
"zoldber", "ashlesp", "apicker", "cradlei", "nratajc", "brreyno", "jcreyno", 
"mrickus", "jsmit62", "jspoone", "jtausch", "lthoma8", "aidant" , "roderiw", 
"gfwilki", "frankyw", "uwosu"  , "dannyz" , "harlanr" 
};

/* prototypes for functions in this file only */
void getCommandLine(int argc, char **argv);
void equilibriumDriver(void);
void RetrieveDriver(void);
void RehashDriver(int);
void UnitDriver(void);
int build_random(table_t *T, int ,int);
int build_seq(table_t *T, int, int);
void performanceFormulas(double);
int find_first_prime(int number);
char *generate_random_id();

int main(int argc, char **argv)
{
    getCommandLine(argc, argv);
    printf("Table size (%d), load factor (%g)\n", TableSize, LoadFactor);
    if (ProbeDec == LINEAR)
        printf("Open addressing with linear probe sequence\n");
    else if (ProbeDec == DOUBLE)
        printf("Open addressing with double hashing\n");
    else if (ProbeDec == CHAIN)
        printf("Separate chaining\n");
    printf("Seed: %d\n", Seed);
    srand48(Seed);

    /* ----- small table tests  ----- */

    if (RehashTest)                         /* enable with -b */
        RehashDriver(TableSize);

    /* ----- large table tests  ----- */
    if (RetrieveTest)                        /* enable with -r flag */
        RetrieveDriver();

    /* test for performance in equilibrium */
    if (EquilibriumTest)                   /* enable with -e flag */
        equilibriumDriver();

	/* ----- Unit Driver Tests  ----- */
	if (UnitNumber)							/* enable with -u flag */
		UnitDriver();

    return 0;
}



/* Driver to test various Unit tests
 * Test 1:		Create and destroy table
 * Test 2:		Insert entries
 * Test 3:		Retrieve entries
 * Test 4:		Delete entries
 * Test 5:		Peek function
 * Test 10:		Each test in succession (mostly to ensure all works well at once)
 */
void UnitDriver()
{
	int i, p, *pp;
	table_t *unit_table;

	/* Test 1: Testing table construct and destruct functions */
	if (UnitNumber >= 1)
	{
		if(UnitNumber == 1) printf("\n----- Begin Test 1: Construction and Destruction -----\n");
		unit_table = table_construct(TableSize, ProbeDec);

		/* Test 2: Testing insert entry function */
		if(UnitNumber >= 2)
		{
			if(UnitNumber == 2) printf("\n----- Begin Test 2: Insert Entry -----\n");
			// fill table sequentially 
			for (i = 0; i < TableSize-1; i++) {
				pp = (int *) malloc(sizeof(int));
				*pp = 10*i;
				assert(table_full(unit_table) == 0);

				//printf the information attempting to be inserted
				printf("i: %d (Key: %s, Info: %d) ", i, classroll[i], *pp);

				p = table_insert(unit_table, strdup(classroll[i]), pp);

				/* Print if the insert was successful */
				if(p == 0) printf("was successfully inserted.\n");
				else if(p == 1) printf("was successfully replaced.\n");
				else printf("failed to be inserted.\n");

				pp = NULL;
			}
			if (Verbose) {
				printf("\nTABLE AFTER INSERTION\n");
				table_debug_print(unit_table);
			}
			printf("\n");
		}

		/* Test 3, Testing retrieve entry function */
		if(UnitNumber == 3 || UnitNumber == 10) {
			printf("\n----- Begin Test 3: Retrieve Entry -----\n");
			for (i = 0; i < TableSize-1; i++) {
				pp = table_retrieve(unit_table, classroll[i]);	// Extract next probe key

				/* Print if key was found */
				if(pp == NULL) 
					printf("i: %d, Key: %s, Not found\n", i, classroll[i]);
				else 
					printf("i: %d, Key: %s, Retrieved Info: %d\n", i, classroll[i], *pp);
			}
			printf("\n");
		}

		/* Test 4: Testing delete entry function */
		if(UnitNumber == 4 || UnitNumber == 10) {
			printf("\n----- Begin Test 4: Delete Entry -----\n");
			for (i = 0; i < TableSize-1; i++) {
				pp = table_delete(unit_table, classroll[i]);	// Extract next probe key

				/* Print if key was deleted */
				if(pp == NULL) 
					printf("i: %d, Key: %s, Not deleted\n", i, classroll[i]);
				else 
					printf("i: %d, Key: %s, Retrieved Info: %d\n", i, classroll[i], *pp);
				free(pp);										// Must free retrieved information
			}
			printf("\n");
		}

		/* Test 5: Testing entry peek function function */
		if(UnitNumber == 5 || UnitNumber == 10) {
			printf("\n----- Begin Test 5: Peek At Entry -----\n");

			printf("Key %s found at %d, %d\n",table_peek(unit_table,TableSize/3,0),TableSize/3,0);
			printf("Key %s found at %d, %d\n", table_peek(unit_table, 4, 1), 4, 1);
			//Testing out of bounds for list position
			printf("Key %s found at %d, %d\n", table_peek(unit_table,2,TableSize+1),2,TableSize+1);
			printf("\n");
		}

		printf("\n----- End Print -----\n");
		table_debug_print(unit_table);
		table_destruct(unit_table);
	}
}




void build_table(table_t *test_table, int num_keys)
{
    int probes = -1;
    printf("  Build table with");
    if (TableType == RAND) {
        printf(" %d random keys\n", num_keys);
        probes = build_random(test_table, TableSize, num_keys);
    } else if (TableType == SEQ) {
        printf(" %d sequential keys\n", num_keys);
        probes = build_seq(test_table, TableSize, num_keys);
    } else {
        printf("invalid option for table type\n");
        exit(7);
    }
    printf("    The average number of probes for a successful search = %g\n", 
            (double) probes/num_keys);

    if (Verbose)
        table_debug_print(test_table);

    int size = table_entries(test_table);
    assert(size == num_keys);
}

/* driver to test small tables.  This is a series of 
 * simple tests and is not exhaustive.
 *
 * input: test_M is the table size for this test run
 */
void RehashDriver(int test_M)
{
    int i, *ip, code;
    table_t *H;

    printf("\n----- Rehash driver -----\n");
    if (test_M < 6 || test_M > CLASSSIZE) {
        printf("RehashDriver designed for table size from 6 to %d\n", CLASSSIZE);
        printf("Re-run with -m x for x in this range\n");
        exit(1);
    }
    assert(test_M > 5 && test_M <= CLASSSIZE);  // tests designed for size at least 6
    H = table_construct(test_M, ProbeDec);

    // fill table sequentially 
    for (i = 0; i < test_M-1; i++) {
        ip = (int *) malloc(sizeof(int));
        *ip = 10*i;
        assert(table_full(H) == 0);
        code = table_insert(H, strdup(classroll[i]), ip);
        ip = NULL;
        assert(code == 0);
        assert(table_entries(H) == i+1);
    }
    if (Verbose) {
        printf("\nfull table, last entry empty\n");
        table_debug_print(H);
    }
    // tests on empty position
    assert(NULL == table_retrieve(H, classroll[i]));
    // if table is full verify cannot insert
    if (ProbeDec != CHAIN) {
        assert(table_full(H) == 1);
        assert(-1 == table_insert(H, classroll[i], NULL));
    }

    for (i = 0; i < test_M-1; i++) {
        ip = table_retrieve(H, classroll[i]);
        assert(*(int *)ip == 10*i);
        ip = NULL;
        ip = (int *) malloc(sizeof(int));
        *ip = 99*i;
        assert(1 == table_insert(H, strdup(classroll[i]), ip));
        ip = NULL;
        ip = table_retrieve(H, classroll[i]);
        assert(*(int *)ip == 99*i);
        ip = NULL;
    }
    assert(table_entries(H) == test_M-1);
    if (ProbeDec != CHAIN) 
        assert(table_full(H) == 1);
    // delete tests
    assert(table_deletekeys(H) == 0);
    ip = table_delete(H, classroll[1]);
    assert(*(int *)ip == 99);
    free(ip); ip = NULL;
    if (Verbose) {
        printf("\nsecond entry deleted, last entry empty\n");
        table_debug_print(H);
    }
    assert(table_entries(H) == test_M-2);
    assert(table_full(H) == 0);
    if (ProbeDec != CHAIN) 
        assert(table_deletekeys(H) == 1);
    ip = table_retrieve(H, classroll[1]);  // check key is not there
    assert(ip == NULL);
    //assert(table_stats(H) >= 2);
    // attempt to delete keys not in table 
    assert(NULL == table_delete(H, classroll[1]));
    assert(NULL == table_delete(H, classroll[test_M-1]));
    // insert key in its place
    ip = (int *) malloc(sizeof(int));
    *ip = 123;
    assert(0 == table_insert(H, strdup(classroll[CLASSSIZE-1]), ip));
    ip = NULL;
    ip = table_retrieve(H, classroll[CLASSSIZE-1]);
    assert(*(int *)ip == 123);
    ip = NULL;
    assert(table_entries(H) == test_M-1);
    if (ProbeDec != CHAIN) 
        assert(table_full(H) == 1);
    for (i = 2; i < test_M-1; i++) {     // clear out all but two keys
        ip = table_delete(H, classroll[i]);
        assert(*(int *)ip == 99*i);
        free(ip); ip = NULL;
    }
    assert(table_entries(H) == 2);
    if (Verbose) {
        printf("\n after clearing out all but two keys\n");
        table_debug_print(H);
    }
    ip = (int *) malloc(sizeof(int));    // fill last empty
    *ip = 456;
    assert(0 == table_insert(H, strdup("abcdefg"), ip));
    ip = NULL;
    assert(table_entries(H) == 3);
    if (Verbose) {
        printf("\n after inserting into delete position \n");
        table_debug_print(H);
    }
    // unsuccessful search when no empty keys
    assert(NULL == table_retrieve(H, "aaaaaaa"));

    // two keys the collide in position 0 -- broken
    ip = (int *) malloc(sizeof(int));
    *ip = 77;
    assert(0 == table_insert(H, strdup(classroll[2]), ip));
    ip = (int *) malloc(sizeof(int));
    *ip = 88;
    assert(0 == table_insert(H, strdup(classroll[3]), ip));
    ip = NULL;
    assert(table_entries(H) == 5);
    ip = table_delete(H, classroll[0]);  // delete position 0
    assert(*(int *)ip == 0);
    free(ip); ip = NULL;
    assert(table_entries(H) == 4);
    ip = (int *) malloc(sizeof(int));  // replace 
    *ip = 87;
    assert(1 == table_insert(H, strdup(classroll[3]), ip));
    ip = NULL;
    assert(table_entries(H) == 4);
    ip = (int *) malloc(sizeof(int));   // put back position 0
    *ip = 76;
    assert(0 == table_insert(H, strdup(classroll[0]), ip));
    ip = NULL;
    assert(table_entries(H) == 5);
    // verify 5 items in table
    if (Verbose) {
        printf("\n verify 5 items in table before rehash \n");
        table_debug_print(H);
    }
    ip = table_retrieve(H, classroll[CLASSSIZE-1]);
    assert(*(int *)ip == 123);
    ip = table_retrieve(H, classroll[2]);
    assert(*(int *)ip == 77);
    ip = table_retrieve(H, classroll[3]);
    assert(*(int *)ip == 87);
    ip = table_retrieve(H, classroll[0]);
    assert(*(int *)ip == 76);
    ip = table_retrieve(H, "abcdefg");
    assert(*(int *)ip == 456);
    ip = NULL;
    // rehash
    H = table_rehash(H, test_M);
    assert(table_entries(H) == 5);
    assert(table_deletekeys(H) == 0);
    if (Verbose) {
        printf("\ntable after rehash with 5 items\n");
        table_debug_print(H);
    }
    // verify 5 items in table
    ip = table_retrieve(H, classroll[CLASSSIZE-1]);
    assert(*(int *)ip == 123);
    ip = table_retrieve(H, classroll[2]);
    assert(*(int *)ip == 77);
    ip = table_retrieve(H, classroll[3]);
    assert(*(int *)ip == 87);
    ip = table_retrieve(H, classroll[0]);
    assert(*(int *)ip == 76);
    ip = table_retrieve(H, "abcdefg");
    assert(*(int *)ip == 456);
    ip = NULL;

    // rehash and increase table size
    // If linear or chain then double the size
    // If double, need new prime
    int new_M = 2*test_M;
    if (ProbeDec == DOUBLE)
        new_M = find_first_prime(new_M);

    H = table_rehash(H, new_M);
    assert(table_entries(H) == 5);
    assert(table_deletekeys(H) == 0);
    if (Verbose) {
        printf("\nafter increase table to %d with 5 items\n", new_M);
        table_debug_print(H);
    }
    // verify 5 keys and information not lost during rehash
    ip = table_retrieve(H, classroll[CLASSSIZE-1]);
    assert(*(int *)ip == 123);
    ip = table_retrieve(H, classroll[2]);
    assert(*(int *)ip == 77);
    ip = table_retrieve(H, classroll[3]);
    assert(*(int *)ip == 87);
    ip = table_retrieve(H, classroll[0]);
    assert(*(int *)ip == 76);
    ip = table_retrieve(H, "abcdefg");
    assert(*(int *)ip == 456);
    ip = NULL;

    // fill the new larger table
    assert(table_full(H) == 0);
    int new_items = new_M - 1 - 5;
    char *table[new_items];
    for (i = 0; i < new_items; i++) {
        ip = (int *) malloc(sizeof(int));
        *ip = 10*i;
        char *key = generate_random_id();
        table[i] = strdup(key);
        code = table_insert(H, key, ip);
        ip = NULL;
        assert(code == 0);
        assert(table_entries(H) == i+1+5);
    }
    if (ProbeDec != CHAIN) 
        assert(table_full(H) == 1);
    assert(table_entries(H) == new_M-1);
    if (Verbose) {
        printf("\nafter larger table filled\n");
        table_debug_print(H);
    }
    // verify new items are found 
    for (i = 0; i < new_items; i++) {
        ip = table_retrieve(H, table[i]);
        assert(*(int *)ip == 10*i);
        ip = NULL;
        free(table[i]);
    }

    // clean up table
    table_destruct(H);
    printf("----- Passed rehash driver -----\n\n");
}

/* driver to build and test tables. Note this driver  
 * does not delete keys from the table.
 */
void RetrieveDriver()
{
    int i;
    int num_keys;
    int suc_search, suc_trials, unsuc_search, unsuc_trials;
    table_t *test_table;
    hashkey_t key;
    data_t dp;

    /* print parameters for this test run */
    printf("\n----- Retrieve driver -----\n");
    printf("  Trials: %d\n", Trials);

    num_keys = (int) (TableSize * LoadFactor);
    test_table = table_construct(TableSize, ProbeDec);

    build_table(test_table, num_keys);

    if (Trials > 0) {
        /* access table to measure probes for an unsuccessful search */
        suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
        for (i = 0; i < Trials; i++) {
            /* random key */
            key = generate_random_id();
            if (Verbose)
                printf("%d: looking for %s\n", i, key);
            dp = table_retrieve(test_table, key);
            if (dp == NULL) {
                unsuc_search += table_stats(test_table);
                unsuc_trials++;
                if (Verbose)
                    printf("\t not found with %d probes\n", 
                            table_stats(test_table));
            } else {
                // this should be very rare
                suc_search += table_stats(test_table);
                suc_trials++;
                if (Verbose)
                    printf("\t\t FOUND with %d probes (this is rare!)\n", 
                            table_stats(test_table));
                assert(*(int *)dp >= 0 && *(int *)dp < num_keys);
            }
            free(key);
        }
        assert(num_keys == table_entries(test_table));
        if (suc_trials > 0)
            printf("    Avg probes for successful search = %g measured with %d trials\n", 
                    (double) suc_search/suc_trials, suc_trials);
        if (unsuc_trials > 0)
            printf("    Avg probes for unsuccessful search = %g measured with %d trials\n", 
                    (double) unsuc_search/unsuc_trials, unsuc_trials);
    }

    /* print expected values from analysis with compare to experimental
     * measurements */
    performanceFormulas(LoadFactor);

    // access table to measure probes for known keys 
    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    printf("Start looking for class keys\n");
    for (i = 0; i < CLASSSIZE; i++) {
        /* random key */
        if (Verbose)
            printf("%d: looking for %s\n", i, classroll[i]);
        dp = table_retrieve(test_table, classroll[i]);
        if (dp == NULL) {
            unsuc_search += table_stats(test_table);
            unsuc_trials++;
            if (Verbose)
                printf("\t not found with %d probes\n", 
                        table_stats(test_table));
        } else {
            suc_search += table_stats(test_table);
            suc_trials++;
            if (Verbose)
                printf("\t found with %d probes\n", 
                        table_stats(test_table));
            assert(*(int *)dp >= 0 && *(int *)dp < num_keys);
        }
    }
    assert(num_keys == table_entries(test_table));
    if (suc_trials > 0)
        printf("    Avg probes for successful search = %g measured with %d trials\n", 
                (double) suc_search/suc_trials, suc_trials);
    if (unsuc_trials > 0)
        printf("    Avg probes for unsuccessful search = %g measured with %d trials\n", 
                (double) unsuc_search/unsuc_trials, unsuc_trials);
    /* remove and free all items from table */
    table_destruct(test_table);
    printf("----- End of access driver -----\n\n");
}

/* driver to test sequence of inserts and deletes.
*/
void equilibriumDriver(void)
{
    int i, code;
    int num_keys;
    int size;
    int ran_index;
    int suc_search, suc_trials, unsuc_search, unsuc_trials;
    int keys_added, keys_removed;
    int *ip;
    table_t *test_table;
    hashkey_t key;
    data_t dp;
    clock_t start, end;

    /* print parameters for this test run */
    printf("\n----- Equilibrium test driver -----\n");
    printf("  Trials: %d\n", Trials);

    test_table = table_construct(TableSize, ProbeDec);
    num_keys = (int) (TableSize * LoadFactor);

    /* build a table as starting point */
    build_table(test_table, num_keys);
    size = num_keys;

    /* in equilibrium make inserts and removes with equal probability */
    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    keys_added = keys_removed = 0;
    start = clock();
    for (i = 0; i < Trials; i++) {
        if (drand48() < 0.5 && table_full(test_table) == FALSE) {
            // insert only if table not full
            // for separate chaining table is never full
            key = generate_random_id();
            ip = (int *) malloc(sizeof(int));
            *ip = -i;
            /* insert returns 0 if key not found, 1 if older key found */
            if (Verbose) printf("Trial %d, Insert Key %s", i, key);
            code = table_insert(test_table, key, ip);
            if (code == 0) {
                /* key was not in table so added */
                unsuc_search += table_stats(test_table);
                unsuc_trials++;
                keys_added++;
                if (Verbose) printf(" added\n");
            } else if (code == 1) {
                suc_search += table_stats(test_table);
                suc_trials++;
                if (Verbose) printf(" replaced (rare!)\n");
            } else {
                printf("!!!Trial %d failed to insert key (%s) with code (%d)\n", i, key, code);
                exit(10);
            }
        } else if (table_entries(test_table) > TableSize/4) {
            // delete only if table is at least 25% full
            // why 25%?  Would 10% be better?  Lower than 10% will
            // be computationally expensive
            do {
                ran_index = (int) (drand48() * TableSize);
                key = table_peek(test_table, ran_index,0);
            } while (key == NULL);
            if (Verbose) printf("Trial %d, Delete Key %s", i, key);
            int len = strlen(key);
            if (len < 3 || MAXCHARS < len)
            {
                printf("\n\n  table peek failed: invalid key (%s) during trial (%d)\n", key, i);
                exit(12);
            }
            dp = table_delete(test_table, key);
            if (dp != NULL) {
                if (Verbose) printf(" removed\n");
                suc_search += table_stats(test_table);
                suc_trials++;
                keys_removed++;
                //assert(*(int *)dp == key);
                free(dp);
            } else {
                printf("\n!!! failed to find key (%s) in table, trial (%d)!\n", key, i);
                printf("this is a catastrophic error!!!\n");
                exit(11);
            }
        }
    }
    end = clock();

    if (Verbose) {
        printf("Table after equilibrium trials\n");
        table_debug_print(test_table);
    }

    size += keys_added - keys_removed;
    printf("  Keys added (%d), removed (%d) new size should be (%d) and is (%d)\n",
            keys_added, keys_removed, size, table_entries(test_table));
    assert(size == table_entries(test_table));
    printf("  After exercise, time=%g \n",
            1000*((double)(end-start))/CLOCKS_PER_SEC);
    printf("  successful searches during exercise=%g, trials=%d\n", 
            (double) suc_search/suc_trials, suc_trials);
    printf("  unsuccessful searches during exercise=%g, trials=%d\n", 
            (double) unsuc_search/unsuc_trials, unsuc_trials);


    /* test access times for new table */

    /* separate chaining handled differently
     * should improve design of table_peek function so it 
     * returns 0 if count is invalid when using open addressing.  
     * In current design it is ignored
     */
    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    start = clock();
    /* check each position in table for key */
    if (ProbeDec == CHAIN) {
        for (i = 0; i < TableSize; i++) {
            int count = 0;
            key = table_peek(test_table, i, count);
            while (key != NULL) {
                dp = table_retrieve(test_table, key);
                if (dp == NULL) {
                    printf("Failed key (%s) should be at (%d)\n", key, i);
                    exit(15);
                } else {
                    suc_search += table_stats(test_table);
                    suc_trials++;
                    //assert(*(int *)dp == key);
                }
                key = table_peek(test_table, i, ++count);
            }
        }
    }
    else {
        for (i = 0; i < TableSize; i++) {
            key = table_peek(test_table, i, 0);
            if (key != NULL) {
                dp = table_retrieve(test_table, key);
                if (dp == NULL) {
                    printf("Failed to find key (%s) but it is in location (%d)\n", 
                            key, i);
                    exit(16);
                } else {
                    suc_search += table_stats(test_table);
                    suc_trials++;
                    //assert(*(int *)dp == key);
                }
            }
        }
    }
    for (i = 0; i < Trials; i++) {
        /* random key */
        key = generate_random_id();
        dp = table_retrieve(test_table, key);
        if (dp == NULL) {
            unsuc_search += table_stats(test_table);
            unsuc_trials++;
        } 
        free(key);
    }
    end = clock();
    size = table_entries(test_table);
    printf("  After retrieve experiment, time=%g\n",
            1000*((double)(end-start))/CLOCKS_PER_SEC);
    printf("  New load factor = %g\n", (double) size/TableSize);
    printf("  Percent empty locations marked deleted = %g\n",
            (double) 100.0 * table_deletekeys(test_table)
            / (TableSize - table_entries(test_table)));

    printf("   Measured avg probes for successful search=%g, trials=%d\n", 
            (double) suc_search/suc_trials, suc_trials);

    if (ProbeDec == CHAIN && LoadFactor > 0.5  && LoadFactor < 1.5)
    {
        printf("     ** This measure is biased.  See comments\n\n");
        /* The design of the equilibirum driver depends on the uniform
         * selection of keys to insert and remove.  For linear, double, and
         * quadratic probing selecting a key to remove is done with a uniform
         * distribution among all possible keys.  However, for separate
         * chaining, the algorithm simply picks a table location with a uniform
         * distribution, but this is not the same as picking a key with a
         * uniform distribution.  So, there is a bias that a key in a table
         * location with fewer other keys is more likely to be selected.  This
         * causes the average number of probes for a successful search to
         * increase as the equilibrium driver runs for a long time.  To remove
         * the bias, a solution is needed to pick a key with a uniform
         * distribution when chaining is used.  It is not clear how to select a
         * key with low computational cost.
         */
    }
    printf("   Measured avg probes for unsuccessful search=%g, trials=%d\n", 
            (double) unsuc_search/unsuc_trials, unsuc_trials);
    printf("    Do deletions increase avg number of probes?\n");
    performanceFormulas((double) size/TableSize);

    if (Verbose)
        printf("Start looking for class keys\n");
    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    for (i = 0; i < CLASSSIZE; i++) {
        /* random key */
        if (Verbose)
            printf("%d: looking for %s\n", i, classroll[i]);
        dp = table_retrieve(test_table, classroll[i]);
        if (dp == NULL) {
            unsuc_search += table_stats(test_table);
            unsuc_trials++;
            if (Verbose)
                printf("\t not found with %d probes\n", 
                        table_stats(test_table));
        } else {
            suc_search += table_stats(test_table);
            suc_trials++;
            if (Verbose)
                printf("\t found with %d probes\n", 
                        table_stats(test_table));
            //assert(*(int *)dp >= 0 && *(int *)dp < num_keys);
        }
    }
    printf("Class keys not in table %d in table %d\n", unsuc_trials, suc_trials);

    /* rehash and retest table */
    printf("  Rehash table\n");
    test_table = table_rehash(test_table, TableSize);
    /* number entries in table should not change */
    assert(size == table_entries(test_table));
    /* rehashing must clear all entries marked for deletion */
    assert(0 == table_deletekeys(test_table));

    // all the class keys in the table must still be there
    if (Verbose)
        printf("Start looking for class keys\n");
    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    for (i = 0; i < CLASSSIZE; i++) {
        /* random key */
        if (Verbose)
            printf("%d: looking for %s\n", i, classroll[i]);
        dp = table_retrieve(test_table, classroll[i]);
        if (dp == NULL) {
            unsuc_search += table_stats(test_table);
            unsuc_trials++;
            if (Verbose)
                printf("\t not found with %d probes\n", 
                        table_stats(test_table));
        } else {
            suc_search += table_stats(test_table);
            suc_trials++;
            if (Verbose)
                printf("\t found with %d probes\n", 
                        table_stats(test_table));
            //assert(*(int *)dp >= 0 && *(int *)dp < num_keys);
        }
    }
    printf("Class keys not in table %d in table %d\n", unsuc_trials, suc_trials);

    /* test access times for rehashed table */

    suc_search = suc_trials = unsuc_search = unsuc_trials = 0;
    start = clock();
    /* check each position in table for key */
    if (ProbeDec == CHAIN) {
        for (i = 0; i < TableSize; i++) {
            int count = 0;
            key = table_peek(test_table, i, count);
            while (key != NULL) {
                dp = table_retrieve(test_table, key);
                if (dp == NULL) {
                    printf("Failed key (%s) should be at (%d)\n", key, i);
                    exit(25);
                } else {
                    suc_search += table_stats(test_table);
                    suc_trials++;
                    //assert(*(int *)dp == key);
                }
                key = table_peek(test_table, i, ++count);
            }
        }
    }
    else {
        for (i = 0; i < TableSize; i++) {
            key = table_peek(test_table, i, 0);
            if (key != NULL) {
                dp = table_retrieve(test_table, key);
                if (dp == NULL) {
                    printf("Failed to find key (%s) after rehash but it is in location (%d)\n", 
                            key, i);
                    exit(26);
                } else {
                    suc_search += table_stats(test_table);
                    suc_trials++;
                    //assert(*(int *)dp == key);
                }
            }
        }
    }
    for (i = 0; i < Trials; i++) {
        /* random key with uniform distribution */
        key = generate_random_id();
        dp = table_retrieve(test_table, key);
        if (dp == NULL) {
            unsuc_search += table_stats(test_table);
            unsuc_trials++;
        } 
        free(key);
    }
    end = clock();
    size = table_entries(test_table);
    printf("  After rehash, time=%g\n",
            1000*((double)(end-start))/CLOCKS_PER_SEC);
    printf("   Measured avg probes for successful search=%g, trials=%d\n", 
            (double) suc_search/suc_trials, suc_trials);

    printf("   Measured avg probes for unsuccessful search=%g, trials=%d\n", 
            (double) unsuc_search/unsuc_trials, unsuc_trials);

    /* remove and free all items from table */
    table_destruct(test_table);

    printf("----- End of equilibrium test -----\n\n");
}

char *generate_random_id() 
{
    int i;
    float rv = drand48();
    int num_chars;
    if (rv < 0.7)
        num_chars = MAXCHARS;
    else if (rv < 0.9)
        num_chars = MAXCHARS-1;
    else 
        num_chars = MAXCHARS-2;
    char *id = (char *) malloc((num_chars+1) * sizeof(char));
    for (i = 0; i < num_chars; i++) {
        if (i < 3) {
            id[i] = (drand48() * 26) + 'a';
        } else if (i == num_chars-1) {
            id[i] = 'Z';
        } else {
            if (drand48() < 0.5)
                id[i] = (drand48() * 26) + 'a';
            else
                id[i] = (drand48() * 10) + '0';
        }
    }
    id[i] = '\0';
    return id;
}

/* build a table with random keys.  The keys are generated with a uniform
 * distribution.  
 *
 * Use Knuth shuffle to create random permutation of known keys
 */
int build_random(table_t *T, int table_size, int num_addr)
{
    hashkey_t key;
    int i, code;
    int probes = 0;
    int known_cnt = 0;
    int *ip;
    int *narray;
    int spacing = num_addr / CLASSSIZE;
    if (spacing < 1) spacing = 1;
    narray = (int *) malloc(CLASSSIZE*sizeof(int));
    for (i = 0; i<CLASSSIZE; i++)
        narray[i] = i;
    for (i = 0; i<CLASSSIZE-1; i++) { // leave last as last 
        int spot = (int) (drand48() * (CLASSSIZE-1 - i)) + i;
        assert(i <= spot && spot < CLASSSIZE-1);
        int temp = narray[i]; narray[i] = narray[spot]; narray[spot] = temp;
    }
    for (i = 0; i < num_addr; i++) {
        if ((i+1)%spacing == 0 && known_cnt < CLASSSIZE) {
            key = strdup(classroll[narray[known_cnt]]);
            known_cnt++;
        } else {
            key = generate_random_id();
        }
        ip = (int *) malloc(sizeof(int));
        *ip = i;
        code = table_insert(T, key, ip);
        if (code == 1) {
            i--;   // since does not increase size of table
            // replaced.  The chances should be very small
            //printf("during random build generated duplicate key (%s) on trial (%d)\n", key, i);
            //printf("this should be unlikely: if see more than a few you have a problem\n");
        }
        else if (code != 0) {
            printf("build of random table failed code (%d) index (%d) key (%s)\n",
                    code, i, key);
            exit(2);
        }
        probes += table_stats(T);
    }
    free(narray);
    return probes;
}

// could change first to '!' and last to '~' for longer sequences.
#define FIRSTCHAR 'a'
#define LASTCHAR 'z'
#define KEYBASE (LASTCHAR - FIRSTCHAR + 1)
/* build a table with sequential keys.  The starting address is random.  The
 * keys are in adjacent table locations.
 *
 * This is rather limited form of sequential keys.  It does create sequences
 * of length 26 for the lower case letters.  Could make the sequences longer
 * but this may be good enought to create large clusters.  
 */
int build_seq(table_t *T, int table_size, int num_addr)
{
    int number[MAXCHARS];
    int i, j, code, carry;
    int *ip;
    int probes = 0;
    assert(KEYBASE >= 10);
    // random initial position
    for (i = 0; i < MAXCHARS; i++) {
            number[i] = (drand48() * KEYBASE); 
    }
    for (j = 0; j < num_addr; j++) {
        char *key = (char *) malloc((MAXCHARS+1) * sizeof(char));
        for (i = 0; i < MAXCHARS; i++) {
            key[i] = number[i] + FIRSTCHAR; 
        }
        key[i] = '\0';
        ip = (int *) malloc(sizeof(int));
        *ip = j;
        code = table_insert(T, key, ip);
        if (code != 0) {
            printf("build of sequential table failed code (%d) index (%d) key (%s)\n",
                    code, i, key);
            exit(3);
        }
        probes += table_stats(T);
        // increment with circular carry
        i = MAXCHARS - 1;
        do {
            carry = 0;
            number[i] += 1;
            if (number[i] >= KEYBASE) {
                number[i] = 0;
                carry = 1;
            }
            i--;
        } while (carry && i >=0);
        if (carry) {
            assert(i == -1);
            assert(number[MAXCHARS-1] == 0);
            number[MAXCHARS-1] = 1;
        }
    }
    return probes;
}

/* return first prime number at number or greater
 *
 * There is at least one prime p such that n < p < 2n
 * for n>=25, n < p < 1.2n
 */
int find_first_prime(int number)
{
    int i, foundfactor;
    double upper;
    assert(number > 1);
    // if even move to next odd
    if (number % 2 == 0)
        number++;
    do {
        foundfactor = 0;      // assume number is prime
        upper = sqrt(number);
        for (i = 3; i < upper + 1; i += 2)
            if (number % i == 0) {
                foundfactor = 1;
                number += 2;  // only test odds
                break;
            }
    } while (foundfactor);
    return number;
}


/* print performance evaulation formulas from Standish pg. 479 and pg 484
 */
void performanceFormulas(double load_factor)
{
    if (TableType == RAND) {
        if (ProbeDec == LINEAR) {
            printf("--- Linear probe sequence performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    0.5 * (1.0 + 1.0/(1.0 - load_factor)));
            printf("    Expected probes for unsuccessful search %g\n",
                    0.5 * (1.0 + pow(1.0/(1.0 - load_factor),2)));
        }
        else if (ProbeDec == DOUBLE) {
            printf("--- Double hashing performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    (1.0/load_factor) * log(1.0/(1.0 - load_factor)));
            printf("    Expected probes for unsuccessful search %g\n",
                    1.0/(1.0 - load_factor));
        }
        else if (ProbeDec == CHAIN) {
            printf("--- Separate chaining performance formulas ---\n");
            printf("    Expected probes for successful search %g\n",
                    1.0 + 0.5 * load_factor);
            printf("    Expected probes for unsuccessful search %g\n", 
                    load_factor);
        }
    }
}

/* read in command line arguments and store in global variables for easy
 * access by other functions.
 */
void getCommandLine(int argc, char **argv)
{
    /* optopt--if an unknown option character is found
     * optind--index of next element in argv 
     * optarg--argument for option that requires argument 
     * "x:" colon after x means argument required
     */
    int c;
    int index;

    while ((c = getopt(argc, argv, "u:m:a:h:i:t:s:erbv")) != -1)
        switch(c) {
			case 'u': UnitNumber = atoi(optarg);	 break;
            case 'm': TableSize = atoi(optarg);      break;
            case 'a': LoadFactor = atof(optarg);     break;
            case 's': Seed = atoi(optarg);           break;
            case 't': Trials = atoi(optarg);         break;
            case 'v': Verbose = TRUE;                break;
            case 'e': EquilibriumTest = TRUE;        break;
            case 'r': RetrieveTest = TRUE;           break;
            case 'b': RehashTest = TRUE;             break;
            case 'h':
                      if (strcmp(optarg, "linear") == 0)
                          ProbeDec = LINEAR;
                      else if (strcmp(optarg, "double") == 0)
                          ProbeDec = DOUBLE;
                      else if (strcmp(optarg, "chain") == 0)
                          ProbeDec = CHAIN;
                      else {
                          fprintf(stderr, "invalid type of probing decrement: %s\n", optarg);
                          fprintf(stderr, "must be {linear | double | chain}\n");
                          exit(1);
                      }
                      break;
            case 'i':
                      if (strcmp(optarg, "rand") == 0)
                          TableType = RAND;
                      else if (strcmp(optarg, "seq") == 0)
                          TableType = SEQ;
                      else {
                          fprintf(stderr, "invalid type of address generation: %s\n", optarg);
                          fprintf(stderr, "must be {rand | seq}\n");
                          exit(1);
                      }
                      break;
            case '?':
                      if (isprint(optopt))
                          fprintf(stderr, "Unknown option %c.\n", optopt);
                      else
                          fprintf(stderr, "Unknown option character `\\x%x'.\n", optopt);
            default:
                      printf("Lab7 command line options\n");
                      printf("General options ---------\n");
                      printf("  -m 11     table size\n");
                      printf("  -a 0.9    load factor\n");
                      printf("  -h linear|double|chain\n");
                      printf("            Type of probing decrement\n");
                      printf("  -r        run retrieve test driver \n");
                      printf("  -b        run basic test driver \n");
                      printf("  -e        run equilibrium test driver\n");
					  printf("  -u 1 | 2 | 3 | 4 | 5 | 10\n");
                      printf("            run unit test driver\n");
                      printf("  -i rand | seq\n");
                      printf("            type of keys for retrieve test driver \n");
                      printf("\nOptions for test driver ---------\n");
                      printf("  -t 50000  number of trials in drivers\n");
                      printf("  -v        turn on verbose prints (default off)\n");
                      printf("  -s 26214  seed for random number generator\n");
                      exit(1);
        }
    for (index = optind; index < argc; index++)
        printf("Non-option argument %s\n", argv[index]);
}


