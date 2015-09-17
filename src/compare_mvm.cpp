/////////////////////////////////////////
// hangman_scored_mvm.cpp              //
// Machine vs Machine                  //
// Hangman game for interface          //
// Author: Syed Faisal Habib           //
// Last Modified: 4/15/2014            //
/////////////////////////////////////////

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#define MACHINE1 1
#define MACHINE2 2
#define MAX_LENGTH 200
#define LINES 1

extern char machine(char* sentence, std::vector<char> selected, char &letter, char* guess, std::vector<std::string> &list_guessed_sentenced);
extern char machine_two(char* sentence, std::vector<char> selected, char &letter, char* guess, std::vector<std::string> &list_guessed_sentenced);
extern void initialize();

//This function gets a character and strings and place the char in string if it is in reference string
bool guessChar(const char * original_sentence, char * guessed_sentence,char guess)
{
	bool right_guess=false;
	for(int i=0; i<MAX_LENGTH;i++)
	{
		if(original_sentence[i]=='\0')
		{
			break;
		}
		if(original_sentence[i]==guess)
		{
			guessed_sentence[i]=guess;
			right_guess = true;
		}
	}
	return right_guess;
}

bool guessSentence(const char* original, const std::string guess)
{
     for(int i=0; i<MAX_LENGTH;i++)
     {
				if((original[i]=='\n')&&(guess[i]=='\0'))
				{
					return true;
				}
				if(original[i]!=guess[i])
				{
					return false;
				}
			}
}

//This functions reads a line from the te	xt file and stores it in sentence and hidden sentence
void getSentence(char * secret_sentence, char * sentence, int line)
{
	int random_number;
	FILE* pFILE;
	srand(time(0));
	
	pFILE = fopen("gingerbread.txt","r");
	//random_number = rand()%LINES + 1;
	random_number = line;
	for(int i=0;i<random_number;i++)
		fgets(secret_sentence,MAX_LENGTH,pFILE);
		
	for(int i=0;i<MAX_LENGTH;i++)
	{
		//To convert all alphabets to lowercase
		secret_sentence[i] = static_cast<char>(tolower(secret_sentence[i]));
		
		//fgets also inputs endline
		if(secret_sentence[i]=='\n')
		{
			sentence[i]='\0';
			break;
		}
		else if(secret_sentence[i]==' ')
			sentence[i] = ' ';
		else
			sentence[i] = '?';
	}
}

//This function searches for the character in vector and returns true if found
template <typename T>
bool searchList(std::vector <T> & list, T character)
{
	bool found=false;
	for(int i=0;i<list.size();i++)
	{	
		if(list.at(i)==character)
			found=true;
	}
	return found;	
}

//Gets called when a game finished to print score and winner
char endGame(int PLAYER,int score)
{
	char play_again;
  std::cout << "===========================\n";
	//std::cout << "Sentence: " << secret_sentence;
	std::cout << " Machine "<< PLAYER <<" scored " << score << " points\n"; 
	std::cout << "===========================\n";
  //std::cout << "Do you want to play again? (Y/N): ";
  //std::cin >> play_again;
  return 'Y';
}

int main(void)
{
	char secret_sentence[MAX_LENGTH],sentence[MAX_LENGTH];
	int PLAYER;
	int score;
	int unknown_chars, temp_size, line;
	std::string complete_guess;
	char machine_guess[MAX_LENGTH];
	std::vector <char> list_guess, list_wrong_guess;
  std::vector <std::string> list_guessed_sentence;	
	char guess,complete,play_again;
	bool wrong_guess = 0, game_finished = 0;
  initialize();
  for(PLAYER=MACHINE1;PLAYER<=MACHINE1;PLAYER++)
  {
	for(line=1;line<=LINES;line++)
	{
     //Initializing the variables
     score=0;
     unknown_chars=0;
     list_guess.clear();
     list_wrong_guess.clear();
     list_guessed_sentence.clear();
     
     
     getSentence(secret_sentence,sentence,line);
     std::cout<<"Reading Line "<<line<<"\n";
     std::cout<<"Machine "<<PLAYER<<"'s turn\n";

   	while(1)
   	{
   		if(PLAYER==MACHINE1)
   		{
   			complete = machine(sentence, list_guess, guess, machine_guess,list_guessed_sentence);
   		}
   		else
   		{
   			complete = machine_two(sentence, list_guess, guess, machine_guess,list_guessed_sentence);
   		}	
   		if((complete=='Y')||(complete=='y'))
   		{ 
   			complete_guess = std::string(machine_guess);
   			list_guessed_sentence.push_back(complete_guess);
   			//removing space as last letter
   			temp_size=complete_guess.length();
   			complete_guess[temp_size-1]='\0';
   			
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
   				score=score+2*unknown_chars;
   				std::cout << "Sentence: " << secret_sentence;
   				play_again=endGame(PLAYER,score);			
   				break;
   			}
   			else
   			{
   				score=score-unknown_chars;
   				continue;
   			}
   		}
   		
   		//Guessing by character part
   		if (searchList(list_guess,guess))
   		{
   	    	continue;
   		}
   		if(guessChar(secret_sentence,sentence,guess))
   		{
   			score++;
         //Check if all blanks have been filed  
   			for(int i=0; i<MAX_LENGTH;i++)
   			{
   				if(sentence[i]=='\0')
   				{
   					play_again=endGame(PLAYER,score);
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
   			score--;
   		}
   		
   		list_guess.push_back(guess);
   		
   	  }
     }
  }
	return 0;
}
