#ifndef CARD_PARSER_H
#define CARD_PARSER_H
#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define IS_EOF(x) (x == 0 || x == EOF)

typedef struct Token { int type; char *val; } Token;
typedef struct ParserCell
{
	int type;
	union { double number; char *name; struct { size_t len; struct ParserCell **data; } list; };
	struct ParserCell *next;
} ParserCell;
typedef struct { char *input; } Tokenizer;
enum
{
	TT_VAR, TT_NUM, TT_EOF, TT_ERR,
	TT_OP, TT_CP, TT_EQ, TT_SC, TT_DT
};

void free_parser_cells(ParserCell *root)
{
	LOG("free");
	ParserCell *tmp;
	while(root != NULL)
	{
		LOG("free");
		tmp = root;
		LOGF("root: 0x%x", root)
		root = root->next;
		LOGF("%d: ", tmp->type); 
		if(tmp->type == 0) free(tmp->name);
		if(tmp->type == 2)
		{
			for(int i = 0; i < tmp->list.len; ++i) free_parser_cells(tmp->list.data[i]);
			free(tmp->list.data);
		}
		LOG("done freeing info");
		LOGF("tmp: 0x%x", tmp)
		free(tmp);
		LOG("done");
	}
	LOG("free end");
}

void display_parser_cell_value(ParserCell *root)
{
	printf("%d: ", root->type); 
	if(root->type == 0)
		printf("'%s'", root->name);
	if(root->type == 1)
		printf("%f", root->number); 
	if(root->type == 2)
	{
		LOGNONL("(");
		for(int i=0;i<root->list.len;++i) { display_parser_cell_value(root->list.data[i]); LOGC(' '); }
		LOGNONL(")");
	}
}

void display_parser_cells(ParserCell *root)
{
	LOG("display");
	int level = 0;
	while(root != NULL)
	{
		for(int i=0;i<level;++i) LOGC(' ');
		LOGF("root->next.ptr: 0x%x", root->next)
		for(int i=0;i<level;++i) LOGC(' ');
		display_parser_cell_value(root); LOGC('\n');
		root = root->next;
		level++;
	}
	LOG("display end");
}

Value generate_cell_value(ParserCell *ps, FunctionContext *fc)
{
	if(ps->type == 0)
	{
		ERROR("A name is not a value :(\n");
	}
	if(ps->type == 1)
	{
		return create_number(ps->number);
	}
	if(ps->type == 2)
	{
		Value v = create_list(ps->list.len);
		for(size_t i=0;i<ps->list.len;i++) v.list.data[i] = generate_cell_value(ps->list.data[i], fc);
		return v;
	}
	if(ps->type == 3)
	{
		if(strcmp(ps->name, "print") == 0) return create_native_function(card__print);
		if(strcmp(ps->name, "add") == 0) return create_native_function(card__add);
		if(strcmp(ps->name, "map") == 0) return create_native_function(card__map);
		if(strcmp(ps->name, "half") == 0) return create_native_function(card__half);
		// TODO: user defined functions...
	}
}

void show_cells(Cell *r)
{
	#ifdef DEBUG
	LOG_GROUP_BEGIN_MSG("Cell:");
	if(r->is_c) { LOGNONL("Constant "); display_value(&r->c); LOGC('\n'); }
	else LOGF("Func: %s", r->f == &card__print ? "print" : "add");
	LOG_GROUP_END_MSG("done");
	#endif
	if(r->next) show_cells(r->next);
}

Cell *generate_cells(ParserCell *r, FunctionContext *fc)
{
	LOG_GROUP_BEGIN_MSG("Gen:");
	Cell *gen;
	Cell **c = &gen;
	while(r != NULL)
	{
		*c = malloc(sizeof(Cell));
		LOG_GROUP_BEGIN_MSG("Gen Loop:");
		if(r->type == 0)
		{
			LOGF("name: %s", r->name);
			if(strcmp(r->name, "print") == 0)
			{
				**c = create_cell(*card__print, NULL);
			}
			if(strcmp(r->name, "add") == 0) 
			{
				**c = create_cell(*card__add, NULL);
			}
		}
		if(r->type == 1)
		{
			LOG("number");
			**c = create_const(generate_cell_value(r, fc), NULL);
		}
		if(r->type == 2)
		{
			LOG("list");
			**c = create_const(generate_cell_value(r, fc), NULL);
		}
		if(r->type == 3)
		{
			LOG("func");
			**c = create_const(generate_cell_value(r, fc), NULL);
		}
		c = &(*c)->next;
		r = r->next;
		LOG_GROUP_END_MSG("Gen Loop Done");
	}
	LOG_GROUP_END_MSG("Gen Done");
	return gen;
}

