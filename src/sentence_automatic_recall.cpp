#include <assert.h>
#include <sys/time.h>
#include <vector>
#include "recall.h"

extern char KB_file_sentence[128];
extern char out_file[128];
extern struct input_sentence_entry input_sentence[SENTENCE_BLOCK_SIZE][DEFAULT_SENTENCE_LENGTH];
extern char recall_sentence[2048];
extern int *lexicon_array_sentence[MAX_LEX_SENTENCE];
extern int *old_lexicon_array_sentence[MAX_LEX_SENTENCE];

extern int num_KB;

extern struct kb_row_entry *sentence_KB_row[MAX_KB_NUM_SENTENCE]; //each entry is a nonempty row in KB

extern int source_target_array_sentence[MAX_LEX_SENTENCE][MAX_LEX_SENTENCE];
extern char ** dictionary;
extern char incorrect_recall[26][2048];
extern int incorrect_index;
extern int repeat_recall;

//extern double start_time, end_time, initial_time, recall_time, total_time;


extern int candidate_size_sentence[MAX_LEX_SENTENCE];
extern int match;
extern int mismatched_words;
extern int total_words_in_sentence;

extern struct hash_entry hash_table[LONG_HASH_SIZE];
extern FILE *test_fp_sentence, *out_fp_sentence;

void     receive_input_sentence(int);
void 	 output_sentence(int);
void	 free_input_sentence(int);

extern void     initialize_sentence(void);
extern void     read_KB_sentence(void);
extern void     do_recall_sentence(int, int,std::vector<std::vector<float> > &);

//double start_time, end_time, initial_time, recall_time, total_time;

int sentence_main(int sentence_id,std::vector<std::vector<float> >& excitationLvl)
{
    char str[256];
	match = 0;
	strcpy(recall_sentence, "\0");
	receive_input_sentence(sentence_id);
//fyang

	do_recall_sentence(1, sentence_id,excitationLvl);
	//Test
	//int i,j;
	
	//end Test
	output_sentence(sentence_id);

	#ifdef DEBUG_ALL
	printf("total words=%d \n", total_words_in_sentence);
    printf("confabulated sentence %d\n", sentence_id);
	#endif
}

void receive_input_sentence(int sentence_id)
{
	int i, word_count, size, letter_count, lex_count;
	char c;
	char str[512], line[2048];

    #ifdef DEBUG_SENTENCE
	printf("got sentence\n");
	for(word_count = 0; word_count < DEFAULT_SENTENCE_LENGTH; word_count++) {
		printf("(");
        if(input_sentence[sentence_id][word_count].size < 0)
        {
            printf("???");
        }
        else{
            for(i = 0; i < input_sentence[sentence_id][word_count].size; i++) {
                printf("%s ", (input_sentence[sentence_id][word_count].word)[i].str);
            }
        }
		printf(")");
	}
	printf("\n");
    #endif

	//allocate memory
    lex_count = 0;
    for(i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) {

        if(lexicon_array_sentence[lex_count] != NULL) {
            printf("SEVERE ERROR!!! lexicon_array_sentence[%d] == %x\n", lex_count,  lexicon_array_sentence[lex_count]);
            exit(1);
        }
		lexicon_array_sentence[lex_count] = (int *)malloc(sizeof(int)*input_sentence[sentence_id][i].size);
        if(old_lexicon_array_sentence[lex_count]!= NULL) {
            printf("SEVERE ERROR!!! old_lexicon_array_sentence[%d] == %x\n", lex_count,  old_lexicon_array_sentence[lex_count]);
            exit(1);
        }
        
		old_lexicon_array_sentence[lex_count] = (int *)malloc(sizeof(int)*input_sentence[sentence_id][i].size);
        lex_count++;
    }

    for(i = 0; i < DEFAULT_SENTENCE_LENGTH - 1; i++) {

        if(lexicon_array_sentence[lex_count] != NULL) {
            printf("SEVERE ERROR!!! lexicon_array_sentence[%d] == %x\n", lex_count,  lexicon_array_sentence[lex_count]);
            exit(1);
        }
		lexicon_array_sentence[lex_count] = (int *)malloc(sizeof(int)*(input_sentence[sentence_id][i].size*input_sentence[sentence_id][i+1].size));
            //printf("allocate %d: lex_count = %d, size = %d\n", i,lex_count, input_sentence[sentence_id][i].size*input_sentence[sentence_id][i+1].size);
        if(old_lexicon_array_sentence[lex_count]!= NULL) {
            printf("SEVERE ERROR!!! old_lexicon_array_sentence[%d] == %x\n", lex_count,  old_lexicon_array_sentence[lex_count]);
            exit(1);
        }
		old_lexicon_array_sentence[lex_count] = (int *)malloc(sizeof(int)*(input_sentence[sentence_id][i].size*input_sentence[sentence_id][i+1].size));
            //printf("allocate old %d: lex_count = %d, size = %d\n", i, lex_count, input_sentence[sentence_id][i].size*input_sentence[sentence_id][i+1].size);
        lex_count++;
    }
    //fyang
    for(i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) {

        if(lexicon_array_sentence[lex_count] != NULL) {
            printf("SEVERE ERROR!!! lexicon_array_sentence[%d] == %x\n", lex_count,  lexicon_array_sentence[lex_count]);
            exit(1);
        }
        lexicon_array_sentence[lex_count] = (int *)malloc(sizeof(int)*MAX_AMBIGUOUS);
        if(old_lexicon_array_sentence[lex_count]!= NULL) {
            printf("SEVERE ERROR!!! old_lexicon_array_sentence[%d] == %x\n", lex_count,  old_lexicon_array_sentence[lex_count]);
            exit(1);
        }
        old_lexicon_array_sentence[lex_count] = (int *)malloc(sizeof(int)*MAX_AMBIGUOUS);
        
        lex_count++;
    }
}

