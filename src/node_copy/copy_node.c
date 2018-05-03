#include <stdio.h>

extern void *INT_descriptor,*CHAR,*BOOL,*REAL,*__STRING__,*__ARRAY__;

int output_cons_c(int *node) {
	int desc = *node;
	unsigned int arity = ((unsigned short *) desc)[-1];

	printf("Desc: %d\n", desc);
	printf("Arity: %d\n", arity);
	return 1;
}
