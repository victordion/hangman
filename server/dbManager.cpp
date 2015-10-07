#ifndef _DBMANAGER_H_
#define _DBMANAGER_H_
#include <cmath>
#include <iostream>
#include "leveldb/db.h"
#include <sstream>
#include <string>
#include <cassert>
using namespace std;

class dbManager {
private:
  leveldb::DB *db;
  leveldb::Options options;
  std::string path = "/tmp/leveldb";
  leveldb::Status status;

public:
  dbManager() {
    options.create_if_missing = true;
    leveldb::Status status = leveldb::DB::Open(options, path, &db);
  }
  
  ~dbManager() {
    delete db;
  }
  /*
   * Get the excitation level
   */
  double getEl(int lex_s, int lex_t, int word_s, int word_t) {
    std::string key = std::to_string(lex_s) + 
                      "@" +
                      std::to_string(lex_t) +
                      "@" + 
                      std::to_string(word_s) +
                      "@" +
                      std::to_string(word_t);
    std::string el_str;
    status = db->Get(leveldb::ReadOptions(), key, &el_str);
     
    /*
     * Debuging
     */
    std::cout << "Read db. Key: " << key << " Value: " << el_str << std::endl;
    return std::stod(el_str);
  }

  /*
   * Write in the excitation level
   */
  int putEl(int lex_s, int lex_t, int word_s, int word_t, double el) {
    std::ostringstream el_str;
    el_str << el;
    std::string value = el_str.str();

    std::string key = std::to_string(lex_s) + 
                      "@" +
                      std::to_string(lex_t) +
                      "@" + 
                      std::to_string(word_s) +
                      "@" +
                      std::to_string(word_t);

    status = db->Put(leveldb::WriteOptions(), key, value);                   
    /*
     * Debuging
     */
    std::cout << "Write to db. Key: " << key << " Value: " << value << std::endl;
  }
};

#ifdef UNIT_TEST
int main(int argc, char** argv){

  dbManager dbmanager;
  double input = 0.2453453245;
  double output;
  dbmanager.putEl(0, 1, 6666, 7777, input);
  output = dbmanager.getEl(0, 1, 6666, 7777);

  assert(std::abs(input - output) < 0.001);

  std::cout << "Unit test pass." << std::endl;

  return 0;
}

#endif // UNIT_TEST
#endif // _DBMANAGER_H_
