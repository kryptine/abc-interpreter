#pragma once

#include "bytecode.h"

struct host_references {
	void *hr_descriptor;
	BC_WORD **hr_reference;
	struct host_references *hr_rest;
};

extern void *__Cons;
extern void *__Nil;

extern struct host_references *host_references;
