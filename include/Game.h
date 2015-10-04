#ifndef _GAME_H_
#define _GAME_H_
#include "Player.h"
#include <vector>
#include <iostream>
#include <fstream>

class Game {
	public:
		
		Game(int num_players) : m_num_players(num_players), m_curr_round(0) {
		  srand(time(0));
      std::cout << "Initializing Game ..." << std::endl;
      
			m_players.resize(num_players);
			std::fill(m_scores.begin(), m_scores.end(), 0);
			
      m_curr_turn = 0;
      m_num_players = num_players;

			for (int i = 0; i < num_players; ++i) {
				std::string name = std::to_string(i);
				m_players[i].setName(name);
				std::cout << "Added Player " << name << " to game" << std::endl;
			} 
		}

		void loadSentence(std::string file_name = "./data/gingerbread.txt") {
			std::cout << "Loading sentence..." << std::endl;
      
      std::ifstream input(file_name, std::ifstream::in);

			getline(input, m_true_sentence);

      for (size_t i = 0; i < m_true_sentence.length(); ++i) {
        if (m_true_sentence[i] >= 'A' && m_true_sentence[i] <= 'Z') {
          m_true_sentence[i] -= 'A' - 'a';
        } 
      }

			std::cout << "Ground truth sentence is " << m_true_sentence << std::endl;

      m_masked_sentence.resize(m_true_sentence.length());

      for (size_t i = 0; i < m_true_sentence.length(); ++i) {
        if (m_true_sentence[i] == ' ') {
          m_masked_sentence[i] = ' ';
        }
        else {
          m_masked_sentence[i] = '?';
        }
      }
			std::cout << "Masked sentence is " << m_masked_sentence << std::endl;
		}

		int move() {
      m_curr_round++;

			std::string guess = m_players[m_curr_turn].guess(m_masked_sentence);

      std::cout << "Current move is by player " << m_curr_turn << std::endl;
      std::cout << "Guess is " << guess << std::endl;

			checkAnswer(guess);
			return checkStatus();
		}

		int checkStatus() {
			return m_masked_sentence == m_true_sentence ? 1 : 0;
		}

		void checkAnswer(std::string guess) {
			// Case: player guessed a char
			if (guess.length() == 1) {
				for (size_t i = 0; i < m_true_sentence.length(); ++i) {
					if (m_true_sentence[i] == guess[0]) {
						m_masked_sentence[i] = m_true_sentence[i];
					}
				}
			}
			m_curr_turn = (m_curr_turn + 1) % m_num_players;
		}

    void finalize() {
      std::cout << "Current round is " << m_curr_round << std::endl;
      showProgress();
    }

		void showProgress() {
			std::cout << "Current turn is Player " << m_curr_turn << std::endl;
			std::cout << "Current progrss is " << m_masked_sentence << std::endl;
		}

	protected:
    
		int m_num_players;
    int m_curr_round;
		int m_curr_turn;  // which player's turn
		std::vector<Player> m_players;
		std::vector<int> m_scores;
		std::string m_true_sentence;
		std::string m_masked_sentence;

};


#endif
