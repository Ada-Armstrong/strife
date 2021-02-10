IDIR=include
SDIR=src
ODIR=$(SDIR)/obj
EXE=strife

CC=gcc
CFLAGS=-I$(IDIR)

LIBS=-lm -lreadline

_DEPS = board.h bot.h generate.h piece.h player.h
DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = main.o board.o bot.o generate.o piece.o player.o
OBJ = $(patsubst %,$(ODIR)/%,$(_OBJ))


$(ODIR)/%.o: $(SDIR)/%.c $(DEPS)
		$(CC) -c -o $@ $< $(CFLAGS)

$(EXE): $(OBJ)
		$(CC) -o $@ $^ $(CFLAGS) $(LIBS)

.PHONY: clean

clean:
		rm -f $(ODIR)/*.o $(EXE)
