/*
 * SpiderWeb Scripting Suite
 * 
 * module_cgi.c
 * - CGI Interface code
 */
#include "spiderweb_internal.h"
#include <stdio.h>

void CGI_SendHeadersOnce(void)
{
	static int	bHeadersSent = 0;
	
	if( bHeadersSent )
		return ;

	printf("Content-Type: text/html\n");
	printf("\n");

	bHeadersSent = 1;
}

SCRIPT_METHOD("CGI@SetHeader", CGI_SetHeader, SS_DATATYPE_STRING, SS_DATATYPE_STRING, 0)
{
	return 0;
}

SCRIPT_METHOD("CGI@ReadGET", CGI_ReadGET, SS_DATATYPE_STRING)
{
	return 0;
}

SCRIPT_METHOD("CGI@ReadPOST", CGI_ReadPOST, SS_DATATYPE_STRING)
{
	return 0;
}

void Module_CGI_Initialise(void)
{
	SpiderWeb_AppendFunction(&gScript_CGI_SetHeader);
	SpiderWeb_AppendFunction(&gScript_CGI_ReadGET);
	SpiderWeb_AppendFunction(&gScript_CGI_ReadPOST);
}

