#include <assert.h>
#include <vector>
#include <string>
#include "recall.h"


char letter_frequency[26] = {'e','t','a','o','i','n','s','h','r','d','l','c','u','m','w','f','g','y','p','b','v','k','j','x','q','z'};
int rcv_count = 0;
char KB_file_word[128]="dump_word.txt";
int  scratch_size = 1;
int  num_subclusters = 1;
int  num_display = 1;
int  subcluster_size = 24;
struct wconf_input_entry wconf_input;
char reference_word[DEFAULT_WORD_LENGTH+1];


struct kb_row_entry KB_table_word[MAX_KB_NUM_WORD][800]; //each entry is a nonempty row in KB

int source_target_array_word[MAX_LEX_WORD][MAX_LEX_WORD];
int global_dictionary_size;


char **symbol_list_word[MAX_LEX_WORD];
int symbol_size_word[MAX_LEX_WORD];

extern void     do_recall_word(int flag);
extern void     read_KB_word(void);
extern void     initialize_word(void);

FILE  *out_fp;

void     check_command_line_parameters(int argc, char ** argv);
void     expand_recall_word();
void     free_word();

//**********SENTENCE RECALL*************//
char KB_file_sentence[128]="dump_sentence.txt";
char out_file[128]="out_sentence.txt";
struct input_sentence_entry input_sentence[SENTENCE_BLOCK_SIZE][DEFAULT_SENTENCE_LENGTH];
char recall_sentence[2048];
int *lexicon_array_sentence[MAX_LEX_SENTENCE];
int *old_lexicon_array_sentence[MAX_LEX_SENTENCE];

int num_KB;
char incorrect_recall[26][2048];
int incorrect_index;
int repeat_recall;


struct kb_row_entry *sentence_KB_row[MAX_KB_NUM_SENTENCE]; //each entry is a nonempty row in KB

int source_target_array_sentence[MAX_LEX_SENTENCE][MAX_LEX_SENTENCE];
char ** dictionary;

int candidate_size_sentence[MAX_LEX_SENTENCE];
int match;
int mismatched_words = 0;
int total_words_in_sentence = 0;
int money = 1000;

struct hash_entry hash_table[LONG_HASH_SIZE];
FILE *out_fp_sentence, *original_fp;

extern int sentence_main(int);
extern void initialize_sentence(void);
extern void read_KB_sentence(void);
extern void free_input_sentence(int);

extern void func_itr(char *, unsigned char *);

/************VARIABLES FOR MPI****/

/******** variables for diifferent fonts*****/
/*********format ***********/
/*word_id  letter_id  32_tag 225_bit_map*/
/*0~25 lower case unscratched*/
/*26~51 upper case unscratched*/
/*100~125 lower case scratched*/
/*126~151 upper case scratched*/

//format of returning message
//word_id letter_id #candidate candidatelist


extern void load_fonts(int scratch_size);
void clear_sentence(int);
void clear_word();
void receive_and_process(void);
int collect_wconf_results(char *, int);
void sentence_confab(int);
void output_word();
void letter_select(char *str, char &letter);

int word_length_rec[SENTENCE_BLOCK_SIZE][DEFAULT_SENTENCE_LENGTH], sentence_length_rec[SENTENCE_BLOCK_SIZE];
int unknown_count[SENTENCE_BLOCK_SIZE][DEFAULT_SENTENCE_LENGTH];
int letter_select_candidate[26];
double letter_select_frequency[26];

FILE * testfp;

void initialize(void)
{ 
	int success;
	int word_count = 0, letter_count, sentence_count;
	int bsb_rank, bsb_rank_start;
	char str[2048];
	clock_t l_start, l_end;

	int input;

    int i, j, k, stop;
    int adjusted;
    int rc; 

	srand(time(NULL));

    out_fp = fopen("out_word.txt","w");
    if (out_fp == NULL)
    {
        printf("cannot open out_word.txt to write\n");
        exit(0);
    }

    out_fp_sentence = fopen("out_sentence.txt","w");
    if (out_fp_sentence == NULL)
    {
        printf("cannot open out_sentence.txt to write\n");
        exit(0);
    }

    // clear the word and sentence knowledge base
	l_start = clock();
	initialize_word();
	initialize_sentence();

	read_KB_word();
	#ifdef DEBUG_ALL
	printf("done reading word KB\n");
	#endif

	read_KB_sentence();
	#ifdef DEBUG_ALL
	printf("done reading sentence KB\n");
	#endif
	l_end = clock();
	#ifdef DEBUG_TIME
	printf("Load time %.6lf seconds\n", ((double)(l_end - l_start))/CLOCKS_PER_SEC);
	#endif

}

