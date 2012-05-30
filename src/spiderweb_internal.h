
#ifndef SPIDERWEB_INTERNAL_H_
#define SPIDERWEB_INTERNAL_H_

#include <spiderscript.h>

#define SCRIPT_METHOD_EX(_rv, _scriptName, _ident, _args...)\
	tSpiderValue	*Script_##_ident(tSpiderScript *Script, int NArgs, tSpiderValue **Args);\
	tSpiderFunction	gScript_##_ident = {NULL,_scriptName,Script_##_ident,_rv,{_args}};\
	tSpiderValue	*Script_##_ident(tSpiderScript *Script, int NArgs, tSpiderValue **Args)
#define SCRIPT_METHOD(_scriptName, _ident, _args...)	SCRIPT_METHOD_EX(SS_DATATYPE_NOVALUE, _scriptName, _ident, _args)

extern void	SpiderWeb_AppendFunction(tSpiderFunction *Function);

#define g_fcn_NULL	*(int*)0
#define DEF_OBJ_FCN(sym, name, next, rettype, args...) \
	tSpiderValue	*sym(tSpiderScript *Script, int nParams, tSpiderValue **Parameters);\
	tSpiderFunction g_fcn_##sym = {(void*)&g_fcn_##next,name,sym,rettype,{args}};

#endif

