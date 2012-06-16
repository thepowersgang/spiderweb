/*
 * SpiderWeb
 * - By John Hodge (thePowersGang)
 * 
 * Template Object
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <spiderscript.h>
#include "../spiderweb_internal.h"
#include "template_common.h"

// === PROTOTYPES ===
FCN_PROTO(Template__construct);
void	Template__destruct(tSpiderObject *This);
FCN_PROTO(Template_Assign_S);
FCN_PROTO(Template_Display);


// === GLOBALS ===
DEF_OBJ_FCN(Template_Display, "Display", NULL, SS_DATATYPE_NOVALUE, -2, SS_DATATYPE_STRING, 0);
DEF_OBJ_FCN(Template_Assign_S,  "Assign",  Template_Display, SS_DATATYPE_NOVALUE, -2, SS_DATATYPE_STRING, SS_DATATYPE_STRING, 0);
DEF_OBJ_FCN(Template__construct, "_construct", NULL, SS_DATATYPE_NOVALUE, 0);
tSpiderClass	g_obj_Template = {
	NULL, "SpiderWeb@Template",
	&g_fcn_Template__construct,	// Constructor
	Template__destruct,
	&g_fcn_Template_Assign_S,	// First function
	0,
	{
	}
};

// === CODE ===
FCN_PROTO(Template__construct)
{
	tSpiderObject	*ret;
	t_obj_Template	*info;
	
	if( NArgs != 0 )	return -1;
	
	ret = SpiderScript_AllocateObject(Script, &g_obj_Template, sizeof(t_obj_Template));
	if(!ret)	return -1;
	
	info = ret->OpaqueData;
	info->ValueMap.FirstEnt = NULL;
	
	*(tSpiderObject**)RetData = ret;
	
	return 0;
}
void Template__destruct(tSpiderObject *This)
{
	t_obj_Template	*info = This->OpaqueData;
	
	Template_int_FreeMap( &info->ValueMap );
}

FCN_PROTO(Template_Assign_S)
{
	if( NArgs != 3 )	return -1;
	if( ArgTypes[1] != SS_DATATYPE_STRING )	return -1;
	if( ArgTypes[2] != SS_DATATYPE_STRING )	return -1;
	
	const tSpiderObject	*this = Args[0];
	t_obj_Template	*info = this->OpaqueData;
	const tSpiderString	*key = Args[1];
	const tSpiderString	*value = Args[2];
	
	Template_int_AddMapItem_String( &info->ValueMap, key->Data, value->Data);
	
	return 0;
}

FCN_PROTO(Template_Display)
{
	const tSpiderObject	*this;
	const tSpiderString	*tpl_name;
	t_obj_Template	*info;
	t_template	*tpl_data;
	
	if(NArgs != 2)	return -1;
	if(ArgTypes[1] != SS_DATATYPE_STRING)	return -1;
	if(Args[1] == NULL)	return -1;

	this = Args[0];
	info = this->OpaqueData;
	tpl_name = Args[1];

	CGI_SendHeadersOnce();
	
	// Load template into memory
	tpl_data = Template_int_Load(tpl_name->Data);
	if( !tpl_data )
	{
		printf("Unable to find template `%s`\n", tpl_name->Data);
	}
	else
	{
		// Cache for faster loading later?
		// Write to output filling template slots with data from param map
		Template_int_Output(info, tpl_data);
	
		// Clean up	
		Template_int_Unload(tpl_data);
	}
	
	return 0;
}

