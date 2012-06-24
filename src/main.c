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
void	SpiderWeb_AppendClass(tSpiderClass *FirstClass);

// === GLOBALS ===
extern tSpiderClass	*gpfileio_FirstClass;
extern tSpiderClass	*gptemplate_object_FirstClass;
extern tSpiderClass	*gpmysql_FirstClass;
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
char	*gsCacheFile;
 int	gbCacheCompiled;

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
	// - Register functions
	SpiderWeb_AppendFunction(&gScript_IO_Print);
	SpiderWeb_AppendFunction(&gScript_IO_ReadLine);
	
	// - Register classes
	// > NOTE: Order matters, they must be in the opposite order to the makefile order
	// TODO: Remove the above restriction
	SpiderWeb_AppendClass(gpmysql_FirstClass);
	SpiderWeb_AppendClass(gptemplate_object_FirstClass);
	SpiderWeb_AppendClass(gpfileio_FirstClass);

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
	SpiderScript_ExecuteFunction(script, "", &ret, 0, argt, args, NULL);
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

void SpiderWeb_AppendClass(tSpiderClass *FirstClass)
{
	tSpiderClass	*e, *p = NULL;
	for( e = gScriptVariant.Classes; e; p = e, e = e->Next )
	{
		if( e == FirstClass )	return ;	// TODO: Flag a bug report
	}
	if( p )
		p->Next = FirstClass;
	else
		gScriptVariant.Classes = FirstClass;
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

