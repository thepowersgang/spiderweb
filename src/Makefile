# 
# SpiderWeb - Web targeted SpiderScript variant
#

OBJ := main.o
OBJ += template/load.o template/run.o template/common.o
OBJ += module_cgi.o
BIN = ../sw

EXPORT_FILES := misc.ssf mysql.ssf template/object.ssf fileio.ssf cgi.ssf
EXPORT_FILES := $(EXPORT_FILES:%.gen.o=%.ssf)

OBJ += $(patsubst %.c,%.o,.exports/_index.c $(EXPORT_FILES:%.ssf=.exports/%.c))
OBJ := $(addprefix $(OBJDIR),$(OBJ))
OBJ := $(OBJ:%=obj/%)
DEPFILES=$(OBJ:%=%.dep)

CPPFLAGS += -I .. -I .
CFLAGS  += -Wall -Werror -g
LDFLAGS += -L .. -lspiderscript -lmysqlclient -g

GENSFCNS := ../libspiderscript/tools/gen_scriptfcns.pl

.PHONY: all clean install

.PRECIOUS: $(EXPORT_FILES:%.ssf=.exports/%.c)) exports/_index.c

all: $(BIN)

clean:
	$(RM) $(BIN) $(OBJ) $(DEPFILES)
	$(RM) -r obj/ .exports/

$(BIN): $(OBJ)
	$(CC) -o $(BIN) $(OBJ) $(LDFLAGS)

types.gen.h: $(EXPORT_FILES) $(GENSFCNS)
	@mkdir -p $(dir $@)
	perl $(GENSFCNS) --mkhdr $@ $(EXPORT_FILES)

.exports/_index.c: $(EXPORT_FILES) $(GENSFCNS)
	@mkdir -p $(dir $@)
	perl $(GENSFCNS) -H types.gen.h --mkidx $@ $(EXPORT_FILES)

.exports/%.c: %.ssf types.gen.h $(GENSFCNS)
	@mkdir -p $(dir $@)
	perl $(GENSFCNS) -H types.gen.h $@ $<

obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $<
	@$(CC) $(CPPFLAGS) -M -MF $@.dep $<

-include $(DEPFILES)

