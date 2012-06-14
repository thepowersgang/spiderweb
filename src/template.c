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
#include "template.h"

// === TYPES ===
typedef struct {
	t_map_entry	*ValueMap;	// TODO: Maybe have a hash table here too
} t_obj_Template;

// === PROTOTYPES ===
FCN_PROTO(Template__construct);
void	Template__destruct(tSpiderObject *This);
FCN_PROTO(Template_Assign_S);
FCN_PROTO(Template_Display);
// ---
t_map_entry	*Template_int_GetMapItem(t_map_entry *FirstEntry, const char *Key);
void	Template_int_FreeMap(t_map_entry *FirstEntry);
// ---
t_template	*Template_int_Load(const char *Filename);
void	Template_int_FreeSec(t_tplop *Section);
void	Template_int_Unload(t_template *Template);
// ---
 int	Template_int_RunSec_Arith(t_obj_Template *State, struct s_tplop_arith *Arith, void **ValuePtr);
 int	Template_int_RunSec(t_obj_Template *State, t_tplop *Section, void **ValuePtr);
void	Template_int_Output(t_obj_Template *State, t_template *Template);

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
	info->ValueMap = NULL;
	
	*(tSpiderObject**)RetData = ret;
	
	return 0;
}
void Template__destruct(tSpiderObject *This)
{
	t_obj_Template	*info = This->OpaqueData;
	
	Template_int_FreeMap( info->ValueMap );
}

FCN_PROTO(Template_Assign_S)
{
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

	// TODO: Send headers
	
	// Load template into memory
	tpl_data = Template_int_Load(tpl_name->Data);
	// Cache for faster loading later?
	// Write to output filling template slots with data from param map
	Template_int_Output(info, tpl_data);

	// Clean up	
	Template_int_Unload(tpl_data);
	
	return 0;
}