char machine(char * sentence, std::vector<char> selected, char &letter, char * guess, std::vector<std::string>& guessed_sentence)
{
	int OCF = 0;
	char str_tok[2048];
	int word_count, word_len;
	char * pch;

	clear_sentence(0);
	// Update letter candidate index
	int selected_size = selected.size();
	int i;
	for(i = 0; i < 26; i++)
	{
		letter_select_frequency[i] = 0.0;
		letter_select_candidate[i] = 0;
	}
	for(i=0;i<selected_size;i++)
	{
		letter_select_frequency[i] = 0.0;
		letter_select_candidate[selected[i] - 'a'] = -1;
	}

	// Count numbers of candidates for each word
	strcpy(str_tok, sentence);
	word_count = 0;
	OCF = 0;
	pch = strtok(str_tok, " \n");
	while(pch != NULL)
	{
		word_len = strlen(pch);
		if( collect_wconf_results(pch, word_count) == 1)
		{
			OCF = 1;
		}

		pch = strtok(NULL, " \n");
		word_count++;
	}

	// If too many candidates or confabulate an incorrect answer again, select a letter; otherwise, confabulate
	if(OCF == 1)
	{
		letter_select(sentence, letter);
		free_input_sentence(0);
		return 'N';
	}
	else
	{
		sentence_length_rec[0] = word_count;
		sentence_confab(0);
		free_input_sentence(0);
		int guessed_size = guessed_sentence.size();
		for(i = 0; i < guessed_size; i++)
		{
			if(!strcmp(guessed_sentence[i].c_str(), recall_sentence))
			{
				letter_select(sentence, letter);
				return 'N';
			}
		}
		strcpy(guess, recall_sentence);
		strcpy(recall_sentence, "\0");
		return 'Y';
	}
}

void letter_select(char *str, char &letter)
{
    int str_len, i, max_id;
	double max;
	char slct;
	str_len = strlen(str);
	#ifdef DEBUG_SLCT
	printf("\n");
	#endif
	if(money < 250)
	{
		letter_select_frequency['a'-'a'] = 0;
		letter_select_frequency['e'-'a'] = 0;
		letter_select_frequency['i'-'a'] = 0;
		letter_select_frequency['o'-'a'] = 0;
		letter_select_frequency['u'-'a'] = 0;
	}
	max = 0;
	for(i=0;i<26;i++)
	{
		#ifdef DEBUG_SLCT
		printf("lc[%c] %f,", 'a'+i, letter_select_frequency[i]);
		#endif
		if(letter_select_frequency[i] > max)
		{
			max = letter_select_frequency[i];
			max_id = i;
		}
	}
	if((max - 0) < 1e-8)
	{
		printf("Random selection\n");
		do
		{
			max_id = rand() % 26;
		}while(letter_select_candidate[max_id] == -1);
	}
	slct = 'a' + max_id;
	#ifdef DEBUG_SLCT
	printf("\nmaxid = %d, slct = %c\n", max_id, slct);
	#endif
	letter_select_candidate[max_id] = -1;
	for(i=0;i<26;i++)
	{
		letter_select_frequency[i] = 0.0;
		if(letter_select_candidate[i] != -1)
			letter_select_candidate[i] = 0;
	}

	// send selected letter
	letter = slct;
	#ifdef DEBUG_ALL
	fprintf(out_fp_sentence,"Select %c: %s\n", slct, str);//letter_frequency[cnt],str);
	printf("Select %c: %s\n", slct, str);// letter_frequency[cnt],str);
	#endif
	
}

void    check_command_line_parameters(int argc, char ** argv)
{

	return;
}

