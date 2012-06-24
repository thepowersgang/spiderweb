# 
# SpiderWeb - Web targeted SpiderScript variant
#

OBJ := main.o fileio.gen.o
OBJ += template/load.o template/object.gen.o template/run.o template/common.o
OBJ += module_cgi.o
OBJ += mysql.gen.o
BIN = ../sw

EXPORT_FILES := $(filter %.gen.o,$(OBJ))
EXPORT_FILES := $(EXPORT_FILES:%.gen.o=%.ssf)

OBJ := $(OBJ:%=obj/%)

DEPFILES=$(OBJ:%=%.dep)

CPPFLAGS += -I .. -I .
CFLAGS  += -Wall -Werror -g
LDFLAGS += -L .. -lspiderscript -lmysqlclient -g

GENSFCNS := ../libspiderscript/tools/gen_scriptfcns.pl

.PHONY: all clean install

.PRECIOUS: mysql.gen.c template/object.gen.c

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(DEPFILES)
	$(RM) -r obj/

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

types.gen.h: $(EXPORT_FILES) $(GENSFCNS)
	perl $(GENSFCNS) -makeheader types.gen.h $(EXPORT_FILES)

%.gen.c: %.ssf types.gen.h $(GENSFCNS)
	perl $(GENSFCNS) -H types.gen.h -nc -p $(subst /,_,$*) $< $@

obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	@$(CC) $(CPPFLAGS) -M -MF $@.dep $<

-include $(DEPFILES)

