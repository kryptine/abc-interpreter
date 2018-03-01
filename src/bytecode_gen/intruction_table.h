#include "../util.h"

// Holds information on instructions
// Their name, and what functions to call for them
struct instruction {
	char* instruction_name;
	int (* instruction_parse_function)();
	void (* instruction_code_function)();
};

// Linked List of instructions element
struct in_name {
	struct in_name* in_name_next;
	Instruction* instruction;
};

void load_instruction_table(struct in_name*);
instruction instruction_lookup(char*);
