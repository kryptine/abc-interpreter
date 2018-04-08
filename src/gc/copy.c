#include "copy.h"
#include "mark.h"
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

	mark_all_nodes(stack, asp, *heap, heap_size, &nodes_set);

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

		int16_t arity = ((int16_t*)node[0])[-1];

		if (node[0] & 2) { /* HNF */
			if (node[0] == (BC_WORD) &INT + 2) { /* TODO more basic types */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(INT)\n");
#endif
				*new_heap = node[0];
				node[0] = (BC_WORD) new_heap;
				new_heap++;
				*new_heap++ = node[1];
			} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\t(HNF with arity %d)\n", arity);
#endif
				*new_heap = node[0];
				node[0] = (BC_WORD) new_heap;
				new_heap++;

				if (arity > 0) {
					if ((BC_WORD) old_heap <= node[1] && node[1] < (BC_WORD) (old_heap + heap_size)) {
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

					if (arity > 1) {
						BC_WORD **rest = (BC_WORD**) node[2];
						if ((BC_WORD) *rest >= (BC_WORD) old_heap && (BC_WORD) *rest < (BC_WORD) (old_heap + heap_size)) {
							/* 3-node with pointer to rest */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
							fprintf(stderr, "\t\tNode is split over two memory blocks\n");
#endif
							*new_heap = (BC_WORD) (new_heap+1);
							new_heap++;
							int i;
							for (i = 0; i < arity-1; i++) {
								if (old_heap <= rest[i] && rest[i] < old_heap + heap_size) {
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
						} else {
							/* full arity node */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
							fprintf(stderr, "\t\tNode has one memory block\n");
#endif
							uint16_t i = 2;
							for (; i <= arity; i++) {
								if ((BC_WORD) old_heap <= node[i] && node[i] < (BC_WORD) (old_heap + heap_size)) {
									if (node[i] <= (BC_WORD) node) { /* Indirected */
#if (DEBUG_GARBAGE_COLLECTOR > 3)
										fprintf(stderr, "\t\tArg %d is indirected; now %p\n", i, (void*)*(BC_WORD*)node[i]);
#endif
										*new_heap++ = *(BC_WORD*)node[i];
									} else { /* Reverse pointer */
										BC_WORD temp = *(BC_WORD*)node[i];
#if (DEBUG_GARBAGE_COLLECTOR > 3)
										fprintf(stderr, "\t\tReversing arg %d (%p)\n", i, (void*)temp);
#endif
										*(BC_WORD*)node[i] = (BC_WORD) new_heap | 1;
										*new_heap++ = temp;
									}
								} else {
#if (DEBUG_GARBAGE_COLLECTOR > 3)
									fprintf(stderr, "\t\tArg %d is not on the heap\n", i);
#endif
									*new_heap++ = node[i];
								}
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
		} else if (arity < 0) { /* TODO ??? */
			fprintf(stderr, "Arity < 0 not implemented\n");
			exit(1);
		} else if (arity < 3) { /* Thunk, three places */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
			fprintf(stderr, "\t\t(thunk with arity %d, i.e. 3 places)\n", arity);
#endif
			*new_heap = node[0];
			node[0] = (BC_WORD) new_heap;
			new_heap++;

			if (arity > 0) {
				if (node[1] <= (BC_WORD) node) { /* Indirected */
					*new_heap++ = *(BC_WORD*)node[1];
				} else { /* Reverse pointer */
					BC_WORD temp = *(BC_WORD*)node[1];
					*(BC_WORD*)node[1] = (BC_WORD) new_heap | 1;
					*new_heap++ = temp;
				}

				if (arity > 1) {
					if (node[2] <= (BC_WORD) node) { /* Indirected */
						*new_heap++ = *(BC_WORD*)node[2];
					} else { /* Reverse pointer */
						BC_WORD temp = *(BC_WORD*)node[2];
						*(BC_WORD*)node[2] = (BC_WORD) new_heap | 1;
						*new_heap++ = temp;
					}
				} else {
					new_heap++;
				}
			} else {
				new_heap += 2;
			}
		} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
			fprintf(stderr, "\t\t(thunk with arity %d, i.e. 3 places + pointer to rest)\n", arity);
#endif
			*new_heap = node[0];
			node[0] = (BC_WORD) new_heap;
			new_heap++;

			if (arity > 256) {
				/* arity = a_size + b_size + (b_size << 8) */
				int16_t a_arity = arity;
				int16_t b_arity = a_arity >> 8;
				a_arity = (a_arity & 0xff) - b_arity;
				int16_t i;

				/* TODO: jump to next block if arity > 2 */

				for (i = 1; i <= a_arity; i++) {
					if (node[i] <= (BC_WORD) node) { /* Indirected */
						*new_heap++ = *(BC_WORD*)node[i];
					} else { /* Reverse pointer */
						BC_WORD temp = *(BC_WORD*)node[i];
						*(BC_WORD*)node[i] = (BC_WORD) new_heap | 1;
						*new_heap++ = temp;
					}
				}
				for (i = 0; i < b_arity; i++) {
					*new_heap++ = node[a_arity+i+1];
				}
			} else {
				/* Thunk, two places and pointer to rest */
				fprintf(stderr, "Arity > 2 (%d) not implemented\n", arity);
				exit(1);
			}
		}
	}

	free(old_heap);
	*heap = heap2;
	return new_heap;
}
