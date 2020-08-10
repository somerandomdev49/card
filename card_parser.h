#ifndef CARD_PARSER_H
#define CARD_PARSER_H
#include "card.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>
#define LOG(x) puts(x);
#define LOGC(c) putc(c, stdout);
#define LOGF(x, ...) printf(x "\n", __VA_ARGS__);
#define IS_EOF(x) (x == 0 || x == EOF)

typedef struct Token { int type; char *val; } Token;
typedef struct ParserCell
{
	int type;
	union { double number; char *name; struct { size_t len; struct ParserCell *data; } list; };
	struct ParserCell *next;
} ParserCell;
typedef struct { char *input; } Tokenizer;
enum
{
	TT_VAR, TT_NUM, TT_EOF, TT_ERR,
	TT_OP, TT_CP, TT_EQ, TT_SC
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
			for(int i = 0; i < tmp->list.len; ++i) free_parser_cells(&tmp->list.data[i]);
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
		printf("%d", root->number); 
	if(root->type == 2)
	{
		LOGC("(\n");
		for(int i=0;i<root->list.len;++i) display_parser_cell_value(&root->list.data[i]);
		LOGC(")");
	}
}

void display_parser_cells(ParserCell *root)
{
	LOG("display");
	int level = 0;
	while(root != NULL)
	{
		for(int i=0;i<level;++i) LOGC(' ');
		display_parser_cell_value(root); LOGC('\n');
		root = root->next;
		level++;
	}
	LOG("display end");
}

Value generate_cell_value(ParserCell *ps)
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
		for(size_t i=0;i<ps->list.len;i++) v.list.data[i] = generate_cell_value(&ps->list.data[i]);
		return v;
	}
}

Cell *generate_cells(ParserCell *r)
{
	Cell *gen;
	Cell *c = gen;
	while(r != NULL)
	{
		if(r->type == 0)
		{
			if(strcmp(r->name, "print")) 
			{
				c = malloc(sizeof(Cell));
				*c = create_cell(*card__print, NULL);
			}
			if(strcmp(r->name, "add")) 
			{
				c = malloc(sizeof(Cell));
				*c = create_cell(*card__add, NULL);
			}
		}
		if(r->type == 1)
		{
			c = malloc(sizeof(Cell));
			*c = create_const(generate_cell_value(r), NULL);
		}
		if(r->type == 2)
		{
			c = malloc(sizeof(Cell));
			*c = create_const(generate_cell_value(r), NULL);
		}
		c = c->next;
		r = r->next;
	}
}

void free_cells(Cell *cell)
{
	Cell *tmp;
	while(cell != NULL)
	{
		tmp = cell;
		cell = cell->next;
		if(tmp->is_c) if(tmp->c.type == 2) free(tmp->c.list.data);
		free(tmp);
	}
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


void parse_expr(int *size, Token **toks, ParserCell *root)
{
	while((**toks).type != TT_SC)
	{
		LOG("while -> loop");
		if((**toks).type == TT_OP)
		{
			LOG("op");
			++*toks; --*size;
			ParserCell *l = malloc(sizeof(ParserCell));
			root->next = l;
			root = l;
			root->type = 2;
			root->list.data = malloc(0);
			root->list.len = 0;
			while((**toks).type != TT_CP)
			{
				root->list.data = realloc(root->list.data, sizeof(ParserCell) * ++root->list.len);
				parse_expr(size, toks, &root->list.data[root->list.len-1]);
			} ++*toks; --*size;
		}
		if((**toks).type == TT_VAR)
		{
			LOG("var");
			ParserCell *l = malloc(sizeof(ParserCell));
			root->next = l;
			root = l;
			root->next = NULL;
			root->type = 0;
			root->name = malloc(strlen((**toks).val) + 1);
			strcpy(root->name, (**toks).val);
			++*toks; --*size;
		}
		if((**toks).type == TT_NUM)
		{
			LOG("num");
			ParserCell *l = malloc(sizeof(ParserCell));
			root->next = l;
			root = l;
			root->next = NULL;
			root->type = 1;
			root->number = atof((**toks).val);
			++*toks; --*size;
		}
	} ++*toks; --*size;
	LOGF("-> end.\n=> '%s' (%d) ->", (**toks).val, *size)
}

ParserCell *parse(int *size, Token **toks)
{
	LOG("parse ->")
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
		parse_expr(size, toks, l);
		return l;
	}
}

#endif
