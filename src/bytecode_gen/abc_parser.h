#include "../util.h"

// Holds information on instructions
// Their name, and what functions to call for them
struct instruction {
	char* instruction_name;
	int (* instruction_parse_function)();
	void (* instruction_code_function)();
};

instruction* parse(char*);
