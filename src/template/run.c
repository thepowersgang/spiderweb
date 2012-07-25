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
		if( lt != MAPENT_STRING && lt != rt )	return 0;
		if( strcmp(lv, rv) != 0 )	return 0;
		*ValuePtr = "true";
		return MAPENT_STRING;
	
	case ARITHOP_CMPNE:
		*ValuePtr = "true";
		lt = Template_int_RunSec(State, Arith->Left, &lv);
		rt = Template_int_RunSec(State, Arith->Right, &rv);
		if( lt != MAPENT_STRING && lt != rt )	return MAPENT_STRING;
		if( strcmp(lv, rv) != 0 )	return MAPENT_STRING;
		*ValuePtr = NULL;
		return 0;
	
	case ARITHOP_FIELD:
		lt = Template_int_RunSec(State, Arith->Left, &lv);
		rt = Template_int_RunSec(State, Arith->Right, &rv);

		if( lt != MAPENT_VECTOR || rt != MAPENT_STRING ) {
			return 0;
		}	

		t_map_entry *ent = Template_int_GetMapItem(lv, rv);
		if( !ent ) {
			*ValuePtr = NULL;
			return 0;
		}
		switch(ent->Type)
		{
		case MAPENT_STRING:
			*ValuePtr = ent->String;
			break;
		case MAPENT_VECTOR:
			*ValuePtr = &ent->SubMap;
			break;
		case MAPENT_POINTER:
		case MAPENT_UNSET:
			*ValuePtr = NULL;
			return 0;
		}
		return ent->Type;
	default:
		printf("Template Error: Unimplimented BinOp %i\n", Arith->Operation);
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
		// TODO: Filters?
		rv = Template_int_RunSec(State, Section->Output.Value, &ptr);
		if( rv == MAPENT_STRING )	fputs(ptr, stdout);
		return 0;
	
	case TPLOP_CONSTANT:
		*ValuePtr = Section->Constant.Data;
		return MAPENT_STRING;
	case TPLOP_GETVALUE:
//		printf("GetValue '%s'\n", Section->Value.Name);
		val = Template_int_GetMapItem(&State->IteratorValues, Section->Value.Name);
		if( !val )
			val = Template_int_GetMapItem(&State->ValueMap, Section->Value.Name);
	getvalue_retry:
//		printf("val = %p\n", val);
		if( !val )	return 0;
		switch(val->Type)
		{
		case MAPENT_UNSET:	*ValuePtr = NULL;	break;	// Unset
		case MAPENT_VECTOR:	*ValuePtr = &val->SubMap;	break;	// Vector
		case MAPENT_STRING:	*ValuePtr = val->String;	break;	// String
		case MAPENT_POINTER:	val = val->Ptr;	goto getvalue_retry;	// Indirect
		default:	*ValuePtr = NULL;	fprintf(stderr, "Unknown %i\n", val->Type);	return -1;
		}
//		printf("ret %i\n", val->Type);
		return val->Type;
	
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
		if( rv != MAPENT_VECTOR || ptr == NULL || !((t_map*)ptr)->FirstEnt ) {
			Template_int_RunSecList(State, Section->Iterator.IfEmpty);
		}
		else {
			t_map_entry	*ent;
			t_map_entry	*val = Template_int_AddMapItem_Ptr(&State->IteratorValues, Section->Iterator.ItemName, NULL);
//			printf("Iterating into '%s'\n", Section->Iterator.ItemName);
			for( ent = ((t_map*)ptr)->FirstEnt; ent; ent = ent->Next )
			{
//				printf("Item %p '%s'\n", ent, ent->Key);
				val->Ptr = ent;
				Template_int_RunSecList(State, Section->Iterator.PerItem);
			}
			Template_int_DelMapItem(&State->IteratorValues, Section->Iterator.ItemName);
		}
		return 0;
	case TPLOP_ARITH:
		return Template_int_RunSec_Arith(State, &Section->Arith, ValuePtr);
	case TPLOP_ASSIGN:
		// TODO: Support {assign $var = <value>}
		return 0;
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

