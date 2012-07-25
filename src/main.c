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
 int	main(int argc, char *argv[], char **envp);
 int	ParseArguments(int argc, char *argv[]);
void	HandleErrorMessage(tSpiderScript *Script, const char *Message);


// === GLOBALS ===
extern int	giNumExportedClasses;
extern tSpiderClass	*gapExportedClasses;
extern int	giNumExportedFunctions;
extern tSpiderFunction	*gapExportedFunctions;

tSpiderVariant	gScriptVariant = {
	"SpiderWeb",
	1,	// Implicit casts allowed
	HandleErrorMessage,
	0, &gapExportedFunctions,	// Functions - count filled in main()
	0, &gapExportedClasses, 	// Classes - same again
	NULL,	// ReadConstant
	0, {},	// Global (namespaceless) Constants
};

char	*gsScriptFile;
char	*gsCacheFile;
 int	gbCacheCompiled;

// === CODE ===
/**
 * \brief Program Entrypoint
 */
int main(int argc, char *argv[], char **envp)
{
	 int	rv;
	tSpiderScript	*script = NULL;

	rv = ParseArguments(argc, argv);
	if( rv )	return rv;

	// Parse CGI values
	// - Maybe delay until CGI::ReadGET,CGI::ReadPOST are used	
	Module_CGI_Initialise();


	// Prepare script engine
	gScriptVariant.nFunctions = giNumExportedFunctions;
	gScriptVariant.nClasses   = giNumExportedClasses;

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
	tSpiderInteger	ret;
	 int	argt[] = {};
	const void	*args[] = {};
	rv = SpiderScript_ExecuteFunction(script, "", &ret, 0, argt, args, NULL);
	CGI_SendHeadersOnce();
	if( rv < 0 )
	{
		const char *msg;
		CGI_SendHeadersOnce();
		switch( (rv = SpiderScript_GetException(script, &msg)) )
		{
		case SS_EXCEPTION_NONE:
			printf("BUG - _ExecuteFunction returned <0, but no exception\n");
			break;
		default:
			printf("Unknown Exception %i: %s\n", rv, msg);
			break;
		}
		SpiderScript_ClearException(script);
	}
	
	SpiderScript_Free(script);
	if( gbCacheCompiled )
	{
		free(gsCacheFile);
	}
	
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

void HandleErrorMessage(tSpiderScript *Script, const char *Message)
{
	CGI_SendHeadersOnce();
	// TODO: HTML Prettyness?
	printf("Runtime Error: %s\n", Message);
}
