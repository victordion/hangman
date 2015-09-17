#ifndef _GAME_H_
#define _GAME_H_
#include "ForwardDeclarations.h"

class Game {
	public:
		
		Game(int num_players) : m_num_players(num_players) {
		  std::cout << "Initializing Game ..." << std::endl;
			
			m_players.resize(num_players);
			std::fill(m_scores.begin(), m_scores.end(), 0);
			m_curr_turn = 0;

			for (int i = 0; i < num_players; ++i) {
				std::string name = std::to_string(i);
				m_players[i].setName(name);
				std::cout << "Added Player" << name << " to game" << endl;
			} 
		}

		void loadSentence(std::string filename = "./data/gingerbread.txt") {
			std::cout << "Loading sentence..." << std::endl;
			ifstream input(filename);
			getline(input, m_true_sentence);
			std::cout << "Ground truth sentence is " << m_true_sentence << std::endl;
			m_masked_sentence.assign(m_true_sentence.length(), '?');
		}


		int  move() {
			std::string guess = m_players[m_curr_turn].guess(m_masked_sentence);
			checkAnswer(guess);
			return checkStatus();
		}

		int checkStatus() {
			
		}

		void checkAnswer(std::string guess) {
			// Case: player guessed a char
			/
			if (guess.length() == 1) {
				for (int i = 0; i < m_true_sentence.length(); ++i) {
					if (m_true_sentence[i] == guess) {
						m_masked_sentence[i] = m_true_sentence[i];
					}
				}
			}
			m_curr_turn = (m_curr_turn + 1) % m_num_players;
		}

		void showProgress() {
			std::cout << "Current turn is Player " << m_curr_turn << std::endl;
			std::cout << "Current progrss is " << m_masked_sentence << std::endl;
		}



	protected:
		int m_num_players;
		int m_curr_turn;  // which player's turn
		std::vector<Player> m_players;
		std::vector<int> m_scores;
		std::string m_true_sentence;
		std::string m_masked_sentence;

};


#endif
