#include <stdlib.h>

#include "compact.h"
#include "mark.h"
#include "../abc_instructions.h"
#include "../interpret.h"
#include "../settings.h"

#if (DEBUG_GARBAGE_COLLECTOR > 0)
static uint32_t gc_count = 0;
#endif
BC_WORD *collect_compact(BC_WORD *stack, BC_WORD *asp, BC_WORD *heap, size_t heap_size
#ifdef DEBUG_GARBAGE_COLLECTOR
		, BC_WORD *code, BC_WORD *data
#endif
		) {
	BC_WORD *asp_temp, *node;

#if (DEBUG_GARBAGE_COLLECTOR > 0)
	fprintf(stderr, "Collecting trash (%u)... stack @ %p; heap @ %p; code @ %p; data @ %p\n",
			++gc_count,
			(void*) stack, (void*) heap, (void*) code, (void*) data);
#endif

	struct nodes_set nodes_set;
	init_nodes_set(&nodes_set, heap_size);

	mark_a_stack(stack, asp, heap, heap_size, &nodes_set);
	evaluate_grey_nodes(heap, heap_size, &nodes_set);

	/* Pass 0: reverse pointers on the A-stack */
#if (DEBUG_GARBAGE_COLLECTOR > 1)
	fprintf(stderr, "Pass 0: reverse pointers on the A-stack\n");
#endif
	for (asp_temp = asp; asp_temp > stack; asp_temp--) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t%p\n", (void*) asp_temp);
		fprintf(stderr, "\t\t%p -> %p\n", (void*) *asp_temp, (void*) *((BC_WORD*)*asp_temp));
#endif
		if (*asp_temp < (BC_WORD) heap || *asp_temp > (BC_WORD) (heap + heap_size))
			continue;
		BC_WORD *temp = (BC_WORD*) *asp_temp;
		*asp_temp = *temp;
		*temp = (BC_WORD) asp_temp | 1;
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t\t%p -> %p -> %p\n", (void*) *asp_temp, (void*) *temp, (void*) *(BC_WORD*)(*temp ^ 1));
#endif
	}

	/* Pass 1: reverse pointers; update forward pointers to new addresses */
	BC_WORD *new_heap = heap;
#if (DEBUG_GARBAGE_COLLECTOR > 1)
	fprintf(stderr, "Pass 1: reverse pointers; update forward pointers\n");
#endif
	for (;;) {
		BC_WORD offset = next_black_node(&nodes_set);
		if (offset == -1)
			break;
#if (DEBUG_GARBAGE_COLLECTOR > 3)
		fprintf(stderr, "\tOffset %p\n", (void*) offset);
#endif
		node = heap + offset;

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

		int16_t arity = ((int16_t*)node[0])[-1];

#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\tDealing with %p -> %p\n", (void*) node, (void*) node[0]);
#endif
		if (node[0] & 2) { /* HNF */
			if (node[0] == (BC_WORD) &INT + 2) { /* TODO more basic types */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(INT)\n");
#endif
				node += 2;
				new_heap += 2;
			} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(HNF with arity %d)\n", arity);
#endif
				/* TODO: distinguish nodes split up over two blocks and full-sized nodes,
				 * like in the grey set evaluation above but now including pointer reversal. */

				for (; arity; arity--) {
					node++;
					new_heap++;

					if (*node >= (BC_WORD) heap && *node < (BC_WORD) (heap + heap_size)) { /* Pointer */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
						fprintf(stderr, "\t\tReversing pointer %p\n", (void*) *node);
#endif
						BC_WORD *temp = (BC_WORD*) *node;
						*node = *temp;
						*temp = (BC_WORD) node | 1;
					} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
						fprintf(stderr, "\t\tNot reversing %p\n", (void*) *node);
#endif
					}
				}

				node++;
				new_heap++;
			}
		} else if (node[0] == (BC_WORD) &__cycle__in__spine) {
			/* No pointer arguments */
			node += 3;
			new_heap += 3;
		} else if (arity < 0) { /* TODO ??? */
			fprintf(stderr, "Arity < 0 not implemented\n");
			exit(1);
		} else if (arity < 3) { /* Thunk, three places */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
			fprintf(stderr, "\t\t(thunk with arity %d, i.e. 3 places)\n", arity);
#endif

			if (arity >= 1 && node[1] >= (BC_WORD) heap && node[1] < (BC_WORD) (heap + heap_size)) { /* Pointer */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\tReversing pointer %p\n", (void*) node[1]);
#endif
				BC_WORD *temp = (BC_WORD*) node[1];
				node[1] = *temp;
				*temp = (BC_WORD) &node[1] | 1;
			} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\tNot reversing %p\n", (void*) node[1]);
#endif
			}

			if (arity >= 2 && node[2] >= (BC_WORD) heap && node[2] < (BC_WORD) (heap + heap_size)) { /* Pointer */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\tReversing pointer %p\n", (void*) node[2]);
#endif
				BC_WORD *temp = (BC_WORD*) node[2];
				node[2] = *temp;
				*temp = (BC_WORD) &node[2] | 1;
			} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\tNot reversing %p\n", (void*) node[2]);
