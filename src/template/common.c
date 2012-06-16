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
t_map_entry	*Template_int_GetMapItem(t_map *Map, const char *Key);
t_map_entry	*Template_int_AddMapItem(t_map *Map, int Type, const char *Key, int DataSize);
t_map_entry	*Template_int_AddMapItem_String(t_map *Map, const char *Key, const char *String);
void	Template_int_FreeMap(t_map *Map);

// --- Key/Value Map Manipulation ---
t_map_entry *Template_int_GetMapItem(t_map *Map, const char *Key)
{
	t_map_entry	*ent;
	for( ent = Map->FirstEnt; ent; ent = ent->Next )
	{
		if( strcmp(ent->Key, Key) == 0 )
			return ent;
	}
	return NULL;
}

t_map_entry *Template_int_AddMapItem(t_map *Map, int Type, const char *Key, int DataSize)
{
	t_map_entry	*ent, *prev;
	
	for( prev = Map->FirstEnt; prev && prev->Next; prev = prev->Next )
	{
		if( strcmp(prev->Key, Key) == 0 )	return NULL;
	}
	if( prev && strcmp(prev->Key, Key) == 0 )	return NULL;
	
	ent = malloc( sizeof(*ent) + strlen(Key) + 1 + DataSize);
	ent->Next = NULL;
	ent->Type = 2;
	ent->Key = ent->Data + DataSize;
	strcpy(ent->Key, Key);
	
	if(prev)
		prev->Next = ent;
	else
		Map->FirstEnt = ent;
	return ent;
}

t_map_entry *Template_int_AddMapItem_String(t_map *Map, const char *Key, const char *String)
{
	t_map_entry *ent;
	ent = Template_int_AddMapItem(Map, 2, Key, strlen(String)+1);
	ent->String = ent->Data;
	strcpy(ent->String, String);
	return ent;
}

void Template_int_FreeMap(t_map *Map)
{
	t_map_entry	*ent, *tmp;
	
	for( ent = Map->FirstEnt; ent; ent = tmp )
	{
		tmp = ent->Next;
		
		if(ent->Type == 1)	Template_int_FreeMap(&ent->SubMap);
		
		ent->Next = NULL;
		free(ent);
	}
}

// --- Template Parsing ---
void Template_int_FreeSec(t_tplop *Section)
{
	switch(Section->Type)
	{
	case TPLOP_CONSTOUT:
	case TPLOP_CONSTANT:
	case TPLOP_GETVALUE:
		break;
	
	case TPLOP_VALUEOUT:
		Template_int_FreeSec(Section->Output.Value);
		break;
	
	case TPLOP_CONDITIONAL:
		Template_int_FreeSec(Section->Conditional.Condition);
		Template_int_FreeSec(Section->Conditional.True);
		Template_int_FreeSec(Section->Conditional.False);
		break;
	case TPLOP_ITERATOR:
//		Template_int_FreeSec(Section->Conditional.Value);
		Template_int_FreeSec(Section->Iterator.PerItem);
		Template_int_FreeSec(Section->Iterator.IfEmpty);
		break;
	case TPLOP_ARITH:
		Template_int_FreeSec(Section->Arith.Left);
		Template_int_FreeSec(Section->Arith.Right);
		break;
	}
	free(Section);
}

void Template_int_Unload(t_template *Template)
{
	t_tplop *section, *next;
	
	for( section = Template->Sections; section; section = next )
	{
		next = section->Next;
		Template_int_FreeSec(section);
	}
	free(Template);
}



