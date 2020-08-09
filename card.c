#include "card.h"
#include "card_stdlib.h"
#include "card_parser.h"

int main(int argc, char **argv)
{
	const char *src =  " abc qwerty\0";
	char str[strlen(src) + 1]; strcpy(str, src);
	
	Tokenizer t = (Tokenizer){str};
	Token *toks;
	int size = all_tokens(&t, &toks);
	
	for(int i=0;i<size;i++)
	{
		printf("'%s'\n", toks[i].val);
	}
	
	Cell c01, c02;
	c01	= create_cell(&card__add, &c02);
	c02 = create_cell(&card__print, NULL);
	
	Value input = create_list(2);
	*(input.list.data+0) = create_number(2);
	*(input.list.data+1) = create_number(5);
	
	eval(input, &c01);
	free(input.list.data);
	return 0;
}
