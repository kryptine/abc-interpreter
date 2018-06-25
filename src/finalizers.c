#include "copy_interpreter_to_host.h"
#include "finalizers.h"

extern struct finalizers *finalizer_list;
extern void *__Nil;
extern void *e____system__kFinalizer;

struct finalizers *next_interpreter_finalizer(struct finalizers *now) {
	now = now == NULL ? finalizer_list : now->next;
	while (((BC_WORD) now != (BC_WORD) &__Nil-8) && now->cur->fun != interpreter_finalizer)
		now = now->next;
	return ((BC_WORD) now == (BC_WORD) &__Nil-8) ? NULL : now;
}

BC_WORD *build_finalizer(BC_WORD *heap, void (*fun)(BC_WORD), BC_WORD arg) {
	heap[0] = (BC_WORD) &e____system__kFinalizer+2;
	heap[1] = (BC_WORD) finalizer_list;
	heap[2] = (BC_WORD) &heap[3];
	heap[3] = (BC_WORD) fun;
	heap[4] = arg;
	finalizer_list = (struct finalizers*) &heap[0];
	return heap+5;
}
