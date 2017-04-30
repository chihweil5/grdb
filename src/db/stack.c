#include <stdio.h>
#include "stack.h"
#define MAXSIZE   10  

int isempty(stack_pt s) {

   if(s->top == -1)
      return 1;
   else
      return 0;
}
   
int isfull(stack_pt s) {

   if(s->top == MAXSIZE)
      return 1;
   else
      return 0;
}

vertex_t peek(stack_pt s) {
   vertex_t *stack = s->s;
   int top = s->top;
   return stack[top];
}

void pop(stack_pt s) {

   if(!isempty(s)) {
      s->top = s->top - 1;   
   } else {
      printf("Could not retrieve data, Stack is empty.\n");
   }
}

void push(vertex_t data, stack_pt s) {
   vertex_t *stack = s->s;
   if(!isfull(s)) {
      s->top = s->top + 1;   
      stack[s->top] = data;
   } else {
      printf("Could not insert data, Stack is full.\n");
   }
}

int isExist(vertex_t data, stack_pt s){
   if(s->top < 0)
      return 0;

   vertex_t *stack = s->s;

   for(int i = 0; i <= s->top; i++){
      if(data == stack[i]){
         return 1;
      }
   }
   return 0;
}
