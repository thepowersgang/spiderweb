# 
# SpiderWeb - Web targeted SpiderScript variant
#

OBJ = main.o fileio.o template.o
BIN = ../sw

DEPFILES=$(OBJ:%=%.dep)

CPPFLAGS += -I ..
CFLAGS  += -Wall -Werror -g
LDFLAGS += -L .. -lspiderscript -g

.PHONY: all clean install

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(DEPFILES)

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

%.o: %.c
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	@$(CC) $(CPPFLAGS) -M -MF $@.dep $<

