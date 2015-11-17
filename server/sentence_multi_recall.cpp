#include "recall.h"
#include <sys/time.h>
#include <vector>
#include <iostream>

char* tag_sym[41] = {"#","$","ls",":","``","uh","fw","sym","wrb","rp","prp$","nnps","wp$","cd","pos","jjr","rbs","vb","md","vbg","wp","ex","wdt","jjs","cc","to","vbp","nns","vbd","prp","vbn","rbr","pdt","jj","rb","nn","dt","vbz","nnp","`","in"};

extern struct input_sentence_entry input_sentence[SENTENCE_BLOCK_SIZE][DEFAULT_SENTENCE_LENGTH];
extern char recall_sentence[DEFAULT_SENTENCE_LENGTH][DEFAULT_WORD_LENGTH];
extern int *lexicon_array_sentence[MAX_LEX_SENTENCE];
extern int *old_lexicon_array_sentence[MAX_LEX_SENTENCE];
extern int kb_source_size[MAX_KB_NUM_SENTENCE];

extern int num_KB;

extern struct sentence_lex_table_entry lex_table_sentence[MAX_LEX_SENTENCE];

extern struct sentence_lex_table_entry lex_table_sentence[MAX_LEX_SENTENCE];

extern int source_target_array_sentence[MAX_LEX_SENTENCE][MAX_LEX_SENTENCE];
extern int swirl;

//extern double start_time, end_time, initiall_time, recall_time, total_time;

extern int candidate_size_sentence[MAX_LEX_SENTENCE];

extern struct kb_row_entry *sentence_KB_row[MAX_KB_NUM_SENTENCE]; //each entry is a nonempty row in KB

extern char **dictionary;

int 	locate_row(int, int);
int      locate_target_sentence(int, struct KB_entry *, int);
int      equal_sentence(int *t[MAX_LEX_SENTENCE], int *s[MAX_LEX_SENTENCE], int o[MAX_LEX_SENTENCE], int c[MAX_LEX_SENTENCE]);
extern int add_hash(char * s, int i, int j);
//fang
int start_usec,end_usec,sentence_usec,total_tusec;
struct timeval tp;

