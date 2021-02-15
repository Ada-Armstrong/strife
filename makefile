IDIR=include
SDIR=src
ODIR=$(SDIR)/obj
EXE=strife

CC=gcc
CFLAGS=-I$(IDIR) -g -Wall -Werror -DDEBUG

LIBS=-lm -lreadline

_DEPS = board.h bot.h generate.h piece.h player.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = board.o bot.o generate.o piece.o player.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(ODIR)/main.o $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

TDIR=tests

$(ODIR)/generate_unit_test.o: $(TDIR)/generate_unit_test.c $(DEPS) 
		$(CC) -c -o $@ $< $(CFLAGS)
	
gtest: $(ODIR)/generate_unit_test.o $(OBJ)
		$(CC) -o $(TDIR)/$@ $^ $(CFLAGS) $(LIBS)


.PHONY: clean 

clean:
		rm -f $(ODIR)/*.o $(EXE) $(TDIR)/gtest

