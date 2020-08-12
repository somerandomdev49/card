// #define DEBUG
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
	#ifdef DEBUG
	printf("%s", str);
	#endif

	Tokenizer t = (Tokenizer){str};
	Token *toks;
	int size = all_tokens(&t, &toks);
	int size2 = size;
	free(str);

	#ifdef DEBUG
	for(int i=0;i<size;i++)
		LOGF("'%s'", toks[i].val);
	#endif

	LOG("-=------------ PARSE ------------=-");
	ParserCell**parsed_funcs = malloc(0);
	int parsed_func_size = 0;
	while(toks[0].type != TT_EOF)
	{
		parsed_funcs = realloc(parsed_funcs, (++parsed_func_size) * sizeof(ParserCell*));
		parsed_funcs[parsed_func_size-1] = parse(&size, &toks);
	}
	LOG_INDENT = 0;
	toks -= size2 - 1;
	LOG("-=------------ FREE TOKS ------------=-");
	LOGF("size: %d", size2);
	for(int i=0;i<size2;i++)
	{
		LOGF("Free tok %d at %d", toks[i].type, i);
		if(toks[i].type == TT_VAR || toks[i].type == TT_NUM)
			free(toks[i].val);
	}
	free(toks);
	LOG("-=------------ DISPLAY ------------=-");
	#ifdef DEBUG
	for(int i=0;i<parsed_func_size;++i) display_parser_cells(parsed_funcs[i]);
	#endif
	LOG("-=------------ GEN CELLS ------------=-");
	Cell**funcs = malloc(0);
	int func_size = 0; 
	for(int i=0;i<parsed_func_size;++i)
	{ 
		funcs = realloc(funcs, (++func_size) * sizeof(Cell*));
		funcs[func_size-1] = generate_cells(parsed_funcs[i]->next);
	}

	LOG("-=------------ DISPLAY ------------=-");
	for(int i=0;i<func_size;++i) show_cells(funcs[i]);

	LOG("-=------------ EVAL ------------=-");
	
	FunctionContext fc = (FunctionContext){func_size, malloc(func_size*sizeof(Cell*))};
	for(int i=0;i<func_size;++i)
	{
		LOGF("%s -> %p", parsed_funcs[i]->name, funcs[i]);
		fc.funcs[i] = (struct FuncDecl){parsed_funcs[i]->name, funcs[i]};
	}

	Value v = eval(create_number(argc), get_func_in(&fc, "main"), &fc);
	free_value(v);
	LOG("-=------------ FREE ------------=-");
	for(int i=0;i<func_size;++i) { LOGF("Free function: %s", parsed_funcs[i]->name); free_cells(funcs[i]); }
	free(funcs);

	LOG("-=------------ FREE PARSE ------------=-");
	for(int i=0;i<parsed_func_size;++i) free_parser_cells(parsed_funcs[i]);
	free(parsed_funcs);

	LOG("-=------------ FUNC CTX ------------=-");
	// free(fc.funcs);

	LOG("-=------------ EXIT ------------=-");
	return 0;
}
