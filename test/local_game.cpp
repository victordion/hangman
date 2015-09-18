#include "ForwardDeclarations.h"
#include "INCLUDE"

int main() {

  Game game(2);
  game.loadSentence();


  while(!game.move()) {
    game.showProgress();
  }
  game.finalize();
  /*
  game.move();
  game.move();
  game.move();
  game.move();
  game.move();
  game.move();
  game.move();
  */
  return 0;  


}
