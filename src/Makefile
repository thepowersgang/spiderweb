# 
# SpiderWeb - Web targeted SpiderScript varian
#

OBJ = main.o
BIN = ../sw

DEPFILES=$(OBJ:%.o=%.d)

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
	@$(CC) -M -MF $*.d $<

