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
#include "spiderweb_internal.h"

// === TYPES ===
typedef struct s_map_entry	t_map_entry;

struct s_map_entry {
	t_map_entry	*Next;
	char	*Key;	// Key is stored after value
	 int	Type;	// 0: Null, 1: Integer, 2: String, 3: Vector
	union {
		t_map_entry	*FirstChild;
		 int	Integer;
		char	*String;
	};
	char	Data[];
};

typedef struct {
	t_map_entry	*ValueMap;	// TODO: Maybe have a hash table here too
} t_obj_Template;

// === PROTOTYPES ===
tSpiderObject	*Template__construct(tSpiderScript *Script, int NArgs, tSpiderValue **Args);
void	Template__destruct(tSpiderObject *This);
void	Template_int_FreeMap(t_map_entry *FirstEntry);

// === GLOBALS ===
DEF_OBJ_FCN(Template_Display, "Display", NULL, SS_DATATYPE_UNDEF, SS_DATATYPE_STRING, 0);
tSpiderClass	g_obj_Template = {
	NULL, "Template",
	Template__construct,	// Constructor
	Template__destruct,
	NULL,	// Attributes
	&g_fcn_Template_Display,	// First function
	0,
	{
	}
};

// === CODE ===
tSpiderObject *Template__construct(tSpiderScript *Script, int NArgs, tSpiderValue **Args)
{
	tSpiderObject	*ret;
	t_obj_Template	*info;
	
	if( NArgs != 0 )	return ERRPTR;
	
	ret = SpiderScript_AllocateObject(Script, &g_obj_Template, sizeof(t_obj_Template));
	if(!ret)	return ERRPTR;
	
	info = ret->OpaqueData;
	
	info->ValueMap = NULL;
	
	return ret;
}
void Template__destruct(tSpiderObject *This)
{
	t_obj_Template	*info = This->OpaqueData;
	
	Template_int_FreeMap( info->ValueMap );
	
	free( This );
}

void Template_int_FreeMap(t_map_entry *FirstEntry)
{
	t_map_entry	*ent, *tmp;
	
	for( ent = FirstEntry; ent; ent = tmp )
	{
		tmp = ent->Next;
		
		if(ent->Type == 3)	Template_int_FreeMap(ent->FirstChild);
		
		ent->Next = NULL;
		free(ent);
	}
}

tSpiderValue *Template_Display(tSpiderScript *Script, int nParams, tSpiderValue **Parameters)
{
	if(nParams != 2)	return ERRPTR;
	if(Parameters[1]->Type != SS_DATATYPE_STRING)	return ERRPTR;

	// TODO: Send headers
	
	// Load template into memory
	// Parse into some sort of structure for execution
	// Cache for faster loading later?
	// Write to output filling template slots with data from param map
	
	return NULL;
}
