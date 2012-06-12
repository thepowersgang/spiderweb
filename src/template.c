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

struct s_tplsec_verb
{
	union u_tplsec	*Next;
	 int	Type;	// = 0;
	char	Data[];
};

struct s_tplsec_sval
{
	union u_tplsec	*Next;
	 int	Type;	// = 1
	char	Name[];
};

struct s_tplsec_loop
{
	union u_tplsec	*Next;
	 int	Type;	// = 2
	const char	*Array;
	union u_tplsec	*SubItems;
};

struct s_tplsec_cond
{
	union u_tplsec	*Next;
	 int	Type;	// = 3
	const char	*Condition;
	union u_tplsec	*True;
	union u_tplsec	*False;
};

union u_tplsec
{
	struct s_tplsec_verb	Verbatim;
	struct s_tplsec_sval	Single;
	struct s_tplsec_loop	Loop;
	struct s_tplsec_cond	Conditional;
};

struct s_template
{
	union u_tplsec	*Sections;
};

typedef struct {
	t_map_entry	*ValueMap;	// TODO: Maybe have a hash table here too
} t_obj_Template;

// === PROTOTYPES ===
void	Template__destruct(tSpiderObject *This);
void	Template_int_FreeMap(t_map_entry *FirstEntry);

// === GLOBALS ===
DEF_OBJ_FCN(Template_Display, "Display", NULL, SS_DATATYPE_NOVALUE, SS_DATATYPE_STRING, 0);
DEF_OBJ_FCN(Template__construct, "_construct", NULL, SS_DATATYPE_NOVALUE, 0);
tSpiderClass	g_obj_Template = {
	NULL, "Template",
	&g_fcn_Template__construct,	// Constructor
	Template__destruct,
	&g_fcn_Template_Display,	// First function
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
	info->ValueMap = NULL;
	
	*(tSpiderObject**)RetData = ret;
	
	return 0;
}
void Template__destruct(tSpiderObject *This)
{
	t_obj_Template	*info = This->OpaqueData;
	
	Template_int_FreeMap( info->ValueMap );
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

FCN_PROTO(Template_Display)
{
	const tSpiderObject	*this;
	const tSpiderString	*tpl_name;
	t_obj_Template	*info;
	
	if(NArgs != 2)	return -1;
	if(ArgTypes[1] != SS_DATATYPE_STRING)	return -1;
	if(Args[1] == NULL)	return -1;

	this = Args[0];
	info = this->OpaqueData;
	tpl_name = Args[1];

	// TODO: Send headers
	
	// Load template into memory
	#if 0
	FILE *fp = fopen(tpl_name->Data, "r");
	do {
		int len = fread(buffer + ofs, BUFSIZ - ofs, fp);
		if( len == 0 )	break;
		
		for( int i = ofs; i < ofs + len; i ++ )
		{
			if
		}
	}
	#endif
	// Parse into some sort of structure for execution
	// Cache for faster loading later?
	// Write to output filling template slots with data from param map
	
	return 0;
}

