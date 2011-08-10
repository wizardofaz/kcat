#ifndef CSTACK_H
#define CSTACK_H

class cStack {
public:
	cStack (int = 50);
	~cStack () { delete [] stackPtr; };
	bool push( const unsigned char &);
	bool pop( unsigned char &);
	bool isEmpty() const { return top == -1;};
	bool isFull() const { return top == size - 1; };
private:
	int size;
	int top;
	unsigned char *stackPtr;
};

#endif
