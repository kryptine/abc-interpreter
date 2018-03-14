#pragma once

// Holds information on instructions
// Their name, and what functions to call for them
typedef struct instruction {
	char* name;
	int (* parse_function)();
	void (* code_function)();
} instruction;

// Linked List of instructions element
typedef struct inst_element {
	struct inst_element *next;
	instruction* instruction;
} inst_element;

void load_instruction_table(void);
instruction* instruction_lookup(char*);
void init_instruction_table(void);
