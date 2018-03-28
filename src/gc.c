#include <stdlib.h>

#include "abc_instructions.h"
#include "gc.h"
#include "interpret.h"
#include "settings.h"
#include "util.h"

struct nodes_set {
	struct {
		BC_WORD **nodes;
		size_t size;
		size_t write_ptr;
		size_t read_ptr;
		int last_was_read;
	} grey;

	struct {
		BC_WORD *bitmap;
		size_t size;
		size_t ptr_i;
		BC_WORD ptr_j;
	} black;
};

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
#if (DEBUG_GARBAGE_COLLECTOR > 3)
	fprintf(stderr, "\tBitmap size = %d\n", set->black.size);
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
#if (DEBUG_GARBAGE_COLLECTOR > 3)
	fprintf(stderr, "\t\tbitmap[%d] |= %x\n", i, m);
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
	fprintf(stderr, "\tSearching with %d/%d\n", set->black.ptr_i, set->black.ptr_j);
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
		fprintf(stderr, "\tSearching with %d/%d\n", set->black.ptr_i, set->black.ptr_j);
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

#if (DEBUG_GARBAGE_COLLECTOR > 0)
static uint32_t gc_count = 0;
#endif
BC_WORD *garbage_collect(BC_WORD *stack, BC_WORD *asp, BC_WORD *heap, size_t heap_size, BC_WORD *code, BC_WORD *data) {
	BC_WORD *asp_temp;

#if (DEBUG_GARBAGE_COLLECTOR > 0)
	fprintf(stderr, "Collecting trash (%u)... stack @ %p; heap @ %p; code @ %p; data @ %p\n",
			++gc_count,
			(void*) stack, (void*) heap, (void*) code, (void*) data);
#endif

	struct nodes_set nodes_set;
	init_nodes_set(&nodes_set, heap_size);

	/* Add full A-stack to the grey set */
	for (asp_temp = asp; asp_temp > stack; asp_temp--) {
		add_grey_node(&nodes_set, (BC_WORD*) *asp_temp, heap, heap_size);
	}

	/* Evaluate full grey set */
	BC_WORD *node;
	while ((node = get_grey_node(&nodes_set)) != NULL) {
		int16_t arity = ((int16_t*)(node[0]))[-1];
#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t%p -> black: %lx = " BC_WORD_FMT "; arity %d\n", (void*) node, node[0], node[0] - (BC_WORD) code, arity);
#endif

		if (node[0] & 2) { /* HNF */
			if (node[0] == (BC_WORD) &INT + 2) { /* TODO more basic types */
				/* No pointer elements */
			} else {
				for (; arity; arity--) {
					add_grey_node(&nodes_set, (BC_WORD*) node[arity], heap, heap_size);
				}
			}
		} else if (node[0] == (BC_WORD) &__cycle__in__spine) {
			/* No pointer arguments */
		} else if (arity < 0) { /* TODO ??? */
			fprintf(stderr, "Arity < 0 not implemented\n");
			exit(1);
		} else if (arity == 0) {
		} else if (arity < 3) { /* Thunk, three places */
			add_grey_node(&nodes_set, (BC_WORD*) node[1], heap, heap_size);
			if (arity == 2)
				add_grey_node(&nodes_set, (BC_WORD*) node[2], heap, heap_size);
		} else { /* Thunk, pointer to rest of arguments */
			fprintf(stderr, "Arity > 2 not implemented\n");
			exit(1);
		}
	}

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

				for (; arity; arity--) {
					node++;
					new_heap++;

					if (*node >= (BC_WORD) heap && *node < (BC_WORD) (heap + heap_size)) { /* Pointer */
#if (DEBUG_GARBAGE_COLLECTOR > 2)
						fprintf(stderr, "\t\tReversing pointer %p\n", *node);
#endif
						BC_WORD *temp = (BC_WORD*) *node;
						*node = *temp;
						*temp = (BC_WORD) node | 1;
					} else {
#if (DEBUG_GARBAGE_COLLECTOR > 2)
						fprintf(stderr, "\t\tNot reversing %p\n", *node);
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
			fprintf(stderr, "Arity > 2 (%d) not implemented\n", arity);
			exit(1);
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
			fprintf(stderr, "Arity > 2 not implemented\n");
			exit(1);
		}
	}

	free_nodes_set(&nodes_set);

	return new_heap;
}
