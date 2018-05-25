#include "copy.h"

#include <stdlib.h>

#include "mark.h"
#include "util.h"

#include "../interpret.h"
#include "../util.h"

BC_WORD *collect_copy(BC_WORD *stack, BC_WORD *asp, BC_WORD **heap, size_t heap_size
#ifdef DEBUG_GARBAGE_COLLECTOR
		, BC_WORD *code, BC_WORD *data
#endif
		) {
	BC_WORD *old_heap = *heap;
	BC_WORD *heap2 = safe_malloc(sizeof(BC_WORD) * heap_size);
	BC_WORD *new_heap = heap2;

#if (DEBUG_GARBAGE_COLLECTOR > 0)
	fprintf(stderr, "Collecting trash... stack @ %p; heap @ %p; code @ %p; data @ %p\n",
			(void*) stack, (void*) old_heap, (void*) code, (void*) data);
#endif

	struct nodes_set nodes_set;
	init_nodes_set(&nodes_set, heap_size);

	mark_a_stack(stack, asp, *heap, heap_size, &nodes_set);
	evaluate_grey_nodes(*heap, heap_size, &nodes_set);

	/* Pass 1: reverse pointers on the A-stack */
#if (DEBUG_GARBAGE_COLLECTOR > 1)
	fprintf(stderr, "Pass 1: reverse pointers on the A-stack\n");
#endif
	for (BC_WORD *asp_temp = asp; asp_temp > stack; asp_temp--) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t%p\n", (void*) asp_temp);
		fprintf(stderr, "\t\t%p -> %p\n", (void*) *asp_temp, (void*) *((BC_WORD*)*asp_temp));
#endif
		if (*asp_temp < (BC_WORD) old_heap || *asp_temp > (BC_WORD) (old_heap + heap_size))
			continue;
		BC_WORD *temp = (BC_WORD*) *asp_temp;
		*asp_temp = *temp;
		*temp = (BC_WORD) asp_temp | 1;
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t\t%p -> %p -> %p\n", (void*) *asp_temp, (void*) *temp, (void*) *(BC_WORD*)(*temp ^ 1));
#endif
	}

#if (DEBUG_GARBAGE_COLLECTOR > 1)
	fprintf(stderr, "Pass 2: copy nodes\n");
#endif
	for (;;) {
		BC_WORD offset = next_black_node(&nodes_set);
		if (offset == -1)
			break;
#if (DEBUG_GARBAGE_COLLECTOR > 3)
		fprintf(stderr, "\tOffset %p\n", (void*) offset);
#endif
		BC_WORD *node = old_heap + offset;

#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\tDealing with %p -> %p -> %p\n", (void*) node, (void*) node[0], (void*) *(BC_WORD*)(node[0] ^ 1));
#endif
		while (node[0] & 1) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
			fprintf(stderr, "\t\tUnreversing %p, updating to %p\n", (void*) node[0], (void*) new_heap);
#endif
			BC_WORD *temp = (BC_WORD*) (node[0] ^ 1);
			node[0] = *temp;
			*temp = (BC_WORD) new_heap;
		}
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\tDealing with %p -> %p -> %p\n", (void*) node, (void*) node[0], (void*) *(BC_WORD*)(node[0] ^ 1));
#endif

		if (node[0] & 2) { /* HNF */
			int16_t a_arity = ((int16_t*)(node[0]))[-1];
			int16_t b_arity = 0;
			if (a_arity > 256) { /* record */
				a_arity = ((int16_t*)(node[0]))[0];
				b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
			}

			if (node[0] == (BC_WORD) &INT + 2 ||
					node[0] == (BC_WORD) &CHAR + 2 ||
					node[0] == (BC_WORD) &BOOL + 2 ||
					node[0] == (BC_WORD) &REAL + 2) { /* TODO more basic types */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(INT / CHAR / BOOL / REAL)\n");
#endif
				*new_heap = node[0];
				node[0] = (BC_WORD) new_heap;
				new_heap++;
				*new_heap++ = node[1];
			} else if (node[0] == (BC_WORD) &__STRING__ + 2) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(__STRING__)\n");
#endif
				*new_heap = node[0];
				node[0] = (BC_WORD) new_heap;
				new_heap++;
				*new_heap++ = node[1];
				uint32_t l = node[1] / IF_INT_64_OR_32(8,4) + (node[1] % IF_INT_64_OR_32(8,4) ? 1 : 0);
				BC_WORD *start_string = &node[2];
				for (; l; l--) {
					*new_heap++ = *start_string++;
				}
			} else if (node[0] == (BC_WORD) &__ARRAY__ + 2) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(__ARRAY__)\n");
