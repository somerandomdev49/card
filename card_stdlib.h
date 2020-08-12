#ifndef CARD_STDLIB_H
#define CARD_STDLIB_H
#include "card.h"
Value card__print(Value v, FunctionContext *fc)
{
	display_value(v); putchar('\n');
	return v; // look what i found:   \._./  \;-;/ \;o;/
}

Value card__map(Value i, FunctionContext *fc)
{	
	if(i->type != VT_LIST) ERROR("Expected a list");
	if(i->list.len < 2) ERROR("Expected at least 2 elements");
	if(i->list.data[0]->type != 2) ERROR("Expected element 0 to be a function");
	if(i->list.data[1]->type != 1) ERROR("Expected element 1 to be a list");
	Value v = create_list(i->list.data[1]->list.len);
	for(int j=0;j<v->list.len;++j)
		v->list.data[j] = run_value_function(i->list.data[0], i->list.data[1]->list.data[j], fc);
	free_value(i);
	return v;
}

Value card__half(Value v, FunctionContext *fc)
{
	if(v->type != VT_LIST) ERROR("Expected a list");
	return v; 
}

Value card__head(Value v, FunctionContext *fc)
{
	if(v->type != VT_LIST) ERROR("Expected a list");
	if(v->list.len == 0) ERROR("List of zero length");
	Value tmp = copy_value(v->list.data[0]);
	free_value(v);
	return tmp;
}

Value card__tail(Value v, FunctionContext *fc)
{
	if(v->type != VT_LIST) ERROR("Expected a list");
	if(v->list.len == 0) ERROR("List of zero length");
	Value tmp = copy_value(v);
	free_value(v);
	free_value(tmp->list.data[0]);
	--tmp->list.len;
	++tmp->list.data;
	return tmp;
}

Value card__len(Value v, FunctionContext *fc)
{
	if(v->type != VT_LIST) ERROR("Expected a list");
	Value tmp = create_number(v->list.len);
	free_value(v);
	return tmp;
}

Value card__if(Value v, FunctionContext *fc)
{
	if(v->type != VT_LIST) ERROR("Expected a list");
	if(v->list.len != 3) ERROR("Expected a list of 3 elements.");
	if(v->list.data[0]->type != VT_NUM) ERROR("Expected element 0 to be a number / boolean");
	if(v->list.data[1]->type != VT_CELL) ERROR("Expected element 0 to be a cell");
	if(v->list.data[2]->type != VT_CELL) ERROR("Expected element 0 to be a cell");
	Value tmp = create_number(v->list.len);
	free_value(v);
	return tmp;
}

#define __CARD_STDLIB_OPERATOR_DEFINE(name, op)\
	Value card__##name(Value v, FunctionContext *fc)\
	{\
		if(v->type != VT_LIST) ERROR("Expected a list");\
		if(v->list.len < 2) ERROR("Expected at least 2 elements");\
		if(v->list.data[0]->type != VT_NUM) ERROR("Expected element 0 to be a number");\
		if(v->list.data[1]->type != VT_NUM) ERROR("Expected element 1 to be a number");\
		return create_number(v->list.data[0]->number op v->list.data[1]->number);\
	}

__CARD_STDLIB_OPERATOR_DEFINE(add, +)
__CARD_STDLIB_OPERATOR_DEFINE(sub, -)
__CARD_STDLIB_OPERATOR_DEFINE(mul, *)
__CARD_STDLIB_OPERATOR_DEFINE(div, /)

#endif
