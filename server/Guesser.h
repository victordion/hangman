#ifndef _GUESSER_H_
#define _GUESSER_H_
#include <iostream>
#include <vector>
#include <string>
#include <sstream>
#include <algorithm>
#include <iterator>
#include <unordered_set>
#include <folly/FBString.h>
#include <mutex>

extern char recall_sentence[2048];
extern int letter_select_candidate[26];
extern double letter_select_frequency[26];
extern std::mutex mtx;

void sentence_confab(int sid);
void letter_select(std::string str, char &letter);
void free_input_sentence(int sid);
int collect_wconf_results(std::string str, int word_count);
void clear_sentence(int sid);

namespace HangmanService {
class Guesser {
  private:
	public:
		Guesser() {
      std::cout << "*** New Guesser created." << std::endl;
		}

		folly::fbstring guess(folly::fbstring masked_sentence, std::vector<folly::fbstring> wrong_guesses) {
      std::lock_guard<std::mutex> lck(mtx);
			std::vector<char> wrong_chars;
			std::unordered_set<folly::fbstring> wrong_sentences;

			folly::fbstring guessed_sentence = "";
			clear_sentence(0);

			for(folly::fbstring & str : wrong_guesses) {
				if(str.length() == 1) {
					wrong_chars.push_back(str[0]);
				}
				else {
					wrong_sentences.insert(str);
				}
			} 
			
			int selected_size = wrong_chars.size();
			int i;
			for(i = 0; i < 26; i++)
			{
				letter_select_frequency[i] = 0.0;
				letter_select_candidate[i] = 0;
			}
			for(i=0;i<selected_size;i++)
			{
				letter_select_frequency[i] = 0.0;
				letter_select_candidate[wrong_chars[i] - 'a'] = -1;
			}
	
			// Syed Entry - 10/15/2015
			// Splitting a sentence into words
			std::istringstream iss(folly::toStdString(masked_sentence));
			std::vector<std::string> words;
			std::copy(std::istream_iterator<std::string>(iss), std::istream_iterator<std::string>(), std::back_inserter(words));

			// Calling word confabulation for all the words and deciding whether to guess letter or word
			bool guess_letter = false;
			for (int i = 0; i < words.size(); i++)
			{
				if (collect_wconf_results(words.at(i), i) == 1)
					guess_letter = true;
			}

			// Calls letter_select function to guess the letter and converts it to string before returning
			if (guess_letter)
			{
				char guessed_letter;
				letter_select(folly::toStdString(masked_sentence), guessed_letter);
				free_input_sentence(0);
				return folly::fbstring(1, guessed_letter); 
			}
			else 
			{
				//Calls sentence confabulation to get the sentence and if that sentence is already guessed, guesses a letter instead
				//guessed_sentence = sentence_confab(0);
				sentence_confab(0);
				free_input_sentence(0);
				std::cout<<"Recall sentence: "<<recall_sentence<<std::endl;
				for(int i = 0; recall_sentence[i]!= '\0'; i++)
				{
					guessed_sentence += recall_sentence[i];
				}
				guessed_sentence = guessed_sentence.substr(0, guessed_sentence.length() - 1);
				
				std::unordered_set<folly::fbstring>::const_iterator got = wrong_sentences.find (guessed_sentence);
				if (got != wrong_sentences.end())
				{
					char guessed_letter;
					letter_select(folly::toStdString(masked_sentence), guessed_letter);
					//char guessed_letter = letter_select(masked_sentence);

					return folly::fbstring(1, guessed_letter); 
				}
				
				strcpy(recall_sentence, "\0");
				return guessed_sentence;
			}
		}
};



}

#endif
