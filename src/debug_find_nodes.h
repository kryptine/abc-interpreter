#pragma once

#include "bytecode.h"

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

void init_nodes_set(struct nodes_set *set, size_t heap_size);
void free_nodes_set(struct nodes_set *set);

BC_WORD next_black_node(struct nodes_set *set);

void mark_a_stack(BC_WORD *stack, BC_WORD *asp, BC_WORD *heap, size_t heap_size, struct nodes_set *set);
void evaluate_grey_nodes(BC_WORD *heap, size_t heap_size, struct nodes_set *set);
