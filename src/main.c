/*
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spiderscript.h>

// === MACROS ===
#define ARRAY_SIZE(x)	((sizeof(x))/(sizeof((x)[0])))

#define SCRIPT_METHOD(_scriptName, _ident, _args...)\
	tSpiderValue	*Script_##_ident(tSpiderScript *Script, int NArgs, tSpiderValue **Args);\
	tSpiderFunction	gScript_##_ident = {NULL,_scriptName,Script_##_ident,{_args}};\
	tSpiderValue	*Script_##_ident(tSpiderScript *Script, int NArgs, tSpiderValue **Args)

// === PROTOTYPES ===
void	Script_int_System_IO_DestroyPtr(void *Data);
void	SpiderWeb_AppendFunction(tSpiderFunction **ListHead, tSpiderFunction *Function);

// === GLOBALS ===
extern tSpiderObjectDef	g_obj_IO_File;
extern tSpiderObjectDef	g_obj_Template;
extern tSpiderFunction	gScript_IO_Print;

tSpiderNamespace	g_ns_IO = {
	NULL,
	NULL,	// Children
	&gScript_IO_Print,	// Functions
	&g_obj_IO_File,	// Classes
	0, NULL,
	"IO"
	};

tSpiderVariant	gScriptVariant = {
	"SpiderWeb",
	0, 1,	// Static Typed, Implicit casts allowed
	NULL,	// Global (namespaceless) Functions
	0, NULL,	// Global (namespaceless) Constants
	{	// Root Namespace (.)
		.FirstChild = &g_ns_IO,
		.Classes = &g_obj_Template
	}
};

char	*gsScriptFile;

/**
 * \brief Program Entrypoint
 */
int main(int argc, char *argv[])
{
	tSpiderScript	*script;
	tSpiderValue	*ret;
	
	// TODO: Argument handling
	gsScriptFile = argv[1];
	
	// Register functions
	SpiderWeb_AppendFunction(&g_ns_IO.Functions, &gScript_IO_Print);
	
	// Parse file
	script = SpiderScript_ParseFile(&gScriptVariant, gsScriptFile);
	if( script == NULL ) {
		fprintf(stderr, "ERROR: '%s' failed to parse\n", gsScriptFile);
		return -1;
	}
	
	ret = SpiderScript_ExecuteFunction(script, NULL, "", 0, NULL);
	{
		char	*valStr = SpiderScript_DumpValue(ret);
		printf("\nmain: ret = %s\n", valStr);
		free(valStr);
	}
	SpiderScript_FreeValue(ret);
	SpiderScript_Free(script);
	
	return 0;
}

void SpiderWeb_AppendFunction(tSpiderFunction **ListHead, tSpiderFunction *Function)
{
	tSpiderFunction	*fcn, *prev = NULL;
	
	if( ListHead == NULL ) {
		ListHead = &gScriptVariant.Functions;
	}
	
	// Search to remove duplication
	for( fcn = *ListHead; fcn; prev = fcn, fcn = fcn->Next )
	{
		if(strcmp(Function->Name, fcn->Name) == 0) {
			// Error?
			return ;
		}
	}
	
	// Assumes that ->Next is the first
	if( prev )
		prev->Next = Function;
	else
		*ListHead = Function;
	Function->Next = NULL;
}

/**
 * \brief Sets a template parameter
 */
SCRIPT_METHOD("Print", IO_Print, SS_DATATYPE_STRING, 0)
{
	if(NArgs < 1)	return ERRPTR;
	if(!Args[0] || Args[0]->Type != SS_DATATYPE_STRING)	return ERRPTR;
	
	fwrite(Args[0]->String.Data, Args[0]->String.Length, 1, stdout);
	
	return NULL;
}

/*
tSpiderFunction	gSystem_IO_Open = {NULL,"Sys.IO.Open", System_IO_Open, {SS_DATATYPE_STRING,0}};
tSpiderFunction	gSystem_IO_IOCtl = {NULL,"Sys.IO.IOCtl", System_IO_IOCtl, {SS_DATATYPE_OPAQUE,SS_DATATYPE_INTEGER,SS_DATATYPE_STRING,0}};
tSpiderFunction	gSystem_IO_Close = {NULL,"Sys.IO.Close", System_IO_Close, {SS_DATATYPE_OPAQUE,0}};
*/

