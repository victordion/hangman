#include "recall.h"
#include <iostream>

extern char KB_file_sentence[128];
extern char input_sentence[DEFAULT_SENTENCE_LENGTH][DEFAULT_WORD_LENGTH];
extern char recall_sentence[DEFAULT_SENTENCE_LENGTH][DEFAULT_WORD_LENGTH];

extern int num_KB;
extern int global_dictionary_size;

extern struct kb_row_entry *sentence_KB_row[MAX_KB_NUM_SENTENCE]; //each entry is a nonempty row in KB

extern int source_target_array_sentence[MAX_LEX_SENTENCE][MAX_LEX_SENTENCE];
extern char** dictionary;

extern struct hash_entry hash_table[LONG_HASH_SIZE];

int add_hash(char *, int, int);
u4      hash( register char *k, u4 length, u4 initval, int size);   

void read_source_target_sentence(void);
void read_dictionary_sentence(void);
void read_lex_sentence(void);
void read_kb_sentence(void);

FILE * KB_fp_sentence;
int kb_source_size[MAX_KB_NUM_SENTENCE];

void initialize_sentence(void)
{

	for(int i = 0; i < MAX_LEX_SENTENCE; i++) {
		for(int j = 0; j < MAX_LEX_SENTENCE; j++) {
			source_target_array_sentence[i][j] = -1;
		}
	}

	for(int i = 0; i < LONG_HASH_SIZE; i++) {
		hash_table[i].sym_id = 0;
		hash_table[i].next_ptr = NULL;	
	}
}

void read_KB_sentence(void)
{
	int kb_count, total_target, row_length, source_count, target_count, i;
	char str[512], str1[512], str2[512];
	struct KB_entry *row_ptr;
	int source_lex, target_lex;
	char **ptr;

	printf("reading KB....\n");

	KB_fp_sentence = fopen(KB_file_sentence, "r");
	
	if(KB_fp_sentence == NULL) {
		printf("ERROR: cannot open %s for read\n", KB_file_sentence);
		exit(0);
	}

	printf("read source target .....\n");
	read_source_target_sentence();

	printf("read dictionary....\n");
	read_dictionary_sentence();

	//printf("read lex....\n");
	//read_lex_sentence();

	printf("read kb...\n");
	read_kb_sentence();

}

int add_hash(char *s, int loc, int flag)
{
	int len, row;
	struct hash_entry *ptr, *old_ptr;

	len = strlen(s);
	row = hash(s, len, 1, LONG_HASH_SIZE);


	if( hash_table[row].sym_id == 0) {//no collision
		old_ptr = &hash_table[row];
	}
	else {
		ptr = (hash_table[row].next_ptr);
		while(ptr != NULL) {
			if(strcmp(dictionary[ptr->sym_id], s) != 0) {//keep searching
				old_ptr = ptr;
				ptr = ptr->next_ptr;
			}
			else return ptr->sym_id;	
		}
	}
	//if we are here, then we have scanned through the link list but not find the word
	if(flag == 1) return -1;
	ptr = (struct hash_entry*)malloc(sizeof(struct hash_entry));
	ptr->sym_id = loc;
	ptr->next_ptr = NULL;
	old_ptr->next_ptr = ptr;
	(hash_table[row].sym_id)++;
	return 0;
}

#define mix(a,b,c) \
{ \
	a=a-b;  a=a-c;  a=a^(c>>13); \
	b=b-c;  b=b-a;  b=b^(a<<8);  \
	c=c-a;  c=c-b;  c=c^(b>>13); \
	a=a-b;  a=a-c;  a=a^(c>>12); \
	b=b-c;  b=b-a;  b=b^(a<<16); \
	c=c-a;  c=c-b;  c=c^(b>>5);  \
	a=a-b;  a=a-c;  a=a^(c>>3);  \
	b=b-c;  b=b-a;  b=b^(a<<10); \
	c=c-a;  c=c-b;  c=c^(b>>15); \
}

