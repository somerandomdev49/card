#ifndef CARD_H
#define CARD_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define ERROR(msg) { printf("[%s]: %s", __FUNCTION__, msg); exit(1); }

typedef struct Value { int type; union { double number; struct { size_t len; struct Value *data; } list; }; } Value;
typedef struct Cell { Value(*f)(Value); char is_c; Value c; struct Cell *next; } Cell;

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

Value eval(Value v, Cell *cell) { if(cell->is_c) return cell->c; return cell->next? eval(cell->f(v), cell->next) : cell->f(v); }

#endif 
