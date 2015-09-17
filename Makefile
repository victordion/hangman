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
#SOURCES :=$(shell find $(SRCDIR) -name '*.cpp')
SOURCES :=$(wildcard $(SRCDIR)/*.cpp)
OBJECTS :=$(addprefix $(OBJDIR)/,$(notdir $(SOURCES:.cpp=.o)))

BINARY = play

show:
	@echo "Source files are:"
	@echo $(SOURCES)
	@echo "Object files are:"
	@echo $(OBJECTS)
.PHONY: all
all: $(BINARY)

$(BINARY) : $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BINARY) 

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@


.PHONY: clean

clean:
	rm $(BINDIR)/*
	rm $(LIBDIR)/*
	rm $(OBJDIR)/*

