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
#include <setjmp.h>	// used for error handling
#include <assert.h>

typedef struct sParser
{
	const char	*Base;
	jmp_buf 	JmpTarget;
	
	struct {
		const char	*Pos;
		 int	Token;
		const char	*TokenStr;
		 int	TokenLen;
	} CurState, LastState, NextState;
} tParser;

enum eTokens
{
	TOK_NONE,
	TOK_EOF,
	
	TOK_VARIABLE,	TOK_IDENT,
	TOK_STRING,	TOK_INTEGER,

	TOK_QUESTION,	TOK_COLON,

	TOK_FIELD,
	TOK_SQUARE_O,	TOK_SQUARE_C,
	TOK_PAREN_O,	TOK_PAREN_C,

	TOK_NOT,
	TOK_AND, TOK_OR,
	TOK_CMPEQ, TOK_CMPNE,
	TOK_CMPLE, TOK_CMPLT,
	TOK_CMPGE, TOK_CMPGT,
};

// === PROTOTYPES ===
t_template	*Template_int_Load(const char *Filename);
void	Template_int_FreeSec(t_tplop *Section);
void	Template_int_Unload(t_template *Template);

// === CODE ===
void Template_int_PutBack(tParser *Parser)
{
//	printf("Putback\n");
	if( Parser->LastState.Token == -1 ) {
		return ;
	}
	
	Parser->NextState = Parser->CurState;
	Parser->CurState = Parser->LastState;
	Parser->LastState.Token = -1;
}

int Template_int_GetToken(tParser *Parser)
{
	 int	len = 0;
	 int	rv;
	
	if( Parser->NextState.Token ) {
		Parser->LastState = Parser->CurState;
		Parser->CurState = Parser->NextState;
		Parser->NextState.Token = 0;
		
//		printf("Fast got %i '%.*s'\n", Parser->CurState.Token,
//			Parser->CurState.TokenLen, Parser->CurState.TokenStr);
		return Parser->CurState.Token;
	}

	int issymchar(char ch)
	{
		if( 'a' <= ch && ch <= 'z' )	return 1;
		if( 'A' <= ch && ch <= 'Z' )	return 1;
		if( '0' <= ch && ch <= '9' )	return 1;
		if( '_' == ch )	return 1;
		return 0;
	}

	while( *Parser->CurState.Pos == ' ' )
		Parser->CurState.Pos++;
	
	Parser->LastState = Parser->CurState;

	Parser->CurState.TokenStr = Parser->CurState.Pos;
	switch( Parser->CurState.Pos[len++] )
	{
	case '$':	// Variable
		while( issymchar(Parser->CurState.Pos[len]) )
			len ++;
		rv = TOK_VARIABLE;
		break;
	case '"':	// String
		// TODO: Handle \"
		while( Parser->CurState.Pos[len] && Parser->CurState.Pos[len] != '"' )
			len ++;
		if( Parser->CurState.Pos[len] == '"' )
			len ++;
		rv = TOK_STRING;
		break;
	case '0'...'9':
		while( Parser->CurState.Pos[len] >= '0' && Parser->CurState.Pos[len] <= '9' )
			len ++;
		rv = TOK_INTEGER;
		break;
	
	case '=':
		switch( Parser->CurState.Pos[len++] )
		{
		case '=': rv = TOK_CMPEQ; break;
		default:  return -1;
		}
		break;
	case '<':
		switch( Parser->CurState.Pos[len++] )
		{
		case '=': rv = TOK_CMPLE; break;
		default:  rv = TOK_CMPLT; len --; break;
		}
		break;
	case '>':
		switch( Parser->CurState.Pos[len++] )
		{
		case '=': rv = TOK_CMPGE; break;
		default:  rv = TOK_CMPGT; len --; break;
		}
		break;
	case '!':
		switch( Parser->CurState.Pos[len++] )
		{
		case '=': rv = TOK_CMPNE; break;
		default:  rv = TOK_NOT; len --; break;
		}
		break;
	// Used for the ternary hack
	case '?':	rv = TOK_QUESTION;	break;
	case ':':	rv = TOK_COLON; 	break;

	case 'A'...'Z':
	case 'a'...'z':
	case '_':
		while( issymchar(Parser->CurState.Pos[len]) )
			len ++;
		rv = TOK_IDENT;
		break;
	case '\0':
		rv = TOK_EOF;
		len = 0;
		break;
	default:
		fprintf(stderr, "Unexpected character '%c'\n", Parser->CurState.Pos[len-1]);
		longjmp(Parser->JmpTarget, -1);
	}

//	printf("Read token %i '%.*s'\n", rv, len, Parser->CurState.Pos);
	Parser->CurState.Pos += len;
	Parser->CurState.TokenLen = len;
	Parser->CurState.Token = rv;
	return rv;
}

