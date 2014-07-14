/*
 * SpiderWeb
 * - By John Hodge (thePowersGang)
 * 
 * template/load.c
 * - Template Parsing
 */
#include "template_common.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// === PROTOTYPES ===
 int	Template_int_RunSec_Arith(t_obj_Template *State, struct s_tplop_arith *Arith, void **ValuePtr);
void	Template_int_RunSecList(t_obj_Template *State, t_tplop *First);
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
	case ARITHOP_CMPNE:
	case ARITHOP_CMPLT:
		*ValuePtr = "true";
		lt = Template_int_RunSec(State, Arith->Left, &lv);
		rt = Template_int_RunSec(State, Arith->Right, &rv);
		switch(Arith->Operation)
		{
		case ARITHOP_CMPEQ:
			if( lt != MAPENT_STRING && lt == rt )	return MAPENT_STRING;
			if( strcmp(lv, rv) == 0 )	return MAPENT_STRING;
			break;
		case ARITHOP_CMPNE:
			if( lt != MAPENT_STRING && lt != rt )	return MAPENT_STRING;
			if( strcmp(lv, rv) != 0 )	return MAPENT_STRING;
			break;
		case ARITHOP_CMPLT:
			if( lt != MAPENT_STRING && lt < rt )	return MAPENT_STRING;
			if( strcmp(lv, rv) < 0 )	return MAPENT_STRING;
			break;
		default:
			fprintf(stderr, "Template Error: Unimplimented Arith BinOp %i\n", Arith->Operation);
			break;
		}
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
		fprintf(stderr, "Template Error: Unimplimented BinOp %i\n", Arith->Operation);
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
		if( rv == MAPENT_STRING )
		{
			// Run filters on string
			// TODO: Break out into function?
			t_tplop_output_filter *f = Section->Output.Filters;
			if( f )
			{
				void *str;
				
				// Create SpiderString
				str = Template_int_CreateFilterString(State, ptr);
				
				// Process
				while( f )
				{
					Template_int_FilterString(State, &str, f->Name);
					f = f->Next;
				}
				
				const char *outstr;
				size_t outlen;
				// Get pointer/length of SpiderString
				Template_int_GetFilterString(State, str, &outstr, &outlen);
				fwrite(outstr, outlen, 1, stdout);
				
				// Free SpiderString
				Template_int_FreeFilterString(State, str);
			}
			else
			{
				// No filters, simple :)
				fputs(ptr, stdout);
			}
		}
		else
		{
			//fputs("UNSET", stdout);
		}
		return 0;
	
	case TPLOP_CONSTANT:
		*ValuePtr = Section->Constant.Data;
		return MAPENT_STRING;
	case TPLOP_CALLMACRO: {
		t_tplop_callmacro	*call = &Section->Call;
		// Create a new state
		t_obj_Template	substate = {0};
		t_tplmacro_param	*param = call->Macro->Params;
		t_tplop	*op;
		for( op = call->Args; op && param; op = op->Next )
		{
			t_map_entry	*ent = Template_int_AddMapItem_SubMap(&substate.LocalValues, param->Name);
			switch( Template_int_RunSec(State, op, &ptr) )
			{
			case MAPENT_VECTOR:
				ent->Type = MAPENT_VECTOR;
				ent->SubMap = *(t_map*)ptr;
				break;
			case MAPENT_STRING:
				//printf("%s = '%s'\n", param->Name, (char*)ptr);
				ent->Type = MAPENT_STRING;
				ent->String = ptr;
				break;
			case MAPENT_POINTER:
				ent->Type = MAPENT_POINTER;
				ent->Ptr = ptr;
				break;
			}
			param = param->Next;
		}
		if( op || param ) {
			// Oh... oops?
		}
		for( op = call->Macro->Sections; op; op = op->Next )
		{
			Template_int_RunSec(&substate, op, &ptr);
		}
		// Clean up
		for( t_map_entry *ent = substate.LocalValues.FirstEnt, *next; ent; ent = next )
		{
			next = ent->Next;
			free(ent);
		}
		substate.LocalValues.FirstEnt = NULL;
		return 0; }
	case TPLOP_GETVALUE:
//		printf("GetValue '%s'\n", Section->Value.Name);
		val = Template_int_GetMapItem(&State->IteratorValues, Section->Value.Name);
		if( !val )
			val = Template_int_GetMapItem(&State->LocalValues, Section->Value.Name);
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
			t_map_entry	*val = Template_int_AddMapItem_Ptr(
				&State->IteratorValues, Section->Iterator.ItemName, NULL);
			for( ent = ((t_map*)ptr)->FirstEnt; ent; ent = ent->Next )
			{
				val->Ptr = ent;
				Template_int_RunSecList(State, Section->Iterator.PerItem);
			}
			Template_int_DelMapItem(&State->IteratorValues, Section->Iterator.ItemName);
		}
		return 0;
	case TPLOP_ARITH:
		return Template_int_RunSec_Arith(State, &Section->Arith, ValuePtr);
	case TPLOP_ASSIGN:
		rv = Template_int_RunSec(State, Section->Assign.Value, &ptr);
		if( rv < 0 )	return rv;
		val = Template_int_GetMapItem(&State->LocalValues, Section->Assign.Name);
		if( val ) {
			// TODO: Make sure it's not the value we just calculated
			Template_int_DelMapItem(&State->LocalValues, Section->Assign.Name);
		}
		val = Template_int_DuplicateMapItem(&State->LocalValues, Section->Assign.Name, rv, ptr);
		if( !val ) {
			// Oops?
		}
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
	
	Template_int_FreeMap(&State->LocalValues);
}

