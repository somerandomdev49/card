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

typedef struct Token { int type; const char *val; } Token;
typedef struct { char *input; } Tokenizer;
enum
{
	TT_VAR, TT_NUM, TT_EOF, TT_ERR
};

Token next_token(Tokenizer *t)
{
	while((!IS_EOF(*t->input)) && isspace(*t->input)) ++t->input;

	if(IS_EOF(*t->input)) return (Token){TT_EOF, "<eof>"};
	if((!IS_EOF(*t->input)) && (isalpha(*t->input) || *t->input == '_'))
	{
		size_t start = t->input;
		while((!IS_EOF(*t->input)) && (isalnum(*t->input) || *t->input == '_')) ++t->input;
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
	*toks = malloc(0);
	int len = 0;
	while(!IS_EOF(t->input[0]))
	{
		*toks = realloc(*toks, sizeof(Token) * ++len);
		(*toks)[len-1] = next_token(t);
	}
	return len;
}

#endif