void Template_int_AppendOp(t_tplop **List, t_tplop *Ent)
{
	t_tplop	*prev;
	Ent->Next = NULL;
	
	for( prev = *List; prev && prev->Next; prev = prev->Next )
		;
	if(prev) {
//		printf("Placed %p at end of %p (prev=%p)\n", Ent, List, prev);
		prev->Next = Ent;
	}
	else {
//		printf("Placed %p as only in %p\n", Ent, List);
		*List = Ent;
	}
	
}

t_tplop *Template_int_NewOutput(t_tplop *Value)
{
	t_tplop_output	*out;
	
	out = malloc( sizeof(*out) );
	out->Type = TPLOP_VALUEOUT;
	out->Next = NULL;
	out->Value = Value;
	return (t_tplop*)out;
}

t_tplop *Template_int_ParseExpr(tParser *Parser)
{
	t_tplop *_Variable(const char *Name, int NameLen)
	{
		t_tplop_value	*ret;
		ret = malloc( sizeof(*ret) + NameLen + 1 );
		ret->Type = TPLOP_GETVALUE;
		ret->Next = NULL;
		memcpy(ret->Name, Name, NameLen);
		ret->Name[NameLen] = 0;
		return (t_tplop*)ret;
	}

	t_tplop *_String(const char *String, int Length)
	{
		t_tplop_const	*ret;
		ret = malloc( sizeof(*ret) + Length + 1 );
		ret->Type = TPLOP_CONSTANT;
		ret->Next = NULL;
		ret->Length = Length;
		memcpy(ret->Data, String, Length);
		ret->Data[Length] = 0;
		return (t_tplop*)ret;
	}
	
	t_tplop *_BinOp(enum e_arithops Op, t_tplop *Left, t_tplop *Right)
	{
		t_tplop_arith	*ret;
		ret = malloc( sizeof(*ret) );
		ret->Type = TPLOP_ARITH;
		ret->Next = NULL;
		ret->Operation = Op;
		ret->Left = Left;
		ret->Right = Right;
		return (t_tplop*)ret;
	}

	auto t_tplop *DoExprB(void);
	auto t_tplop *DoExpr0(void);
	
	t_tplop *DoExprVar(void)
	{
		t_tplop	*vop = _Variable(Parser->CurState.TokenStr+1, Parser->CurState.TokenLen-1);
		for( ;; )
		{
			switch( Template_int_GetToken(Parser) )
			{
			case TOK_FIELD:
				vop = _BinOp(ARITHOP_FIELD, vop, DoExprB());
				break;
			case TOK_SQUARE_O:
				vop = _BinOp(ARITHOP_INDEX, vop, DoExpr0());
				assert(Template_int_GetToken(Parser) == TOK_SQUARE_C);
				break;
			default:
				Template_int_PutBack(Parser);
				return vop;
			}
		}
	}

	t_tplop *DoExprV(void)
	{
		switch( Template_int_GetToken(Parser) )
		{
		case TOK_VARIABLE:
			return DoExprVar();
		case TOK_INTEGER:
			//return _Integer(Parser->CurState.TokenStr);
		case TOK_STRING:
			return _String(Parser->CurState.TokenStr+1, Parser->CurState.TokenLen-2);
		default:
			// TODO: Error
			fprintf(stderr, "Expected value (string/variable), got %i\n",
				Parser->CurState.Token);
			return NULL;
		}
	}
	
	t_tplop *DoExprB(void)
	{
		if( Template_int_GetToken(Parser) == TOK_PAREN_O )
		{
			t_tplop *rv;
			rv = DoExpr0();
			assert(Template_int_GetToken(Parser) == TOK_PAREN_C);
			return rv;
		}
		else {
			Template_int_PutBack(Parser);
			return DoExprV();
		}
	}

	t_tplop *DoExpr2(void)
	{
		t_tplop *rv = DoExprB();
		switch(Template_int_GetToken(Parser))
		{
		case TOK_NOT:	rv = _BinOp(ARITHOP_NOT, rv, NULL);	break;
		default:
			Template_int_PutBack(Parser);
			break;
		}
		return rv;
	}

	t_tplop *DoExpr1(void)
	{
		#define _next	DoExpr2
		t_tplop *rv = _next();
		for( ;; )
		{
			switch(Template_int_GetToken(Parser))
			{
			case TOK_AND:	rv = _BinOp(ARITHOP_AND, rv, _next());	break;
			case TOK_OR:	rv = _BinOp(ARITHOP_OR,  rv, _next());	break;
			default:
				Template_int_PutBack(Parser);
				return rv;
			}
		}
		#undef _next
	}
	t_tplop *DoExpr0(void)
	{
		#define _next	DoExpr1
		t_tplop *rv = _next();
		for( ;; )
		{
			switch(Template_int_GetToken(Parser))
			{
			case TOK_CMPEQ:	rv = _BinOp(ARITHOP_CMPEQ, rv, _next());	break;
			case TOK_CMPNE:	rv = _BinOp(ARITHOP_CMPNE, rv, _next());	break;
			case TOK_CMPLT:	rv = _BinOp(ARITHOP_CMPLT, rv, _next());	break;
			case TOK_CMPLE:	rv = _BinOp(ARITHOP_CMPLE, rv, _next());	break;
			case TOK_CMPGT:	rv = _BinOp(ARITHOP_CMPGT, rv, _next());	break;
			case TOK_CMPGE:	rv = _BinOp(ARITHOP_CMPGE, rv, _next());	break;
			default:
				Template_int_PutBack(Parser);
				return rv;
			}
		}
		#undef _next
	}

	return DoExpr0();
}

