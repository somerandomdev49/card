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

typedef struct Token { int type; char *val; } Token;

Token next_token(char *input)
{
	while(input && isspace(*input)) ++input;
	if(input && (isalpha(*input) || *input == '_'))
	{
		LOG("h1")
		size_t start = input;
		while(input && (isalnum(*input) || *input == '_')) ++input;
		size_t len = input - start;
		LOGF("len: %d", len);
		LOGF("'%s'", input-len);
		LOGC(' ');for(int i=0;i<len;++i)LOGC(' ');LOG("^");
		char *s = malloc(len + 1);
		memcpy(s, input - len, len);
		LOGF("-> '%s'", s);
		LOG("h2")
		s[len] = 0;
		return (Token){0, s};
	}
}

#endif
