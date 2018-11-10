#include <stdlib.h>

#include "mark.h"
#include "util.h"
#include "../interpret.h"
#include "../util.h"

#ifdef LINK_CLEAN_RUNTIME
# include "../copy_interpreter_to_host.h"
# include "../finalizers.h"
#endif

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

void mark_a_stack(BC_WORD *stack, BC_WORD *asp, BC_WORD *heap, size_t heap_size, struct nodes_set *set) {
	BC_WORD *asp_temp;
	for (asp_temp = asp; asp_temp > stack; asp_temp--) {
		add_grey_node(set, (BC_WORD*) *asp_temp, heap, heap_size);
	}
}

void mark_cafs(void **cafs, BC_WORD *heap, size_t heap_size, struct nodes_set *set) {
	BC_WORD **cafptr=(BC_WORD**)&cafs[1];
	while (cafptr[-1]!=0) {
		cafptr=(BC_WORD**)cafptr[-1];
		int n_a=(int)(BC_WORD)cafptr[0];
		for (; n_a>0; n_a--)
			add_grey_node(set, cafptr[n_a], heap, heap_size);
	}
}

#ifdef LINK_CLEAN_RUNTIME
void mark_host_references(BC_WORD *heap, size_t heap_size, struct nodes_set *set) {
	struct finalizers *finalizers = NULL;
	while ((finalizers = next_interpreter_finalizer(finalizers)) != NULL)
		add_grey_node(set, (BC_WORD*) finalizers->cur->arg, heap, heap_size);
}
#endif

void evaluate_grey_nodes(BC_WORD *heap, size_t heap_size, struct nodes_set *set) {
	BC_WORD *node;
	while ((node = get_grey_node(set)) != NULL) {
		int16_t a_arity, b_arity;

		if (node[0] & 2) {
			a_arity = ((int16_t*)(node[0]))[-1];
			b_arity = 0;
			if (a_arity > 256) {
				a_arity = ((int16_t*)(node[0]))[0];
				b_arity = ((int16_t*)(node[0]))[-1] - 256 - a_arity;
			}
		} else {
			int16_t arity = ((int16_t*)(node[0]))[-1];
			if (arity < 0) {
				a_arity = 1;
				b_arity = 0;
			} else {
				b_arity = arity >> 8;
				a_arity = (arity & 0xff) - b_arity;
			}
		}

#if (DEBUG_GARBAGE_COLLECTOR > 2)
		fprintf(stderr, "\t%p -> black: %lx; arity %d/%d\n", (void*) node, node[0], a_arity, b_arity);
#endif

		if (node[0] & 2) { /* HNF */
			if (node[0] == (BC_WORD) &INT + 2 ||
					node[0] == (BC_WORD) &CHAR + 2 ||
					node[0] == (BC_WORD) &BOOL + 2 ||
					node[0] == (BC_WORD) &REAL + 2 ||
					node[0] == (BC_WORD) &__STRING__ + 2) {
			} else if (node[0] == (BC_WORD) &__ARRAY__ + 2) {
				if (node[2] == (BC_WORD) &INT + 2 ||
						node[2] == (BC_WORD) &REAL + 2 ||
						node[2] == (BC_WORD) &BOOL + 2) { /* unboxed, no nodes */
				} else {
					int16_t arity=1;
					int16_t a_arity=1;
					int16_t b_arity=0;
					if (node[2]!=0) { /* unboxed */
						BC_WORD *desc=(BC_WORD*)node[2];
						arity=((int16_t*)desc)[0];
						a_arity=((int16_t*)desc)[1];
						b_arity=arity-256-a_arity;
						arity=a_arity+b_arity;
					}
					BC_WORD **arr=(BC_WORD**)&node[3];
					for (int len=node[1]; len; len--) {
						for (int i=0; i<a_arity; i++)
							add_grey_node(set, arr[i], heap, heap_size);
						arr+=arity;
					}
				}
			} else if (a_arity >= 1) {
				add_grey_node(set, (BC_WORD*) node[1], heap, heap_size);
				if (a_arity == 2 && b_arity == 0) {
					add_grey_node(set, (BC_WORD*) node[2], heap, heap_size);
				} else if (a_arity >= 2) {
					BC_WORD **rest = (BC_WORD**) node[2];
					int i;
					for (i = 0; i < a_arity-1; i++)
						add_grey_node(set, rest[i], heap, heap_size);
				}
			}
		} else if (node[0] == (BC_WORD) &__cycle__in__spine) {
			/* No pointer arguments */
		} else { /* Thunk */
			int i;
			for (i = 1; i <= a_arity; i++)
				add_grey_node(set, (BC_WORD*) node[i], heap, heap_size);
		}
	}
}
