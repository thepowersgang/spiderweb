# 
# SpiderWeb - Web targeted SpiderScript variant
#

OBJ = main.o fileio.o
BIN = ../sw

DEPFILES=$(OBJ:%=%.dep)

CPPFLAGS = 
CFLAGS = -Wall -Werror
LDFLAGS = -lspiderscript

.PHONY: all clean install

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(DEPFILES)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	@$(CC) -M -MF $@.dep $<

