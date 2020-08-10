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
	int size2 = size;
	free(str);

	for(int i=0;i<size;i++)
		printf("'%s'\n", toks[i].val);

	ParserCell *ps = parse(&size, &toks);
	toks -= size2 - 1;
	for(int i=0;i<size;i++) free(toks[i].val);
	display_parser_cells(ps);

	Cell *c = generate_cells(ps);
	free_parser_cells(ps);

	eval(create_number(argc), c);
	free_cells(c);
	return 0;
}
