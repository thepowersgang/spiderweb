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
extern tSpiderClass	g_obj_IO_File;
extern tSpiderClass	g_obj_Template;
extern tSpiderFunction	gScript_IO_Print;
extern tSpiderFunction	gScript_IO_ReadLine;

tSpiderVariant	gScriptVariant = {
	"SpiderWeb",
	1,	// Implicit casts allowed
	NULL,	// Functions
	NULL,	// Classes
	NULL,	// ReadConstant
	0, {},	// Global (namespaceless) Constants
};

char	*gsScriptFile;

/**
 * \brief Program Entrypoint
 */
int main(int argc, char *argv[], char **envp)
{
	tSpiderScript	*script = NULL;

	// TODO: Argument handling
	gsScriptFile = argv[1];

	Module_CGI_Initialise();

	// TODO: Parse CGI values?
	// - Maybe delay until CGI::ReadGET,CGI::ReadPOST are used	

	// Prepare script engine
	// - Register functions
	SpiderWeb_AppendFunction(&gScript_IO_Print);
	SpiderWeb_AppendFunction(&gScript_IO_ReadLine);
	
	// - Register classes (less elegant)
	gScriptVariant.Classes = &g_obj_IO_File;
	g_obj_IO_File.Next = &g_obj_Template;
	g_obj_Template.Next = NULL;
	
	// Parse Script file
	script = SpiderScript_ParseFile(&gScriptVariant, gsScriptFile);
	if( script == NULL ) {
		fprintf(stderr, "ERROR: '%s' failed to parse\n", gsScriptFile);
		return -1;
	}
	
	// Execute
	tSpiderInteger	ret;
	 int	argt[] = {};
	const void	*args[] = {};
	SpiderScript_ExecuteFunction(script, "", &ret, 0, argt, args, NULL);
	SpiderScript_Free(script);
	
	return 0;
}

void SpiderWeb_AppendFunction(tSpiderFunction *Function)
{
	tSpiderFunction	*fcn, *prev = NULL;
	tSpiderFunction	**ListHead = NULL;
	
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
SCRIPT_METHOD("IO@Print", IO_Print, SS_DATATYPE_STRING, 0)
{
	if(NArgs != 1) {
		fprintf(stderr, "IO@Print - NArgs bad %i != 1\n", NArgs);
		return -1;
	}
	if(!Args[0] || ArgTypes[0] != SS_DATATYPE_STRING) {
		fprintf(stderr, "IO@Print - Args[0] 0x%x %p not SS_DATATYPE_STRING\n",
			ArgTypes[0], Args[0]);
		return -1;
	}
	
	// Send headers
	CGI_SendHeadersOnce();

	const tSpiderString	*s = Args[0];
	fwrite(s->Data, s->Length, 1, stdout);
	
	return 0;
}

SCRIPT_METHOD_EX(SS_DATATYPE_STRING, "IO@ReadLine", IO_ReadLine, 0)
{
	tSpiderString	*ret = NULL;
	tSpiderString	*tv;
	char	tmpbuf[sizeof(*tv)+BUFSIZ];

	tv = (void*)tmpbuf;
	tv->Data[0] = '\0';
	tv->Length = 0;

	while( tv->Data[tv->Length-1] != '\n' )
	{
		fgets(tv->Data, BUFSIZ, stdin);

		tv->Length = strlen(tv->Data);

		if(ret) {
			tSpiderString	*new;
			new = SpiderScript_StringConcat(ret, tv);
			SpiderScript_DereferenceString(ret);
			ret = new;
		}
		else
			ret = SpiderScript_CreateString(tv->Length, tv->Data);
	}
	
	*(tSpiderString**)RetData = ret;
	return SS_DATATYPE_STRING;
}

SCRIPT_METHOD("IO@ScanF", IO_ScanF, SS_DATATYPE_STRING, 0)
{
	// TODO: Implement
	return -1;
}

/*
tSpiderFunction	gSystem_IO_Open = {NULL,"Sys.IO.Open", System_IO_Open, {SS_DATATYPE_STRING,0}};
tSpiderFunction	gSystem_IO_IOCtl = {NULL,"Sys.IO.IOCtl", System_IO_IOCtl, {SS_DATATYPE_OPAQUE,SS_DATATYPE_INTEGER,SS_DATATYPE_STRING,0}};
tSpiderFunction	gSystem_IO_Close = {NULL,"Sys.IO.Close", System_IO_Close, {SS_DATATYPE_OPAQUE,0}};
*/

