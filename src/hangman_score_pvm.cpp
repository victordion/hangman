/////////////////////////////////////////
// hangman_scored_pvm.cpp              //
// Player vs Machine                   //
// Hangman game for interface          //
// Author: Syed Faisal Habib           //
// Last Modified: 4/3/2014            //
/////////////////////////////////////////

#include <iostream>
#include <cstdio>
#include <vector>
#include <string>
#include <cstdlib>
#include <ctime>

#define HUMAN 0
#define MACHINE 1
#define MAX_LENGTH 200
#define LINES 16

extern char machine(char* sentence, std::vector<char> selected, char &letter, char* guess, std::vector<std::string> &list_guessed_sentenced);
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

//This functions reads a line from the text file and stores it in sentence and hidden sentence
void getSentence(char * secret_sentence, char * sentence)
{
	int random_number;
	FILE* pFILE;
	srand(time(0));
	
	pFILE = fopen("./data/gingerbread.txt","r");
	//random_number = rand()%LINES + 1;
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
char endGame(int player1_score,int player2_score)
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
	char secret_sentence[MAX_LENGTH],sentence[MAX_LENGTH];
	bool PLAYER;
	int PLAYER1_score,PLAYER2_score;
	int unknown_chars, temp_size;
	std::string complete_guess;
	char machine_guess[MAX_LENGTH];
	std::vector <char> list_guess, list_wrong_guess;
  std::vector <std::string> list_guessed_sentence;	
	char guess,complete,play_again;
	bool wrong_guess = 0, game_finished = 0;
 
  initialize();
  do{
  //Initializing the variables
  PLAYER1_score=0;
  PLAYER2_score=0;
  unknown_chars=0;
  list_guess.clear();
  list_wrong_guess.clear();
  list_guessed_sentence.clear();
  
	getSentence(secret_sentence,sentence);
	//std::cout << secret_sentence;
  
  if(rand()%2 == 0)
      PLAYER=HUMAN;
  else
     PLAYER=MACHINE;
  	
	while(1)
	{
		std::cout << "=========================\n";
		if(PLAYER==HUMAN)
    {
			std::cout << "Human Player's turn \n";
      std::cout << "Human Player's score: " << PLAYER1_score << std::endl;
      std::cout << sentence << "\n";		
	    std::cout<<"Do you want to guess the complete sentence: (Y/N)";
		  std::cin>>complete;
    }
		else
    {
			std::cout << "MACHINE's turn \n";
      std::cout << "MACHINE's score: " << PLAYER2_score << std::endl;
      std::cout << sentence << "\n";
      complete = machine(sentence, list_guess, guess, machine_guess,list_guessed_sentence);
			if(complete=='N')
         std::cout<<"Machines guessed: "<<guess<<std::endl;
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