void output_sentence(int sentence_id)
{
	int msg[MSG_LEN];
	int lex_count, dcount, i, word_count1, word_count2, i1, i2;

	int size[MAX_LEX_SENTENCE];
	char candidate_char[DEFAULT_SENTENCE_LENGTH];

	char temp[128], *s, s1[DEFAULT_SENTENCE_LENGTH][DEFAULT_WORD_LENGTH+2],  s2[DEFAULT_SENTENCE_LENGTH][DEFAULT_WORD_LENGTH+2]; 
	int temp_size;
	int *temp_lexicon_array_sentence = NULL;
	std::vector<std::vector<float> > excitationLvl;

	int ambiguity = 0, first_ambiguity = 0;

	for(dcount = 0; dcount < MAX_LEX_SENTENCE; dcount++) {
		size[dcount] = candidate_size_sentence[dcount];
//		if(size[dcount] > 1) {
//			if(ambiguity == 0) first_ambiguity = dcount;
//			ambiguity++;
//		}
	}

    if(temp_lexicon_array_sentence != NULL) {
        printf("SEVERE ERROR!!! temp_lexicon_array_sentence = %x\n", temp_lexicon_array_sentence);
        exit(1);
    }
	temp_lexicon_array_sentence = (int *)malloc(sizeof(int)*candidate_size_sentence[first_ambiguity]);
    #ifdef DEBUG_SENTENCE
	printf("total ambiguity = %d first ambiguity lex = %d\n", ambiguity, first_ambiguity);
    #endif
	for(i = 0; i < candidate_size_sentence[first_ambiguity]; i++) {
		temp_lexicon_array_sentence[i] = lexicon_array_sentence[first_ambiguity][i];
        #ifdef DEBUG_SENTENCE
		printf("temp_lexicon_array_sentence[%d] = %d\n", i, temp_lexicon_array_sentence[i]);
        #endif
	}

	if(ambiguity == 0 ) {
        #ifdef DEBUG_SENTENCE
		printf("************the sentence is*************** \n");
        #endif
		//getchar();
    	for(dcount = 0; dcount < DEFAULT_SENTENCE_LENGTH; dcount++) {
            #ifdef DEBUG_SENTENCE
            printf("size[%d] = %d\n",dcount,size[dcount]);
	//		printf("size[%d]=%d str = %s\n", dcount, size[dcount], (input_sentence[sentence_id][dcount].word)[0].str);
            #endif
			 if(size[dcount] <= 0) {
				if((input_sentence[sentence_id][dcount].word)[0].str == NULL) {
					strcat(recall_sentence, "` ");
				}
				else if(strlen( (input_sentence[sentence_id][dcount].word)[0].str) > 0 && strcmp( (input_sentence[sentence_id][dcount].word)[0].str, "`")) {
					strcat(recall_sentence, (input_sentence[sentence_id][dcount].word)[0].str);
       	     		strcat(recall_sentence, " ");
				}
			 }
			 else {
			 	if(strlen(dictionary[lexicon_array_sentence[dcount][0]]) > 0 && 
					strcmp(dictionary[lexicon_array_sentence[dcount][0]], "`")) {
			 		strcat(recall_sentence, dictionary[lexicon_array_sentence[dcount][0]]);
       	     		strcat(recall_sentence, " ");
				}
			 }
       	}
//fyang
        /*strcat(recall_sentence, "\nTag:");
    	for(dcount = 2*DEFAULT_SENTENCE_LENGTH-1; dcount < 3*DEFAULT_SENTENCE_LENGTH-1; dcount++) {
            printf("size[%d] = %d\n",dcount,size[dcount]);
			 if(size[dcount] <= 0) {
				if((input_sentence[sentence_id][dcount].word)[0].str == NULL) {
                    printf("a ");
					strcat(recall_sentence, "` ");
				}
				else if(strlen( (input_sentence[sentence_id][dcount].word)[0].str) > 0 && strcmp( (input_sentence[sentence_id][dcount].word)[0].str, "`")) {
                    printf("b ");
					strcat(recall_sentence, (input_sentence[sentence_id][dcount].word)[0].str);
       	     		strcat(recall_sentence, " ");
				}
			 }
			 else {
			 	if(strlen(dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][0]]]) > 0){ //&& 
			//		strcmp(dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][0]]], "`")) {
                    printf("c ");
                    strcat(recall_sentence, dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][0]]]);
       	     		strcat(recall_sentence, " ");
				}
			 }
       	}*/

        #ifdef DEBUG_SENTENCE
		printf("ambiguity = 0 Got:%sLen=%d\n", recall_sentence, strlen(recall_sentence));
        #endif

	}
	else if(ambiguity == 1) {
		//printf("************the sentences are*************** \n");
        #ifdef DEBUG_SENTENCE
		printf("size of first ambiguity(%d) = %d\n", first_ambiguity, size[first_ambiguity]);
        #endif
		//for(i = 0; i < size[first_ambiguity]; i++) { for MPI
			for(dcount = 0; dcount < DEFAULT_SENTENCE_LENGTH; dcount++) {
				#ifdef DEBUG_ALL
				printf("size[%d] = %d\n",dcount,size[dcount]);
				#endif
/*				if(dcount == first_ambiguity) {
			 		if(strlen(dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][i]]]) > 0 &&
						strcmp(dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][i]]], "`")) {
			 			strcat(recall_sentence, dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][i]]]);
       	     			strcat(recall_sentence, " ");
					}
				}
				else {
*/			 		if(size[dcount] <= 0) {
						if( (input_sentence[sentence_id][dcount].word)[0].str == NULL) {
							strcat(recall_sentence, "` ");
						}
			 			else if(strlen( (input_sentence[sentence_id][dcount].word)[0].str) > 0 && strcmp( (input_sentence[sentence_id][dcount].word)[0].str, "`")) {
			 				strcat(recall_sentence, (input_sentence[sentence_id][dcount].word)[0].str);
       	     				strcat(recall_sentence, " ");
						}
					}
					else {
			 			if(strlen(dictionary[lexicon_array_sentence[dcount][0]]) > 0 && 
							strcmp(dictionary[lexicon_array_sentence[dcount][0]], "`")) {
			 				strcat(recall_sentence, dictionary[lexicon_array_sentence[dcount][0]]);
       	     				strcat(recall_sentence, " ");
						}
					}
//				}
			}
      //fyang
           /* strcat(recall_sentence, "\nTag:");
			for(dcount = 2*DEFAULT_SENTENCE_LENGTH-1; dcount < 3*DEFAULT_SENTENCE_LENGTH-1; dcount++) {
            printf("size[%d] = %d\n",dcount,size[dcount]);
				if(dcount == first_ambiguity) {
			 		if(strlen(dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][i]]]) > 0 &&
						strcmp(dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][i]]], "`")) {
			 			strcat(recall_sentence, dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][i]]]);
       	     			strcat(recall_sentence, " ");
					}
				}
				else {
			 		if(size[dcount] <= 0) {
						if( (input_sentence[sentence_id][dcount].word)[0].str == NULL) {
							strcat(recall_sentence, "` ");
						}
			 			else if(strlen( (input_sentence[sentence_id][dcount].word)[0].str) > 0 && strcmp( (input_sentence[sentence_id][dcount].word)[0].str, "`")) {
			 				strcat(recall_sentence, (input_sentence[sentence_id][dcount].word)[0].str);
       	     				strcat(recall_sentence, " ");
						}
					}
					else {
			 			if(strlen(dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][0]]]) > 0 && 
							strcmp(dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][0]]], "`")) {
			 				strcat(recall_sentence, dictionary[lex_table_sentence[dcount].lex_list[lexicon_array_sentence[dcount][0]]]);
       	     				strcat(recall_sentence, " ");
						}
					}
				}
			}*/
            #ifdef DEBUG_SENTENCE
			printf("ambiguity = 1 Got:%sLen=%d\n", recall_sentence, strlen(recall_sentence));
            #endif

	}
	else {
        #ifdef DEBUG_SENTENCE
			printf("\n\nambiguity = %d, first ambiguity[%d] size %d\n", ambiguity, first_ambiguity, size[first_ambiguity]);
        	printf("************the partial sentence is *************** \n");
			for(dcount = 0; dcount < DEFAULT_SENTENCE_LENGTH; dcount++) {
				printf("( ");
				for(i = 0; i < candidate_size_sentence[dcount]; i++) {
					printf("%s %d ", dictionary[lexicon_array_sentence[dcount][i]], lexicon_array_sentence[dcount][i]);
				}	
				printf(") ");
			}
			printf("\n");
        #endif
		temp_size = input_sentence[sentence_id][first_ambiguity].size;
        input_sentence[sentence_id][first_ambiguity].size = 1;
        strcpy(temp, input_sentence[sentence_id][first_ambiguity].word[0].str);

        //for(i = 0; i < size[first_ambiguity]; i++) { for MPI
        for(i = 0; i < 1; i++) {
			//printf("str %s copy %s\n", input_sentence[first_ambiguity].word[0].str, dictionary[lex_table_sentence[first_ambiguity].lex_list[temp_lexicon_array_sentence[i]]]);
            strcpy(input_sentence[sentence_id][first_ambiguity].word[0].str, dictionary[temp_lexicon_array_sentence[i]]);

			//printf("str %s copy %s\n", input_sentence[first_ambiguity].word[0].str, dictionary[lex_table_sentence[first_ambiguity].lex_list[temp_lexicon_array_sentence[i]]]);

			//for(i2 = 0; i2 < size[first_ambiguity]; i2++) printf("%s %d ", dictionary[lex_table_sentence[first_ambiguity].lex_list[temp_lexicon_array_sentence[i2]] ], temp_lexicon_array_sentence[i2]);
			//printf("\n");
            do_recall_sentence(1, sentence_id,excitationLvl);

            output_sentence(sentence_id);
			//printf("done partial sentence i = %d\n firsts ambiguity = %d size = %d remaining: \n", i, first_ambiguity, size[first_ambiguity]);
			//for(i2 = 0; i2 < size[first_ambiguity]; i2++) printf("%s ", dictionary[lex_table_sentence[first_ambiguity].lex_list[temp_lexicon_array_sentence[i2]]]);
        }
		//getchar();

        //printf("multi ambiguity\n");
        //exit(1);
		free(input_sentence[sentence_id][first_ambiguity].word[0].str);
        #ifdef DEBUG_MEM
        printf("loc free 8 %x\n", input_sentence[sentence_id][first_ambiguity].word[0].str);
        #endif
        input_sentence[sentence_id][first_ambiguity].word[0].str = NULL;
		input_sentence[sentence_id][first_ambiguity].word[0].str = (char *)malloc(sizeof(char)*(strlen(temp) + 1 ) );
        strcpy(input_sentence[sentence_id][first_ambiguity].word[0].str, temp);
        input_sentence[sentence_id][first_ambiguity].size = temp_size;

	}

	free(temp_lexicon_array_sentence);
  temp_lexicon_array_sentence = NULL;

}

