#pragma once

#include "bytecode.h"
#include "finalizers.h"
#include "interpret.h"

void interpreter_finalizer(BC_WORD interpret_node);
int copy_to_host_or_garbage_collect(struct interpretation_environment *ie,
		BC_WORD **target, BC_WORD *node, int hyperstrict_if_requested);
extern BC_WORD *__interpret__evaluate__host(
		struct interpretation_environment *ie, BC_WORD *node);
#ifdef WINDOWS
extern BC_WORD *__interpret__evaluate__host_with_args(
		BC_WORD *arg2, BC_WORD *arg1, BC_WORD *node, void *ap_address,
		struct interpretation_environment *ie);
#else
extern BC_WORD *__interpret__evaluate__host_with_args(
		struct interpretation_environment *ie, void *dummy,
		BC_WORD *arg2, BC_WORD *arg1, BC_WORD *node, void *ap_address);
#endif
