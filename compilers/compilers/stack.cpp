#include<stdio.h>
int stack[100],top=0;
void push(int key)
{
	stack[top++] = key;
}
int pop()
{
	return stack[--top];
}