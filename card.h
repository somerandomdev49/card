#ifndef CARD_H
#define CARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "debug.h"

typedef struct Cell Cell;
typedef struct Value *Value;
struct FuncDecl { char *name; Cell *cell; };
typedef struct 
{
	int length;
	struct FuncDecl *funcs;
} FunctionContext;

enum
{
	VT_NUM, VT_LIST, VT_FUNC, VT_CELL
};

struct Value {
	char type;
	union
	{
		double number;
		struct { int len; struct Value **data; } list;
		struct { char is_native : 1; union { Value(*f)(Value,FunctionContext*); char *u; }; } func;
		Cell *cell;
	};
};
struct Cell { Value(*f)(Value,FunctionContext*); char is_c; Value c; Cell *next; };
Value eval(Value v, Cell *cell, FunctionContext *fc);

void free_value(Value val);
void free_cells(Cell *cell)
{
	LOG_GROUP_BEGIN_MSG("Free Cells");
	Cell *tmp;
	while(cell != NULL)
	{
		LOG_GROUP_BEGIN_MSG("Loop");
		LOGF("Freeing %p is constant: %d.", cell, cell->is_c);
		tmp = cell;
		cell = cell->next;
		if(tmp->is_c) if(tmp->c->type == VT_FUNC) goto end; else free_value(tmp->c);
		free(tmp);
		end:
		LOG_GROUP_END_MSG("Done loop");
	}
	LOG_GROUP_END_MSG("Done free cells");
}

void display_value(Value root)
{
	#ifdef DEBUG
	printf("%d: ", root->type);
	#endif
	if(root->type == 0)
		printf("%f", root->number); 
	if(root->type == 1)
	{
		putchar('(');
		for(int i=0;i<root->list.len;++i) { display_value(root->list.data[i]); putchar(' '); }
		putchar(')');
	}
	if(root->type == 2)
	{
		printf("[Function %s]", root->func.u);
	}
}

void free_value(Value val)
{
	LOGF("Free value %p of type %i", val, val->type);
	LOGNONL("Value: "); display_value(val); LOGC('\n');
	if(val->type == VT_LIST) for(int i=0;i<val->list.len;++i) free_value(val->list.data[i]);
	if(val->type == VT_CELL) free_cells(val->cell);
	if(val->type == VT_FUNC) return;
	free(val);
}

#ifdef DEBUG
Value card__print(Value v, FunctionContext *fc);
#endif


Value create_list(int length)
{
	Value v = malloc(sizeof(Value));
	v->type = VT_LIST;
	v->list.len = length;
	v->list.data = malloc(sizeof(Value) * v->list.len);
	return v;
}

void add_list(Value list, Value v)
{
	list->list.data = realloc(list->list.data, ++list->list.len);
	list->list.data[list->list.len-1] = v;
}

Value create_number(double value)
{
	Value v = malloc(sizeof(Value));
	v->type = VT_NUM;
	v->number = value;
	return v;
}

Value create_cell_value(Cell *cell)
{
	Value v = malloc(sizeof(Value));
	v->type = VT_CELL;
	v->cell = cell;
	return v;
}

Value create_native_function(Value(*f)(Value,FunctionContext*))
{
	Value v = malloc(sizeof(Value));
	v->type = VT_FUNC;
	v->func.is_native = 1;
	v->func.f = f;
	return v;
}

Value create_user_function(char *name)
{
	Value v = malloc(sizeof(Value));
	v->type = VT_FUNC;
	v->func.is_native = 0;
	v->func.u = name;
	return v;
}

Cell *get_func_in(FunctionContext *fc,  char *name)
{
	LOGF("Finding function: %s", name);
	for(int i=0;i<fc->length;++i) if(strcmp(fc->funcs[i].name, name) == 0) return fc->funcs[i].cell;
	ERROR("Function not found")
}

Value run_value_function(Value func, Value input, FunctionContext *fc)
{
	if(func->type != 2) ERROR("Expected a function.");
	if(func->func.is_native) return func->func.f(input, fc);
	else                     return eval(input, get_func_in(fc, func->func.u), fc);
}

Cell create_cell(Value(*f)(Value,FunctionContext*), Cell *next)
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

Value copy_value(Value val)
{
	LOGF("Copy %p of type %i", val, val->type);
	switch (val->type)
	{
		case VT_NUM: return create_number(val->number);
		case VT_LIST:
		{
			Value l = create_list(val->list.len);
			for(int i=0;i<val->list.len;++i) l->list.data[i] = copy_value(val->list.data[i]);
			return l;
		}
		case VT_FUNC: return val->func.is_native? create_native_function(val->func.f) : create_user_function(val->func.u);
	}
	ERROR("Copy not implemented!");
}

Value eval(Value v, Cell *cell, FunctionContext *fc)
{
	if(cell->is_c)
	{
		if(cell->c->type == VT_FUNC)
			return cell->next? eval(run_value_function(cell->c, v, fc), cell->next, fc) : run_value_function(cell->c, v, fc);
		free_value(v);
		return cell->next? eval(cell->c, cell->next, fc) : cell->c;
	}
	LOGF("Cell function: %s", cell->f == &card__print ? "print" : "other");
	return cell->next? eval(cell->f(v, fc), cell->next, fc) : cell->f(v, fc);
}

#endif 
