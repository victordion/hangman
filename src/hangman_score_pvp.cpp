/////////////////////////////////////////
// hangman_scored_pvp.cpp              //
// Hangman game for interface          //
// Author: Syed Faisal Habib           //
// Last Modified: 3/24/2014            //
/////////////////////////////////////////

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#define PLAYER1 0
#define PLAYER2 1
#define MAX_LENGTH 100

//This function gets a character and strings and place the char in string if it is in reference string
bool guessChar(const char * secret_sentence, char * guessed_sentence,char guess)
{
	bool right_guess=false;
	for(int i=0; i<MAX_LENGTH;i++)
	{
		if(secret_sentence[i]=='\0')
		{
			break;
		}
		if(secret_sentence[i]==guess)
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
void getSentence(char * secret_sentence, char * sentence)
{
	int random_number;
	FILE* pFILE;
	
	pFILE = fopen("gingerbread.txt","r");
	//random_number = rand()%4 + 1;
	std::cout<<"Please enter the line number(1-13): ";
	std::cin>>random_number;
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
bool searchList(std::vector <char> & list, char character)
{
	bool found=false;
	for(int i=0;i<list.size();i++)
	{	
		if(list.at(i)==character)
			found=true;
	}
	return found;	
}

void endGame(int player1_score,int player2_score)
{
	std::cout << "===========================\n";
	//std::cout << "Sentence: " << secret_sentence;
	std::cout << " Player 1 scored " << player1_score << " points\n"; 
	std::cout << " Player 2 scored " << player2_score << " points\n";
	if(player1_score > player2_score)
		std::cout<<" Player 1 won\n";
	else
		std::cout<<" Player 2 won\n";
	std::cout << "===========================\n";
}

int main(void)
{
	char secret_sentence[MAX_LENGTH],sentence[MAX_LENGTH];
	bool PLAYER=PLAYER1;
	int PLAYER1_score=0,PLAYER2_score=0,score;
	int unknown_chars=0;
	std::string complete_guess;
	char machine_guess[MAX_LENGTH];
	std::vector <char> list_guess, list_wrong_guess;	
	char guess,complete;
	bool wrong_guess = 0;
	
	getSentence(secret_sentence,sentence);
	//std::cout << secret_sentence<<std::endl;
	
	while(1)
	{
		std::cout << "=========================\n";
		if(PLAYER==PLAYER1)
    {
			std::cout << "Player 1's turn \n";
      std::cout << "Player 1's score: " << PLAYER1_score << std::endl;
    }
		else
    {
			std::cout << "Player 2's turn \n";
      std::cout << "Player 2's score: " << PLAYER2_score << std::endl;
    }	
		std::cout << sentence << "\n";		
		std::cout<<"Do you want to guess the complete sentence: (Y/N)";
		std::cin>>complete;
		if((complete=='Y')||(complete=='y'))
		{
			std::cout<<"Guess the complete sentence: ";
			std::getline(std::cin,complete_guess);
			std::getline(std::cin,complete_guess);	
			
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
          if(PLAYER==PLAYER1)
						PLAYER1_score=PLAYER1_score+2*unknown_chars;
					else
						PLAYER2_score=PLAYER2_score+2*unknown_chars;
					
					std::cout << "Sentence: " << secret_sentence;
          endGame(PLAYER1_score,PLAYER2_score);
					return 0;
      }
      else
      {
          std::cout << "Sorry, that is not the right answer \n";
					if(PLAYER==PLAYER1)
					{
						PLAYER1_score=PLAYER1_score-unknown_chars;
            std::cout << "Player 1's score: " << PLAYER1_score << std::endl;                                                
						PLAYER=PLAYER2;
					}
					else
					{
						PLAYER2_score=PLAYER2_score+unknown_chars;
            std::cout << "Player 2's score: " << PLAYER2_score << std::endl;                                                   
						PLAYER=PLAYER1;
					}
					wrong_guess=1;
      }
		}
		if(wrong_guess)
		{
			wrong_guess=0;
			continue;
		}
		
		std::cout<<"Guess a character:";
		std::cin>>guess;
		if (searchList(list_guess,guess))
		{
	    	std::cout << guess << " already tried \n";
	    	continue;
		}
		if(guessChar(secret_sentence,sentence,guess))
		{
			if(PLAYER==PLAYER1)
      {
				PLAYER1_score++;
        std::cout << "Player 1's score: " << PLAYER1_score << std::endl;
			}
      else
			{
      	PLAYER2_score++;
       std::cout << "Player 2's score: " << PLAYER2_score << std::endl;
			}
      for(int i=0; i<MAX_LENGTH;i++)
			{
				if(sentence[i]=='\0')
				{
					endGame(PLAYER1_score,PLAYER2_score);
					return 0;
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
			
			if(PLAYER==PLAYER1)
			{
				PLAYER1_score--;
        std::cout << "Player 1's score: " << PLAYER1_score << std::endl;
				PLAYER=PLAYER2;
			}
			else
			{
				PLAYER2_score--;
        std::cout << "Player 2's score: " << PLAYER2_score << std::endl;
				PLAYER=PLAYER1;
			}
		}
		
		list_guess.push_back(guess);
		
		std::cout << "Wrong Guesses: ";
		for(int i=0;i<list_wrong_guess.size();i++) 
			std::cout << list_wrong_guess.at(i) <<",";
		std::cout<<"\n";
	}
	return 0;
}
