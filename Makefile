CC=g++

CFLAGS=-O3 -std=c++11 -I$(IDIR)

LDFLAGS=-lleveldb 

SRCDIR=./src
BINDIR=./bin
IDIR=./include
OBJDIR=./obj
LIBDIR=./lib
LIBS=-lm -lleveldb

#SOURCES=machine_algorithm_v2.cpp multi_recall_word.cpp read_KB_word.cpp sentence_automatic_recall.cpp sentence_multi_recall.cpp sentence_read_KB.cpp hangman_score_pvm.cpp
SOURCES=$(SRCDIR)/*

OBJECTS=$(OBJDIR)/*

EXECUTABLE=play_against_machine

.PHONY: all
all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE) : $(OBJECTS)
	$(CC) $(OBJECTS) -o $@ $(LDFLAGS) 

%(OBJDIR)/%.o : %(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $< -o $@

create_leveldb: ./obj/create_leveldb.o
	$(CC) $^ -o $@ $(LDFLAGS)

./obj/create_leveldb.o: $(SRCDIR)/create_leveldb.cpp
	$(CC) -c $(CFLAGS) $< -o $@

.PHONY: clean

clean:
	rm $(BINDIR)/*
	rm $(LIBDIR)/*
	rm $(OBJDIR)/*

