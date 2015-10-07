#ifndef _DBMANAGER_H_
#define _DBMANAGER_H_
#include <cmath>
#include <iostream>
#include "leveldb/db.h"
#include <sstream>
#include <string>
#include <cassert>
#include <fstream>
#include <unordered_set>
#include <vector>
using namespace std;

struct PairHash {
  public:
    template <typename T, typename U>
      std::size_t operator()(const std::pair<T, U> &x) const
      {
        return std::hash<T>()(x.first) ^ std::hash<U>()(x.second);
      }
};

/*
 This leveldb stores three types of key value relations

 1. Example: "word@12345" -> "beautiful place"
    It means that word "beautiful place" is encoded as 12345

 2. Example: "lexpair2fileid@5@10" -> "3"
    It means that relations of lexicon 5 to lexicon 10 is in the file "KB_3.KB" 
 
 3. Example: "filewithel@5@1111@2222" -> "-0.213434"
    It means that in the file "KB_5.KB", Word 1111 to Word 2222 has a weight of -0.213434 
*/

class dbManager {
  private:
    leveldb::DB *db;
    leveldb::Options options;
    std::string path;
    leveldb::Status status;

  public:
    dbManager(std::string p = "/tmp/leveldb") : path(p) {
      options.create_if_missing = true;
      leveldb::Status status = leveldb::DB::Open(options, path, &db);
    }

    ~dbManager() {
      delete db;
    }

    std::vector<std::string> &split(const std::string &s, char delim, std::vector<std::string> &elems) {
      std::stringstream ss(s);
      std::string item;
      while (std::getline(ss, item, delim)) {
        elems.push_back(item);
      }
      return elems;
    }

    std::vector<std::string> split(const std::string &s, char delim) {
      std::vector<std::string> elems;
      split(s, delim, elems);
      return elems;
    }

    /*
     * Read the word dictionary file and store into leveldb, note that a word here can mean a word-gram,
     * instead of just one word
     */
    void createWordDict(std::string file) {
      std::ifstream input(file);
      std::string line;
      while(std::getline(input, line)) {
        std::vector<std::string> tokens = split(line, ' ');
        std::string key = "word@" + tokens[0];
        std::string value;
        for(size_t i = 1; i < tokens.size(); ++i) {
          value += " " + tokens[i];
        }

        /*
         * There is a data entry in the dictionary file where value is empty
         */
        if(value.length() > 0) value = value.substr(1);

        status = db->Put(leveldb::WriteOptions(), key , value);
        /*
         * Debuging
         */
        std::cout << "Write to db. Key: " << key << " Value: " << value << std::endl;
      } 
    }

    /*
     * Create the excitation connection for words pairs, among all possible lexicon pairs
     */
    void createExConn(std::string lex_table_path, std::string kb_dir) {
      std::ifstream lex_table_input(lex_table_path);
      string lex_t, lex_s, file_id;
      std::unordered_set<std::string> file_ids;

      while(lex_table_input >> lex_s >> lex_t >> file_id) {
        if(file_id != "-1") file_ids.insert(file_id);
        
        std::string key = "lexpair2fileid@" + lex_s + "@" + lex_t;
        std::string value = file_id;
        status = db->Put(leveldb::WriteOptions(), key , value);
        /*
         * Debuging
         */
        std::cout << "Write to db. Key: " << key << " Value: " << value << std::endl;
      }

      for(const string & file_id : file_ids) {
        std::string kb_file = kb_dir + "KB_" + file_id + ".KB";
        std::ifstream rels(kb_file);
        #ifdef DEBUG
        std::cout << "Processing file: " << kb_file << std::endl;
        #endif

        std::string line, word_s, word_t, el;
        while(std::getline(rels, line)) {
          std::vector<std::string> tokens = split(line, ' ');
          if(tokens[0] == "size") continue;
          if(tokens[0] == "s") {
            word_s = tokens[1];
            continue;
          }
          else {
            word_t = tokens[0];
            el = tokens[1];
            std::string key = "filewithel@" + file_id + "@" + word_s + "@" +  word_t;
            std::string value = el;

            status = db->Put(leveldb::WriteOptions(), key , value);
            /*
             * Debuging
             */
            //std::cout << "Write to db. Key: " << key << " Value: " << value << std::endl;
          }
        }
      }
    }

    /*
     * Get the excitation level
     */
    double getEl(int lex_s, int lex_t, int word_s, int word_t) {
      std::string file_id;
      status = db->Get(leveldb::ReadOptions(), "lexpair2fileid@" + std::to_string(lex_s) + "@" + std::to_string(lex_t), &file_id);
      
      std::string el_str;
      std::string key = "filewithel@" + file_id + "@" + std::to_string(word_s) + "@" + std::to_string(word_t);
      status = db->Get(leveldb::ReadOptions(), key, &el_str);
      
      /*
       * Debuging
       */
      std::cout << "Read db. lex_s: " << lex_s << " lex_t: " << lex_t << " word_s: " << word_s << " word_t " << word_t << " Weight: " << el_str << std::endl;
      return std::stod(el_str);
    }
    
    /*
     * Iterate all key value pairs in leveldb
     */ 
    void printAll() {
      leveldb::Iterator* it = db->NewIterator(leveldb::ReadOptions());
      for (it->SeekToFirst(); it->Valid(); it->Next()) {
        cout << it->key().ToString() << ": "  << it->value().ToString() << endl;
      }
      assert(it->status().ok());  // Check for any errors found during the scan
      delete it;
    }
};

#ifdef UNIT_TEST
int main(int argc, char** argv){

  dbManager dbmanager;
  dbmanager.printAll();

  /*
   * Run createWordDict and createExConn once to build the leveldb database.
   */
  //dbmanager.createWordDict("../KB_large/dictionary.KB");
  //dbmanager.createExConn("../KB_large/table.KB", "../KB_large/");

  std::cout << "Unit test pass." << std::endl;

  return 0;
}

#endif // UNIT_TEST
#endif // _DBMANAGER_H_
