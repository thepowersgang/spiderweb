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
FCN_PROTO(Template_Assign);
FCN_PROTO(Template_Display);


// === GLOBALS ===
DEF_OBJ_FCN(Template_Display, "Display", NULL, SS_DATATYPE_NOVALUE, -2, SS_DATATYPE_STRING, 0);
DEF_OBJ_FCN(Template_Assign,  "Assign",  Template_Display, SS_DATATYPE_NOVALUE, -2, SS_DATATYPE_STRING, -1);
DEF_OBJ_FCN(Template__construct, "_construct", NULL, SS_DATATYPE_NOVALUE, 0);
tSpiderClass	g_obj_Template = {
	NULL, "SpiderWeb@Template",
	&g_fcn_Template__construct,	// Constructor
	Template__destruct,
	&g_fcn_Template_Assign,	// First function
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
	info->IteratorValues.FirstEnt = NULL;
	
	*(tSpiderObject**)RetData = ret;
	
	return 0;
}
void Template__destruct(tSpiderObject *This)
{
	t_obj_Template	*info = This->OpaqueData;
	
	Template_int_FreeMap( &info->ValueMap );
}

int Template_int_AddItem(tSpiderScript *Script, t_map *Map, const char *Key, int ValueType, const void *ValuePtr)
{
//	printf("Adding 0x%x %p to %p as '%s'\n",
//		ValueType, ValuePtr, Map, Key);
	if( SS_GETARRAYDEPTH(ValueType) )
	{
		 int	i;
		 int	type = SS_DOWNARRAY(ValueType);
		const tSpiderArray	*array = ValuePtr;
		t_map_entry *submap = Template_int_AddMapItem_SubMap( Map, Key );
		
		for( i = 0; i < array->Length; i ++ )
		{
			char	key[10];
			snprintf(key, 10, "%i",i);
			Template_int_AddItem(Script, &submap->SubMap, key, type, SpiderScript_GetArrayPtr(array, i));
		}
	}
	else if( ValueType == SpiderScript_GetTypeCode(Script, "Lang@StringMap") )
	{
		tSpiderArray	*keys;
		tSpiderString	*val;
		 int	rv, i;
	
		// Call the ->keys() method (with the `this` parameter)	
		rv = SpiderScript_ExecuteMethod(Script, "keys", &keys, 1, &ValueType, &ValuePtr, NULL);
		if(rv != SS_MAKEARRAY(SS_DATATYPE_STRING))
			return -1;	// Oops?
		
		t_map_entry *submap = Template_int_AddMapItem_SubMap( Map, Key );
		
		for( i = 0; i < keys->Length; i ++ )
		{
			 int	types[] = {ValueType, SS_DATATYPE_STRING};
			const void	*args[] = {ValuePtr, keys->Strings[i]};
			if( !keys->Strings[i] )
				continue ;
			rv = SpiderScript_ExecuteMethod(Script, "get", &val, 2, types, args, NULL);
			if(rv != SS_DATATYPE_STRING)
				break;
			Template_int_AddItem(Script, &submap->SubMap, keys->Strings[i]->Data, SS_DATATYPE_STRING, val);
			SpiderScript_DereferenceString(val);
		}
		SpiderScript_DereferenceArray(keys);
	}
	else
	{
		tSpiderString	*str = SpiderScript_CastValueToString(ValueType, ValuePtr);
		if( str == NULL ) {
			// TODO: Throw exception
			return -1;
		}
		Template_int_AddMapItem_String(Map,Key, str->Data);
		SpiderScript_DereferenceString(str);
	}
	return 0;
}

FCN_PROTO(Template_Assign)
{
	if( NArgs != 3 )	return -1;
	if( ArgTypes[1] != SS_DATATYPE_STRING )	return -1;

	const tSpiderObject	*this = Args[0];
	t_obj_Template	*info = this->OpaqueData;
	const tSpiderString	*key = Args[1];

	Template_int_AddItem(Script, &info->ValueMap, key->Data, ArgTypes[2], Args[2] );
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

