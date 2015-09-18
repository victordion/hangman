CC=g++

CFLAGS=-g -O3 -std=c++11 -I$(IDIR)

LDFLAGS=-lleveldb 

SRCDIR=./test
BINDIR=./bin
IDIR=./include
OBJDIR=./obj
LIBDIR=./lib
LIBS=-lm -lleveldb

#SOURCES = machine_algorithm_v2.cpp multi_recall_word.cpp read_KB_word.cpp sentence_automatic_recall.cpp sentence_multi_recall.cpp sentence_read_KB.cpp hangman_score_pvm.cpp

SOURCES = ./test/local_game.cpp
DEPS :=$(wildcard $(IDIR)/*.h)

#SOURCES :=$(addprefix ./src/,$(SOURCES))

#SOURCES :=$(wildcard $(SRCDIR)/*.cpp)

OBJECTS :=$(addprefix $(OBJDIR)/,$(notdir $(SOURCES:.cpp=.o)))

BINARY = local_game

.PHONY: show
show:
	@echo "Source files are:"
	@echo $(SOURCES)
	@echo "Object files are:"
	@echo $(OBJECTS)
	@echo "Dep files are:"
	@echo $(DEPS)

.PHONY: all
all: $(BINARY)

$(BINARY) : $(OBJECTS)
	$(CC) $(CFLAGS) $(LDFLAGS) $(OBJECTS) -o $(BINARY) 

$(OBJDIR)/%.o : $(SRCDIR)/%.cpp $(DEPS)
	$(CC) $(CFLAGS) $(LDFLAGS) -c $< -o $@

.PHONY: clean
clean:
	rm $(OBJDIR)/*
	rm *.txt

