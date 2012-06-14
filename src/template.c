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

enum e_tplsec_types
{
	TPLSEC_VERBATIM,
	TPLSEC_VALUE,
	TPLSEC_ITERATOR,
	TPLSEC_CONDITIONAL
};

struct s_tplsec_verb
{
	union u_tplsec	*Next;
	enum e_tplsec_types	Type;	// = 0;
	size_t	Length;
	char	Data[];
};

struct s_tplsec_sval
{
	union u_tplsec	*Next;
	enum e_tplsec_types	Type;	// = 1
	char	Name[];
};

struct s_tplsec_loop
{
	union u_tplsec	*Next;
	enum e_tplsec_types	Type;	// = 2
	const char	*Array;
	union u_tplsec	*SubItems;
};

struct s_tplsec_cond
{
	union u_tplsec	*Next;
	enum e_tplsec_types	Type;	// = 3
	const char	*Condition;
	union u_tplsec	*True;
	union u_tplsec	*False;
};

typedef union u_tplsec
{
	struct {
		union u_tplsec	*Next;
		enum e_tplsec_types	Type;
	};
	struct s_tplsec_verb	Verbatim;
	struct s_tplsec_sval	Single;
	struct s_tplsec_loop	Loop;
	struct s_tplsec_cond	Conditional;
} t_tplsec;

typedef struct s_template
{
	union u_tplsec	*Sections;
} t_template;

typedef struct {
	t_map_entry	*ValueMap;	// TODO: Maybe have a hash table here too
} t_obj_Template;

// === PROTOTYPES ===
void	Template__destruct(tSpiderObject *This);
void	Template_int_FreeMap(t_map_entry *FirstEntry);

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

FCN_PROTO(Template_Assign_S)
{
	return 0;
}

static void _addVerbatim(t_tplsec **List, const char *Data, size_t Size)
{
	struct s_tplsec_verb	*ent;
	t_tplsec	*prev;

	if( Size == 0 )	return ;

	printf("%i bytes of verbatim text\n", (int)Size);

	ent = malloc( sizeof(*ent) + Size );
	if( !ent ) {
		perror("malloc fail");
		return ;
	}
	ent->Next = NULL;
	ent->Type = TPLSEC_VERBATIM;
	ent->Length = Size;
	memcpy(ent->Data, Data, Size);
	
	for( prev = *List; prev && prev->Next; prev = prev->Next )
		;
	if(prev)
		prev->Next = (t_tplsec*)ent;
	else
		*List = (t_tplsec*)ent;
}

static char *_addValue(t_tplsec **List, char *Data)
{
/*	char *start = Data;
	for( ; *Data; Data ++ )
	{
		if( isalpha(*Data) )	continue ;
		if( *Data == '.' )	continue ;
		break ;
	}
*/	return Data;
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
	t_tplsec	**listhead;

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
					ctrl_data = _addValue(listhead, ctrl_data);
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

void Template_int_FreeSec(t_tplsec *Section)
{
	switch(Section->Type)
	{
	case TPLSEC_VERBATIM:
		break;
	case TPLSEC_VALUE:
	case TPLSEC_CONDITIONAL:
	case TPLSEC_ITERATOR:
		// TODO:
		break;
	}
	free(Section);
}

void Template_int_Unload(t_template *Template)
{
	t_tplsec *section, *next;
	
	for( section = Template->Sections; section; section = next )
	{
		next = section->Next;
		Template_int_FreeSec(section);
	}
	free(Template);
}

void Template_int_OutputSec(t_obj_Template *State, t_tplsec *Section)
{
	switch(Section->Type)
	{
	case TPLSEC_VERBATIM:
		fwrite(Section->Verbatim.Data, 1, Section->Verbatim.Length, stdout);
		break;
	case TPLSEC_VALUE:
	case TPLSEC_CONDITIONAL:
	case TPLSEC_ITERATOR:
		// TODO:
		break;
	}
}

// TODO: Should there be support for alternate destinations?
void Template_int_Output(t_obj_Template *State, t_template *Template)
{
	t_tplsec *section;
	
	for( section = Template->Sections; section; section = section->Next )
	{
		Template_int_OutputSec(State, section);
	}
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

