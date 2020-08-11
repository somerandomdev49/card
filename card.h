#ifndef CARD_H
#define CARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

typedef struct Cell Cell;
typedef struct Value Value;

typedef struct 
{
	int length;
	struct { char *name; Cell *cell; } funcs[];
} FunctionContext;

struct Value { // is value too big? it fits in 3 registers... (maybe?)
	char type;
	union
	{
		double number;
		struct { int len; struct Value *data; } list;
		struct { char is_native : 1; union { Value(*f)(Value); Cell *u; }; } func;
	};
};

struct Cell { Value(*f)(Value); char is_c; Value c; Cell *next; };

#ifdef DEBUG
Value card__print(Value v);
#endif
void free_cells(Cell *cell);
Value eval(Value v, Cell *cell);
void display_value(Value *root);
Value create_list(int length);
void add_list(Value *list, Value v);
Value create_number(double value);
Value create_native_function(Value(*f)(Value));
Value create_user_function(Cell *c);
Cell create_cell(Value(*f)(Value), Cell *next);
Cell create_const(Value v, Cell *next);
Value eval(Value v, Cell *cell);
Value run_value_function(Value func, Value input);



void free_cells(Cell *cell)
{
	LOG_GROUP_BEGIN_MSG("Free Cells");
	Cell *tmp;
	while(cell != NULL)
	{
		LOG_GROUP_BEGIN_MSG("Loop");
		LOGF("Freeing %p.", cell);
		tmp = cell;
		cell = cell->next;
		if(tmp->is_c) if(tmp->c.type == 1)
		{
			LOG("Constant list.");
			// for(int i=0;i<tmp->c.list.len;++i) free(tmp->c.list.data + i);
			free(tmp->c.list.data);
		}
		free(tmp);
		LOG_GROUP_END_MSG("Done loop");
	}
	LOG_GROUP_END_MSG("Done free cells");
}

void display_value(Value *root)
{
	#ifdef DEBUG
	printf("%d: ", root->type);
	#endif
	if(root->type == 0)
		printf("%f", root->number); 
	if(root->type == 1)
	{
		putchar('(');
		for(int i=0;i<root->list.len;++i) { display_value(&root->list.data[i]); putchar(' '); }
		putchar(')');
	}
}


Value create_list(int length)
{
	Value v;
	v.type = 1;
	v.list.len = length;
	v.list.data = malloc(sizeof(Value) * v.list.len);
	return v;
}

void add_list(Value *list, Value v)
{
	list->list.data = realloc(list->list.data, ++list->list.len);
	list->list.data[list->list.len-1] = v;
}

Value create_number(double value)
{
	Value v;
	v.type = 0;
	v.number = value;
	return v;
}

Value create_native_function(Value(*f)(Value))
{
	Value v;
	v.type = 2;
	v.func.is_native = 1;
	v.func.f = f;
	return v;
}

Value create_user_function(Cell *c)
{
	Value v;
	v.type = 2;
	v.func.is_native = 0;
	v.func.u = c;
	return v;
}

Value run_value_function(Value func, Value input)
{
	if(func.type != 2) ERROR("Expected a function.");
	if(func.func.is_native) return input.func.f(input);
	else                    return eval(input, input.func.u);
}

Cell create_cell(Value(*f)(Value), Cell *next)
{
	Cell c;
	c.is_c = 0;
	c.f = f;
	c.next = next;
	return c;
}

Cell create_const(Value v, Cell *next)
{
	Cell c;
	c.is_c = 1;
	c.c = v;
	c.next = next;
	return c;
}



Value eval(Value v, Cell *cell)
{
	if(cell->is_c)
	{
		if(v.type == 1) free(v.list.data);
		return cell->next? eval(cell->c, cell->next) : cell->c;
	}
	LOGF("Cell function: %s", cell->f == &card__print ? "print" : "add")
	return cell->next? eval(cell->f(v), cell->next) : cell->f(v);
}

#endif 
