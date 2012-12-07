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
#include <module_cgi.h>

// === GLOBALS ===
 int	gbCGI_IsInvokedAsCGI = 0;
char	*gsCGI_GETData;
 int	giCGI_GETNArgs;
tCGI_Param	*gaCGI_GETArgs;
char	*gsCGI_POSTData;
 int	giCGI_POSTNArgs;
tCGI_Param	*gaCGI_POSTArgs;
char	*gsCGI_ContentType;
 int	gbCGI_HeadersSent = 0;
	

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

void CGI_ParsePOSTData(void)
{
	CGI_SendHeadersOnce();	
	
	if( !gbCGI_IsInvokedAsCGI )	return ;
	if( gsCGI_POSTData )	return ;

	gsCGI_POSTData = (void*)-1;

	if( strcmp(getenv("REQUEST_METHOD"), "POST") != 0 )
		return ;

	const char *lenstr = getenv("CONTENT_LENGTH");
	if( !lenstr )	return ;
	int len = atoi( lenstr );
	if( len <= 0 )	return ;
	
	char *query = malloc(len+1);
	fread(query, 1, len, stdin);
	query[len] = 0;

	
	int datalen = CGI_int_ParseQueryString(query, NULL, &giCGI_POSTNArgs, NULL);
	
	gsCGI_POSTData = malloc(datalen);
	gaCGI_POSTArgs = malloc( sizeof(*gaCGI_POSTArgs) * giCGI_POSTNArgs );
	CGI_int_ParseQueryString(query, gsCGI_POSTData, &giCGI_POSTNArgs, gaCGI_POSTArgs);
}

void CGI_SendHeadersOnce(void)
{
	if( !gbCGI_IsInvokedAsCGI )
		return ;
	
	if( gbCGI_HeadersSent )
		return ;

	printf("Content-Type: %s\n", (gsCGI_ContentType ? gsCGI_ContentType : "text/html"));
	printf("\n");
	
	if( gsCGI_ContentType )
		free(gsCGI_ContentType);

	gbCGI_HeadersSent = 1;
}

int CGI_SetHeader(const char *Name, const char *Value)
{
	if( gbCGI_HeadersSent )
	{
		return -1;
	}

	// TODO: Sanity check Name/Value

	if( strcmp(Name, "Content-Type") == 0 ) {
		if( gsCGI_ContentType )
			free(gsCGI_ContentType);
		gsCGI_ContentType = strdup(Value);
	}
	else {
		// TODO: Append to list (or just output?)
		printf("%s: %s", Name, Value);
	}
	return 0;
}

void Module_CGI_Initialise(void)
{
	if( getenv("REQUEST_METHOD") )
		gbCGI_IsInvokedAsCGI = 1;
}

