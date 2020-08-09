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
#define EOF 0

typedef struct Token { int type; const char *val; } Token;
typedef struct { char *input; } Tokenizer;
enum
{
	TT_VAR, TT_NUM, TT_EOF, TT_ERR
};

Token next_token(Tokenizer *t)
{
	if(*t->input == EOF) return (Token){TT_EOF, "<eof>"};
	while(*t->input != EOF && isspace(*t->input)) ++t->input;
	if(*t->input != EOF && (isalpha(*t->input) || *t->input == '_'))
	{
		size_t start = t->input;
		while(*t->input != EOF && (isalnum(*t->input) || *t->input == '_')) ++t->input;
		size_t len = t->input - start;
		LOGF("len: %d", len);
		LOGF(" %s ", t->input-len);
		LOGC('\\');for(int i=0;i<len;++i)LOGC('_');LOG("/");
		char *s = malloc(len + 1);
		memcpy(s, t->input - len, len);
		s[len] = 0;
		LOGF("'%s'\n\n", s);
		LOGF("-> '%s' ->\n", t->input);
		return (Token){TT_VAR, s};
	}
}

int all_tokens(Tokenizer *t, Token **toks)
{
	*toks = malloc(0); int len = 0;
	while(t->input[0] != EOF)
	{
		*toks = realloc(*toks, ++len);
		(*toks)[len-1] = next_token(t);
	}
	return len;
}

#endif