u4 hash(register char *k, u4 length, u4 initval, int size)
{
	register u4 a,b,c;  /* the internal state */
	u4          len;    /* how many key bytes still need mixing */

	/* Set up the internal state */
	len = length;
	a = b = 0x9e3779b9;  /* the golden ratio; an arbitrary value */
	c = initval;         /* variable initialization of internal state */

	/*---------------------------------------- handle most of the key */
	while (len >= 12)
	{
		a=a+(k[0]+((u4)k[1]<<8)+((u4)k[2]<<16) +((u4)k[3]<<24));
		b=b+(k[4]+((u4)k[5]<<8)+((u4)k[6]<<16) +((u4)k[7]<<24));
		c=c+(k[8]+((u4)k[9]<<8)+((u4)k[10]<<16)+((u4)k[11]<<24));
		mix(a,b,c);
		k = k+12; len = len-12;
	}

	/*------------------------------------- handle the last 11 bytes */
	c = c+length;
	switch(len)              /* all the case statements fall through */
	{
		case 11: c=c+((u4)k[10]<<24);
		case 10: c=c+((u4)k[9]<<16);
		case 9 : c=c+((u4)k[8]<<8);
						 /* the first byte of c is reserved for the length */
		case 8 : b=b+((u4)k[7]<<24);
		case 7 : b=b+((u4)k[6]<<16);
		case 6 : b=b+((u4)k[5]<<8);
		case 5 : b=b+k[4];
		case 4 : a=a+((u4)k[3]<<24);
		case 3 : a=a+((u4)k[2]<<16);
		case 2 : a=a+((u4)k[1]<<8);
		case 1 : a=a+k[0];
						 /* case 0: nothing left to add */
	}
	mix(a,b,c);
	a=size - 1;
	c = c & a;
	/*-------------------------------------------- report the result */
	return c;
}

void read_dictionary_sentence(void)
{
	int i, dictionary_size, j;
	char str[512], s1[512], s2[512];

	fgets(str, 512, KB_fp_sentence);
	sscanf(str, "dictionary size %d\n", &dictionary_size);

	global_dictionary_size = dictionary_size;

	dictionary = (char **) malloc(sizeof(char *) * dictionary_size);

	for(i = 0; i < dictionary_size; i++) {
		strcpy(s1, "\0");
		strcpy(s2, "\0|");
		fgets(str, 512, KB_fp_sentence);
		sscanf(str, "%d %s %s", &j, s1, s2);
		if(strlen(s2) != 0) {
			strcat(s1, " ");
			strcat(s1, s2);
			//printf("dictionary s1 %s\n", s1);
			//getchar();
		}
		//fyang test
		//printf("expected: %d actual: %d word: %s \n",i,j,s1);
		if(i != j) {
			printf("ERROR: dictionary index does not match\n");
			exit(0);
		}
		dictionary[i] = (char*)malloc(sizeof(char)*strlen(s1));
		strcpy(dictionary[i], s1);

		add_hash(s1, i, 0); 	
	}
}

void read_source_target_sentence(void)
{
	int i, j, s, t, k;
	char str[512];

	for(i = 0; i < MAX_LEX_SENTENCE; i++) {
		for(j = 0; j < MAX_LEX_SENTENCE; j++) {
			fgets(str, 512, KB_fp_sentence);
			sscanf(str, "source lex %d target lex %d kb %d\n", &s, &t, &k);
			std::cout << s << " " << t << " "<< k << std::endl;
			source_target_array_sentence[s][t] = k;
		}
	}
}

void read_kb_sentence(void)
{
	int kb_count, source_count, target_count, i, j, k;
	int source_size, target_lex;
	float value, pb;
	int num_rows, row_length;
	char str[512];

	for(kb_count = 0; kb_count < REAL_KB_NUM_SENTENCE; kb_count++) {
		fgets(str, 512, KB_fp_sentence);
		sscanf(str, "KB %d source size %d\n", &i, &source_size);
		if(i != kb_count) {
			printf("ERROR: kb not match %d != %d\n", kb_count, i);
			exit(0);
		}

		num_rows = source_size;	
		sentence_KB_row[kb_count] = (struct kb_row_entry *)malloc(sizeof(struct kb_row_entry)*source_size );
		kb_source_size[kb_count] = num_rows;

#ifdef DEBUG_ALL
		printf("kb %d num rows = %d\n", kb_count, num_rows);
#endif
		for(source_count = 0; source_count < num_rows; source_count++) {
			fgets(str, 512, KB_fp_sentence);
			//sscanf(str, "source %d size %d\n", &i, &row_length);
			sscanf(str, "source %d size %d pb %f\n", &i, &row_length, &pb);

			sentence_KB_row[kb_count][source_count].size = row_length;
			sentence_KB_row[kb_count][source_count].source_sym = i;
			sentence_KB_row[kb_count][source_count].ptr = 
				(struct KB_entry *)malloc(sizeof(struct KB_entry)*row_length);

			for(target_count = 0; target_count < row_length; target_count++) {
				fgets(str, 512, KB_fp_sentence);
				sscanf(str, "%d %d %f\n", &i, &j, &value);
				(sentence_KB_row[kb_count][source_count].ptr)[target_count].target_id = j;
				(sentence_KB_row[kb_count][source_count].ptr)[target_count].KB_value = value;
			}
		}

	}	

	fclose(KB_fp_sentence);

}