#define NEST_STACK_SIZE	10
typedef struct s_pstackent
{
	void	*Op;
	t_tplop	**List;
} t_pstackent;

t_tplop **Template_int_ParseStatement(t_tplop **List, const char *Data, t_pstackent Stack[])
{
	tParser	parser_state = {0};
	tParser	*Parser = &parser_state;

	Parser->Base = Data;
	Parser->CurState.Pos = Data;
	
	if( setjmp(Parser->JmpTarget) != 0 )
	{
		// Oops?
		return NULL;
	}

	// Quick hack for 'ternary'
	if( *Data == '?' )
	{
		t_tplop_cond	*cond;
			
		cond = malloc( sizeof(*cond) );
		cond->Type = TPLOP_CONDITIONAL;
		cond->Next = NULL;

		Parser->CurState.Pos ++;
	
		cond->Condition = Template_int_ParseExpr(Parser);
		assert( Template_int_GetToken(Parser) == TOK_QUESTION );
		
		cond->True = Template_int_NewOutput(Template_int_ParseExpr(&parser_state));
		assert( Template_int_GetToken(Parser) == TOK_COLON );
		
		cond->False = Template_int_NewOutput(Template_int_ParseExpr(&parser_state));		
		assert( Template_int_GetToken(Parser) == TOK_EOF );
	
		Template_int_AppendOp(List, (t_tplop*)cond);
		return List;
	}

	switch( Template_int_GetToken(&parser_state) )
	{
	#define CMPTOK(str)	(parser_state.CurState.TokenLen == sizeof(str)-1 && strncmp(parser_state.CurState.TokenStr, str, parser_state.CurState.TokenLen) == 0)
	case TOK_IDENT:
//		printf("ident '%.*s'\n", parser_state.CurState.TokenLen, parser_state.CurState.TokenStr);
		// If block
		if( CMPTOK("if") ) {
			 int	i;
			for(i = 0; i < NEST_STACK_SIZE && Stack[i].Op; i ++);
			if(i == NEST_STACK_SIZE)	return NULL;
//			printf("{if} block\n");
			
			t_tplop_cond	*cond;
			cond = malloc( sizeof(*cond) );
			cond->Type = TPLOP_CONDITIONAL;
			cond->Next = NULL;
			
			cond->Condition = Template_int_ParseExpr(Parser);
			cond->True = NULL;
			cond->False = NULL;
			
			Stack[i].Op = cond;
			Stack[i].List = List;

			Template_int_AppendOp(List, (t_tplop*)cond);

			return &cond->True;
		}
		else if( CMPTOK("else") ) {
			 int	i;
			for(i = 0; i < NEST_STACK_SIZE && Stack[i].Op; i ++);
			if(i == 0) {
				fprintf(stderr, "{else} with no matching {if}\n");
				return NULL;
			}
			
			t_tplop_cond	*cond = Stack[i-1].Op;
			if(cond->Type != TPLOP_CONDITIONAL) {
				fprintf(stderr, "{else} on non {if} block\n");
				return NULL;
			}
			
			if( cond->False || List == &cond->False ) {
				fprintf(stderr, "Two {else} blocks\n");
				return NULL;
			}
			return &cond->False;
		}
		else if( CMPTOK("endif") ) {
			 int	i;
			for(i = 0; i < NEST_STACK_SIZE && Stack[i].Op; i ++);
			if(i == 0) {
				fprintf(stderr, "{endif} with no matching {if}\n");
				return NULL;
			}
			
			t_tplop_cond	*cond = Stack[i-1].Op;
			if(cond->Type != TPLOP_CONDITIONAL) {
				fprintf(stderr, "{endif} terminates non {if} block\n");
				return NULL;
			}
			Stack[i-1].Op = NULL;
			return Stack[i-1].List;
		}
		// Foreach block
		else if( CMPTOK("foreach") )
			;
		else if( CMPTOK("endforeach") )
			;
		else {
	default:
			Template_int_PutBack(&parser_state);
			// Output stuff
			Template_int_AppendOp( List, Template_int_NewOutput( Template_int_ParseExpr(&parser_state) ) );
		}
		break;
	#undef CMPTOK
	}

	return List;
}

