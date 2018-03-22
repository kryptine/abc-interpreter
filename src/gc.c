#include <stdlib.h>

#include "gc.h"
#include "interpret.h"
#include "util.h"

struct grey_nodes_set {
	BC_WORD **nodes;
	size_t size;
	size_t write_ptr;
	size_t read_ptr;
	int last_was_read;
};

#define GREY_NODES_INITIAL 3 /* TODO make larger; this is just for testing */
#define GREY_NODES_ENLARGE 2

void init_grey_nodes_set(struct grey_nodes_set *set) {
	set->size = GREY_NODES_INITIAL;
	set->nodes = safe_calloc(1, sizeof(BC_WORD*) * GREY_NODES_INITIAL);
	set->write_ptr = 0;
	set->read_ptr = 0;
	set->last_was_read = 1;
}

void realloc_grey_nodes_set(struct grey_nodes_set *set) {
	fprintf(stderr, "\tReallocating grey nodes set\n");
	set->write_ptr = set->size;
	set->read_ptr = 0;
	set->size *= GREY_NODES_ENLARGE;
	set->nodes = safe_realloc(set->nodes, sizeof(BC_WORD*) * set->size);
}

void add_grey_node(struct grey_nodes_set *set, BC_WORD *node) {
	if (set->read_ptr == set->write_ptr && !set->last_was_read) {
		realloc_grey_nodes_set(set);
	}

	fprintf(stderr, "\t%p -> grey\n", (void*) node);
	set->nodes[set->write_ptr++] = node;
	if (set->write_ptr == set->size)
		set->write_ptr = 0;

	set->last_was_read = 0;
}

BC_WORD *get_grey_node(struct grey_nodes_set *set) {
	if (set->write_ptr == set->read_ptr && set->last_was_read) {
		return NULL;
	} else {
		BC_WORD *node = set->nodes[set->read_ptr++];

		if (set->read_ptr == set->size)
			set->read_ptr = 0;

		set->last_was_read = 1;

		return node;
	}
}

BC_WORD *garbage_collect(BC_WORD *stack, BC_WORD *asp, BC_WORD *heap, BC_WORD *code, BC_WORD *data) {
	fprintf(stderr, "Collecting trash...\n");

	struct grey_nodes_set grey;
	init_grey_nodes_set(&grey);

	/* Add full A-stack to the grey set */
	for (; asp > stack; asp--) {
		add_grey_node(&grey, (BC_WORD*) *asp);
	}

	/* Evaluate full grey set */
	BC_WORD *node;
	uint32_t black_nodes = 0;
	while ((node = get_grey_node(&grey)) != NULL) {
		if (node < heap)
			/* FIXME: this ignores pointers to code/data.
			 * This assumes that the heap is higher in memory.
			 */
			continue;

		if (node[0] & 1) /* Already black */
			continue;

		int16_t arity = ((int16_t*)(node[0]))[-1];
		fprintf(stderr, "\t%p -> black: %lx = " BC_WORD_FMT "; arity %d\n", (void*) node, node[0], node[0] - (BC_WORD) data, arity);

		if (arity < 0) {
			fprintf(stderr, "Arity < 0 not implemented\n");
			exit(1);
		} else if (arity < 3) {
			for (; arity; arity--) {
				add_grey_node(&grey, (BC_WORD*) node[arity]);
			}
		} else {
			fprintf(stderr, "Arity > 2 not implemented\n");
			exit(1);
		}

		node[0] |= 1;
		black_nodes++;
	}

	/* Defragment black nodes */
	node = heap;
	while (black_nodes) {
		fprintf(stderr, "\t%d black nodes to go...\n", black_nodes);

		while (!(*node & 1)) {
			fprintf(stderr, "\t\tSkipping over %p\n", (void*) node);
			int16_t arity = ((int16_t*)(node[0]))[-1];
			if ((BC_WORD) node & 2) { /* HNF, skip over <arity> elements */
				if (node[0] == (BC_WORD) &INT + 2) { /* TODO more basic types */
					node += 2;
				} else {
					node += arity;
				}
			} else if (arity < 0) {
				fprintf(stderr, "Arity < 0 not implemented\n");
				exit(1);
			} else if (arity < 3) {
				node += 3;
			} else {
				fprintf(stderr, "Arity > 2 not implemented\n");
				exit(1);
			}
		}

		node[0] ^= 1;

		int16_t arity = ((int16_t*)(node[0]))[-1];
		fprintf(stderr, "\t\tFound %p (%d)\n", (void*) node, arity);
		if (node[0] & 2) { /* HNF */
			if (node[0] == (BC_WORD) &INT + 2) { /* TODO more basic types */
				*heap++ = *node++;
				*heap++ = *node++;
			} else {
				arity++;
				for (; arity; arity--) {
					*heap++ = *node++;
				}
			}
		} else { /* Thunk */
			if (arity < 0) {
				fprintf(stderr, "Arity < 0 not implemented\n");
				exit(1);
			} else if (arity < 3) {
				*heap++ = *node++;
				*heap++ = *node++;
				*heap++ = *node++;
			} else {
				fprintf(stderr, "Arity > 2 not implemented\n");
				exit(1);
			}
		}

		black_nodes--;
	}
	
	fprintf(stderr, "Done!\n");

	return heap;
}
