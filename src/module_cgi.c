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
#include <stdarg.h>
#include <ctype.h>

// === GLOBALS ===
 int	gbCGI_IsInvokedAsCGI = 0;
 int	gbCGI_HeadersSent = 0;
char	*gsCGI_GETData;
 int	giCGI_GETNArgs;
tCGI_Param	*gaCGI_GETArgs;
char	*gsCGI_POSTData;
 int	giCGI_POSTNArgs;
tCGI_Param	*gaCGI_POSTArgs;
char	*gsCGI_CookieData;
 int	giCGI_NCookies;
tCGI_Param	*gaCGI_Cookies;

// === CODE ===
int isvalidurlchar(char ch)
{
	if( ch < 0x20 )
		return 0;
	
	if( ch > 128 )
		return 0;
	
	switch(ch)
	{
	case '$':	case '&':
	case '+':	case ',':
	case '/':	case ':':
	case ';':	case '=':
	case '?':	case '@':
	case '[':	case ']':
	case '{':	case '}':
	case '<':	case '>':
	case '%':	case '"':
	case '#':	case '|':
	case '`':	case '\\':
	case '^':	case '~':
		return 0;
	default:
		return 1;
	}
}
int CGI_int_URLEncode(const char *Source, int SourceLen, char *Dest)
{
	 int	outlen = 0;
	 int	i;
	
	for( i = 0; i < SourceLen; i ++ )
	{
		if( Source[i] == ' ' )
		{
			if(Dest)
				Dest[outlen] = '+';
			outlen ++;
		}
		else if( !isvalidurlchar(Source[i]) )
		{
			if(Dest) {
				snprintf(Dest+outlen, 4, "%%%02x", (uint8_t)Source[i]);
			}
			outlen += 3;
		}
		else
		{
			if(Dest)
				Dest[outlen] = Source[i];
			outlen ++;
		}
	}
	return outlen;
}

int CGI_int_UnURLEncode(const char *Source, int SourceLen, char *Dest)
{
	 int	outlen = 0;
	 int	i;

	for( i = 0; i < SourceLen; i ++ )
	{
		char	ch;
		
		if( Source[i] == '%' ) {
			// Ignore trailing incomplete escape sequences
			if( i + 3 > SourceLen )
				break;

			char tmp[3] = {Source[i+1], Source[i+2], 0};
			ch = strtol(tmp, NULL, 16);
			i += 2;
		}
		else if( Source[i] == '+' ) {
			ch = ' ';
		}
		else {
			ch = Source[i];
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

static inline int _ParseCookieString(const char *base, char *dataptr, int *count, tCGI_Param *dest)
{
	const char	*end;
	 int	len = 0, dlen;
	 int	idx = 0;
	do
	{
		while(isspace(*base))
			base ++;

		end = strchr(base, '=');
		if( end == NULL )	break;

		len += dlen = CGI_int_UnURLEncode(base, end - base, dataptr) + 1;
		if(dataptr)
			dataptr[dlen-1] = '\0';
		if( dest ) {
			dest[idx].Name = dataptr;
		}

		if( dataptr )	dataptr += dlen;
		base = end + 1;

		end = strchr(base, ';');

		if( end == NULL )
			len += dlen = CGI_int_UnURLEncode(base, strlen(base), dataptr) + 1;
		else
			len += dlen = CGI_int_UnURLEncode(base, end - base, dataptr) + 1;
		if(dataptr)
			dataptr[dlen-1] = '\0';

		if( dest ) {
			dest[idx].Data = dataptr;
			dest[idx].DataLen = dlen - 1;
		}
			
		if( dataptr )	dataptr += dlen;
		if( end )	base = end + 1;

		idx ++;
	} while(end);

	*count = idx;	

	return len;
}

void CGI_ParseCookies(void)
{
	if( !gbCGI_IsInvokedAsCGI )	return ;
	if( gsCGI_CookieData )	return;
	
	char *e = getenv("HTTP_COOKIE");
	if( !e )	return ;
	
	int len = _ParseCookieString(e, NULL, &giCGI_NCookies, NULL);
	gsCGI_CookieData = malloc(len);
	gaCGI_Cookies = malloc( giCGI_NCookies * sizeof(tCGI_Param) );
	_ParseCookieString(e, gsCGI_CookieData, &giCGI_NCookies, gaCGI_Cookies);
}

void CGI_SendHeadersOnce(void)
{
	if( !gbCGI_IsInvokedAsCGI )
		return ;
	
	if( gbCGI_HeadersSent )
		return ;

	printf("Content-Type: text/html\n");
	printf("\n");

	gbCGI_HeadersSent = 1;
}

void Module_CGI_Initialise(void)
{
	if( getenv("REQUEST_METHOD") )
		gbCGI_IsInvokedAsCGI = 1;
}

void CGI_AddHeader(const char *Format, ...)
{
	if( !gbCGI_IsInvokedAsCGI )
		return ;
	if( gbCGI_HeadersSent )
		return;	// TODO: Warn
	
	// TODO: Determine if Content-Type is sent
	
	va_list args;
	va_start(args, Format);
	vprintf(Format, args);
	printf("\n");
	va_end(args);
}

