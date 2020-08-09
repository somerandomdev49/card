#include "card.h"
#include "card_stdlib.h"
//#include "card_parser.h"

int main(int argc, char **argv)
{
	//char str[strlen(src) + 1]; strcpy(str, " abc qwerty\0");
	
	//printf("%s\n\n", next_token(str).val);
	
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
