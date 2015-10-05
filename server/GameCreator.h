#ifndef _GAMECREATOR_H_
#define _GAMECREATOR_H_
#include <vector>
#include <string>
#include <unordered_set>
#include <folly/FBString.h>

class GameCreator {
private:
  std::vector<folly::fbstring> repo;
public:
  folly::fbstring genSentence() {
    return folly::fbstring("i see you smile on the tower");
  }
};


#endif
