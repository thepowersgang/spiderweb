# 
# SpiderWeb - Web targeted SpiderScript variant
#

OBJ := main.o
OBJ += template/load.o template/run.o template/common.o template/filter.o
OBJ += module_cgi.o
BIN = ../sw

USE_MYSQL ?= yes
USE_SQLITE ?= yes

EXPORT_FILES := misc.ssf template/object.ssf fileio.ssf cgi.ssf
EXPORT_FILES += json.ssf
EXPORT_FILES += sql.ssf

CPPFLAGS += -I .. -I .
CFLAGS  += -Wall -Werror -g -std=gnu99
LDFLAGS += -L .. -lspiderscript -lm -g

ifeq ($(USE_MYSQL),yes)
  EXPORT_FILES += mysql.ssf
  LDFLAGS += -lmysqlclient
#  CPPFLAGS += -DENABLE_MYSQL=1
endif
ifeq ($(USE_SQLITE),yes)
  EXPORT_FILES += sqlite.ssf
  LDFLAGS += -lsqlite3
  CPPFLAGS += -DENABLE_SQLITE=1
endif

EXPORT_FILES := $(EXPORT_FILES:%.gen.o=%.ssf)
OBJ += $(patsubst %.c,%.o,.exports/_index.c $(EXPORT_FILES:%.ssf=.exports/%.c))
OBJ := $(addprefix $(OBJDIR),$(OBJ))
OBJ := $(OBJ:%=obj/%)
DEPFILES := $(OBJ:%.o=%.d)

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

.exports/_index.c: $(EXPORT_FILES) types.gen.h $(GENSFCNS)
	@mkdir -p $(dir $@)
	perl $(GENSFCNS) -H types.gen.h --mkidx $@ $(EXPORT_FILES)

.exports/%.c: %.ssf types.gen.h $(GENSFCNS)
	@mkdir -p $(dir $@)
	perl $(GENSFCNS) -H types.gen.h $@ $<

obj/%.o: %.c
	@mkdir -p $(dir $@)
	$(CC) $(CFLAGS) $(CPPFLAGS) -o $@ -c $< -MMD -MP

-include $(DEPFILES)

