#ifndef CARD_STDLIB_H
#define CARD_STDLIB_H
#include "card.h"
Value card__print(Value v)
{
	display_value(&v); putchar('\n');
	return v; // look what i found:   \._./  \;-;/ \;o;/
}

Value card__map(Value i)
{	
	if(i.type != 1) ERROR("Expected a list");
	if(i.list.len < 2) ERROR("Expected at least 2 elements");
	if(i.list.data[0].type != 2) ERROR("Expected element 0 to be a function");
	if(i.list.data[1].type != 1) ERROR("Expected element 1 to be a list");
	Value v = create_list(i.list.data[1].list.len);
	for(int j=0;j<v.list.len;++j)
		v.list.data[j] = run_value_function(i.list.data[0], i.list.data[1].list.data[j]);
	return v;
}

Value card__half(Value v)
{
	if(v.type != 1) ERROR("Expected a list");
	return v; 
}


#define __CARD_STDLIB_OPERATOR_DEFINE(name, op)\
	Value card__##name(Value v)\
	{\
		if(v.type != 1) ERROR("Expected a list");\
		if(v.list.len < 2) ERROR("Expected at least 2 elements");\
		if(v.list.data[0].type != 0) ERROR("Expected element 0 to be a number");\
		if(v.list.data[1].type != 0) ERROR("Expected element 1 to be a number");\
		return create_number(v.list.data[0].number op v.list.data[1].number);\
	}

__CARD_STDLIB_OPERATOR_DEFINE(add, +)
__CARD_STDLIB_OPERATOR_DEFINE(sub, -)
__CARD_STDLIB_OPERATOR_DEFINE(mul, *)
__CARD_STDLIB_OPERATOR_DEFINE(div, /)

#endif
