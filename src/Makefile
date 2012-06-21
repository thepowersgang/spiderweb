# 
# SpiderWeb - Web targeted SpiderScript variant
#

OBJ = main.o fileio.o
OBJ+= template/load.o template/object.o template/run.o template/common.o
OBJ+= module_cgi.o
BIN = ../sw
OBJ := $(OBJ:%=obj/%)

DEPFILES=$(OBJ:%=%.dep)

CPPFLAGS += -I ..
CFLAGS  += -Wall -Werror -g
LDFLAGS += -L .. -lspiderscript -g

.PHONY: all clean install

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(DEPFILES)
	$(RM) -r obj/

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	@$(CC) $(CPPFLAGS) -M -MF $@.dep $<

-include $(DEPFILES)