void output_word()
{
	int lex_count, dcount, i, i1;

	int size[DEFAULT_WORD_LENGTH*2];

	int ambiguity = 0, first_ambiguity = 0;

	int org_size, org_valid;
	char org_str;

    int *total_words_count = &(wconf_input.total_words_count);
    int *current_recall_size_word = &(wconf_input.current_recall_size_word);

	for(dcount = 0; dcount < DEFAULT_WORD_LENGTH; dcount++) {
		size[dcount] = THREAD_CDDT_SIZE_WORD[dcount];
		#ifdef DEBUG_ALL
		printf("size[%d] = %d\n", dcount, size[dcount]);
		#endif
		if(size[dcount] > 1) {
			if(ambiguity == 0) first_ambiguity = dcount;
			ambiguity++;
		}
	}
	
	#ifdef DEBUG_ALL
	printf("ambiguity = %d, first ambiguity = %d\n", ambiguity, first_ambiguity);
	#endif

	if(ambiguity == 0 ) {
		for(dcount = 0; dcount < DEFAULT_WORD_LENGTH; dcount++) {
             if(THREAD_CDDT_SIZE_WORD[dcount] == 0) 
                THREAD_RECALL_WORD[*total_words_count][dcount] = '`';
             else if(symbol_list_word[dcount][THREAD_LEX_ARRAY_WORD[dcount][0]][0] != '`')  
				THREAD_RECALL_WORD[*total_words_count][dcount] = symbol_list_word[dcount][THREAD_LEX_ARRAY_WORD[dcount][0]][0];
			else 
				THREAD_RECALL_WORD[*total_words_count][dcount] = '\0';
		}
    	THREAD_RECALL_WORD[*total_words_count][DEFAULT_WORD_LENGTH+1] = '\0';
		(*total_words_count)++;
		if(*total_words_count >= *current_recall_size_word) expand_recall_word();
	}
	else if(ambiguity == 1) {
		for(i = 0; i < size[first_ambiguity]; i++) {
			for(dcount = 0; dcount < DEFAULT_WORD_LENGTH; dcount++) {
				if(dcount == first_ambiguity) {
					if(symbol_list_word[dcount][THREAD_LEX_ARRAY_WORD[dcount][i]][0] != '`') {
						THREAD_RECALL_WORD[*total_words_count][dcount] = 
							symbol_list_word[dcount][THREAD_LEX_ARRAY_WORD[dcount][i]][0];
					}
					else THREAD_RECALL_WORD[*total_words_count][dcount] = '\0';
				}
				else {
					if(symbol_list_word[dcount][THREAD_LEX_ARRAY_WORD[dcount][0]][0] != '`') {
						THREAD_RECALL_WORD[*total_words_count][dcount] = 
							symbol_list_word[dcount][THREAD_LEX_ARRAY_WORD[dcount][0]][0];
					}
					else THREAD_RECALL_WORD[*total_words_count][dcount] = '\0';
				}
			}
    		THREAD_RECALL_WORD[*total_words_count][DEFAULT_WORD_LENGTH+1] = '\0';
			(*total_words_count)++;
			if(*total_words_count >= *current_recall_size_word) expand_recall_word();
		}
	}
	else {
		#ifdef DEBUG_ALL
		printf("AAA ambiguity = %d\n", ambiguity);
		#endif
		org_size = THREAD_INPUT_WORD[first_ambiguity].candidate_size_word;
		org_str = THREAD_INPUT_WORD[first_ambiguity].candidate[0].str;
		org_valid = THREAD_INPUT_WORD[first_ambiguity].candidate[0].valid;

		for(i = 0; i < size[first_ambiguity]; i++) {
			if(symbol_list_word[first_ambiguity][THREAD_LEX_ARRAY_WORD[first_ambiguity][i]][0] == '`') {
				continue;
            }


			THREAD_INPUT_WORD[first_ambiguity].candidate[0].str = 
					symbol_list_word[first_ambiguity][THREAD_LEX_ARRAY_WORD[first_ambiguity][i]][0];
			THREAD_INPUT_WORD[first_ambiguity].candidate_size_word = 1;
			THREAD_INPUT_WORD[first_ambiguity].candidate[0].valid = 1;

			do_recall_word(0);

			output_word();

			//getchar();
		}
		THREAD_INPUT_WORD[first_ambiguity].candidate_size_word = org_size;
		THREAD_INPUT_WORD[first_ambiguity].candidate[0].str = org_str;
		THREAD_INPUT_WORD[first_ambiguity].candidate[0].valid = org_valid;

	}

	fflush(out_fp);

}

void expand_recall_word()
{
	int i, new_size;
    int *current_recall_size_word = &(wconf_input.current_recall_size_word);

	new_size = *current_recall_size_word + MAX_AMBIGUOUS*MAX_AMBIGUOUS;
	#ifdef DEBUG_ALL
    printf("WARNING: reallocate!!!\n");
	#endif
    exit(1);
	THREAD_RECALL_WORD = (char **)realloc(THREAD_RECALL_WORD, sizeof(char *)*new_size);
	for(i = *current_recall_size_word; i < new_size; i++) {
        THREAD_RECALL_WORD[i] = (char *)malloc(sizeof(char)*DEFAULT_WORD_LENGTH);
        if(THREAD_RECALL_WORD[i] == NULL) {
            printf("ERROR: cannot allocate enough memory\n");
            exit(0);
        }
    }
	*current_recall_size_word = new_size;
}

