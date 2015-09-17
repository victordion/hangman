/////////////////////////////////////////
// hangman_triesd_mvm.cpp              //
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

#define MACHINE1 0
#define MACHINE2 1
#define MAX_LENGTH 200
#define LINES 16

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

//Gets called when a game finished to print tries and winner
char endGame(int player1_tries,int player2_tries)
{
	char play_again;
  std::cout << "===========================\n";
	//std::cout << "Sentence: " << secret_sentence;
	std::cout << " Machine 1 took " << player1_tries << " turns\n"; 
	std::cout << " Machine 2 took " << player2_tries << " turns\n";
	if(player1_tries < player2_tries)
		std::cout<<" Machine 1 won\n";
	else
		std::cout<<" Machine 2 won\n";
	std::cout << "===========================\n";
  std::cout << "Do you want to play again? (Y/N): ";
  std::cin >> play_again;
  return play_again;
}

int main(void)
{
	char secret_sentence[MAX_LENGTH],sentence[MAX_LENGTH];
	int PLAYER;
	int PLAYER1_tries,PLAYER2_tries;
	int unknown_chars, temp_size, line;
	std::string complete_guess;
	char machine_guess[MAX_LENGTH];
	std::vector <char> list_guess, list_wrong_guess;
  std::vector <std::string> list_guessed_sentence;	
	char guess,complete,play_again;
	bool wrong_guess = 0, game_finished = 0;
	bool Machine_turn;
 
  initialize();
  do{
  //Initializing the variables
  PLAYER1_tries=0;
  PLAYER2_tries=0;
  unknown_chars=0;
  
  std::cout<<"Which line number to read: ";
  std::cin>>line;
  
	//std::cout << secret_sentence;
  
  for(PLAYER=MACHINE2;PLAYER<=MACHINE2;PLAYER++)
  {
	list_guess.clear();
	list_wrong_guess.clear();
	list_guessed_sentence.clear();
	getSentence(secret_sentence,sentence,line);
    while(1)
	{
		std::cout << "=========================\n";
		if(PLAYER==MACHINE1)
		{
			std::cout << "Machine 1's turn \n";
			std::cout << "Machine 1's tries: " << PLAYER1_tries << std::endl;
			std::cout << sentence << "\n";
			complete = machine(sentence, list_guess, guess, machine_guess,list_guessed_sentence);
			if(complete=='N')
			std::cout<<"Machines guessed: "<<guess<<std::endl;
		}
		else
		{
			std::cout << "Machine 2's turn \n";
			std::cout << "Machine 2's tries: " << PLAYER2_tries << std::endl;
			std::cout << sentence << "\n";
			complete = machine_two(sentence, list_guess, guess, machine_guess,list_guessed_sentence);
			if(complete=='N')
			std::cout<<"Machines guessed: "<<guess<<std::endl;
		}	
		if((complete=='Y')||(complete=='y'))
		{ 
			complete_guess = std::string(machine_guess);
			list_guessed_sentence.push_back(complete_guess);
			//removing space as last letter
			temp_size=complete_guess.length();
			complete_guess[temp_size-1]='\0';
			std::cout<<"Machine guessed: "<<complete_guess<<std::endl;
			
			//To get unknown chars left in string
			/*unknown_chars=0;
			for(int i=0;i<MAX_LENGTH;i++)
			{
				if(sentence[i]=='\0')
					break;
				else if(sentence[i]=='?')
					unknown_chars++;
			}*/
				
			if(guessSentence(secret_sentence,complete_guess))
			{
				if(PLAYER==MACHINE1)
					PLAYER1_tries=PLAYER1_tries++;
				else
					PLAYER2_tries=PLAYER2_tries++;
				std::cout << "Sentence: " << secret_sentence;
				break;
				//play_again=endGame(PLAYER1_tries,PLAYER2_tries);			
			}
			else
			{
				std::cout << "Sorry, that is not the right answer \n";
				if(PLAYER==MACHINE1)
				{
					PLAYER1_tries++;
					std::cout << "Machine 1's tries: " << PLAYER1_tries << std::endl;
				}
				else
				{
					PLAYER2_tries++;
					std::cout << "Machine 2's tries: " << PLAYER2_tries << std::endl;
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
		if (searchList(list_guess,guess))
		{
	    	std::cout << guess << " already tried \n";
	    	continue;
		}
		if(guessChar(secret_sentence,sentence,guess))
		{
			if(PLAYER==MACHINE1)
			{
				PLAYER1_tries++;
				std::cout << "Machine 1's tries: " << PLAYER1_tries << std::endl;
			}
			else
			{
				PLAYER2_tries++;
				std::cout << "Machine 2's tries: " << PLAYER2_tries << std::endl;
			}
      //Check if all blanks have been filed  
			for(int i=0; i<MAX_LENGTH;i++)
			{
				if(sentence[i]=='\0')
				{
					//play_again=endGame(PLAYER1_tries,PLAYER2_tries);
					game_finished=1;
					break;
				}
				if(sentence[i]=='?')
				{
					break;
				}
			}
		}
		else
		{
			list_wrong_guess.push_back(guess);
			if(PLAYER==MACHINE1)
			{
				PLAYER1_tries++;
				std::cout << "Machine 1's tries: " << PLAYER1_tries << std::endl;
			}
			else
			{
				PLAYER2_tries++;
				std::cout << "Machine 2's tries: " << PLAYER2_tries << std::endl;
			}
		}
		if(game_finished)
		{
				game_finished=0;
				break;
		}	
		list_guess.push_back(guess);
		
		std::cout << "Wrong Guesses: ";
		for(int i=0;i<list_wrong_guess.size();i++) 
			std::cout << list_wrong_guess.at(i) <<",";
		std::cout<<"\n";
	  }
	}
	play_again=endGame(PLAYER1_tries,PLAYER2_tries);
  }
  while((play_again=='Y')||(play_again=='y'));
	return 0;
}
