#include <stdlib.h>

#include "mark.h"
#include "util.h"
#include "../interpret.h"
#include "../util.h"

#define GREY_NODES_INITIAL 100
#define GREY_NODES_ENLARGE 2

void init_nodes_set(struct nodes_set *set, size_t heap_size) {
	set->grey.size = GREY_NODES_INITIAL;
	set->grey.nodes = safe_malloc(sizeof(BC_WORD*) * GREY_NODES_INITIAL);
	set->grey.write_ptr = 0;
	set->grey.read_ptr = 0;
	set->grey.last_was_read = 1;

	set->black.bitmap = safe_calloc(1, (heap_size / 8 / sizeof(BC_WORD) + 2) * sizeof(BC_WORD));
	set->black.size = heap_size / 8 / sizeof(BC_WORD) + 1;
#if (DEBUG_GARBAGE_COLLECTOR > 4)
	fprintf(stderr, "\tBitmap size = %d\n", (int) set->black.size);
#endif
	set->black.ptr_i = 0;
	set->black.ptr_j = 0;
}

void free_nodes_set(struct nodes_set *set) {
	free(set->grey.nodes);
	free(set->black.bitmap);
}

void reset_black_nodes_set(struct nodes_set *set) {
	set->black.ptr_i = 0;
	set->black.ptr_j = 0;
}

/* Returns 1 if the node is new; 0 if it was already black */
int add_black_node(struct nodes_set *set, BC_WORD *node, BC_WORD *heap) {
	BC_WORD val = ((BC_WORD) node - (BC_WORD) heap) / sizeof(BC_WORD);
	BC_WORD i = val / 8 / sizeof(BC_WORD);
	BC_WORD m = (BC_WORD) 1 << (val % (8 * sizeof(BC_WORD)));
#if (DEBUG_GARBAGE_COLLECTOR > 4)
	fprintf(stderr, "\t\tbitmap[%d] |= %x\n", (int) i, (int) m);
#endif
	if (set->black.bitmap[i] & m) {
		return 0;
	} else {
		set->black.bitmap[i] |= m;
		return 1;
	}
}

BC_WORD next_black_node(struct nodes_set *set) {
#if (DEBUG_GARBAGE_COLLECTOR > 4)
	fprintf(stderr, "\tSearching with %d/%d\n", (int) set->black.ptr_i, (int) set->black.ptr_j);
#endif
	if (set->black.ptr_i > set->black.size)
		return -1;
	while (!(set->black.bitmap[set->black.ptr_i] & ((BC_WORD) 1 << set->black.ptr_j))) {
		set->black.ptr_j++;
		set->black.ptr_j %= 8 * sizeof(BC_WORD);
		if (set->black.ptr_j == 0) {
			do {
				set->black.ptr_i++;
				if (set->black.ptr_i > set->black.size)
					return -1;
			} while (!set->black.bitmap[set->black.ptr_i]);
		}
#if (DEBUG_GARBAGE_COLLECTOR > 4)
		fprintf(stderr, "\tSearching with %d/%d\n", (int) set->black.ptr_i, (int) set->black.ptr_j);
#endif
	}

	BC_WORD ret = 8 * sizeof(BC_WORD) * set->black.ptr_i + set->black.ptr_j;

	set->black.ptr_j++;
	set->black.ptr_j %= 8 * sizeof(BC_WORD);
	if (set->black.ptr_j == 0) {
		set->black.ptr_i++;
	}

	return ret;
}

void realloc_grey_nodes_set(struct nodes_set *set) {
#if (DEBUG_GARBAGE_COLLECTOR > 1)
	fprintf(stderr, "\tReallocating grey nodes set\n");
#endif
	set->grey.write_ptr = set->grey.size;
	set->grey.read_ptr = 0;
	set->grey.size *= GREY_NODES_ENLARGE;
	set->grey.nodes = safe_realloc(set->grey.nodes, sizeof(BC_WORD*) * set->grey.size);
}

