#ifndef __STACK_H
#define __STACK_H

#include "graph.h"
#define MAXSIZE   10 

struct stack{
	vertex_t *s;
	int top;
};

typedef struct stack *stack_pt;
                
int isempty(stack_pt s);
int isfull(stack_pt s);
vertex_t peek(stack_pt s);
void pop(stack_pt s);
void push(vertex_t data, stack_pt s);
int isExist(vertex_t data, stack_pt s);

#endif