void do_recall_sentence(int flag, int sentence_id,std::vector<std::vector<float> >& excitationLvl)
{
	int word_count, lex_count, candidate_count, i, i1;	
	int num_unknown = 0, unknown_count, target_lex;
	int unknown_lex[MAX_LEX_SENTENCE];
	int old_candidate_size_sentence[MAX_LEX_SENTENCE];
	int converge = 0;

	int source_count, target_count, target_size, row_length;

	int input[MAX_LEX_SENTENCE];

	struct KB_entry *row_ptr;

	int row, col, kb;
	int s, t;

	float min;
	float max;
	int max_id;

    //fyang
    int max_candidate;
    float el_value;

	int dcount;	
	double r;

	int candidate_id, dic_id;

	char str[512];
	float *el = NULL;
    	
    	int endflag;
	float *el_array_sentence[MAX_LEX_SENTENCE];
    int selected, num_iter;
    float el_sum;
    int max_ambiguous, max_ambiguous_word, max_ambiguous_tag;
    int *tmp_array;
    int **link_num;
    float max_el,min_el,sum_el,average_el,square_sum_el,variance_el;
	excitationLvl.resize(MAX_LEX_SENTENCE);
	lex_count = 0;


    for(word_count = 0; word_count < DEFAULT_WORD_LENGTH; word_count++)  {
		
		candidate_size_sentence[word_count] = 0;
		
		candidate_count = 0;
		for(i = 0; i < input_sentence[sentence_id][word_count].size; i++) {
			input[lex_count] = add_hash(input_sentence[sentence_id][word_count].word[i].str, 0, 1);
            #ifdef DEBUG_SENTENCE 
            printf("found %s at %d\n", input_sentence[sentence_id][word_count].word[i].str, input[lex_count]);
            #endif
			if(input[lex_count] == -1 && input_sentence[sentence_id][word_count].size > 1) {
				input_sentence[sentence_id][word_count].word[i].valid = 0;
			} else if (input[lex_count] != -1) {
				lexicon_array_sentence[lex_count][candidate_count]= input[lex_count];
				if(lexicon_array_sentence[lex_count][candidate_count] == -1 && input_sentence[sentence_id][word_count].size > 1) {
					input_sentence[sentence_id][word_count].word[i].valid = 0;
				}else if (lexicon_array_sentence[lex_count][candidate_count] != -1) {
					//printf("input[%d]=%d (%s)\n", lex_count, lexicon_array_sentence[lex_count][candidate_count], input_sentence[word_count][i].str);
					#ifdef DEBUG_ALL
					if(flag == MANUAL) printf("lex[%d][%d] = %d size %d\n", lex_count, candidate_count, lexicon_array_sentence[lex_count][candidate_count], candidate_size_sentence[word_count]);
					#endif
					candidate_count++;
					candidate_size_sentence[word_count] = candidate_count;
				}
			}
		} //end of else
		candidate_size_sentence[word_count] = candidate_count;

		if(candidate_size_sentence[word_count] > 1) {
			unknown_lex[num_unknown] = lex_count;
			num_unknown++;
		}

        lex_count++;

    } //end of for each word

    for(word_count = 0; word_count < DEFAULT_WORD_LENGTH - 1; word_count++) {

		candidate_size_sentence[lex_count] = 0;
		candidate_count = 0;
		for(i = 0; i < input_sentence[sentence_id][word_count].size; i++) {
			if(input_sentence[sentence_id][word_count].word[i].str == NULL) continue;

			for(i1 = 0; i1 < input_sentence[sentence_id][word_count+1].size; i1++) {
				if(input_sentence[sentence_id][word_count+1].word[i1].str == NULL) continue;

				strcpy(str, input_sentence[sentence_id][word_count].word[i].str);
				strcat(str, " ");
				strcat(str, input_sentence[sentence_id][word_count+1].word[i1].str);

	        	input[lex_count] = add_hash(str, 0, 1);
				if(input[lex_count] != -1) {
					lexicon_array_sentence[lex_count][candidate_count]= input[lex_count];
					if(lexicon_array_sentence[lex_count][candidate_count] != -1) {
						//printf("input[%d]=%d (%s)\n", lex_count, lexicon_array_sentence[lex_count][candidate_count], str);
						#ifdef DEBUG_ALL
						if(flag == MANUAL) printf("lex[%d][%d] = %d size %d\n", lex_count, candidate_count, lexicon_array_sentence[lex_count][candidate_count], candidate_size_sentence[lex_count]);
						#endif
						candidate_count++;
						candidate_size_sentence[lex_count] = candidate_count;
					}
				}
			} //end of loop on i1
		} //end of for loop on i

		if(candidate_size_sentence[lex_count] > 1) {
			unknown_lex[num_unknown] = lex_count;
			num_unknown++;
		}

        lex_count++;
	}
    //fyang

    for(word_count = 0; word_count < DEFAULT_WORD_LENGTH; word_count++) {
        candidate_size_sentence[lex_count] = MAX_TAG_CANDIDATE;
        for(i = 0; i < candidate_size_sentence[lex_count]; i++) {
            lexicon_array_sentence[lex_count][i] = add_hash(tag_sym[i], 0, 1);
            }
        if((candidate_size_sentence[word_count] == 0) && (input_sentence[sentence_id][word_count].size > 0)) {          //word exist, eliminate "no tag"
            candidate_size_sentence[lex_count] -= 1;
            lexicon_array_sentence[lex_count][candidate_size_sentence[lex_count]-1] += 1;
            }
        if(candidate_size_sentence[lex_count] > 1) {
            unknown_lex[num_unknown] = lex_count;
            num_unknown++;
            }

            lex_count ++;
    }
    for(word_count = 0; word_count < MAX_LEX_SENTENCE; word_count++){
	    el_array_sentence[word_count] = (float*)malloc(candidate_size_sentence[word_count]*sizeof(float));
	}
    	for(word_count = 0; word_count < MAX_LEX_SENTENCE; word_count++){
		for(i=0;i<candidate_size_sentence[word_count];i++){
			el_array_sentence[word_count][i] = 1.0/candidate_size_sentence[word_count];
		}
	}

    link_num = (int **)malloc(MAX_LEX_SENTENCE*sizeof(int*));
    for(word_count = 0; word_count < MAX_LEX_SENTENCE; word_count++)
    {
        link_num[word_count] = (int *)malloc(candidate_size_sentence[word_count]*sizeof(int));
    }
    for(word_count = 0; word_count<MAX_LEX_SENTENCE; word_count++)
    {
        for(i=0;i<candidate_size_sentence[word_count];i++)
        {
            link_num[word_count][i] = 0;
        }
    }


    //arrange the unknown lex so that we search for word i and phrase i together
    i = 0;
    for(target_lex = 0 ; target_lex < DEFAULT_SENTENCE_LENGTH; target_lex++) {
        if(candidate_size_sentence[target_lex] !=1) unknown_lex[i++] = target_lex;
        if(target_lex != DEFAULT_SENTENCE_LENGTH-1 && candidate_size_sentence[target_lex+20] != 1) unknown_lex[i++] = target_lex + 20;
        if(candidate_size_sentence[target_lex+20 + 19] != 1) unknown_lex[i++] = target_lex + 20 + 19;
    }
    
    max_ambiguous_word = MAX_AMBIGUOUS_WORD;
    max_ambiguous_tag = MAX_AMBIGUOUS_TAG;
    while(max_ambiguous_word > 0) {
          converge = 0;
          num_iter = 0;
	while(converge == 0 && num_iter < MAX_ITERATION) {
		if(flag == 1) {
			//for(dcount = 0; dcount < MAX_LEX_SENTENCE; dcount++) {
			for(dcount = 0; dcount < MAX_LEX_SENTENCE; dcount++) {
				#ifdef DEBUG_ALL
				printf("<%d %d>(", dcount, candidate_size_sentence[dcount]);
				#endif
				for(i = 0; i < candidate_size_sentence[dcount]; i++)  {
					dic_id = lexicon_array_sentence[dcount][i];
					#ifdef DEBUG_ALL
					printf("%s ", dictionary[dic_id]);
					#endif
                    //getchar();
				}
				#ifdef DEBUG_ALL
				printf(")" );
				#endif
			}
			#ifdef DEBUG_ALL
			printf("\n");
			#endif
/*	    	for(dcount = 0; dcount < MAX_LEX_SENTENCE; dcount++) {
				printf("(");
				//if(candidate_size_sentence[dcount] <= 0 && input_sentence[sentence_id][dcount][0] != NULL) 
				//	printf("%s ", input_sentence[sentence_id][dcount][0]); //print the original
				for(i = 0; i < candidate_size_sentence[dcount]; i++)  {
					printf("%d ", lexicon_array_sentence[dcount][i]);
				}
				printf(")" );
			}
			printf("\n");
			getchar();
*/		}

		for(lex_count = 0; lex_count < MAX_LEX_SENTENCE; lex_count++) {
			for(i = 0; i < candidate_size_sentence[lex_count]; i++) {
				old_lexicon_array_sentence[lex_count][i] = lexicon_array_sentence[lex_count][i];
			}
			old_candidate_size_sentence[lex_count] = candidate_size_sentence[lex_count];
			//printf("candidate[%d] size=%d\n", lex_count, old_candidate_size_sentence[lex_count]);
		}
		for(unknown_count = 0; unknown_count < num_unknown; unknown_count++) {
			target_lex = unknown_lex[unknown_count];
            if(target_lex >= 2*DEFAULT_SENTENCE_LENGTH - 1) max_ambiguous = max_ambiguous_tag;
            else max_ambiguous = max_ambiguous_word;
			#ifdef DEBUG_ALL
            printf("unknown count %d target lex %d size %d\n",unknown_count, target_lex, candidate_size_sentence[target_lex]);
			#endif
            //fyang unknown cndidate count balance
            //if(target_lex >= 2*DEFAULT_SENTENCE_LENGTH-1) continue;
            //if(target_lex < 2*DEFAULT_SENTENCE_LENGTH-1 && max_candidate > 5) continue;
			if(candidate_size_sentence[target_lex] == 1) continue;

			//printf("handing unknown #%d target lex %d\n", unknown_count, target_lex);
			//getchar();

			target_size = candidate_size_sentence[target_lex];//lex_table_sentence[target_lex].symbol_size;
			if(target_size == 1) continue;
	
			el = (float *)malloc(sizeof(float)*target_size);
            for(i = 0; i < target_size; i++) {
                el[i] = 0;
            }
	
		endflag = 1;
            //new confabulation model by Wei Liu, Dec 1 2012
            for(source_count = 0; source_count < MAX_LEX_SENTENCE; source_count++) {
                //if(candidate_size_sentence[source_count] == 1) el_array_sentence[source_count][0] = 1;
                if(candidate_size_sentence[source_count] <= 0) continue; //if source is unrecognized
                if(source_count >= DEFAULT_SENTENCE_LENGTH && source_count < 2*DEFAULT_SENTENCE_LENGTH-1 && target_lex >= 2*DEFAULT_SENTENCE_LENGTH -1 )
                    	continue; //phrases are not used to confab tags
                if(target_lex >= 2*DEFAULT_SENTENCE_LENGTH -1 && source_count <= DEFAULT_SENTENCE_LENGTH -1 && (source_count + TAG_OFFSET -target_lex > 2 || target_lex - source_count - TAG_OFFSET  > 2))
                    	continue; //do not consider words too far
                if(target_lex >= 2*DEFAULT_SENTENCE_LENGTH -1 && source_count >= 2*DEFAULT_SENTENCE_LENGTH -1 && (source_count -target_lex > 2 || target_lex - source_count > 2))
                    	continue; //do not consider tags too 
                if((strcmp(dictionary[lexicon_array_sentence[source_count][0]], "`") != 0) &&
                        (strcmp(dictionary[lexicon_array_sentence[source_count][0]], "` `") != 0))
                    	endflag = 1;

		kb = source_target_array_sentence[source_count][target_lex];
		#ifdef DEBUG_SENTENCE
		printf("source %d, target %d, kb %d\n", source_count, target_lex, kb);
		#endif		

		if(kb != -1) {
		for(candidate_id = 0; candidate_id < candidate_size_sentence[source_count]; candidate_id++) {
			#ifdef DEBUG_ALL
			printf("source lex %d candidate %d sym %d (%s) in kb %d\n",
			source_count, candidate_id, lexicon_array_sentence[source_count][candidate_id], 
			dictionary[lexicon_array_sentence[source_count][candidate_id]], kb);
			#endif
			row = locate_row(kb,lexicon_array_sentence[source_count][candidate_id]);
			if(row == -1) {
				continue;
			}
			row_length = sentence_KB_row[kb][row].size;
			row_ptr = sentence_KB_row[kb][row].ptr;
			
			//if(candidate_size_sentence[target_lex]>excitationLvl.at(candidate_id).size())
			//	excitationLvl.at(candidate_id).resize(candidate_size_sentence[target_lex]);
			for(target_count = 0; target_count < candidate_size_sentence[target_lex]; target_count++) {
				col = locate_target_sentence(lexicon_array_sentence[target_lex][target_count], row_ptr, sentence_KB_row[kb][row].size);
				if(col == -2) {
					#ifdef DEBUG_SENTENCE
					printf("locate target %d KB[%d][%d].size=%d, source_count = %d, candidate_id = %d\n", target_lex, kb, row, sentence_KB_row[kb][row].size, source_count, candidate_id);
					#endif
					//getchar();
				}
				if(col != -1) {
                    if (max_ambiguous_word == MAX_AMBIGUOUS_WORD && num_iter == 0)
                        link_num[target_lex][target_count]++;
					if( (target_lex >= DEFAULT_SENTENCE_LENGTH*2-1 && target_lex - source_count == 2*DEFAULT_SENTENCE_LENGTH - 1) ||
					      (target_lex >= DEFAULT_SENTENCE_LENGTH*2-1 && source_count >= DEFAULT_SENTENCE_LENGTH*2-1 && ((target_lex - source_count <= 1) || (source_count - target_lex <= 1) ) ) )
					{
						double el_tmp = 2.0/candidate_size_sentence[source_count]*BANDGAP_SENTENCE*endflag + row_ptr[col].KB_value * 2.0 / candidate_size_sentence[source_count]*endflag;
						el[target_count] = el[target_count] +  2.0/candidate_size_sentence[source_count]*BANDGAP_SENTENCE*endflag + row_ptr[col].KB_value * 2.0 / candidate_size_sentence[source_count]*endflag;
						#ifdef DEBUG_SENTENCE
						printf("source[%d][%d] %s, target[%d][%d] %s, el %f\n", source_count, candidate_id, dictionary[lexicon_array_sentence[source_count][candidate_id]],target_lex, target_count, dictionary[lexicon_array_sentence[target_lex][target_count]], el_tmp);
						#endif
					}
					else {
						double el_tmp = 1.0/candidate_size_sentence[source_count]*BANDGAP_SENTENCE*endflag + row_ptr[col].KB_value / candidate_size_sentence[source_count]*endflag;
						el[target_count] = el[target_count] +  1.0/candidate_size_sentence[source_count]*BANDGAP_SENTENCE*endflag + row_ptr[col].KB_value / candidate_size_sentence[source_count]*endflag;
						#ifdef DEBUG_SENTENCE
						//printf("source[%d][%d] %s, target[%d][%d] %s, el %f\n", source_count, candidate_id, dictionary[lexicon_array_sentence[source_count][candidate_id]],target_lex, target_count, dictionary[lexicon_array_sentence[target_lex][target_count]], el_tmp);
						#endif
					}
				}
				if(col == -1){
					#ifdef DEBUG_SENTENCE
				//	printf("col == -1, source %d candidate_id %d target %d\n", source_count, candidate_id, target_lex, target_count);
					#endif
					if( (target_lex >= DEFAULT_SENTENCE_LENGTH*2-1 && target_lex - source_count == 2*DEFAULT_SENTENCE_LENGTH - 1) ||
					      (target_lex >= DEFAULT_SENTENCE_LENGTH*2-1 && source_count >= DEFAULT_SENTENCE_LENGTH*2-1 && ((target_lex - source_count <= 1) || (source_count - target_lex <= 1) ) ) )
						el[target_count] = el[target_count] + EL_MIN*endflag*2.0/candidate_size_sentence[source_count];
					else
						el[target_count] = el[target_count] + EL_MIN*endflag*1.0/candidate_size_sentence[source_count];
				}
			}
		}
		} //process one kb
		//if the source lexicon is "`" or "` `", set endflag to 0;
		if((strcmp(dictionary[lexicon_array_sentence[source_count][0]], "`") == 0) ||
		 (strcmp(dictionary[lexicon_array_sentence[source_count][0]], "` `") == 0))
		{
		    endflag = 0;
		}
                   
	} //process one source lexicon

	// Get el
	for(target_count = 0; target_count < candidate_size_sentence[target_lex]; target_count++) {
		if(candidate_size_sentence[target_lex]>excitationLvl.at(target_lex).size())
			excitationLvl.at(target_lex).resize(candidate_size_sentence[target_lex]);
		excitationLvl.at(target_lex).at(target_count)=el[target_count];
		/*if(target_lex<20)
		{
			std::cout<<"Iteration: "<<num_iter<<" Target lexicon " <<target_lex<<" Word Candidates "<< dictionary[lexicon_array_sentence[target_lex][target_count]] <<" Excitation Level: "<<excitationLvl.at(target_lex).at(target_count)<<std::endl; 
		}*/
	}
	//
            selected = 0;
            el_sum = 0;
            tmp_array = (int *)malloc(candidate_size_sentence[target_lex]*sizeof(int));
            for (i=0;i<candidate_size_sentence[target_lex];i++)
                tmp_array[i] = lexicon_array_sentence[target_lex][i];
            while(selected < max_ambiguous && selected < candidate_size_sentence[target_lex]-1) {
                if(candidate_size_sentence[target_lex] <=1) break;
                max = -10000;
                max_id = 0;
                for(i = 0; i < candidate_size_sentence[target_lex]; i++) {
					#ifdef DEBUG_SENTENCE
					//printf("before select el[%s] = %f %d\n", dictionary[tmp_array[i]], el[i], i);
					#endif
                  // if(el[i] > max && dictionary[tmp_array[i]][0] != '`')
                   if(el[i] > max)
                  {
                      max = el[i];
                      max_id = i;
                     }
                 }
                 if(max == 0 ) break;
				 for(i = 0; i < candidate_size_sentence[target_lex] && selected < max_ambiguous && selected < candidate_size_sentence[target_lex]-1; i++) {
					 if(max_id == i && el[i] != 0) {
						#ifdef DEBUG_SENTENCE
						printf("el[%s] = %f %d\n", dictionary[tmp_array[i]], el[i], i);
						#endif
					   lexicon_array_sentence[target_lex][selected] = tmp_array[i];
						  el_array_sentence[target_lex][selected] = el[i];
						 el_sum = el_sum + el[i];
						el[i] = 0;
					   selected++;
                       }
			   }
            }
            free(tmp_array);
            candidate_size_sentence[target_lex] = selected;
            //for(i = 0; i <  candidate_size_sentence[target_lex]; i++) {
			//	#ifdef DEBUG_SENTENCE
             //   printf("demon el[%s] = %f \n", dictionary[lexicon_array_sentence[target_lex][i]], el[i]);
			//	#endif
            //}           

            /*// statistics analysis of excitation level of all candidates
            if(selected != 0)
            {
                max_el = -1000000;
                min_el = 1000000;
                sum_el = 0;
                square_sum_el = 0;
                for(i = 0; i < candidate_size_sentence[target_lex]; i++){
                    printf("target %d candidate %d is %s el %f\n", target_lex, i, dictionary[lex_table_sentence[target_lex].lex_list[lexicon_array_sentence[target_lex][i]]], el_array_sentence[target_lex][i]);
                    if(el_array_sentence[target_lex][i]>max_el)
                        max_el = el_array_sentence[target_lex][i];
                    if(el_array_sentence[target_lex][i]<min_el)
                        min_el = el_array_sentence[target_lex][i];
                    sum_el += el_array_sentence[target_lex][i];
                    square_sum_el += el_array_sentence[target_lex][i]*el_array_sentence[target_lex][i];
                }
                average_el = sum_el / selected;
                variance_el = square_sum_el/selected - average_el*average_el;
                fprintf(fp_inf, "target: %d, candidate num: %d, max: %f, min: %f, average: %f, variance %f\n", target_lex, selected, max_el, min_el, average_el, variance_el);
            }*/
            free(el);
        } //process all the unknowns
        /*if(max_ambiguous_word == MAX_AMBIGUOUS_WORD && num_iter == 0)
        {
            fprintf(fp_inf,"input sentence and link number for each candidate:\n");
			for(dcount = 0; dcount < MAX_LEX_SENTENCE; dcount++) {
				fprintf(fp_inf,"<%d %d>(", dcount, old_candidate_size_sentence[dcount]);
				for(i = 0; i < old_candidate_size_sentence[dcount]; i++)  {
					dic_id = (lex_table_sentence[dcount].lex_list)[old_lexicon_array_sentence[dcount][i]];
					fprintf(fp_inf,"%s:%d ", dictionary[dic_id], link_num[dcount][i]);
				}
				fprintf(fp_inf,")\n" );
			}
        }*/
		if(equal_sentence(old_lexicon_array_sentence, lexicon_array_sentence, old_candidate_size_sentence, candidate_size_sentence) == 1) break;
        num_iter++;
		#ifdef DEBUG_ALL
        printf("num iter=%d\n",num_iter);
		#endif
        //fprintf(fp_inf,"num iter=%d\n",num_iter);
	}	
    max_ambiguous_word--;
    if(max_ambiguous_tag > 1) max_ambiguous_tag--;
    }
    for(word_count=0; word_count < MAX_LEX_SENTENCE; word_count++)
    {  
        free(link_num[word_count]);
        free(el_array_sentence[word_count]);
        el_array_sentence[word_count] = (float *)NULL;
    }
    free(link_num);
	//end_time = clock();
   // recall_time = recall_time + (end_time - start_time);

}

