#include "recall.h"
#include <sys/time.h>

extern char KB_file[128];

extern struct kb_row_entry KB_table_word[MAX_KB_NUM_WORD][800]; //each entry is a nonempty row in KB

extern int source_target_array_word[MAX_LEX_WORD][MAX_LEX_WORD];


extern char **symbol_list_word[MAX_LEX_WORD];
extern int symbol_size_word[MAX_LEX_WORD];

int      locate_target_word(int, int, int);
int      equal_word(int t[MAX_LEX_WORD][MAX_AMBIGUOUS], int s[MAX_LEX_WORD][MAX_AMBIGUOUS], int c[MAX_LEX_WORD], int o[MAX_LEX_WORD]);
int      locate_symbol_word(char*, int);

int      total_usec;


char input[MAX_LEX_WORD][3];

extern struct wconf_input_entry wconf_input;

void do_recall_word(int flag)
{

	int letter_count, lex_count;	
	int num_unknown = 0, unknown_count, target_lex;
	int unknown_lex[MAX_LEX_WORD];
	int old_candidate_size_word[MAX_LEX_WORD];
	int converge = 0;

	int source_count, target_count, i, target_size, row_length, i1;


	int old_lexicon_array_word[MAX_LEX_WORD][MAX_AMBIGUOUS];
	float *el = NULL;
	struct KB_table_word_entry *pel;
	struct KB_table_word_entry *nel;

	char **target_list_ptr;
	struct KB_entry *row_ptr;

	int row, col, kb;
	char *s, *t;

	float min, max;
	int min_id;

	int dcount;	
	double r;
//fyang
    int sym_flag=0;
    int start_usec;
    int end_usec;
    int word_usec;
    struct timeval tp;

	int *check_list = NULL;
	int candidate_id, candidate_count, id;
	char str[128];


    #ifdef DEBUG_ALL
	//printf("start do recall\n");
    #endif

	lex_count = 0;
    for(letter_count = 0; letter_count < DEFAULT_WORD_LENGTH; letter_count++)  {
		THREAD_CDDT_SIZE_WORD[lex_count] = 0;
		for(i = 0; i < THREAD_INPUT_WORD[letter_count].candidate_size_word; i++) {
			str[0] = THREAD_INPUT_WORD[letter_count].candidate[i].str;
			str[1] = '\0';
            //fyang
            gettimeofday(&tp,NULL);
            start_usec = 1000000*tp.tv_sec + tp.tv_usec;
			id = locate_symbol_word(str, lex_count);
			if(id == -1) {
                THREAD_INPUT_WORD[letter_count].candidate[i].valid = 0;
                //fyang
          //      if(lex_count == 0) sym_flag = 1;
            }
			else {
				THREAD_LEX_ARRAY_WORD[lex_count][THREAD_CDDT_SIZE_WORD[lex_count]] = id;
				THREAD_CDDT_SIZE_WORD[lex_count]++;
			} 
            
    gettimeofday(&tp, NULL);
    end_usec = 1000000*tp.tv_sec + tp.tv_usec;
    word_usec = end_usec - start_usec;
    //printf("word time: %d us\n",word_usec);
    total_usec = total_usec + word_usec;
    //printf("total time: %d us\n",total_usec);
		}
        //fyang
      //  if(sym_flag) {
        //    for

        if(THREAD_CDDT_SIZE_WORD[lex_count] == 0) {
            id = locate_symbol_word("`", lex_count);
            if(id != -1) {
                THREAD_LEX_ARRAY_WORD[lex_count][THREAD_CDDT_SIZE_WORD[lex_count]] = id;
                THREAD_CDDT_SIZE_WORD[lex_count] = 1;
            }
        }
		if(THREAD_CDDT_SIZE_WORD[lex_count] > 1){
			unknown_lex[num_unknown] = lex_count;
			num_unknown++ ;
		}
		else if (THREAD_CDDT_SIZE_WORD[lex_count] < 1) {
			THREAD_CDDT_SIZE_WORD[lex_count] = -1;
		};
        lex_count++;
    }

    for(letter_count = 0; letter_count < DEFAULT_WORD_LENGTH - 1; letter_count++) {
		THREAD_CDDT_SIZE_WORD[lex_count] = 0;
		for(i = 0; i < THREAD_INPUT_WORD[letter_count].candidate_size_word; i++) {
			for(i1 = 0; i1 < THREAD_INPUT_WORD[letter_count+1].candidate_size_word; i1++) {
				if(THREAD_INPUT_WORD[letter_count].candidate[i].valid == 1 &&
				   THREAD_INPUT_WORD[letter_count+1].candidate[i1].valid == 1) {
					str[0] = THREAD_INPUT_WORD[letter_count].candidate[i].str;	
					str[1] = THREAD_INPUT_WORD[letter_count+1].candidate[i1].str;	
					str[2] = '\0';
					id = locate_symbol_word(str, lex_count);
					if(id != -1) {
						THREAD_LEX_ARRAY_WORD[lex_count][THREAD_CDDT_SIZE_WORD[lex_count]] = id;
						THREAD_CDDT_SIZE_WORD[lex_count]++;
					}
				}
			}
		}
		if(THREAD_CDDT_SIZE_WORD[lex_count] > 1) {
			unknown_lex[num_unknown] = lex_count;
			num_unknown++ ;
		}
        lex_count++;
    }

    for(letter_count = 0; letter_count < DEFAULT_WORD_LENGTH - 2; letter_count++) {
		THREAD_CDDT_SIZE_WORD[lex_count] = 0;
		for(i = 0; i < THREAD_INPUT_WORD[letter_count].candidate_size_word; i++) {
			for(i1 = 0; i1 < THREAD_INPUT_WORD[letter_count+2].candidate_size_word; i1++) {
				if(THREAD_INPUT_WORD[letter_count].candidate[i].valid == 1 &&
				   THREAD_INPUT_WORD[letter_count+2].candidate[i1].valid == 1) {
					str[0] = THREAD_INPUT_WORD[letter_count].candidate[i].str;	
					str[1] = THREAD_INPUT_WORD[letter_count+2].candidate[i1].str;	
					str[2] = '\0';
					id = locate_symbol_word(str, lex_count);
					if(id != -1) {
						THREAD_LEX_ARRAY_WORD[lex_count][THREAD_CDDT_SIZE_WORD[lex_count]] = id;
						THREAD_CDDT_SIZE_WORD[lex_count]++;
					}
				}
			}
		}
		if(THREAD_CDDT_SIZE_WORD[lex_count] > 1) {
			unknown_lex[num_unknown] = lex_count;
			num_unknown++ ;
		}
        lex_count++;
    }


	while(converge == 0) {
		if(flag == 1) {
			for(dcount = 0; dcount < MAX_LEX_WORD; dcount++) {
				printf("(");

				for(i = 0; i < THREAD_CDDT_SIZE_WORD[dcount]; i++)  printf("%s ", symbol_list_word[dcount][THREAD_LEX_ARRAY_WORD[dcount][i]]);

				printf(")" );
			}
			printf("\n");
		}
		for(lex_count = 0; lex_count < MAX_LEX_WORD; lex_count++) {
			for(i = 0; i < THREAD_CDDT_SIZE_WORD[lex_count]; i++) {
				old_lexicon_array_word[lex_count][i] = THREAD_LEX_ARRAY_WORD[lex_count][i];
			}
			old_candidate_size_word[lex_count] = THREAD_CDDT_SIZE_WORD[lex_count];
		}
		for(unknown_count = num_unknown - 1; unknown_count >= 0; unknown_count--) {
			target_lex = unknown_lex[unknown_count];
			if(THREAD_CDDT_SIZE_WORD[target_lex] <= 1) continue;
			if(flag == 1) {
				//printf("<%d>: handing unknown #%d target lex %d\n", tid, unknown_count, target_lex);
				//getchar();
			}

			//printf("target_size = %d\n", target_size);
        if(el != NULL) {
        printf("ERROR!!! el = %x\n", el);
        exit(1);
      }
			el = (float *)malloc(sizeof(float)*	(THREAD_CDDT_SIZE_WORD[target_lex] + 2));
      if(check_list != NULL) {
        printf("ERROR!!! checklist = %x\n", check_list);
        exit(1);
      }
			check_list = (int *)malloc(sizeof(int)*	(THREAD_CDDT_SIZE_WORD[target_lex] + 2));
			for(i = 0; i < THREAD_CDDT_SIZE_WORD[target_lex]; i++) el[i] = 0;
			//printf("candidate size = %d\n", candidate_size_word[target_lex]);

			for(source_count = 0; source_count < MAX_LEX_WORD; source_count++) {
				if( THREAD_CDDT_SIZE_WORD[source_count] < 1) continue;
				kb = source_target_array_word[source_count][target_lex];

				//printf("\t\t handing s_lex %d sym %s kb %d\n", source_count, 
				//	lexicon_array_word[source_count][0], kb);

				if(kb != -1) {
					//printf("kb = %d\n", kb);
					for(i = 0; i < THREAD_CDDT_SIZE_WORD[target_lex]; i++) check_list[i] = 0;
					for(candidate_id = 0; candidate_id < THREAD_CDDT_SIZE_WORD[source_count]; candidate_id++) {
						//printf("candidate_id = %d\n", candidate_id);
						//printf("search for %s source %d candidate %d in kb %d\n", 
							//lexicon_array_word[source_count][candidate_id], source_count, candidate_id, kb);
						row = THREAD_LEX_ARRAY_WORD[source_count][candidate_id];
						#ifdef DEBUG_ALL
                        if(row == -1) printf("source lex = %d, candidate id = %d CDDT %d\n", source_count, candidate_id, THREAD_CDDT_SIZE_WORD[source_count]);
						#endif

						//printf("\t\t\t row length = %d\n", row_length);

						for(target_count = 0; target_count < THREAD_CDDT_SIZE_WORD[target_lex]; target_count++) {
							//printf("before locate kb %d row %d sym %d\n", kb, row, lexicon_array_word[target_lex][target_count]);
							col = locate_target_word(kb, row, THREAD_LEX_ARRAY_WORD[target_lex][target_count]);
							//printf("locate %d at %d\n", lexicon_array_word[target_lex][target_count], col);
							if(col != -1 && check_list[target_count] == 0 ) {
								el[target_count] = el[target_count] + BANDGAP;
								check_list[target_count] = 1;
                            }
                            //fyang
							//printf("\t\t\t\t target %s value %f\n", t, row_ptr[target_count].KB_value);
						}
					}
				} //process one kb
			} //process one source lexicon

			min = 10000000;
			min_id = -1;
			max = 0;
			for(i = 0; i < THREAD_CDDT_SIZE_WORD[target_lex]; i++) {
				printf("target lex %d candidate %d letter %s el = %f\n", target_lex, i, symbol_list_word[target_lex][THREAD_LEX_ARRAY_WORD[target_lex][i]], el[i]);
				if(min > el[i]) {
					min = el[i];
					min_id = i;
				}
				if(max < el[i]) max = el[i];
			}
			i1 = 0;
		//	printf("min = %f max = %f\n", min, max);
        // fyang mod more candidate
			if(min != max && min < 50) {
				for(i = 0; i < THREAD_CDDT_SIZE_WORD[target_lex]; i++) {
					if(el[i] != min) {
						THREAD_LEX_ARRAY_WORD[target_lex][i1] = THREAD_LEX_ARRAY_WORD[target_lex][i];
						i1++;
					}
				}
				THREAD_CDDT_SIZE_WORD[target_lex] = i1;
			}

			free(el);
      el = NULL;
			free(check_list);
      check_list = NULL;
		} //process all the unknowns

		if(equal_word(old_lexicon_array_word, THREAD_LEX_ARRAY_WORD, old_candidate_size_word, THREAD_CDDT_SIZE_WORD) == 1) break;
    }
    //fyang
    //gettimeofday(&tp, NULL);
    //end_usec = 1000000*tp.tv_sec + tp.tv_usec;
    //word_usec = end_usec - start_usec;
    //printf("word time: %d us\n",word_usec);
    //total_usec = total_usec + word_usec;
    //printf("total time: %d us\n",total_usec);
}

