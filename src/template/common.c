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
#include <assert.h>

// === PROTOTYPES ===
t_map_entry	*Template_int_GetMapItem(t_map *Map, const char *Key);
void	Template_int_DelMapItem(t_map *Map, const char *Key);
t_map_entry	*Template_int_AddMapItem(t_map *Map, enum e_map_entry_types Type, const char *Key, int DataSize);
t_map_entry	*Template_int_AddMapItem_String(t_map *Map, const char *Key, size_t Len, const char *String);
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

void Template_int_DelMapItem(t_map *Map, const char *Key)
{
	t_map_entry	*ent, *prev = NULL;
	
	for( ent = Map->FirstEnt; ent; prev = ent, ent = ent->Next )
	{
		if( strcmp(ent->Key, Key) == 0 )	break;
	}
	if( !ent )	return ;
	
	if( prev )
		prev->Next = ent->Next;
	else
		Map->FirstEnt = ent->Next;
	
	free(ent);
}

t_map_entry *Template_int_AddMapItem(t_map *Map, enum e_map_entry_types Type, const char *Key, int DataSize)
{
	t_map_entry	*ent, *prev;
	
	for( prev = Map->FirstEnt; prev && prev->Next; prev = prev->Next )
	{
		if( strcmp(prev->Key, Key) == 0 )	return NULL;
	}
	if( prev && strcmp(prev->Key, Key) == 0 )	return NULL;
	
	ent = malloc( sizeof(*ent) + strlen(Key) + 1 + DataSize);
	ent->Next = NULL;
	ent->Type = Type;
	ent->Key = ent->Data + DataSize;
	strcpy(ent->Key, Key);
	
	if(prev)
		prev->Next = ent;
	else
		Map->FirstEnt = ent;
	
	return ent;
}

t_map_entry *Template_int_AddMapItem_Ptr(t_map *Map, const char *Key, t_map_entry *Ptr)
{
	t_map_entry *ent;
	ent = Template_int_AddMapItem(Map, MAPENT_POINTER, Key, 0);
	// TODO: Error check
	ent->Ptr = Ptr;
	return ent;
}

t_map_entry *Template_int_AddMapItem_SubMap(t_map *Map, const char *Key)
{
	t_map_entry *ent;
	ent = Template_int_AddMapItem(Map, MAPENT_VECTOR, Key, 0);
	// TODO: Handle pre-existing?
	memset(&ent->SubMap, 0, sizeof(ent->SubMap));
	return ent;
}

t_map_entry *Template_int_AddMapItem_String(t_map *Map, const char *Key, size_t Len, const char *String)
{
	t_map_entry *ent;
	ent = Template_int_AddMapItem(Map, MAPENT_STRING, Key, Len+1);
	if( !ent )	return NULL;
	ent->String = ent->Data;
	strncpy(ent->String, String, Len+1);
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
	Map->FirstEnt = NULL;
}

t_map_entry *Template_int_DuplicateMapItem(t_map *Map, const char *Key, int Type, void *Ptr)
{
	t_map_entry	*ent;
	 int	len;
	
	if( Type == MAPENT_STRING )
		len = strlen(Ptr) + 1;
	else
		len = 0;

	ent = Template_int_AddMapItem(Map, Type, Key, len);

	switch(Type)
	{
	case MAPENT_STRING:
		ent->String = ent->Data;
		strcpy(ent->String, Ptr);
		break;
//	case MAPENT_INTEGER:
//		ent->Integer = *(int*)Ptr;
//		break;
	default:
		// TODO:
		break;
	}
	return ent;
}

// --- Template Parsing ---
void Template_int_FreeSec(t_tplop *Section)
{
	if(!Section)
		return ;

	void _freelist(t_tplop *First)
	{
		t_tplop	*sub, *next;
		for( sub = First; sub; sub = next ) {
			next = sub->Next;
			Template_int_FreeSec(sub);
		}
	}	

	switch(Section->Type)
	{
	case TPLOP_CONSTOUT:
	case TPLOP_CONSTANT:
	case TPLOP_GETVALUE:
		break;
	
	case TPLOP_VALUEOUT:
		Template_int_FreeSec(Section->Output.Value);
		while( Section->Output.Filters ) {
			void *next = Section->Output.Filters->Next;
			free(Section->Output.Filters);
			Section->Output.Filters = next;
		}
		break;
	
	case TPLOP_CONDITIONAL:
		Template_int_FreeSec(Section->Conditional.Condition);
		_freelist(Section->Conditional.True);
		_freelist(Section->Conditional.False);
		break;
	case TPLOP_ITERATOR:
		Template_int_FreeSec(Section->Iterator.Array);
		_freelist(Section->Iterator.PerItem);
		_freelist(Section->Iterator.IfEmpty);
		break;
	case TPLOP_ARITH:
		Template_int_FreeSec(Section->Arith.Left);
		Template_int_FreeSec(Section->Arith.Right);
		break;
	case TPLOP_ASSIGN:
		Template_int_FreeSec(Section->Assign.Value);
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



