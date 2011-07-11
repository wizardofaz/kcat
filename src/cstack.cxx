#include "cstack.h"

cStack::cStack(int s)
{
	size = s > 0 ? s : 50;
	top = -1;
	stackPtr = new unsigned char[size];
}

bool cStack::push (const unsigned char &val)
{
	if (!isFull()) {
		stackPtr[++top] = val;
		return true;
	}
	return false;
}

bool cStack::pop ( unsigned char &val )
{
	if (!isEmpty()) {
		val = stackPtr[top--];
		return true;
	}
	return false;
}
