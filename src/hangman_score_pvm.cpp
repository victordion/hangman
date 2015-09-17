/////////////////////////////////////////
// hangman_scored_pvm.cpp              //
// Player vs Machine                   //
// Hangman game for interface          //
// Author: Syed Faisal Habib           //
// Last Modified: 4/3/2014            //
/////////////////////////////////////////

#include <iostream>
#include <fstream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#define HUMAN 0
#define MACHINE 1
#define MAX_LENGTH 200
#define LINES 16

extern char machine(const std::string masked_sentence, std::vector<char> selected, string& curr_guess, const std::vector<std::string>& wrongly_guessed_sentences);
extern void initialize();

//This function gets a character and strings and place the char in string if it is in reference string
bool guessChar(string original_sentence, string & curr_sentence, char guess)
{
	bool guess_status = false;

	if(original_sentence.length() != curr_sentence.length()) {
		std::cout << "Sentence length mismatch!" << endl;
		return false;
	}

	for(int i = 0; i < original_sentence.length(); i++) {
		if(original_sentence[i] == guess) {
			curr_sentence[i] = guess;
			right_guess = true;
		}
	}
	return guess_status;
}


bool guessSentence(std::string original_sentence, std::string guessed_sentence) {
	if(original_sentence.length() != curr_sentence.length()) {
		std::cout << "Guessed Sentence and original sentence length mismatch!" << endl;
		return false;
	}

	for(int i = 0; i < original_sentence.length(); i++) {
		if(original_sentence[i] != guessed_sentence[i] {
			return false;
		}
	}
	return true;
}

// Reads a line (sentence) from the text file
// Stores the ground truth in original_sentence
// Initialize masked_sentence
void getSentence(std::string& orginal_sentence, std::string& masked_sentence)
{
	std::string sentence_filename = "./data/gingerbread.txt";
	std::ifstream in_stream(sentence_filename);
	
	int num_lines;
	ifstream >> num_lines;
	srand(time(0));
	int line_idx = rand() % num_lines;

	std::cout << "Choosing a random sentence from " << sentence_filename << endl;
	for (int i = 0; i < line_idx; ++i){
		std::getline(in_stream, orginal_sentence);
	}
	std::cout << "Ground truth sentence is " << endl;
	std::cout << original_sentence << endl;

	for(int i = 0; i < original_sentence.length(); ++i) {
		if(original_sentence[i] == ' ') {
			masked_sentence[i] = ' ';
		}
		else {
			masked_sentence[i] = '?';
		}
	}
}

template <typename T>
bool searchVec(const std::vector<T>& vec, const T item)
{
	for(int i = 0; i < vec.size(); i++) {	
		if(list[i] == item)
			return true;
	}
	return false;	
}

//Gets called when a game finished to print score and winner
char endGame(int player1_score, int player2_score)
{
	char play_again;
	std::cout << "===========================\n";
	//std::cout << "Sentence: " << secret_sentence;
	std::cout << " Human Player scored " << player1_score << " points\n"; 
	std::cout << " Machine scored " << player2_score << " points\n";
	if(player1_score > player2_score)
		std::cout<<" Human Player won\n";
	else
		std::cout<<" Machine won\n";
	std::cout << "===========================\n";
	std::cout << "Do you want to play again? (Y/N): ";
	std::cin >> play_again;
	return play_again;
}

int main(void)
{
	const int num_players = 2;
	std::string original_sentence;
	std::string masked_sentence;
	int player_id;
	std::vector<int> player_scores(num_players);

	int unknown_chars, temp_size;
	
	std::string complete_guess;
	char machine_guess[MAX_LENGTH];
	
	std::vector<char> rightly_guessed_chars;
  std::vector<char>	wrongly_guessed_chars;
	std::vector<std::string> wrongly_guessed_sentences;	
	
	char guess,complete,play_again;
	bool wrong_guess = 0, game_finished = 0;

	std::string curr_guess;
	// Computer player id is 0
	// Human player id is 1 ~ num_players-1
	player_id = rand() % num_players;

	initialize();
	do{
		//Initializing the variables
		PLAYER1_score = 0;
		PLAYER2_score = 0;
		unknown_chars = 0;


		getSentence(original_sentence, masked_sentence);


		while(1)
		{
			std::fill(player_scores.begin(), player_scores.end(), 0);
			std::cout << "==========Game Starts============\n";
			if(player_id > 0)
			{
				std::cout << "(Human) Player" << player_id <<"'s turn \n";
				std::cout << "Player's score: " << player_scores[player_id] << std::endl;
				std::cout << masked_sentence << std::endl;		
				std::cout << "Input your guess: " << std::endl;		
				std::cin >> curr_guess;
			}
			else
			{
				std::cout << "MACHINE's turn" << std::endl;
				std::cout << "MACHINE's score: " << player_scores[player_id] << std::endl;
				std::cout << masked_sentence << endl;

				complete = machine(sentence, list_guess, guess, machine_guess,list_guessed_sentence);
				
				if(complete=='N')
					std::cout << "Machine's guessed: " << curr_guess << std::endl;
			}	
			
			if((complete=='Y')||(complete=='y'))
			{
				if(PLAYER==HUMAN)
				{
					std::cout<<"Guess the complete sentence: ";
					std::getline(std::cin,complete_guess);
					std::getline(std::cin,complete_guess);
					if(searchList(list_guessed_sentence,complete_guess))
					{
						std::cout << complete_guess << " already tried \n";
						continue;
					}
					list_guessed_sentence.push_back(complete_guess);
				}
				else
				{

					complete_guess = std::string(machine_guess);
					list_guessed_sentence.push_back(complete_guess);
					//removing space as last letter
					temp_size=complete_guess.length();
					complete_guess[temp_size-1]='\0';
					std::cout<<"Machine guessed: "<<complete_guess<<std::endl;
				}
				//To get unknown chars left in string
				unknown_chars=0;
				for(int i=0;i<MAX_LENGTH;i++)
				{
					if(sentence[i]=='\0')
						break;
					else if(sentence[i]=='?')
						unknown_chars++;
				}

				if(guessSentence(secret_sentence,complete_guess))
				{
					if(PLAYER==HUMAN)
						PLAYER1_score=PLAYER1_score+2*unknown_chars;
					else
						PLAYER2_score=PLAYER2_score+2*unknown_chars;

					std::cout << "Sentence: " << secret_sentence;
					play_again=endGame(PLAYER1_score,PLAYER2_score);

					break;
				}
				else
				{
					std::cout << "Sorry, that is not the right answer \n";
					if(PLAYER==HUMAN)
					{
						PLAYER1_score=PLAYER1_score-unknown_chars;
						std::cout << "Human Player's score: " << PLAYER1_score << std::endl;
						PLAYER=MACHINE;
					}
					else
					{
						PLAYER2_score=PLAYER2_score-unknown_chars;
						std::cout << "MACHINE's score: " << PLAYER2_score << std::endl;
						PLAYER=HUMAN;
					}
					wrong_guess=1;
				}
			}
			if(wrong_guess)
			{
				wrong_guess=0;
				continue;
			}

			//Guessing by character part
			if(PLAYER==HUMAN)
			{
				std::cout<<"Guess a character:";
				std::cin>>guess;
			}
			if (searchList(list_guess,guess))
			{
				std::cout << guess << " already tried \n";
				continue;
			}
			if(guessChar(secret_sentence,sentence,guess))
			{
				if(PLAYER==HUMAN)
				{
					PLAYER1_score++;
					std::cout << "Human Player's score: " << PLAYER1_score << std::endl;
				}
				else
				{
					PLAYER2_score++;
					std::cout << "MACHINE's score: " << PLAYER2_score << std::endl;
				}
				//Check if all blanks have been filed  
				for(int i=0; i<MAX_LENGTH;i++)
				{
					if(sentence[i]=='\0')
					{
						play_again=endGame(PLAYER1_score,PLAYER2_score);
						game_finished=1;
						break;
					}
					if(sentence[i]=='?')
					{
						break;
					}
				}
				if(game_finished)
				{
					game_finished=0;
					break;
				}	
			}
			else
			{
				list_wrong_guess.push_back(guess);
				if(PLAYER==HUMAN)
				{
					PLAYER1_score--;
					std::cout << "Human Player's score: " << PLAYER1_score << std::endl;
					PLAYER=MACHINE;
				}
				else
				{
					PLAYER2_score--;
					std::cout << "MACHINE's score: " << PLAYER2_score << std::endl;
					PLAYER=HUMAN;
				}
			}

			list_guess.push_back(guess);

			std::cout << "Wrong Guesses: ";
			for(int i=0;i<list_wrong_guess.size();i++) 
				std::cout << list_wrong_guess.at(i) <<",";
			std::cout<<"\n";
		}
	}
	while((play_again=='Y')||(play_again=='y'));
	return 0;
}
