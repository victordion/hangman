#include "recall.h"

extern char **symbol_list_word[MAX_LEX_WORD];
extern int symbol_size_word[MAX_LEX_WORD];

extern char KB_file_word[128];
extern char input_word[DEFAULT_WORD_LENGTH+1];
extern char lexicon_array[MAX_LEX_WORD][3];

extern int num_KB;

extern struct kb_row_entry KB_table_word[MAX_KB_NUM_WORD][800]; //each entry is a nonempty row in KB

extern int source_size[MAX_KB_NUM_WORD];

extern int source_target_array_word[MAX_LEX_WORD][MAX_LEX_WORD];


void initialize_word(void)
{
	int i, i1;

	for(i = 0; i < MAX_LEX_WORD; i++) {
		for(i1=0; i1 < MAX_LEX_WORD; i1++) {
			source_target_array_word[i][i1] = -1;
		}
	}
}

void read_KB_word(void)
{
	FILE *KB_fp;
	int kb_count, total_target, row_length, source_count, target_count, i, lex, size, source_id;
	int symbol_count, id;
	char str[512], str1[512], str2[512];
	struct KB_entry *row_ptr;
	int source_lex, target_lex;
	char **ptr;

	printf("reading KB....\n");

	KB_fp = fopen(KB_file_word, "r");
	if(KB_fp == NULL) {
		printf("ERROR: cannot open %s for read\n", KB_file_word);
		exit(0);
	}

	for(i = 0; i < MAX_LEX_WORD; i++) {
		fgets(str, 512, KB_fp);
		sscanf(str, "lex %d symbols size %d\n", &lex, &size);
		symbol_size_word[lex] = size;
		if(lex != i) {
			printf("ERROR: lex id midmatch %d != %d\n", lex, i);
			printf("%s\n", str);
			exit(0);
		}

		symbol_list_word[i] = (char **) malloc(sizeof(char *)*size);
		for(symbol_count = 0; symbol_count < size; symbol_count++) {
			fgets(str, 512, KB_fp);
			sscanf(str, "%d %s\n", &id, str1);
			symbol_list_word[i][symbol_count] = (char *)malloc(sizeof(char)*(strlen(str1)+1));
			strcpy(symbol_list_word[i][symbol_count], str1);
		}
	}
	

	fgets(str, 512, KB_fp);
	while(!feof(KB_fp)) {
		source_count = 0;
		target_count = 0;

		sscanf(str, "KB %d source %d target %d\n", &kb_count, &source_lex, &target_lex);
		//printf(" KB %d source %d target %d\n", kb_count, source_lex, target_lex);
		//getchar();
		source_target_array_word[source_lex][target_lex] = kb_count;

		while(1) {
			fgets(str, 512, KB_fp);
			if(feof(KB_fp)) break;

			if(strstr(str, "size") == NULL) break;

			sscanf(str, "source %d size %d\n", &source_id, &row_length);
			//if(kb_count == 1276) 
			//	printf("source %d count %d length %d\n",  source_id, source_count, row_length);
			KB_table_word[kb_count][source_count].size = row_length;
            //if(kb_count == 1276 && source_count == 561) printf("1276 561 row length = %d\n", row_length);
			row_ptr = (struct KB_entry *)malloc(sizeof(struct KB_entry)*(row_length+2));
			KB_table_word[kb_count][source_count].ptr = row_ptr;
			for(i = 0; i < row_length; i++) {
				fgets(str, 512, KB_fp);
				sscanf(str, "%d %d %f\n", &source_id, 
					&(row_ptr[i].target_id), &(row_ptr[i].KB_value));
				//if(kb_count == 1276 && source_count == 559) {
				//	printf("kb 1276 source 559 target %d\n", row_ptr[i].target_id);
				//}
				
			}	
			source_count++;
			if(feof(KB_fp)) break;
		}
		//printf("done kb %d read string %s str %d\n", kb_count, str, strstr(str, "KB ") == NULL);
		if(feof(KB_fp) || (strstr(str, "KB ") == NULL) ) return;
	}

	
}
