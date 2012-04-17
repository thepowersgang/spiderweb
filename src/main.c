/*
 * SpiderWeb scripting suite
 * - Interpreter core wrapper
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spiderweb_internal.h"

// === MACROS ===
#define ARRAY_SIZE(x)	((sizeof(x))/(sizeof((x)[0])))

// === PROTOTYPES ===
void	Script_int_System_IO_DestroyPtr(void *Data);

// === GLOBALS ===
extern tSpiderObjectDef	g_obj_IO_File;
extern tSpiderObjectDef	g_obj_Template;
extern tSpiderFunction	gScript_IO_Print;
extern tSpiderFunction	gScript_IO_ReadLine;

tSpiderNamespace	g_ns_IO = {
	NULL,
	NULL,	// Children
	NULL,	// Functions
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

	// TODO: Parse CGI values?
	// - Maybe delay until CGI::ReadGET,CGI::ReadPOST are used	

	// Prepare script engine
	// - Register functions
	SpiderWeb_AppendFunction(&g_ns_IO.Functions, &gScript_IO_Print);
	SpiderWeb_AppendFunction(&g_ns_IO.Functions, &gScript_IO_ReadLine);
	
	// Parse Script file
	script = SpiderScript_ParseFile(&gScriptVariant, gsScriptFile);
	if( script == NULL ) {
		fprintf(stderr, "ERROR: '%s' failed to parse\n", gsScriptFile);
		return -1;
	}
	
	// Execute
	ret = SpiderScript_ExecuteFunction(script, "", NULL, 0, NULL, NULL);
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
	
	// TODO: Send headers
//	CGI_SendHeadersOnce();

	fwrite(Args[0]->String.Data, Args[0]->String.Length, 1, stdout);
	
	return NULL;
}

SCRIPT_METHOD("ReadLine", IO_ReadLine, 0)
{
	tSpiderValue	*ret = NULL;
	tSpiderValue	*tv;
	char	tmpbuf[sizeof(*tv)+BUFSIZ];

	tv = (void*)tmpbuf;
	tv->Type = SS_DATATYPE_STRING;
	tv->String.Data[0] = '\0';
	tv->String.Length = 1;	

	while( tv->String.Data[tv->String.Length-1] != '\n' )
	{
		fgets(tv->String.Data, BUFSIZ, stdin);

		tv->String.Length = strlen(tv->String.Data);

		if(ret) {
			tSpiderValue	*new;
			new = SpiderScript_StringConcat(ret, tv);
			SpiderScript_FreeValue(ret);
			ret = new;
		}
		else
			ret = SpiderScript_CreateString(tv->String.Length, tv->String.Data);
	}
	
	return ret;
}

SCRIPT_METHOD("ScanF", IO_ScanF, SS_DATATYPE_STRING, 0)
{
	// TODO: Implement
	return NULL;
}

/*
tSpiderFunction	gSystem_IO_Open = {NULL,"Sys.IO.Open", System_IO_Open, {SS_DATATYPE_STRING,0}};
tSpiderFunction	gSystem_IO_IOCtl = {NULL,"Sys.IO.IOCtl", System_IO_IOCtl, {SS_DATATYPE_OPAQUE,SS_DATATYPE_INTEGER,SS_DATATYPE_STRING,0}};
tSpiderFunction	gSystem_IO_Close = {NULL,"Sys.IO.Close", System_IO_Close, {SS_DATATYPE_OPAQUE,0}};
*/