// --- Key/Value Map Manipulation ---
t_map_entry *Template_int_GetMapItem(t_map_entry *FirstEntry, const char *Key)
{
	t_map_entry	*ent;
	for( ent = FirstEntry; ent; ent = ent->Next )
	{
		if( strcmp(ent->Key, Key) == 0 )
			return ent;
	}
	return NULL;
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

// --- Template Parsing ---
static void _addVerbatim(t_tplop **List, const char *Data, size_t Size)
{
	struct s_tplop_const	*ent;
	t_tplop	*prev;

	if( Size == 0 )	return ;

	printf("%i bytes of verbatim text\n", (int)Size);

	ent = malloc( sizeof(*ent) + Size );
	if( !ent ) {
		perror("malloc fail");
		return ;
	}
	ent->Next = NULL;
	ent->Type = TPLOP_CONSTOUT;
	ent->Length = Size;
	memcpy(ent->Data, Data, Size);
	
	for( prev = *List; prev && prev->Next; prev = prev->Next )
		;
	if(prev)
		prev->Next = (t_tplop*)ent;
	else
		*List = (t_tplop*)ent;
}

t_template *Template_int_Load(const char *Filename)
{
	char	buffer[BUFSIZ];
	 int	ofs;
	 int	ctrl_end;
	 int	ctrl_start;
	FILE *fp;
	 int	i, j;
	t_template	*ret;
	t_tplop	**listhead;

	fp = fopen(Filename, "r");
	if( !fp ) {
		perror("Opening template");
		return NULL;
	}

	printf("Parsing '%s'\n", Filename);	

	ret = malloc( sizeof(*ret) );
	ret->Sections = NULL;
	listhead = &ret->Sections;

	ofs = 0;
	for( ;; )
	{
		int len = fread(buffer + ofs, 1, BUFSIZ - ofs, fp);
		printf("Read %i to offset %i\n", len, ofs);

		if( ofs == 0 && len == 0 ) {
			break;	// Would this be an error?
		}

		ctrl_end = 0;
		ctrl_start = -1;
		for( i = 0; i < ofs + len; i ++ )
		{
			if( buffer[i] == '{' )
			{
				char *ctrl_data;
				
				ctrl_start = i;
				_addVerbatim(listhead, buffer + ctrl_end, i - ctrl_end);
				ctrl_end = 0;
				for( j = i; j < ofs+len; j ++ )
				{
					if( buffer[j] == '}' ) {
						ctrl_end = j;
						break;
					}
				}
				if( ctrl_end == 0 )
					break;	// Force more to be read
				
				buffer[ctrl_end] = '\0';

				ctrl_data = buffer + ctrl_start + 1;
				printf("Control statement: %s\n", ctrl_data);

				if( ctrl_data[0] == '=' ) {
					// Output statement
//					ctrl_data = _addValue(listhead, ctrl_data);
					if(ctrl_data[0] != '\0')
						;	// TODO: Error when {{=Value}} is malformed?
				}
				else if( ctrl_data[0] == '?' ) {
					// Short conditional (ternary)
					printf("TODO: Short conditionals\n");
				}
				#define hstrcmp(value, start)	strncmp(value, start, sizeof(start))
				else if( hstrcmp(ctrl_data, "foreach ") == 0 )
				{
					printf("TODO: Foreach\n");
				}
				else if( strcmp(ctrl_data, "endforeach") == 0 )
				{
					printf("TODO: endforeach\n");
				}
				else
				{
					printf("Unknown operation '%s'\n", ctrl_data);
				}
				#undef hstrcmp

				ctrl_start = -1;
				ctrl_end += 1;
			}
		}
		
		if( ctrl_start == -1 )
			_addVerbatim(listhead, buffer + ctrl_end, (ofs + len) - ctrl_end);
		if( ctrl_start > 0 ) {
			memmove(buffer, buffer + ctrl_start, (ofs + len) - ctrl_start);
			ofs = (ofs + len) - ctrl_start;
		}
		
		if( len == 0 )	{
			_addVerbatim(listhead, buffer, ofs);
			break;
		}
	}
	
	fclose(fp);

	return ret;
}

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


int Template_int_RunSec_Arith(t_obj_Template *State, struct s_tplop_arith *Arith, void **ValuePtr)
{
	 int	 lt,  rt;
	void	*rv, *lv;
	switch(Arith->Operation)
	{
	case ARITHOP_CMPEQ:
		lt = Template_int_RunSec(State, Arith->Left, &lv);
		rt = Template_int_RunSec(State, Arith->Left, &rv);
		if( lt != 2 && lt != rt )	return 0;
		break;
	default:
		break;
	}
	return 0;
}

int Template_int_RunSec(t_obj_Template *State, t_tplop *Section, void **ValuePtr)
{
	 int	rv;
	void	*ptr = NULL;
	t_map_entry	*val;
	
	switch(Section->Type)
	{
	case TPLOP_CONSTOUT:
		fwrite(Section->Constant.Data, 1, Section->Constant.Length, stdout);
		return 0;
	case TPLOP_VALUEOUT:
		rv = Template_int_RunSec(State, Section->Output.Value, &ptr);
		if( rv == 2 )	fputs(ptr, stdout);
		return 0;
	
	case TPLOP_CONSTANT:
		*ValuePtr = Section->Constant.Data;
		return 2;
	case TPLOP_GETVALUE:
		val = Template_int_GetMapItem(State->ValueMap, Section->Value.Name);
		if( !val )	return 0;
		switch(val->Type)
		{
		case 0:	*ValuePtr = NULL;	return 0;	// Unset
		case 1:	*ValuePtr = val->FirstChild;	return 1;	// Vector
		case 2:	*ValuePtr = val->String;	return 2;	// String - return the string
		default:	*ValuePtr = NULL;	return -1;
		}
		return 0;
	
	case TPLOP_CONDITIONAL:
		Template_int_RunSec(State, Section->Conditional.Condition, &ptr);
		if(ptr)
			Template_int_RunSec(State, Section->Conditional.True, &ptr);
		else
			Template_int_RunSec(State, Section->Conditional.False, &ptr);
		return 0;
	case TPLOP_ITERATOR:
		rv = Template_int_RunSec(State, Section->Iterator.Array, &ptr);
		if( rv != 1 || ptr == NULL ) {
			Template_int_RunSec(State, Section->Iterator.IfEmpty, &ptr);
		}
		else {
			t_map_entry	*ent;
			for( ent = ptr; ent; ent = ent->Next )
			{
				// TODO: Assign value into map according to stated name
			}
		}
		return 0;
	case TPLOP_ARITH:
		return Template_int_RunSec_Arith(State, &Section->Arith, ValuePtr);
	}
	return 0;
}

// TODO: Should there be support for alternate destinations?
void Template_int_Output(t_obj_Template *State, t_template *Template)
{
	t_tplop *section;
	void	*unused;
	
	for( section = Template->Sections; section; section = section->Next )
	{
		Template_int_RunSec(State, section, &unused);
	}
}