#endif
				*new_heap = node[0];
				node[0] = (BC_WORD) new_heap;
				new_heap++;
				*new_heap++ = node[1];
				*new_heap++ = node[2];
				BC_WORD *start_array = &node[3];
				uint32_t l = node[1];
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\tlength is %d\n", l);
#endif
				if (node[2] == (BC_WORD) &INT + 2 || node[2] == (BC_WORD) &REAL + 2) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
					fprintf(stderr, "\t\tof type Int/Real\n");
#endif
					for (; l; l--)
						*new_heap++ = *start_array++;
				} else if (node[2] == (BC_WORD) &BOOL + 2) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
					fprintf(stderr, "\t\tof type Bool\n");
#endif
					l = l / IF_INT_64_OR_32(8,4) + (l % IF_INT_64_OR_32(8,4) ? 1 : 0);
					for (; l; l--)
						*new_heap++ = *start_array++;
				} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
					fprintf(stderr, "\t\tof a boxed type\n");
#endif
					for (; l; l--) {
						if (on_heap(*start_array, old_heap, heap_size)) {
							if (*start_array <= (BC_WORD) node) { /* Indirected */
#if (DEBUG_GARBAGE_COLLECTOR > 3)
								fprintf(stderr, "\t\tElem is indirected; now %p\n", (void*)*(BC_WORD*)*start_array);
#endif
								*new_heap++ = *(BC_WORD*)*start_array;
							} else { /* Reverse pointer */
								BC_WORD temp = *(BC_WORD*)*start_array;
#if (DEBUG_GARBAGE_COLLECTOR > 3)
								fprintf(stderr, "\t\tReversing elem (%p)\n", (void*)temp);
#endif
								*(BC_WORD*)*start_array = (BC_WORD) new_heap | 1;
								*new_heap++ = temp;
							}
						} else {
#if (DEBUG_GARBAGE_COLLECTOR > 3)
							fprintf(stderr, "\t\tElem is not on the heap\n");
#endif
							*new_heap++ = *start_array;
						}

						start_array++;
					}
				}
			} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(HNF with arity %d/%d)\n", a_arity,b_arity);
