/* 
 *The guess interface
 * 
 */

class GuessBase{
public:
	// Returns either a string of length 1, representing guessed character
	// Or a string of the same length of masked_sentence, representing guessed sentence
	virtual std::string guess(std::string masked_sentence, std::vector<string> wrongly_guessed_sentences) = 0;
};

