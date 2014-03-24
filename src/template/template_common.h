/*
 * SpiderWeb
 * - By John Hodge (thePowersGang)
 * 
 * template.h
 * - Templating Engine
 */
#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

#include <stddef.h>

#define NEW(type, extra)	((type*)calloc(1,sizeof(type)extra))

typedef struct s_map_entry	t_map_entry;
typedef struct s_map	t_map;
typedef struct s_filter	t_filter;

struct s_map {
	t_map_entry	*FirstEnt;
//	t_map_entry	*LastEnt;
	// TODO: Hash table?
};

struct s_filter {
	struct s_filter	*Next;
	char	*Name;
	char	*BoundFcn;
	void	*FcnPtr;
	// ...
};

typedef struct {
	struct sSpiderScript	*Script;
	t_map	ValueMap;
	t_map	IteratorValues;
	t_map	LocalValues;
	t_filter	*Filters;
} t_obj_Template;

enum e_map_entry_types
{
	MAPENT_UNSET,
//	MAPENT_INTEGER,
	MAPENT_VECTOR,	// Vector
	MAPENT_STRING,	// String
	MAPENT_POINTER	// Indirect value
};

/**
 * \brief Key-Value Map
 */
struct s_map_entry {
	t_map_entry	*Next;
	char	*Key;	// Key is stored after value
	enum e_map_entry_types	Type;
	union {
		t_map	SubMap;
		t_map_entry	*Ptr;
//		 int	Integer;
		char	*String;
	};
	char	Data[];
};


enum e_tplop_types
{
	TPLOP_CONSTOUT,
	TPLOP_VALUEOUT,
	TPLOP_ITERATOR,
	TPLOP_CONDITIONAL,
	TPLOP_ASSIGN,
	TPLOP_CALLMACRO,
	
	TPLOP_CONSTANT,
	TPLOP_GETVALUE,
	TPLOP_ARITH,
};

enum e_arithops
{
	ARITHOP_CMPEQ, ARITHOP_CMPNE,
	ARITHOP_CMPLT, ARITHOP_CMPGE,
	ARITHOP_CMPGT, ARITHOP_CMPLE,

	ARITHOP_NOT,
	ARITHOP_AND, ARITHOP_OR,
		
	ARITHOP_ADD, ARITHOP_SUB,
	ARITHOP_MUL, ARITHOP_DIV,
	ARITHOP_MOD,
	
	ARITHOP_INDEX, ARITHOP_FIELD
};


typedef union u_tplop  	t_tplop;
typedef struct s_tplop_const	t_tplop_const;
typedef struct s_tplop_value	t_tplop_value;
typedef struct s_tplop_output	t_tplop_output;
typedef struct s_tplop_output_filter	t_tplop_output_filter;
typedef struct s_tplop_assign	t_tplop_assign;
typedef struct s_tplop_iter	t_tplop_iter;
typedef struct s_tplop_cond	t_tplop_cond;
typedef struct s_tplop_arith	t_tplop_arith;
typedef struct s_tplop_callmacro	t_tplop_callmacro;
typedef struct s_template	t_template;

#define S_TPLSEC_HDR	t_tplop *Next; enum e_tplop_types Type;
struct s_tplop_const
{
	S_TPLSEC_HDR
	size_t	Length;
	char	Data[];
};

struct s_tplop_value
{
	S_TPLSEC_HDR
	char	Name[];
};

struct s_tplop_output_filter
{
	t_tplop_output_filter *Next;
	// Arguments?
	char	Name[];
};

struct s_tplop_output
{
	S_TPLSEC_HDR
	t_tplop	*Value;
	t_tplop_output_filter	*Filters;
};

struct s_tplop_assign
{
	S_TPLSEC_HDR
	union u_tplop	*Value;
	char	Name[];
};

struct s_tplop_iter
{
	S_TPLSEC_HDR
	union u_tplop	*Array;
	union u_tplop	*PerItem;
	union u_tplop	*IfEmpty;
	char	ItemName[];
};

struct s_tplop_cond
{
	S_TPLSEC_HDR
	union u_tplop	*Condition;
	union u_tplop	*True;
	union u_tplop	*False;
};

struct s_tplop_arith
{
	S_TPLSEC_HDR
	enum e_arithops	Operation;
	union u_tplop	*Left;
	union u_tplop	*Right;
};

struct s_tplop_callmacro
{
	S_TPLSEC_HDR
	union u_tplop	*Args;
	struct s_tplmacro	*Macro;
};

union u_tplop
{
	struct {
		S_TPLSEC_HDR
	};
	struct s_tplop_const	Constant;
	struct s_tplop_value	Value;
	struct s_tplop_output	Output;
	struct s_tplop_assign	Assign;
	struct s_tplop_iter	Iterator;
	struct s_tplop_cond	Conditional;
	struct s_tplop_arith	Arith;
	struct s_tplop_callmacro	Call;
};

typedef struct s_tplmacro	t_tplmacro;
typedef struct s_tplmacro_param	t_tplmacro_param;

struct s_tplmacro_param
{
	t_tplmacro_param	*Next;
	char	Name[];
};

struct s_tplmacro
{
	t_tplmacro	*Next;
	t_tplop	*Sections;
	t_tplmacro_param	*Params;
	t_tplmacro_param	*Params_End;
	char	Name[];
};

struct s_template
{
	t_tplop	*Sections;
	t_tplmacro	*Macros;
};


extern t_map_entry	*Template_int_GetMapItem(t_map *Map, const char *Key);
extern void	Template_int_DelMapItem(t_map *Map, const char *Key);
extern t_map_entry	*Template_int_DuplicateMapItem(t_map *Map, const char *Key, int Type, void *Ptr);
extern t_map_entry	*Template_int_AddMapItem_SubMap(t_map *Map, const char *Key);
extern t_map_entry	*Template_int_AddMapItem_Ptr(t_map *Map, const char *Key, t_map_entry *Ptr);
extern t_map_entry	*Template_int_AddMapItem_String(t_map *Map, const char *Key, size_t Len, const char *String);
//extern t_map_entry	*TemplatE_int_AddMapItem_Integer(t_map *Map, const char *Key, int Value);
extern void	Template_int_FreeMap(t_map *Map);

extern void	Template_int_FreeSec(t_tplop *Section);
extern void	Template_int_Unload(t_template *Template);
extern t_template	*Template_int_Load(const char *Filename);
extern void	Template_int_Output(t_obj_Template *State, t_template *Template);


extern void	*Template_int_CreateFilterString(t_obj_Template *State, const char *InputString);
extern void	Template_int_FreeFilterString(t_obj_Template *State, void *String);
extern int	Template_int_FilterString(t_obj_Template *State, void **StringPtr, const char *FilterName);
extern void	Template_int_GetFilterString(t_obj_Template *State, void *String, const char **DataPtr, size_t *LenPtr);

#endif

