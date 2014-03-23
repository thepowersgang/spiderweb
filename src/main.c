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

const char	*gsEOL = "<br/>\n";
char	*gsScriptFile;
char	*gsCacheFile;
 int	gbCacheCompiled = 0;
enum eSpiderScript_TraceLevel	gTraceLevel = SS_TRACE_NONE;

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

	// Apply tracing level
	SpiderScript_SetTraceLevel(script, gTraceLevel);

	// Execute
	tSpiderTypeRef	rettype;
	tSpiderInteger	ret;
	tSpiderTypeRef	argt[] = {};
	const void	*args[] = {};
	rv = SpiderScript_ExecuteFunction(script, "", &rettype, &ret, 0, argt, args, NULL);
	CGI_SendHeadersOnce();
	if( rv < 0 )
	{
		const char *msg;
		CGI_SendHeadersOnce();
		switch( (rv = SpiderScript_GetException(script, &msg)) )
		{
		case SS_EXCEPTION_NONE:
			printf("BUG - _ExecuteFunction returned <0, but no exception%s",gsEOL);
			break;
		case SS_EXCEPTION_FORCEEXIT:
			// Script forced an exit, not an error
			break;
		default:
			printf("Unknown Exception %i: %s%s", rv, msg, gsEOL);
			break;
		}
		if( rv != SS_EXCEPTION_FORCEEXIT )
		{
			 int	n_bt;
			const tSpiderBacktrace *bt = SpiderScript_GetBacktrace(script, &n_bt);
			for(int i = 0; i < n_bt; i ++ ) {
				printf("#%i: %s+%zi %s:%i%s",
					i, bt[i].Function, bt[i].Offset, bt[i].File, bt[i].Line,
					gsEOL);
			}
		}
		SpiderScript_ClearException(script);
	}
	// TODO: Check if return type is sane
	
	SpiderScript_Free(script);
	if( gbCacheCompiled )
	{
		free(gsCacheFile);
	}
	
	return 0;
}

void PrintUsage(const char *progname)
{
	fprintf(stderr, "Usage: %s [<opts>] <file>\n", progname);
	fprintf(stderr,
		"Options:\n"
		"--[no-]cache : (En/Dis)able caching of bytecode. (Currently %sabled)\n"
		"--trace-bc   : Trace bytecode execution to stdout\n"
		,
		(gbCacheCompiled ? "en" : "dis")
		);
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
			fprintf(stderr, "Unknown option -%s\n", arg+1);
			PrintUsage(argv[0]);
			return 1;
		}
		else
		{
			// Long arguments
			if( strcmp(arg, "--cache") == 0 ) {
				gbCacheCompiled = 1;
			}
			else if( strcmp(arg, "--no-cache") == 0 ) {
				gbCacheCompiled = 0;
			}
			else if( strcmp(arg, "--trace-bc") == 0 ) {
				gTraceLevel = SS_TRACE_OPCODES;
			}
			else {
				// *shrug*
				fprintf(stderr, "Unknown option %s\n", arg);
				PrintUsage(argv[0]);
				return 1;
			}
		}
	}
	return 0;
}

void HandleErrorMessage(tSpiderScript *Script, const char *Message)
{
	CGI_SendHeadersOnce();
	// TODO: HTML Prettyness?
	printf("Runtime Error: %s<br/>\n", Message);
}
