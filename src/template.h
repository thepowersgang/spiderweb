/*
 * SpiderWeb
 * - By John Hodge (thePowersGang)
 * 
 * template.h
 * - Templating Engine
 */
#ifndef _TEMPLATE_H_
#define _TEMPLATE_H_

typedef struct s_map_entry	t_map_entry;

/**
 * \brief Key-Value Map
 */
struct s_map_entry {
	t_map_entry	*Next;
	char	*Key;	// Key is stored after value
	 int	Type;	// 0: Unset, 1 = Vector, 2: String
	union {
		t_map_entry	*FirstChild;
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
	
	ARITHOP_ADD, ARITHOP_SUB,
	ARITHOP_MUL, ARITHOP_DIV,
	ARITHOP_MOD,
	
	ARITHOP_INDEX, ARITHOP_FIELD
};


typedef union u_tplop  	t_tplop;
typedef struct s_tplop_verb	t_tplop_verb;
typedef struct s_tplop_value	t_tplop_value;
typedef struct s_tplop_iter	t_tplop_iter;
typedef struct s_tplop_cond	t_tplop_cond;
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



#endif

