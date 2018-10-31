#pragma once

#include "bytecode.h"
#include "finalizers.h"

// This struct matches the Clean structure that the interpretation_environment
// is kept in.
struct InterpretationEnvironment2 {
	BC_WORD **__ie_shared_nodes;
	int __ie_shared_nodes_ptr;
};
struct InterpretationEnvironment {
	void *__ie_descriptor;
	struct finalizers *__ie_finalizer;
	struct InterpretationEnvironment2 *__ie_2;
};

struct host_status {
	BC_WORD *host_a_ptr; /* The A-stack pointer of the host */
	void *host_hp_ptr;   /* Heap pointer */
	size_t host_hp_free; /* Nr. of free heap words */
	struct InterpretationEnvironment *clean_ie; /* Clean InterpretationEnvironment */
};

struct interpretation_environment {
	struct host_status *host;
	struct program *program;
	BC_WORD *heap;
	BC_WORD heap_size;
	BC_WORD *stack;
	BC_WORD stack_size;
	BC_WORD *asp;
	BC_WORD *bsp;
	BC_WORD *csp;
	BC_WORD *hp;
};

void interpreter_finalizer(BC_WORD interpret_node);
BC_WORD *make_interpret_node(BC_WORD *heap,
		struct InterpretationEnvironment *clean_ie,
		BC_WORD node, int args_needed);
extern BC_WORD *__interpret__evaluate__host(
		struct interpretation_environment *ie, BC_WORD *node);
extern BC_WORD *__interpret__evaluate__host_with_args(
		struct interpretation_environment *ie, void *dummy,
		BC_WORD *arg2, BC_WORD *arg1, BC_WORD *node, void *ap_address);
