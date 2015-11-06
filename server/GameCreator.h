#ifndef _GAMECREATOR_H_
#define _GAMECREATOR_H_
#include <vector>
#include <string>
#include <unordered_set>
#include <folly/FBString.h>
#include <ctime>
#include <fstream>

class GameCreator {
private:
  std::vector<folly::fbstring> repo;
public:
  GameCreator() {
     //repo.push_back("it was a butterfly flying just outside the wind");
     //repo.push_back("that was a mistake as one of the horses trotted over to the fence and whinnied");
     //repo.push_back("the gingerbread man did not look back");
     //repo.push_back("a little further on he met a cow");
     std::ifstream data("./sentence_repo.txt");
     for(std::string line; getline(data, line); ) {
       repo.push_back(line);
       std::cout << "Reading in sentence: " << std::endl << line << std::endl;
     }
  }
  folly::fbstring genSentence(folly::fbstring sentence_id) {
    int num_id = 0;
    try {
      std::string id = sentence_id.toStdString();
      num_id = std::stoi(id); 
    }
    catch(...) {
      num_id = 0;
    }
    return repo[num_id % repo.size()];
  }
};


#endif
