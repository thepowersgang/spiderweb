/*
 * SpiderWeb scripting suite
 * - Interpreter core wrapper
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "spiderweb_internal.h"
#include <sys/stat.h>

// === MACROS ===
#define ARRAY_SIZE(x)	((sizeof(x))/(sizeof((x)[0])))

// === PROTOTYPES ===
 int	ParseArguments(int argc, char *argv[]);

// === GLOBALS ===
extern tSpiderClass	g_obj_IO_File;
extern tSpiderClass	g_obj_Template;
extern tSpiderFunction	gScript_IO_Print;
extern tSpiderFunction	gScript_IO_ReadLine;

tSpiderVariant	gScriptVariant = {
	"SpiderWeb",
	0, 1,	// Static Typed, Implicit casts allowed
	NULL,	// Functions
	NULL,	// Classes
	NULL,	// ReadConstant
	0, {},	// Global (namespaceless) Constants
};

char	*gsScriptFile;
char	*gsCacheFile;
 int	gbCacheCompiled;

/**
 * \brief Program Entrypoint
 */
int main(int argc, char *argv[])
{
	 int	rv;
	tSpiderScript	*script = NULL;
	tSpiderValue	*ret;
	
	rv = ParseArguments(argc, argv);
	if( rv )	return rv;

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

	if( gbCacheCompiled )
	{
		struct stat cache_info;
		struct stat source_info;

		gsCacheFile = malloc( strlen(gsScriptFile) + sizeof(".csw") + 1 );
		strcpy(gsCacheFile, gsScriptFile);
		strcat(gsCacheFile, ".csw");

		if( stat(gsScriptFile, &source_info) ) {
			fprintf(stderr, "Unable to stat(2) '%s'\n", gsScriptFile);
			perror("Opening source file");
			return -1;
		}
		
		if( stat(gsCacheFile, &cache_info) == 0 )
		{
			if( cache_info.st_mtime > source_info.st_mtime ) {
				script = SpiderScript_LoadBytecode(&gScriptVariant, gsCacheFile);
				if( !script ) {
					fprintf(stderr, "Loading bytecode from '%s' failed\n", gsCacheFile);
				}
			}
		}
	}
	
	if( !script )
	{
		// Parse Script file
		script = SpiderScript_ParseFile(&gScriptVariant, gsScriptFile);
		if( script == NULL ) {
			fprintf(stderr, "ERROR: '%s' failed to parse\n", gsScriptFile);
			return -1;
		}
	
		// Cache compiled version
		if( gbCacheCompiled )
		{
			SpiderScript_SaveBytecode(script, gsCacheFile);
		}
	}

	// Execute
	ret = SpiderScript_ExecuteFunction(script, "", NULL, 0, NULL, NULL, 1);
	SpiderScript_FreeValue(ret);
	SpiderScript_Free(script);
	
	return 0;
}

int ParseArguments(int argc, char *argv[])
{
	 int	i;
	for( i = 1; i < argc; i ++ )
	{
		char *arg = argv[i];
		if( arg[0] != '-' )
		{
			gsScriptFile = arg;
		}
		else if( arg[1] != '-' )
		{
			// Short arguments
		}
		else
		{
			// Long arguments
			if( strcmp(arg, "--cache") == 0 ) {
				gbCacheCompiled = 1;
			}
			else {
				// *shrug*
			}
		}
	}
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
//	printf("NArgs = %i\n", NArgs);
	if(NArgs < 1)	return ERRPTR;
//	printf("Args[0] = Type%i\n", Args[0] ? Args[0]->Type : 0);
	if(!Args[0] || Args[0]->Type != SS_DATATYPE_STRING)	return ERRPTR;
	
	// TODO: Send headers
//	CGI_SendHeadersOnce();

	fwrite(Args[0]->String.Data, Args[0]->String.Length, 1, stdout);
	
	return NULL;
}

SCRIPT_METHOD_EX(SS_DATATYPE_STRING, "IO@ReadLine", IO_ReadLine, 0)
{
	tSpiderValue	*ret = NULL;
	tSpiderValue	*tv;
	char	tmpbuf[sizeof(*tv)+BUFSIZ];

	tv = (void*)tmpbuf;
	tv->Type = SS_DATATYPE_STRING;
	tv->String.Data[0] = '\0';
	tv->String.Length = 0;

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

SCRIPT_METHOD("IO@ScanF", IO_ScanF, SS_DATATYPE_STRING, 0)
{
	// TODO: Implement
	return NULL;
}

