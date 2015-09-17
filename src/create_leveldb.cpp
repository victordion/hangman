#include <iostream>
#include <cassert>
#include <string>
#include "leveldb/db.h"

using namespace std;

const int num_kb = 96;

/*
int main() {
  leveldb::DB* db[num_kb];
  leveldb::Options options;
  options.create_if_missing = true;
	

  for (int i = 0; i < num_kb; ++i) {
		std::string path = "/tmp/leveldb_kb" + to_string(i);
  	leveldb::Status status = leveldb::DB::Open(options, path, &db[i]);
  	assert(status.ok());
	}


  return 0;
}
*/
