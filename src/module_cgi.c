/*
 * SpiderWeb Scripting Suite
 * 
 * module_cgi.c
 * - CGI Interface code
 */
#include "spiderweb_internal.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef struct sCGI_Param
{
	const char	*Name;
	 int	DataLen;
	const char	*Data;
} tCGI_Param;

// === GLOBALS ===
 int	gbCGI_IsInvokedAsCGI = 0;
char	*gsCGI_GETData;
 int	giCGI_GETNArgs;
tCGI_Param	*gaCGI_GETArgs;

// === CODE ===
int CGI_int_UnURLEncode(const char *Source, int SourceLen, char *Dest)
{
	 int	outlen = 0;
	 int	i;
	
	for( i = 0; i < SourceLen; i ++ )
	{
		char	ch;
		
		if( Source[i] != '%' ) {
			ch = Source[i];
		}
		else {
			// Ignore trailing incomplete escape sequences
			if( i + 3 > SourceLen )
				break;
			ch = strtol(Source + i + 1, NULL, 16);
			i += 2;
		}
		if( Dest )
			Dest[outlen] = ch;
		outlen ++;
	}
	
	return outlen;
}

int CGI_int_ParseQueryString(const char *Source, char *DestBuffer, int *ParamCount, tCGI_Param *Params)
{
	 int	len = 0, idx = 0;
	 int	i = 0;

	// Calculate length
	while( Source[i] != '\0' )
	{
		 int	arg_val_end = 0;
		 int	arg_key_end = 0;
		 int	val_len = 0;
		 int	start = i;
		
		// Search for '&' (for next param) and '=' (to separate the name from the data)
		for( ; Source[i] != '\0'; i ++ )
		{
			if( Source[i] == '&' ) {
				arg_val_end = i;
				i ++;
				break;
			}
			// Only update key end if it hasn't been set yet
			if( Source[i] == '=' && arg_key_end == 0 )
				arg_key_end = i;
		}
		// Check for no '&' and no '='
		if( arg_val_end == 0 )
			arg_val_end = i;
		if( arg_key_end == 0 )
			arg_key_end = arg_val_end;

		// Get the length of the data
		if( arg_key_end != arg_val_end )
			val_len = arg_val_end - arg_key_end - 1;
		else
			val_len = 0;

		// Handle first/second passes
		if( DestBuffer )
		{
			Params[idx].Name = DestBuffer + len;
			len += CGI_int_UnURLEncode(Source + start, arg_key_end - start, DestBuffer + len) + 1;
			DestBuffer[len-1] = '\0';	// NULL terminate as UnURLEncode doesn't do that
			
			Params[idx].Data = DestBuffer + len;
			Params[idx].DataLen = CGI_int_UnURLEncode(Source+arg_key_end+1, val_len, DestBuffer + len);
			len += Params[idx].DataLen + 1;
			DestBuffer[len-1] = '\0';	// NULL terminate as UnURLEncode doesn't do that
			
//			printf("Param %i '%s' = %i bytes '%.*s'<br/>\n",
//				idx, Params[idx].Name,
//				Params[idx].DataLen, Params[idx].DataLen, Params[idx].Data);
		}
		else
		{
			len += CGI_int_UnURLEncode(Source + start, arg_key_end - start, NULL) + 1;
			len += CGI_int_UnURLEncode(Source + arg_key_end + 1, val_len, NULL) + 1;
		}
		idx ++;
	}

	*ParamCount = idx;

	return len;
}

void CGI_ParseGETData(void)
{
	if( !gbCGI_IsInvokedAsCGI )	return ;
	if( gsCGI_GETData )	return ;
	
	char *query = getenv("QUERY_STRING");

	if( !query )	return ;	

	 int len = CGI_int_ParseQueryString(query, NULL, &giCGI_GETNArgs, NULL);
	
	gsCGI_GETData = malloc(len);
	gaCGI_GETArgs = malloc( sizeof(*gaCGI_GETArgs) * giCGI_GETNArgs );
	CGI_int_ParseQueryString(query, gsCGI_GETData, &giCGI_GETNArgs, gaCGI_GETArgs);
}

void CGI_SendHeadersOnce(void)
{
	static int	bHeadersSent = 0;
	
	if( !gbCGI_IsInvokedAsCGI )
		return ;
	
	if( bHeadersSent )
		return ;

	printf("Content-Type: text/html\n");
	printf("\n");

	bHeadersSent = 1;
}

SCRIPT_METHOD("CGI@SetHeader", CGI_SetHeader, SS_DATATYPE_STRING, SS_DATATYPE_STRING, 0)
{
	// "QUERY_STRING" and "REQUEST_METHOD"
	return 0;
}

SCRIPT_METHOD_EX(SS_DATATYPE_STRING, "CGI@ReadGET", CGI_ReadGET, SS_DATATYPE_STRING, 0)
{
	 int	 i;
	const tSpiderString	*string = Args[0];
	tSpiderString	*ret = NULL;

	if( ArgTypes[0] != SS_DATATYPE_STRING )	printf("NOTSTR\n");
	if( !Args[0] )	return -1;

	CGI_ParseGETData();

	for( i = 0; i < giCGI_GETNArgs; i ++ )
	{
		if( strncmp(gaCGI_GETArgs[i].Name, string->Data, string->Length) != 0 )
			continue ;
		if( strlen(gaCGI_GETArgs[i].Name) != string->Length )
			continue ;
		
		ret = SpiderScript_CreateString(gaCGI_GETArgs[i].DataLen, gaCGI_GETArgs[i].Data);
		break;
	}

	*(tSpiderString**)RetData = ret;
	return SS_DATATYPE_STRING;
}

SCRIPT_METHOD_EX(SS_DATATYPE_STRING, "CGI@ReadPOST", CGI_ReadPOST, SS_DATATYPE_STRING, 0)
{
	*(tSpiderString**)RetData = NULL;
	return SS_DATATYPE_STRING;
}

void Module_CGI_Initialise(void)
{
	SpiderWeb_AppendFunction(&gScript_CGI_SetHeader);
	SpiderWeb_AppendFunction(&gScript_CGI_ReadGET);
	SpiderWeb_AppendFunction(&gScript_CGI_ReadPOST);
	
	if( getenv("REQUEST_METHOD") )
		gbCGI_IsInvokedAsCGI = 1;
}
