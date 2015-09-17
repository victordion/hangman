#ifndef _PLAYER_H_
#define _PLAYER_H_


#include "GuessBase.h"
#include "ForwardDeclarations.h"

class Player : public GuessBase {
	public:
		Player(std::string name): m_name(name) { 
			std::fill(m_guessed_chars.begin(), m_guessed_chars.end(); false);
		}
		
		// Currently will not guess a char
	  virtual std::string guess(const std::string masked_sentence) {
			int guess;
			while(1) {
				guess = rand() % ALPHABET_SIZE;
			  if(!m_guessed_chars[guess]) {
					m_guessed_chars[guess] = true;
					break;
				}	
			}
			char ret = static_cast<char>(guess + 'a');

			return std::string(ret);
		}

	protected:
	  		
		std::string name;
		std::vector<bool> m_guessed_chars;
		std::vector<std::string> m_guessed_sentences;

};

#endif
