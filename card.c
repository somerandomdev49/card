#include "card.h"
#include "card_stdlib.h"
#include "card_parser.h"

char *read(const char *filename)
{
	FILE *f = fopen(filename, "r");
	if(f)
	{
		fseek(f, 0, SEEK_END);
		size_t length = ftell(f);
		fseek (f, 0, SEEK_SET);
		char *buffer = malloc(length + 1);
		buffer[length] = '\0';
		if(buffer) fread(buffer, 1, length, f);
		fclose(f);
		return buffer;
	}
	return NULL;
}

int main(int argc, char **argv)
{
	
	// const char *src =  " abc qwerty\0";
	// char str[strlen(src) + 1]; strcpy(str, src);
	char *str = read("./example.card");
	if(!str) ERROR("Could not read the file :(\n");
	printf("%s", str);

	Tokenizer t = (Tokenizer){str};
	Token *toks;
	int size = all_tokens(&t, &toks);
	free(str);

	for(int i=0;i<size;i++)
	{
		printf("'%s'\n", toks[i].val);
	}
	
	Cell c01, c02;
	c01	= create_cell(&card__add, &c02);
	c02 = create_cell(&card__print, NULL);
	
	Value input = create_list(2);
	input.list.data[0] = create_number(2);
	input.list.data[1] = create_number(5);
	
	eval(input, &c01);
	free(input.list.data);
	return 0;
}