static void _addVerbatim(t_tplop **List, const char *Data, size_t Size)
{
	struct s_tplop_const	*ent;

	if( Size == 0 )	return ;

//	printf("%i bytes of verbatim text\n", (int)Size);

	ent = malloc( sizeof(*ent) + Size );
	if( !ent ) {
		perror("malloc fail");
		return ;
	}
	ent->Next = NULL;
	ent->Type = TPLOP_CONSTOUT;
	ent->Length = Size;
	memcpy(ent->Data, Data, Size);
	
	Template_int_AppendOp(List, (t_tplop*)ent);
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
	t_pstackent	stack[NEST_STACK_SIZE];
	memset(stack, 0, sizeof(stack));

	fp = fopen(Filename, "r");
	if( !fp ) {
		perror("Opening template");
		return NULL;
	}

//	printf("Parsing '%s'\n", Filename);	

	ret = malloc( sizeof(*ret) );
	ret->Sections = NULL;
	listhead = &ret->Sections;

	ofs = 0;
	for( ;; )
	{
		int len = fread(buffer + ofs, 1, BUFSIZ - ofs, fp);
//		printf("Read %i to offset %i\n", len, ofs);

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

				if( i + 1 < ofs + len )
				{
					if( buffer[i+1] == '*' ) {
						// Comment
						_addVerbatim(listhead, buffer + ctrl_end, i - ctrl_end);
						ctrl_end = 0;
						for( j = i; j+1 < ofs+len; j ++ )
						{
							if( buffer[j] == '*' && buffer[j+1] == '}' ) {
								ctrl_end = j+2;
								break ;
							}
						}
						if( ctrl_end == 0 )
							break;	// Force more read
						i = ctrl_end;
						continue ;
					}
					else if( buffer[i+1] == ' ' ) {
						// Ignore if there's a space after the bracket
						continue ;
					}
				}

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
//				printf("Control statement: %s\n", ctrl_data);

				listhead = Template_int_ParseStatement(listhead, ctrl_data, stack);
				if( listhead == NULL ) {
					fprintf(stderr, "Something bad happened\n");
					fclose(fp);
					return ret;
				}

				ctrl_start = -1;
				ctrl_end += 1;
				i = ctrl_end;
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

