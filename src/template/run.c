/*
 * SpiderWeb
 * - By John Hodge (thePowersGang)
 * 
 * template/load.c
 * - Template Parsing
 */
#include "template_common.h"
#include <stdio.h>
#include <string.h>

// === PROTOTYPES ===
 int	Template_int_RunSec_Arith(t_obj_Template *State, struct s_tplop_arith *Arith, void **ValuePtr);
 int	Template_int_RunSec(t_obj_Template *State, t_tplop *Section, void **ValuePtr);
void	Template_int_Output(t_obj_Template *State, t_template *Template);

// === CODE ===
int Template_int_RunSec_Arith(t_obj_Template *State, struct s_tplop_arith *Arith, void **ValuePtr)
{
	 int	 lt,  rt;
	void	*rv, *lv;
	*ValuePtr = NULL;
	switch(Arith->Operation)
	{
	case ARITHOP_CMPEQ:
		lt = Template_int_RunSec(State, Arith->Left, &lv);
		rt = Template_int_RunSec(State, Arith->Right, &rv);
		if( lt != 2 && lt != rt )	return 0;
		if( strcmp(lv, rv) != 0 )	return 0;
		*ValuePtr = "true";
		return 2;
	default:
		break;
	}
	return 0;
}

void Template_int_RunSecList(t_obj_Template *State, t_tplop *First)
{
	t_tplop	*subsec;
	void	*ptr;
	for( subsec = First; subsec; subsec = subsec->Next )
		Template_int_RunSec(State, subsec, &ptr);
}

int Template_int_RunSec(t_obj_Template *State, t_tplop *Section, void **ValuePtr)
{
	 int	rv;
	void	*ptr = NULL;
	t_map_entry	*val;

	if( !Section )
		return 0;	

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
		val = Template_int_GetMapItem(&State->ValueMap, Section->Value.Name);
		if( !val )	return 0;
		switch(val->Type)
		{
		case 0:	*ValuePtr = NULL;	return 0;	// Unset
		case 1:	*ValuePtr = &val->SubMap;	return 1;	// Vector
		case 2:	*ValuePtr = val->String;	return 2;	// String - return the string
		default:	*ValuePtr = NULL;	return -1;
		}
		return 0;
	
	case TPLOP_CONDITIONAL:
//		printf("Section->Conditional = {Condition:%p,True:%p,False:%p}\n",
//			Section->Conditional.Condition, Section->Conditional.True, Section->Conditional.False);

		Template_int_RunSec(State, Section->Conditional.Condition, &ptr);
		if(ptr)
			Template_int_RunSecList(State, Section->Conditional.True);
		else
			Template_int_RunSecList(State, Section->Conditional.False);
//		printf("Section->Conditional = Done, res = %i\n", !!ptr);
		return 0;
	case TPLOP_ITERATOR:
		rv = Template_int_RunSec(State, Section->Iterator.Array, &ptr);
		if( rv != 1 || ptr == NULL ) {
			Template_int_RunSecList(State, Section->Iterator.IfEmpty);
		}
		else {
			t_map_entry	*ent;
			for( ent = ptr; ent; ent = ent->Next )
			{
				// TODO: Assign value into map according to stated name
				// TODO: Run body for each entry
				Template_int_RunSecList(State, Section->Iterator.PerItem);
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