int equal_word(int t[MAX_LEX_WORD][MAX_AMBIGUOUS], int s[MAX_LEX_WORD][MAX_AMBIGUOUS], int o[MAX_LEX_WORD], int c[MAX_LEX_WORD])
{
	int i, i1, dcount;

	for(i = 0; i < MAX_LEX_WORD; i++) {
		if(o[i] != c[i]) {
			//printf("not equal: old size[%d] = %d new size = %d\n", i, o[i], c[i]);
			return 0;
		}
	}
	for(i = 0; i < MAX_LEX_WORD; i++) {
		for(i1 = 0; i1 < c[i]; i1++) {
			if(t[i][i1] != s[i][i1]) {
				//printf("not equal: word [%d][%d] = %d new = %d\n", i, i1, t[i][i1], s[i][i1]);
				return 0;
			}
		}
	}
	//for(dcount = 0; dcount < DEFAULT_WORD_LENGTH * DEFAULT_MAX_LEVEL - 3; dcount++) {
	//	printf("(");
	//	for(i = 0; i < candidate_size_word[dcount]; i++)  printf("%d ", lexicon_array_word[dcount][i]);
	//	printf(")" );
	//}
	//printf("\n");
	return 1;
}

int locate_symbol_word(char *sym, int lex)
{
    int left = 0, right = 0, cur = 0;
    int i;

    //printf("lcoate symbol %s in %d size %d\n", sym, lex, symbol_size_word[lex]);
    //for(i = 0; i < symbol_size_word[lex]; i++) printf("%s ", symbol_list_word[lex][i]);
    right = symbol_size_word[lex] - 1;
    if(strcmp(symbol_list_word[lex][right], sym) == 0) return right;
    if(strcmp(symbol_list_word[lex][left], sym) == 0) return left;

    cur = (right + left)/2;
    while(strcmp(symbol_list_word[lex][cur],sym) != 0) {
        if(strcmp(symbol_list_word[lex][cur],sym) > 0) {
            left = cur;
        } else {
            right = cur;
        }
        if(right - left <= 1) {
            //printf("In lex %d", lex);
            //for(i = 0; i < symbol_size_word[lex]; i++) printf("%s ", symbol_list_word[lex][i]);
            //printf("\n");
            //printf(" cannot find %s\n", sym);
			return -1;
        }
        cur = (right + left)/2;
    }

    return cur;
}