void free_input_sentence(int sentence_id)
{
	int i, i1;
	for(i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) {
            for(i1 = 0; i1 < input_sentence[sentence_id][i].size; i1++) {
                if(input_sentence[sentence_id][i].word[i1].str!= NULL) {
                    free(input_sentence[sentence_id][i].word[i1].str);
                    #ifdef DEBUG_MEM
                    printf("loc free 9 %d %d %d %x\n", sentence_id, i, i1, input_sentence[sentence_id][i].word[i1].str);
                    #endif
                    input_sentence[sentence_id][i].word[i1].str = NULL;
                }
            }
            if(input_sentence[sentence_id][i].word != NULL) {
                free(input_sentence[sentence_id][i].word);
                #ifdef DEBUG_MEM
                printf("loc free 10 %d %d %x\n", sentence_id, i, input_sentence[sentence_id][i].word);
                #endif
                input_sentence[sentence_id][i].word = NULL;
            }
            input_sentence[sentence_id][i].size = 0;
    }

    for(i = 0; i < MAX_LEX_SENTENCE; i++) {
        free(lexicon_array_sentence[i]);
        lexicon_array_sentence[i] = NULL;
        free(old_lexicon_array_sentence[i]);
        old_lexicon_array_sentence[i] = NULL;
    }
}
