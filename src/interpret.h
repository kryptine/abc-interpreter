#pragma once

#include "bytecode.h"

struct interpretation_options {
	int in_first_semispace:1;
	int allow_file_io:1;
#ifdef LINK_CLEAN_RUNTIME
	int hyperstrict:1;
#endif
};

#ifdef LINK_CLEAN_RUNTIME
/* This struct matches the Clean structure that the interpretation_environment
 * is kept in. */
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
	BC_WORD *host_a_ptr;  /* The A-stack pointer of the host */
	BC_WORD *host_hp_ptr; /* Heap pointer */
	size_t host_hp_free;  /* Nr. of free heap words */
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
	void *caf_list[2];
	struct interpretation_options options;
};

extern void *e__ABC_PInterpreter__dDV__ParseError;
extern void *e__ABC_PInterpreter__dDV__HeapFull;
extern void *e__ABC_PInterpreter__dDV__StackOverflow;
extern void *e__ABC_PInterpreter__dDV__Halt;
extern void *e__ABC_PInterpreter__dDV__IllegalInstruction;
extern void *e__ABC_PInterpreter__dDV__FileIOAttempted;
extern void *e__ABC_PInterpreter__dDV__SegmentationFault;
extern void *e__ABC_PInterpreter__dDV__HostHeapFull;
extern void *e__ABC_PInterpreter__kDV__Ok;

extern void **interpret_error;
#endif

extern void* __STRING__[];
extern void* __ARRAY__[];
#ifdef DEBUG_CURSES
extern void** ARRAY;
#endif
extern void* d___Nil[];
extern void* dINT[];
#define INT dINT
extern void* BOOL[];
extern void* CHAR[];
extern void* REAL[];
extern void* d_FILE[];

extern BC_WORD small_integers[66];
extern BC_WORD static_characters[512];
extern BC_WORD static_booleans[4];

#ifdef LINK_CLEAN_RUNTIME
#include "copy_interpreter_to_host.h"
extern void **HOST_NODES[32];
extern BC_WORD HOST_NODE_DESCRIPTORS[1216];
extern BC_WORD HOST_NODE_INSTRUCTIONS[32*6];
#endif

extern BC_WORD Fjmp_ap[32];

extern void* __interpreter_cycle_in_spine[2];
extern void* __interpreter_indirection[9];

#define A_STACK_CANARY 0x87654321 /* random value to check whether the A-stack overflew */

#ifdef COMPUTED_GOTOS
# include "abc_instructions.h"
extern void *instruction_labels[CMAX];
#endif

/**
 * program: the program to run
 * stack: stack block
 * stack_size: size of stack in words
 * heap: heap block
 * heap_size: size of heap in words
 * asp: A-stack pointer address (e.g. stack)
 * bsp: B-stack pointer address (e.g. &stack[stack_size])
 * csp: C-stack pointer address (e.g. &stack[stack_size >> 1])
 * hp: pointer to the next free block on the heap
 * node:
 *  - Pointer to a node to evaluate to HNF;
 *  - NULL if we should just start running at code[0].
 */
int interpret(
#ifdef LINK_CLEAN_RUNTIME
		struct interpretation_environment *ie,
		int create_restore_point,
#else
		struct program *program,
		struct interpretation_options options,
#endif
		BC_WORD *stack, size_t stack_size,
		BC_WORD *heap, size_t heap_size,
		BC_WORD *asp, BC_WORD *bsp, BC_WORD *csp, BC_WORD *hp,
		BC_WORD *node);