void add_grey_node(struct nodes_set *set, BC_WORD *node, BC_WORD *heap, size_t heap_size) {
	if (node < heap || node >= heap + heap_size) {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t%p is not on the heap...\n", (void*) node);
#endif
		return;
	}

	if (!add_black_node(set, node, heap)) { /* Already black */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t%p is already black...\n", (void*) node);
#endif
		return;
	}

	if (set->grey.read_ptr == set->grey.write_ptr && !set->grey.last_was_read) {
		realloc_grey_nodes_set(set);
	}

#if (DEBUG_GARBAGE_COLLECTOR > 2)
	fprintf(stderr, "\t%p -> grey\n", (void*) node);
#endif
	set->grey.nodes[set->grey.write_ptr++] = node;
	if (set->grey.write_ptr == set->grey.size)
		set->grey.write_ptr = 0;

	set->grey.last_was_read = 0;
}

BC_WORD *get_grey_node(struct nodes_set *set) {
	if (set->grey.write_ptr == set->grey.read_ptr && set->grey.last_was_read) {
		return NULL;
	} else {
		BC_WORD *node = set->grey.nodes[set->grey.read_ptr++];

		if (set->grey.read_ptr == set->grey.size)
			set->grey.read_ptr = 0;

		set->grey.last_was_read = 1;

		return node;
	}
}

void mark_all_nodes(BC_WORD *stack, BC_WORD *asp, BC_WORD *heap, size_t heap_size, struct nodes_set *set) {
	BC_WORD *asp_temp;
	/* Add full A-stack to the grey set */
	for (asp_temp = asp; asp_temp > stack; asp_temp--) {
		add_grey_node(set, (BC_WORD*) *asp_temp, heap, heap_size);
	}

	/* Evaluate full grey set */
	BC_WORD *node;
	while ((node = get_grey_node(set)) != NULL) {
		int16_t arity = ((int16_t*)(node[0]))[-1];
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t%p -> black: %lx; arity %d\n", (void*) node, node[0], arity);
#endif

		if (node[0] & 2) { /* HNF */
			if (node[0] == (BC_WORD) &INT + 2) { /* TODO more basic types */
				/* No pointer elements */
			} else {
				if (arity > 0)
					add_grey_node(set, (BC_WORD*) node[1], heap, heap_size);
				if (arity > 1) {
					BC_WORD **rest = (BC_WORD**) node[2];
					if (*rest >= heap && *rest < heap + heap_size) {
						/* 3-node with pointer to rest */
						int i;
						for (i = 0; i < arity-1; i++)
							add_grey_node(set, rest[i], heap, heap_size);
					} else {
						/* full arity node */
						for (; arity; arity--) {
							add_grey_node(set, (BC_WORD*) node[arity], heap, heap_size);
						}
					}
				}
			}
		} else if (node[0] == (BC_WORD) &__cycle__in__spine) {
			/* No pointer arguments */
		} else if (arity < 0) { /* TODO ??? */
			fprintf(stderr, "Arity < 0 not implemented\n");
			exit(1);
		} else if (arity == 0) {
			/* No children */
		} else if (arity < 3) { /* Thunk, three places */
			add_grey_node(set, (BC_WORD*) node[1], heap, heap_size);
			if (arity == 2)
				add_grey_node(set, (BC_WORD*) node[2], heap, heap_size);
		} else { /* Thunk, pointer to rest of arguments */
			if (arity > 256) { /* arity = a_size + b_size + (b_size << 8) */
				arity = (arity & 0xff) - (arity >> 8); /* Only care about A-size */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
				fprintf(stderr, "\t\tA-arity is %d\n", arity);
#endif
			}
			if (arity > 0) {
				add_grey_node(set, (BC_WORD*) node[1], heap, heap_size);
				BC_WORD **rest = (BC_WORD**) node[2];
				if (on_heap((BC_WORD) *rest, heap, heap_size)) {
					int i;
					for (i = 0; i < arity-1; i++)
						add_grey_node(set, rest[i], heap, heap_size);
				} else {
					int i;
					for (i = 2; i <= arity; i++)
						add_grey_node(set, (BC_WORD*) node[i], heap, heap_size);
				}
			}
		}
	}
}
