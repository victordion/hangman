#ifndef _GUESSER_H_
#define _GUESSER_H_
#include <vector>
#include <string>
#include <unordered_set>
#include <folly/FBString.h>

class Guesser {
public:
  folly::fbstring guess(folly::fbstring masked_sentence, std::vector<folly::fbstring> wrong_guesses) {
    std::unordered_set<char> wrong_chars;
    std::unordered_set<folly::fbstring> wrong_sentences;
    std::unordered_set<char> guessed_chars;

    for(folly::fbstring & str : wrong_guesses) {
      if(str.length() == 1) {
        wrong_chars.insert(str[0]);
      }
      else {
        wrong_sentences.insert(str);
      }
    } 
    
    for(char & c : masked_sentence) {
      if(c >= 'a' && c <= 'z') {
        guessed_chars.insert(c);
      }
    }
    
    char c = rand() % 26 + 'a';    
    int count = 0;

    while(wrong_chars.find(c) != wrong_chars.end() || guessed_chars.find(c) != guessed_chars.end()) {
      c = rand() % 26 + 'a';
      if(count++ > 100) {
        return "";
      }
    }
    return folly::fbstring(1, c); 

  }
};


#endif
