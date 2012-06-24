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

typedef struct s_map_entry	t_map_entry;
typedef struct s_map	t_map;

struct s_map {
	t_map_entry	*FirstEnt;
//	t_map_entry	*LastEnt;
	// TODO: Hash table?
};

typedef struct {
	t_map	ValueMap;
	t_map	IteratorValues;
} t_obj_Template;

enum e_map_entry_types
{
	MAPENT_UNSET,
	MAPENT_VECTOR,	// Vector
	MAPENT_STRING,	// String
	MAPENT_POINTER	// Indirect vector
};

/**
 * \brief Key-Value Map
 */
struct s_map_entry {
	t_map_entry	*Next;
	char	*Key;	// Key is stored after value
	enum e_map_entry_types	Type;	// 0: Unset, 1: Vector, 2: String, 3: Indirect vector
	union {
		t_map	SubMap;
		t_map_entry	*Ptr;
		 int	Integer;
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
typedef struct s_tplop_iter	t_tplop_iter;
typedef struct s_tplop_cond	t_tplop_cond;
typedef struct s_tplop_arith	t_tplop_arith;
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

struct s_tplop_output
{
	S_TPLSEC_HDR
	t_tplop	*Value;
};

struct s_tplop_iter
{
	S_TPLSEC_HDR
	union u_tplop	*Array;
	char	*ItemName;
	union u_tplop	*PerItem;
	union u_tplop	*IfEmpty;
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

union u_tplop
{
	struct {
		S_TPLSEC_HDR
	};
	struct s_tplop_const	Constant;
	struct s_tplop_value	Value;
	struct s_tplop_output	Output;
	struct s_tplop_iter	Iterator;
	struct s_tplop_cond	Conditional;
	struct s_tplop_arith	Arith;
};

struct s_template
{
	t_tplop	*Sections;
};


extern t_map_entry	*Template_int_GetMapItem(t_map *Map, const char *Key);
extern void	Template_int_DelMapItem(t_map *Map, const char *Key);
extern t_map_entry	*Template_int_AddMapItem_Ptr(t_map *Map, const char *Key, t_map_entry *Ptr);
extern t_map_entry	*Template_int_AddMapItem_SubMap(t_map *Map, const char *Key);
extern t_map_entry	*Template_int_AddMapItem_String(t_map *Map, const char *Key, const char *String);
extern void	Template_int_FreeMap(t_map *Map);

extern void	Template_int_Unload(t_template *Template);
extern t_template	*Template_int_Load(const char *Filename);
extern void	Template_int_Output(t_obj_Template *State, t_template *Template);

#endif