int locate_target_word(int kb, int row, int sym)
{
    int left = 0, right = 0, cur = 0;
    int i;

    //printf("lcoate symbol %d in row size %d\n", sym, KB_table_word[kb][row].size);
    //for(i = 0; i < symbol_size_word[lex]; i++) printf("%s ", symbol_list_word[lex][i]);
    right = KB_table_word[kb][row].size - 1;
	if(right < 0) {
        printf("ERROR, less than 1 item in kb %d row %d\n", kb, row);
        printf("KB-table_word[%d][%d].size = %d\n", kb, row,  KB_table_word[kb][row].size);
        //getchar();
    }
    if(KB_table_word[kb][row].ptr[right].target_id == sym) return right;
    if(KB_table_word[kb][row].ptr[left].target_id == sym) return left;

	//printf("right = %d\n", right);
    cur = (right + left)/2;
    while(KB_table_word[kb][row].ptr[cur].target_id != sym ) {
        if(KB_table_word[kb][row].ptr[cur].target_id < sym ) {
            left = cur;
        } else {
            right = cur;
        }
		//printf("cur = %d %d right %d left %d\n", cur, KB_table_word[kb][row].ptr[cur].target_id, right, left );
        if(right - left <= 1) {
            //printf("In lex %d", row);
            //for(i = 0; i < symbol_size_word[lex]; i++) printf("%s ", symbol_list_word[lex][i]);
            //printf("\n");
            //printf(" cannot find %d\n", sym);
			return -1;
        }
        cur = (right + left)/2;
    }

	//printf("return %d\n", cur);
    return cur;
}