#endif
			}

			node += 3;
			new_heap += 3;
		} else { /* Thunk, two places and pointer to rest */
			if (arity > 256) /* arity = a_size + b_size + (b_size << 8) */
				arity = (arity & 0xff) - (arity >> 8); /* Only care about A-size */
			if (arity > 0) {
				fprintf(stderr, "Arity > 2 (%d) not implemented\n", arity);
				exit(1);
			}
		}
	}

	/* Pass 2: squeeze; update backward pointers */
	node = new_heap = heap;
	reset_black_nodes_set(&nodes_set);
#if (DEBUG_GARBAGE_COLLECTOR > 1)
	fprintf(stderr, "Pass 2: squeeze; update backward pointers\n");
#endif
	for (;;) {
		BC_WORD offset = next_black_node(&nodes_set);
		if (offset == -1)
			break;
		node = heap + offset;

#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\tDealing with %p -> %p\n", (void*) node, (void*) node[0]);
#endif
		while (node[0] & 1) { /* Reversed pointer */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
			fprintf(stderr, "\t\tUnreversing %p, updating to %p\n", (void*) node[0], (void*) new_heap);
#endif
			BC_WORD *temp = (BC_WORD*) (node[0] ^ 1);
			node[0] = *temp;
			*temp = (BC_WORD) new_heap;
		}

		int16_t arity = ((int16_t*)(node[0]))[-1];
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\tDealing with %p -> %p\n", (void*) node, (void*) node[0]);
#endif
		/* There should not be any reverse pointers here */
		if (node[0] & 2) { /* HNF */
			if (node[0] == (BC_WORD) &INT + 2) { /* TODO more basic types */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(INT)\n");
#endif
				*new_heap++ = *node++;
				*new_heap++ = *node++;
			} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(HNF with arity %d)\n", arity);
#endif
				*new_heap++ = *node++;
				for (; arity; arity--) {
					*new_heap++ = *node++;
				}
			}
		} else if (node[0] == (BC_WORD) &__cycle__in__spine) {
			*new_heap++ = *node++;
			*new_heap++ = *node++;
			*new_heap++ = *node++;
		} else if (arity < 0) { /* TODO ??? */
			fprintf(stderr, "Arity < 0 not implemented\n");
			exit(1);
		} else if (arity < 3) { /* Thunk, three places */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
			fprintf(stderr, "\t\t(thunk with arity %d, i.e. 3 places)\n", arity);
#endif
			*new_heap++ = *node++;
			*new_heap++ = *node++;
			*new_heap++ = *node++;
		} else { /* Thunk, two places and pointer to rest */
			if (arity > 256) /* arity = a_size + b_size + (b_size << 8) */
				arity = (arity & 0xff) - (arity >> 8); /* Only care about A-size */
			if (arity > 0) {
				fprintf(stderr, "Arity > 2 (%d) not implemented\n", arity);
				exit(1);
			}
		}
	}

	free_nodes_set(&nodes_set);

	return new_heap;
}