//void free_word(input_word_entry *input_word)
void free_word()
{
	int i;

	for(i = 0; i < DEFAULT_WORD_LENGTH; i++) {
        if(wconf_input.input_word[i].candidate != NULL) {
		    free(wconf_input.input_word[i].candidate);

            #ifdef DEBUG_MEM
            printf("loc free 4 %x\n", wconf_input.input_word[i].candidate);
            #endif

            wconf_input.input_word[i].candidate = NULL;
        }
		wconf_input.input_word[i].candidate_size_word = 0;
	}

    if(wconf_input.recall_word != NULL)
    {
        for(i=0;i<wconf_input.current_recall_size_word;i++)
        {
            if(wconf_input.recall_word[i] != NULL)
            {
                free(wconf_input.recall_word[i]);
                wconf_input.recall_word[i] = NULL;
            }
        }
        free(wconf_input.recall_word);
        wconf_input.recall_word = NULL;
    }
}

void clear_sentence(int sentence_id)
{
    int letter_count, word_count, i, j;

    for(word_count = 0; word_count < DEFAULT_SENTENCE_LENGTH; word_count++) {
        if(input_sentence[sentence_id][word_count].word != NULL){
          printf("SEVERE ERROR IN function clear!!! input_sentence.word != NULL\n");
          exit(1);
        }
       	input_sentence[sentence_id][word_count].word = (struct sentence_word_entry *)malloc(sizeof(struct sentence_word_entry));

        #ifdef DEBUG_MEM
        printf("alloc 4 %x\n", input_sentence[sentence_id][word_count].word);
        #endif

        if(input_sentence[sentence_id][word_count].word == NULL) {
            printf("ERROR: NOT ENOUGH MEMORY\n");
            exit(0);
        }
       	(input_sentence[sentence_id][word_count].word)[0].str = (char *)malloc(sizeof(char)*(strlen("`")+2));

        #ifdef DEBUG_MEM
        printf("alloc 5 %x\n", (input_sentence[sentence_id][word_count].word)[0].str);
        #endif

        if((input_sentence[sentence_id][word_count].word)[0].str == NULL) {
            printf("ERROR: NOT ENOUGH MEMORY\n");
            exit(0);
        }
       	strcpy((input_sentence[sentence_id][word_count].word)[0].str, "`");
       	(input_sentence[sentence_id][word_count].word)[0].valid=1;
       	input_sentence[sentence_id][word_count].size=1;

        unknown_count[sentence_id][word_count] = 0;

   }
}
void clear_word()
{
    int i;

    wconf_input.recall_word = (char**)malloc(sizeof(char *)*MAX_AMBIGUOUS*MAX_AMBIGUOUS);
    if (wconf_input.recall_word == NULL)
    {
        printf("recall word mem error\n");
        exit(0);
    }

    wconf_input.current_recall_size_word = MAX_AMBIGUOUS*MAX_AMBIGUOUS;
    wconf_input.total_words_count = 0;
    for(i=0;i<wconf_input.current_recall_size_word;i++)
    {
        wconf_input.recall_word[i] = (char *)malloc(sizeof(char)*DEFAULT_WORD_LENGTH);
        if(wconf_input.recall_word[i] == NULL)
        {
            printf("recall word %d mem error\n", i);
            exit(0);
        }
    }
}


