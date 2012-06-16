/*
 * SpiderWeb Scripting Language
 * - By John Hodge (thePowersGang)
 */
#ifndef SPIDERWEB_INTERNAL_H_
#define SPIDERWEB_INTERNAL_H_

#include <spiderscript.h>

#define FCN_PROTO(sym) int	sym(tSpiderScript *Script,void *RetData,int NArgs,const int*ArgTypes,const void*const Args[])
#define FCN_DESC(next,sym,fsym,name,_rv,_args...)	tSpiderFunction sym = {next,name,fsym,_rv,{_args}}

#define SCRIPT_METHOD_EX(_rv, _scriptName, _ident, _args...)\
	FCN_PROTO(Script_##_ident); \
	FCN_DESC(NULL, gScript_##_ident, Script_##_ident, _scriptName, _rv, _args); \
	FCN_PROTO(Script_##_ident)
#define SCRIPT_METHOD(_scriptName, _ident, _args...)	SCRIPT_METHOD_EX(SS_DATATYPE_NOVALUE, _scriptName, _ident, _args)

extern void	SpiderWeb_AppendFunction(tSpiderFunction *Function);

#define g_fcn_NULL	*(int*)0
#define DEF_OBJ_FCN(sym, name, next, rettype, args...) \
	FCN_PROTO(sym);\
	FCN_DESC((void*)&g_fcn_##next,g_fcn_##sym,sym,name,rettype,args);


extern void	CGI_SendHeadersOnce(void);

#endif

