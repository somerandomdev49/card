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
	if(argc < 2) ERROR("Usage:\n\tcard <filename>");
	// const char *src =  " abc qwerty\0";
	// char str[strlen(src) + 1]; strcpy(str, src);
	char *str = read(argv[1]);
	if(!str) ERROR("Could not read the file :(\n");
	#if DEBUG
	printf("%s", str);
	#endif

	Tokenizer t = (Tokenizer){str};
	Token *toks;
	int size = all_tokens(&t, &toks);
	int size2 = size;
	free(str);

	#if DEBUG
	for(int i=0;i<size;i++)
		LOGF("'%s'", toks[i].val);
	#endif

	LOG("-=------------ PARSE ------------=-");
	ParserCell *ps = parse(&size, &toks);
	LOG_INDENT = 0;
	toks -= size2 - 1;
	LOG("-=------------ FREE TOKS ------------=-");
	for(int i=0;i<size;i++) free(toks[i].val);
	LOG("-=------------ DISPLAY ------------=-");
	#if DEBUG
	display_parser_cells(ps);
	#endif

	LOG("-=------------ GEN CELLS ------------=-");
	Cell *c = generate_cells(ps->next);
	LOG("-=------------ FREE PARSE ------------=-");
	FILE *nullout = fopen("/dev/null", "w");
	FILE *tmpout = stdout;
	stdout = nullout;
	free_parser_cells(ps);
	stdout = tmpout;

	LOG("-=------------ DISPLAY ------------=-");
	show_cells(c);
	LOG("-=------------ EVAL ------------=-");
	Value v = eval(create_number(argc), c);
	// display_value(&v); putchar('\n');
	LOG("-=------------ FREE ------------=-");
	free_cells(c);
	return 0;
}