int equal_sentence(int *t[MAX_LEX_SENTENCE], int *s[MAX_LEX_SENTENCE], int o[MAX_LEX_SENTENCE], int c[MAX_LEX_SENTENCE])
{
	int i, i1;

	for(i = 0; i < MAX_LEX_SENTENCE; i++) {
		//printf("o[%d]=%d c[%d]=%d\n", i, o[i], i, c[i]);
		if(o[i] != c[i] ) {
			//printf("lex %d not match %d != %d\n", i, o[i], c[i]);
			return 0;
		}
	}
	for(i = 0; i < MAX_LEX_SENTENCE; i++) {
		//printf("c[%d]=%d\n", i, c[i]);
		for(i1 = 0; i1 < c[i]; i1++) {
			//printf("t[%d][%d]=%d s[%d][%d]=%d\n", i, i1, t[i][i1], i, i1, s[i][i1]);
			if(t[i][i1] != s[i][i1] ) {
				//printf("lex %d candidate %d not match %d != %d\n", i, i1, t[i][i1], s[i][i1]);
				return 0;
			}
		}
	}
	//printf("done\n");
	return 1;
}

int locate_target_sentence(int d, struct KB_entry *symbol_list, int size)
{
    int left = 0, right = 0, cur = 0;
    int i;

	//printf("search for %d in lex %d\n", d, lex);

    right = size - 1;
	if(right < 0) {
		//printf("ERROR symbol size of lex %d is 0\n", lex);
		exit(0);
	}
    if(symbol_list[right].target_id == d) return right;
    if(symbol_list[left].target_id == d) return left;

    cur = (right + left)/2;
    while(symbol_list[cur].target_id != d) {
        if(symbol_list[cur].target_id > d) {
            left = cur;
        } else {
            right = cur;
        }
        if(right - left <= 1) {
            //printf("in lex %d\n");
            //for(i = 0; i < lex_table_sentence[lex].symbol_size; i++) printf("%d ", symbol_list[i]);
            //printf("\n");
            //printf("ERROR: cannot find %d\n", d);
			return -1;
        }
        cur = (right + left)/2;
    }

    return cur;
}

int locate_row(int kb, int d)
{
    int left = 0, right = 0, cur = 0;
    int i;

    //printf("search for %d in lex %d\n", d, lex);
    right = kb_source_size[kb] - 1;
    if(right < 0) {
        printf("locate target sentence ERROR symbol size of lex is 0\n");
         return -2;
    }
    if(sentence_KB_row[kb][right].source_sym == d) return right;
    if(sentence_KB_row[kb][left].source_sym == d) return left;

    cur = (right + left)/2;
    while((sentence_KB_row[kb][cur]).source_sym != d) {
        if((sentence_KB_row[kb][cur]).source_sym > d) {
            left = cur;
        } else {
            right = cur;
        }
        if(right - left <= 1) {
            return -1;
        }
        cur = (right + left)/2;
    }

    return cur;
}
