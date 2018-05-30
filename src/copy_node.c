#include <stdint.h>

#include "bytecode.h"
#include "gc.h"
#include "interpret.h"
#include "util.h"

extern void *e__CodeSharing__ncoerce;
extern void *dINT;

/* This does not contain the ce_symbols from the CoercionEnvironment type. This
 * element is not needed, and like this we can easily dereference the
 * environment from memory.
 */
struct coercion_environment {
	BC_WORD *code;
	BC_WORD code_size;
	BC_WORD *data;
	BC_WORD data_size;
	BC_WORD *heap;
	BC_WORD heap_size;
	BC_WORD *stack;
	BC_WORD stack_size;
	BC_WORD *asp;
	BC_WORD *bsp;
	BC_WORD *csp;
	BC_WORD *hp;
	struct host_references *host_references;
};

void add_host_reference(struct host_references *refs, BC_WORD *reference, BC_WORD *node) {
	if (++refs->count >= refs->size) {
		refs->size += 100;
		refs = safe_realloc(refs,
				sizeof(struct host_references) + refs->size * sizeof(struct host_reference));
	}

	refs->nodes[refs->count-1].reference = reference;
	refs->nodes[refs->count-1].node = node;

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr,"added host reference: %p -> %p\n", reference, (void*) node);
#endif
}

void remove_one_host_reference(struct host_references *refs, BC_WORD *node) {
	for (int i = 0; i < refs->count; i++) {
		if (refs->nodes[i].node == node) {
			refs->nodes[i] = refs->nodes[--refs->count];
			return;
		}
	}
}

BC_WORD copy_interpreter_to_host(BC_WORD *host_heap, size_t host_heap_free, void *coercion_environment, BC_WORD *node) {
	struct coercion_environment *ce = (struct coercion_environment*)(((BC_WORD**)coercion_environment)[2]);

	if (!ce->host_references) {
		ce->host_references = safe_malloc(sizeof(struct host_references) + sizeof(struct host_reference) * 99);
		ce->host_references->count = 0;
		ce->host_references->size = 100;
	}

#if DEBUG_CLEAN_LINKS > 0
	fprintf(stderr,"Copying %p -> %p...\n", node, (void*)*node);
#endif

	if (!(node[0] & 2)) {
#if DEBUG_CLEAN_LINKS > 1
		fprintf(stderr,"\tInterpreting...\n");
#endif
		*++ce->asp = (BC_WORD) node;
		host_references = ce->host_references;
		int result = interpret(
				ce->code, ce->code_size,
				ce->data, ce->data_size,
				ce->stack, ce->stack_size,
				ce->heap, ce->heap_size,
				ce->asp, ce->bsp, ce->csp, ce->hp,
				ce->asp);
		if (result != 0) {
			fprintf(stderr,"Failed to interpret\n");
			return -1;
		}
		node = (BC_WORD*)*ce->asp--;
		ce->hp = get_heap_address();
	}

	if (node[0] == (BC_WORD) &INT+2) {
		host_heap[0] = (BC_WORD) &dINT+2;
		host_heap[1] = node[1];
		return 2;
	} else if (node[0] == (BC_WORD) &CHAR+2 ||
			node[0] == (BC_WORD) &BOOL+2 ||
			node[0] == (BC_WORD) &REAL+2) {
		host_heap[0] = node[0];
		host_heap[1] = node[1];
		return 2;
	}
	/* TODO: more basic types, strings, arrays */

	int16_t a_arity = ((int16_t*)(node[0]))[-1];
	int16_t b_arity = 0;
	if (a_arity > 256) { /* record */
		a_arity = ((int16_t*)(node[0]))[0];
		b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
	}

#if DEBUG_CLEAN_LINKS > 1
	fprintf(stderr, "\tcoercing (arities %d / %d)...\n", a_arity, b_arity);
#endif

	if (host_heap_free < 3 + 5 * a_arity) {
		fprintf(stderr,"Not enough memory (%ld, %d)\n", host_heap_free, a_arity);
		return -2;
	}

	void *host_address = ((void**)(node[0]-2))[-2-2*a_arity]; /* TODO check */
	if (host_address == (void*) 0) {
		fprintf(stderr,"Not a HNF\n");
		return -3;
	} else if (host_address == (void*) -1) {
		fprintf(stderr,"Unresolvable descriptor\n");
		return -4;
	}

	if (b_arity) {
		fprintf(stderr,"TODO: copy unboxed nodes\n"); /* TODO */
		return -5;
	} else if (a_arity > 2) {
		fprintf(stderr,"TODO: copy large nodes\n"); /* TODO */
		return -5;
	}

	remove_one_host_reference(ce->host_references, node);
	host_heap[0] = (BC_WORD)(((BC_WORD*)((BC_WORD)host_address+2))+a_arity); /* TODO check */

	/* TODO: pointers to outside the heap (e.g. Nil) can be translated directly */
	if (a_arity >= 1) {
		host_heap[1] = (BC_WORD) &host_heap[3];
		host_heap[3] = (BC_WORD) &e__CodeSharing__ncoerce;
		host_heap[4] = (BC_WORD) coercion_environment;
		host_heap[5] = (BC_WORD) &host_heap[6];
		host_heap[6] = (BC_WORD) &dINT+2;
		host_heap[7] = node[1];
		add_host_reference(ce->host_references, &host_heap[7], (BC_WORD*) node[1]);

		if (a_arity >= 2) {
			host_heap[ 2] = (BC_WORD) &host_heap[8];
			host_heap[ 8] = (BC_WORD) &e__CodeSharing__ncoerce;
			host_heap[ 9] = (BC_WORD) coercion_environment;
			host_heap[10] = (BC_WORD) &host_heap[11];
			host_heap[11] = (BC_WORD) &dINT+2;
			host_heap[12] = node[2];
			add_host_reference(ce->host_references, &host_heap[12], (BC_WORD*) node[2]);
		}
	}

	return 5 * a_arity + 3;
}