Token next_token(Tokenizer *t)
{
	while((!IS_EOF(*t->input)) && isspace(*t->input)) ++t->input;

	if(IS_EOF(*t->input)) return (Token){TT_EOF, "<eof>"};
	if((!IS_EOF(*t->input)) && (isalpha(*t->input) || *t->input == '_'))
	{
		size_t start = t->input;
		while((!IS_EOF(*t->input)) && (isalnum(*t->input) || *t->input == '_')) ++t->input;
		size_t len = t->input - start;
		// LOGF("len: %d", len);
		// LOGF(" %s ", t->input-len);
		// LOGC('\\');for(int i=0;i<len;++i)LOGC('_');LOG("/");
		char *s = malloc(len + 1);
		memcpy(s, t->input - len, len);
		s[len] = 0;
		// LOGF("'%s'\n\n", s);
		// LOGF("-> '%s' ->\n", t->input);
		return (Token){TT_VAR, s};
	}

	if((!IS_EOF(*t->input)) && isdigit(*t->input))
	{
		size_t start = t->input;
		while((!IS_EOF(*t->input)) && (isdigit(*t->input) || *t->input == '_')) ++t->input;

		if(*t->input == '.')
			while((!IS_EOF(*t->input)) && (isdigit(*t->input) || *t->input == '_')) ++t->input;

		size_t len = t->input - start;
		char *s = malloc(len + 1);
		memcpy(s, t->input - len, len);
		s[len] = 0;
		return (Token){TT_NUM, s};
	}

	if((!IS_EOF(*t->input)) && *t->input == '(') {++t->input; return (Token){TT_OP, "("}; };
	if((!IS_EOF(*t->input)) && *t->input == ')') {++t->input; return (Token){TT_CP, ")"}; };
	if((!IS_EOF(*t->input)) && *t->input == '=') {++t->input; return (Token){TT_EQ, "="}; };
	if((!IS_EOF(*t->input)) && *t->input == ';') {++t->input; return (Token){TT_SC, ";"}; };
	if((!IS_EOF(*t->input)) && *t->input == '.') {++t->input; return (Token){TT_DT, "."}; };

	printf("Unexpected character '%c'\n", *t->input);
	ERROR("Terminating...");
}

int all_tokens(Tokenizer *t, Token **toks)
{
	*toks = malloc(0);
	int len = 0;
	while(!IS_EOF(t->input[0]))
	{
		*toks = realloc(*toks, sizeof(Token) * ++len);
		(*toks)[len-1] = next_token(t);
	}
	return len;
}


ParserCell *parse_expr(int *size, Token **toks)
{
	ParserCell *root = malloc(sizeof(ParserCell));
	LOG_GROUP_BEGIN_MSG("parse -> expr");
	if((**toks).type == TT_OP)
	{
		LOG("op");
		++*toks; --*size;
		root->next = NULL;
		root->type = 2;
		root->list.data = malloc(0);
		root->list.len = 0;
		while((**toks).type != TT_CP)
		{
			root->list.data = realloc(root->list.data, sizeof(ParserCell*) * ++root->list.len);
			root->list.data[root->list.len-1] = parse_expr(size, toks);
		}
		++*toks; --*size;
		LOGF("end list. len: %d", root->list.len);
	}
	else if((**toks).type == TT_VAR)
	{
		LOG("var");
		root->next = NULL;
		root->type = 0;
		root->name = malloc(strlen((**toks).val) + 1);
		strcpy(root->name, (**toks).val);
		++*toks; --*size;
	}
	else if((**toks).type == TT_NUM)
	{
		LOGF("num: '%s'", (**toks).val);
		root->next = NULL;
		root->type = 1;
		LOGF("atof res: %f",atof((**toks).val))
		root->number = atof((**toks).val);
		LOGF("number: %f", root->number)
		++*toks; --*size;
	}
	if((**toks).type == TT_SC) { root->next = NULL; ++*toks; --*size; }
	else                         root->next = parse_expr(size, toks);
	LOGF("-> 0x%x -> end.", root)
	LOG_GROUP_END_MSGF("next: '%s' (%d) ->", (**toks).val, *size)
	return root;
}

ParserCell *parse(int *size, Token **toks)
{
	LOG_GROUP_BEGIN_MSG("parse ->")
	if((**toks).type == TT_VAR)
	{
		LOG("parse -> if")
		ParserCell *l = malloc(sizeof(ParserCell));
		l->type = 0;
		l->name = malloc(strlen((**toks).val) + 1);
		strcpy(l->name, (**toks).val);
		++*toks; --*size;
		++*toks; --*size; // =
		LOG("Parse if -> expression ->");
		l->next = parse_expr(size, toks);
		LOG_GROUP_END_MSG("done.");
		return l;
	}
}

#endif
