# ConfabHangman

## Introduction
An interactive sentence guessing program based on RESTful web services. The purpose of this project is to demonstrate the functionality of sentence confabulation logic that is developed at Syracuse University, and let human users to play with it in friendly user interface.
* The backend (server side) code is written in C++, based on proxygen, a light weight HTTP server library.
* The frontend (client side) code is HTML + CSS + AngularJS.

## Installation and usages

**Frontend:**
User interface is run through a browser (preferably google-chrome), and you can find related files in `./client`.
Just open `index.html` and you are done. Note that the default server address is at `http://128.230.213.57:11000`, if you run the server from a different address, you need to modify the `hostAddress` variable at `./client/game.js`.


**Backend**:
Server side source code resides in `./server/`, to build the server binary, just `cd ./server/` and run `./build`, then you will be able to run the binary`HangmanServer` located in `./server/bin/`.

**Dependencies**:
You will need to install the following dependencies in order to run the server
* Facebool folly
* Facebook proxygen
* Google glog
* Google glag


