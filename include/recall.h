#ifndef RECALL_H
#define RECALL_H

#include <stdio.h>
#include <stdlib.h>
#include <memory.h>
#include <time.h>
#include <math.h>
#include <string.h>
#include <ctype.h>

#include <sys/types.h>
#include <sys/mman.h>
#include <sys/times.h>

#include <unistd.h>
#include <fcntl.h>
#include <fenv.h>
#include <assert.h>
#include <errno.h>
#include <sched.h>
#include <time.h>
#include <math.h>

//#define DEBUG_SLCT
//#define DEBUG_SENTENCE              (50)
//#define DEBUG_ALL                   (40)
//#define DEBUG_HIGH                  (30)
//#define DEBUG_MEDIUM                (20)
//#define DEBUG_LOW                   (10)
//#define DEBUG_MEM                   (60)
#define THREAD_INPUT_WORD       wconf_input.input_word
#define THREAD_RECALL_WORD      wconf_input.recall_word
#define THREAD_LEX_ARRAY_WORD   wconf_input.lexicon_array_word
#define THREAD_CDDT_SIZE_WORD   wconf_input.candidate_size_word

//#define DEBUG_TIME
//#define DEBUG_MATCH
//#define DEBUG_RANK 3
//#define DEBUG_MEDIUM
//#define DEBUG_SYNC
//#define DEBUG_THREAD
//#define DEBUG_HIGH
//#define DEBUG_ALL
//#define DEBUG_DISPLAY
//#define DEBUG_SENTENCE
//#define DELAYED

#define DISPLAY
#define DISPLAY_TEXT
#define DISPLAY_IMAGE
#define DISPLAY_PERFORMANCE

#define HIGH_ACTIVE 9
#define MEDIUM_ACTIVE 5
#define NO_ACTIVE  0

#define NUM_THREADS 1
//#define NUM_THREADS 1
#define THREAD_INIT -1
#define THREAD_RUNNING 0
#define THREAD_COMPLETE 1
#define THREAD_READY 2

#define MSG_LEN 	16
#define EMPTY     0
#define NON_EMPTY 1


#define DEFAULT_WORD_LENGTH        ( 20 )
#define MAX_KB_NUM_WORD               ( MAX_LEX_WORD*(MAX_LEX_WORD-1) )
//#define DEBUG                       ( DEBUG_MEDIUM )
#define MAX_AMBIGUOUS_WORD          (15)
#define MAX_AMBIGUOUS_TAG           (5)
#define MAX_ITERATION               (50)
#define BANDGAP_SENTENCE                    ( 14 )
#define BANDGAP                     (1)
#define EL_MIN                    ( 0 )
#define TAG_OFFSET                    ( 39 )
#define MAX_AMBIGUOUS              (700 )
#define MAX_LEX_WORD                  (DEFAULT_WORD_LENGTH*3-3)
#define MAX_WORD_CANDIDATE			(10)
#define MAX_TAG_CANDIDATE			(41)

#define DEFAULT_SENTENCE_LENGTH        ( 20 )
#define MAX_KB_NUM_SENTENCE           ( 96 )
#define REAL_KB_NUM_SENTENCE        96//2662
#define MANUAL                     ( 1 )

#define LONG_HASH_SIZE             (2097152)
#define LOG_HASH_SIZE              (2)
#define MAX_LEX_SENTENCE           (3*DEFAULT_SENTENCE_LENGTH - 1)
#define MAX_DICTIONARY_SIZE     (500000)


#define NO_ACTION               (0)
#define END_SENTENCE             (1)
#define END_WORD                 (2)
#define SKIP                    (3)
#define SPACE                   (4)

#define CLUSTER_SIZE         (24)

#define SENTENCE_BLOCK_SIZE 1

struct kb_row_entry{
	int size;         //length of row
	int source_sym;
	struct KB_entry *ptr;
};


struct KB_entry {
    int target_id;
    float KB_value;
};

struct candidate_entry {
	char str;
	int valid;
};

struct input_word_entry {
	struct candidate_entry *candidate;
	int candidate_size_word;
};

struct wconf_input_entry{
    struct input_word_entry input_word[DEFAULT_WORD_LENGTH];
    int lexicon_array_word[MAX_LEX_WORD][MAX_AMBIGUOUS];
    int candidate_size_word[MAX_LEX_WORD];
    int sentence_count;
    int total_words_count;
    int word_count;
    int current_recall_size_word;
    char **recall_word;
};


typedef  unsigned long int  u4;   /* unsigned 4-byte type */
typedef  unsigned     char  u1;   /* unsigned 1-byte type */



struct hash_entry {
    int sym_id;
    struct hash_entry *next_ptr;
};

struct sentence_lex_table_entry {
        int *lex_list;
        int symbol_size;
};

struct input_sentence_entry {
	int size;
	struct sentence_word_entry *word;
};

struct sentence_word_entry {
        char *str;
        int valid;
};

#endif
