/*
 * 
 */
#include "template_common.h"
#include <string.h>
#include <spiderscript.h>

// === CODE ===
void *Template_int_CreateFilterString(t_obj_Template *State, const char *InputString)
{
	return SpiderScript_CreateString(strlen(InputString), InputString);
}

void Template_int_FreeFilterString(t_obj_Template *State, void *String)
{
	SpiderScript_DereferenceString(String);
}

int Template_int_FilterString(t_obj_Template *State, void **StringPtr, const char *FilterName)
{
	t_filter	*filter;
	
	// Locate filter binding
	for( filter = State->Filters; filter; filter = filter->Next )
	{
		if( strcmp(filter->Name, FilterName) == 0 )
			break ;
	}
	if( !filter )
		return -1;
	
	// Execute
	tSpiderString	*retval;
	tSpiderTypeRef	rettype;
	tSpiderTypeRef	argtypes[] = {{&gSpiderScript_StringType,0}};
	const void	*args[] = {*StringPtr};
	 int	rv = SpiderScript_ExecuteFunction(State->Script, filter->BoundFcn,
		&rettype, &retval, 1, argtypes, args, &filter->FcnPtr);
	if( rv != 0 || !SS_ISCORETYPE(rettype, SS_DATATYPE_STRING) )
	{
		// uhh...
		return -1;
	}

	// Clean up
	SpiderScript_DereferenceString(*StringPtr);
	*StringPtr = retval;
	
	return 0;
}

void Template_int_GetFilterString(t_obj_Template *State, void *String, const char **DataPtr, size_t *LenPtr)
{
	*DataPtr = ((tSpiderString*)String)->Data;
	*LenPtr = ((tSpiderString*)String)->Length;
}