#endif
				*new_heap = node[0];
				node[0] = (BC_WORD) new_heap;
				new_heap++;

				if (a_arity > 0) {
					if (on_heap(node[1], old_heap, heap_size)) {
						if (node[1] <= (BC_WORD) node) { /* Indirected */
#if (DEBUG_GARBAGE_COLLECTOR > 3)
							fprintf(stderr, "\t\tArg 1 is indirected; now %p\n", (void*)*(BC_WORD*)node[1]);
#endif
							*new_heap++ = *(BC_WORD*)node[1];
						} else { /* Reverse pointer */
							BC_WORD temp = *(BC_WORD*)node[1];
#if (DEBUG_GARBAGE_COLLECTOR > 3)
							fprintf(stderr, "\t\tReversing arg 1 (%p)\n", (void*)temp);
#endif
							*(BC_WORD*)node[1] = (BC_WORD) new_heap | 1;
							*new_heap++ = temp;
						}
					} else {
#if (DEBUG_GARBAGE_COLLECTOR > 3)
						fprintf(stderr, "\t\tArg 1 is not on the heap\n");
#endif
						*new_heap++ = node[1];
					}

					if (a_arity == 2 && b_arity == 0) {
						/* full arity node */
						if (on_heap(node[2], old_heap, heap_size)) {
							if (node[2] <= (BC_WORD) node) { /* Indirected */
#if (DEBUG_GARBAGE_COLLECTOR > 3)
								fprintf(stderr, "\t\tArg %d is indirected; now %p\n", 2, (void*)*(BC_WORD*)node[2]);
#endif
								*new_heap++ = *(BC_WORD*)node[2];
							} else { /* Reverse pointer */
								BC_WORD temp = *(BC_WORD*)node[2];
#if (DEBUG_GARBAGE_COLLECTOR > 3)
								fprintf(stderr, "\t\tReversing arg %d (%p)\n", 2, (void*)temp);
#endif
								*(BC_WORD*)node[2] = (BC_WORD) new_heap | 1;
								*new_heap++ = temp;
							}
						} else {
#if (DEBUG_GARBAGE_COLLECTOR > 3)
							fprintf(stderr, "\t\tArg %d is not on the heap\n", 2);
#endif
							*new_heap++ = node[2];
						}
					} else if (a_arity >= 2) {
						BC_WORD **rest = (BC_WORD**) node[2];
						/* 3-node with pointer to rest */
						*new_heap = (BC_WORD) (new_heap+1);
						new_heap++;
						int i;
						for (i = 0; i < a_arity-1; i++) {
							if (on_heap((BC_WORD) rest[i], old_heap, heap_size)) {
								if (rest[i] <= node) { /* Indirected */
#if (DEBUG_GARBAGE_COLLECTOR > 3)
									fprintf(stderr, "\t\tArg %d is indirected; now %p\n", i+2, (void*)*rest[i]);
#endif
									*new_heap++ = *rest[i];
								} else { /* Reverse pointer */
									BC_WORD temp = *rest[i];
#if (DEBUG_GARBAGE_COLLECTOR > 3)
									fprintf(stderr, "\t\tReversing arg %d (%p)\n", i+2, (void*)temp);
#endif
									*rest[i] = (BC_WORD) new_heap | 1;
									*new_heap++ = temp;
								}
							} else {
#if (DEBUG_GARBAGE_COLLECTOR > 3)
								fprintf(stderr, "\t\tArg %d is not on the heap\n", i+2);
#endif
								*new_heap++ = (BC_WORD) rest[i];
							}
						}
					}
				}
			}
		} else if (node[0] == (BC_WORD) &__cycle__in__spine) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
			fprintf(stderr, "\t\t(internal cycle in spine)\n");
#endif
			/* No pointer arguments */
			*new_heap = node[0];
			node[0] = (BC_WORD) new_heap;
			new_heap++;
			new_heap += 2;
		} else { /* Thunk, 3+ places and perhaps pointer to rest */
			int16_t arity = ((int16_t*)(node[0]))[-1];
			int8_t a_arity, b_arity;
			if (arity < 0) {
				a_arity = 1;
				b_arity = 0;
			} else {
				b_arity = arity >> 8;
				a_arity = (arity & 0xff) - b_arity;
			}
#if (DEBUG_GARBAGE_COLLECTOR > 2)
			fprintf(stderr, "\t\t(thunk with arity %d/%d)\n", a_arity, b_arity);
#endif
			*new_heap = node[0];
			node[0] = (BC_WORD) new_heap;
			new_heap++;

			for (int i = 1; i <= a_arity; i++) {
				if (!on_heap(node[i], old_heap, heap_size)) {
					*new_heap++ = node[i];
				} else if (node[i] <= (BC_WORD) node) { /* Indirected */
					*new_heap++ = *(BC_WORD*)node[i];
				} else { /* Reverse pointer */
					BC_WORD temp = *(BC_WORD*)node[i];
					*(BC_WORD*)node[i] = (BC_WORD) new_heap | 1;
					*new_heap++ = temp;
				}
			}

			for (int i = 0; i < b_arity; i++)
				*new_heap++ = node[a_arity+i+1];

			if (arity + b_arity < 2) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\tFilling up with %d\n", 2 - arity - b_arity);
#endif
				new_heap += 2 - arity - b_arity;
			}
		}
	}

	free_nodes_set(&nodes_set);
	free(old_heap);
	*heap = heap2;
	return new_heap;
}
