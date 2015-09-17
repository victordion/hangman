/////////////////////////////////////////
// hangman.cpp                         //
// Part of hangman game for interface  //
// Author: Syed Faisal Habib           //
// Last Modified: 3/17/2014            //
/////////////////////////////////////////

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#define MACHINE 0
#define HUMAN 1
#define MAX_LENGTH 50

//add points
//winning guy gets extra point

extern char machine(const char* sentence, std::vector<char> selected, char &letter, char* guess);

//This function gets a character and strings and place the char in string if it is in reference string
bool takeGuess(const char * secret_sentence, char * guessed_sentence,char guess)
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
	if(right_guess)
	{
		return false;
	}
	else
	{
		std::cout << "Sorry, you guessed wrong \n" ;
		return true;
	}
}

//This functions reads a line from the te	xt file and stores it in sentence and hidden sentence
void getSentence(char * secret_sentence, char * sentence)
{
	int random_number;
	FILE* pFILE;
	srand(time(0));
	
	pFILE = fopen("sample.txt","r");
	random_number = rand()%4 + 1;
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

int main(void)
{
	char secret_sentence[MAX_LENGTH],sentence[MAX_LENGTH];
	bool PLAYER=HUMAN;
	std::string complete_guess;
	char machine_guess[MAX_LENGTH];
	std::vector <char> list_guess, list_wrong_guess;	
	char guess,complete;
	bool wrong_guess = 0;
	
	getSentence(secret_sentence,sentence);
	std::cout << secret_sentence;
	
	while(1)
	{
		std::cout << "=========================\n";
		if(PLAYER==HUMAN)
		{
			std::cout<<"Human's turn \n";
			std::cout << sentence << "\n";		
			std::cout<<"Do you want to guess the complete sentence: (Y/N)";
			std::cin>>complete;
		}		
		else
		{
			std::cout<<"MACHINE's turn \n";
			complete = machine(sentence, list_guess, guess, machine_guess);
			complete_guess = std::string(machine_guess);
		}
		if((complete=='Y')||(complete=='y'))
		{
			if(PLAYER==HUMAN)
			{
				std::cout<<"Guess the complete sentence: ";
				std::getline(std::cin,complete_guess);
				std::getline(std::cin,complete_guess);
			}	
			//std::cout<<"You entered: " <<complete_guess<<"\n";
			for(int i=0; i<MAX_LENGTH;i++)
			{
				if((secret_sentence[i]=='\n')&&(complete_guess[i]=='\0'))
				{
					if(PLAYER==HUMAN)
						std::cout << "Human Player Won \n";
					else
						std::cout << "Machine Won \n";
					std::cout << "Sentence: " << secret_sentence;
					return 0;
				}
				if(complete_guess[i]!=secret_sentence[i])
				{
					std::cout << "Sorry, that is not the right answer \n";
					std::cout << "====================================\n";
					if(PLAYER==HUMAN)
						PLAYER=MACHINE;
					else
						PLAYER=HUMAN;
					wrong_guess=1;
					break;
				}
			}
		}
		if(wrong_guess)
		{
			wrong_guess=0;
			continue;
		}
		if(PLAYER==HUMAN)
		{
			std::cout<<"Guess a character:";
			std::cin>>guess;
			if (searchList(list_guess,guess))
			{
		    	std::cout << guess << " already tried \n";
		    	continue;
			}
		}
		
		if(takeGuess(secret_sentence,sentence,guess))
		{
			list_wrong_guess.push_back(guess);
			
			if(PLAYER==HUMAN)
				PLAYER=MACHINE;
			else
				PLAYER=HUMAN;
		}
		else
		{
			for(int i=0; i<MAX_LENGTH;i++)
			{
				if(sentence[i]=='\0')
				{
					if(PLAYER==HUMAN)
						std::cout << "Human Player Won \n";
					else
						std::cout << "Machine Won \n";
					std::cout << "Sentence: " << secret_sentence;
					return 0;
				}
				if(sentence[i]=='?')
				{
					break;
				}
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