int collect_wconf_results(char * str, int word_count)
{
    int sid, wid, i,j,k, total_words_num, word_num,flag, word_len, letter_index;
	char dic_str[2048];
	int * candidate_index;

    sid = 0;
    wid = word_count;
    total_words_num = 0;


	candidate_index = (int *)malloc(sizeof(int)*global_dictionary_size);
	word_len = strlen(str);
	for(i=0;i<global_dictionary_size;i++)
	{
		candidate_index[i] = 0;
		strcpy(dic_str, dictionary[i]);
		if(strlen(dic_str) != word_len)
			continue;
		else
		{
			flag = 1;
			for(j=0;j<word_len;j++)
			{
				if((str[j] == '?' && (letter_select_candidate[dic_str[j]-'a'] != -1)) || str[j] == dic_str[j])
				{
					flag *= 1;
				}
				else
				{
					flag *= 0;
				}
			}
			if(flag == 1)
			{
				//printf("word_count %d candidate num %d, %s\n", word_count, total_words_num, dictionary[(lex_table_sentence[word_count].lex_list)[i]]);
                //printf("matched string %s\n", dic_str);
				total_words_num++;
				candidate_index[i] = 1;
				for(j=0;j<word_len;j++)
				{
					//printf("%c %d\n", dic_str[j], dic_str[j]-'a');
					letter_index = dic_str[j]-'a';
					if(letter_index >= 0 && letter_index < 26)
					{
						if(letter_select_candidate[letter_index] != -1)
							letter_select_candidate[letter_index]++;
					}
				}
			}
		}
	}
	for(i=0;i<26;i++)
	{
		if(letter_select_candidate[i] != -1)
		{
			if(total_words_num >= 1e-8)
			{
				letter_select_frequency[i] += letter_select_candidate[i]/(double)total_words_num;
			}
			letter_select_candidate[i] = 0;
		}
	}

#ifdef DEBUG_SLCT
	printf("word_count %d, candidate num = %d\n",word_count, total_words_num);
#endif

	
	if(total_words_num > MAX_WORD_CANDIDATE || total_words_num < 1e-8)
	{
		return 1;
	}

    //by qqiu Nov. 2010
    if(input_sentence[sid][wid].word != NULL) {
        for(i = 0; i < input_sentence[sid][wid].size; i++) {
            if((input_sentence[sid][wid].word)[i].str != NULL) {
                free((input_sentence[sid][wid].word)[i].str);

                #ifdef DEBUG_MEM
                printf("loc free 6 %x\n", (input_sentence[sid][wid].word)[i].str);
                #endif

                (input_sentence[sid][wid].word)[i].str = NULL;
            }
        } 
        free(input_sentence[sid][wid].word);

        #ifdef DEBUG_MEM
        printf("loc free 7 %x\n", input_sentence[sid][wid].word);
        #endif

        input_sentence[sid][wid].word = NULL;
    }
    //***** set sentence input****//
    input_sentence[sid][wid].size = total_words_num;
    if(input_sentence[sid][wid].word != NULL) {
      printf("SEVERE ERROR!!! input_sentence.word != NULL\n");
      exit(1);
    }
    input_sentence[sid][wid].word = (struct sentence_word_entry *)malloc(sizeof(struct sentence_word_entry)*(total_words_num==0?1:total_words_num)); 

    #ifdef DEBUG_MEM
    printf("alloc 7 %x\n", input_sentence[sid][wid].word);
    #endif

    if(input_sentence[sid][wid].word == NULL) {
        printf("ERROR: NOT ENOUGH MEMORY\n");
        exit(0);
    }
	if(total_words_num > 0)
	{
		k = 0;
		for(i = 0; i < global_dictionary_size; i++) { 
			if(candidate_index[i] == 1)
			{
				(input_sentence[sid][wid].word)[k].str = (char *)malloc(sizeof(char)*(strlen(dictionary[i])+1) ); 

				#ifdef DEBUG_MEM
				printf("alloc 8 %x\n", (input_sentence[sid][wid].word)[k].str);
				#endif

				if((input_sentence[sid][wid].word)[k].str == NULL) {
					printf("ERROR: NOT ENOUGH MEMORY\n");
					exit(0);
				}
				strcpy( (input_sentence[sid][wid].word)[k].str, dictionary[i]); 
				(input_sentence[sid][wid].word)[k].valid=1; 
				k++;
			}
		} 
	}
	else
	{
		(input_sentence[sid][wid].word)[0].str = (char *)malloc(sizeof(char)*(strlen(str)+1));
		strcpy((input_sentence[sid][wid].word)[0].str, str);
		(input_sentence[sid][wid].word)[0].valid=1; 
	}
		
    #ifdef DEBUG_MEDIUM 
    fprintf(out_fp, "##### sentence %d word %d size %d length %d\n", 
        sid, wid, input_sentence[sid][wid].size, word_length_rec[sid][wid] ); 
    #endif

	free(candidate_index);
	candidate_index = NULL;
	
	return 0;

    //for(i=0;i<total_words_num;i++) free(word_candidate[i]);
    //free(word_candidate);
}

void sentence_confab(int sid)
{
    int i, unknown_words; 
   
    unknown_words = 0; 
    for(i = 0; i < DEFAULT_SENTENCE_LENGTH; i++) { 
        if(input_sentence[sid][i].size > 1) unknown_words ++; 
    } 
    #ifdef DEBUG_ALL 
    printf("#%d: recall sentence with %d unknown words out of %d\n", sid, unknown_words, sentence_length_rec[sid]); 
    printf("recall_sentence....\n"); 
    #endif 
    
    sentence_main(sid);  
    #ifdef DEBUG_ALL 
    fprintf(out_fp, "*********************\n"); 
    #endif 
    fflush(out_fp); 
}
