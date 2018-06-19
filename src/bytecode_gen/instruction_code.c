#include "instruction_code.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "../abc_instructions.h"
#include "../util.h"
#include "bytecode_gen.h"

#define max_implemented_instruction_n CMAX-1

#define N_ADD_ARG_LABELS 32
#define MAX_Cadd_arg_INSTRUCTION_N 32

struct program pgrm;
uint32_t last_d, last_jsr_with_d;

struct label_node {
	struct label_node *label_node_left;
	struct label_node *label_node_right;
	struct label *label_node_label_p;
};

struct label_node *labels;
uint32_t label_id;
uint32_t global_label_count;
uint32_t global_label_string_count;

uint32_t module_n;

uint32_t list_code = 0;

uint32_t allocated_code_size;
uint32_t allocated_data_size;
uint32_t allocated_strings_size;
uint32_t allocated_code_relocations_size;
uint32_t allocated_data_relocations_size;

static uint32_t Fadd_arg_label_used[N_ADD_ARG_LABELS];

void initialize_code(void) {
	int i;
	for(i = 0; i < N_ADD_ARG_LABELS; ++i)
		Fadd_arg_label_used[i] = 0;

	labels = NULL;
	label_id = 0;
	global_label_count = 0;
	global_label_string_count = 0;

	module_n = 0;

	pgrm = (struct program) {
		0,
		0,
		0,
		0,
		0,
		0,
		(struct word*) safe_malloc(512 * sizeof(struct word)),
		(uint32_t*) safe_malloc(512 * sizeof(uint32_t)),
		(uint64_t*) safe_malloc(512 * sizeof(uint64_t)),
		(relocation*) safe_malloc(512 * sizeof(relocation)),
		(relocation*) safe_malloc(512 * sizeof(relocation)),
		};

	allocated_code_size = 512;
	allocated_data_size = 512;
	allocated_strings_size = 512;
	allocated_code_relocations_size = 512;
	allocated_data_relocations_size = 512;
}

void code_next_module(void) {
	module_n++;

	last_d = 0;
	last_jsr_with_d = 0;
}

static void realloc_code(void) {
	allocated_code_size *= 2;
	pgrm.code = (struct word*) safe_realloc(pgrm.code, allocated_code_size * sizeof(struct word));
}

static void realloc_strings(void) {
	allocated_strings_size *= 2;
	pgrm.strings = (uint32_t*) safe_realloc(pgrm.strings, allocated_strings_size * sizeof(uint32_t));
}

static void realloc_data(void) {
	allocated_data_size *= 2;
	pgrm.data = (uint64_t*) safe_realloc(pgrm.data, allocated_data_size * sizeof(uint64_t));
}

void store_code_elem(uint8_t bytewidth, uint64_t value) {
	if (pgrm.code_size >= allocated_code_size)
		realloc_code();

	pgrm.code[pgrm.code_size++] = (struct word) {bytewidth, value};
}

struct label *enter_label(char *label_name) {
	struct label_node **label_node_pp;

	label_node_pp = &labels;
	while (*label_node_pp!=NULL) {
		struct label_node *label_node_p;
		int r;

		label_node_p=*label_node_pp;
		r=strcmp(label_name,label_node_p->label_node_label_p->label_name);
		if (r==0) {
			if (label_node_p->label_node_label_p->label_module_n>=0 && label_node_p->label_node_label_p->label_module_n!=module_n) {
				if (label_node_p->label_node_label_p->label_offset<0) {
					label_node_p->label_node_label_p->label_module_n = -1;

					return label_node_p->label_node_label_p;
				} else {
					struct label *new_label_p;

					new_label_p = (struct label*) safe_malloc(sizeof(struct label));
					new_label_p->label_name=label_node_p->label_node_label_p->label_name;
					new_label_p->label_id=label_id++;
					new_label_p->label_module_n=module_n;
					new_label_p->label_offset=-1;

					label_node_p->label_node_label_p=new_label_p;
					return new_label_p;
				}
			}

			return label_node_p->label_node_label_p;
		}
		if (r<0)
			label_node_pp=&label_node_p->label_node_left;
		else
			label_node_pp=&label_node_p->label_node_right;
	}

	struct label_node *new_label_node_p;
	struct label *new_label_p;

	new_label_node_p = (struct label_node*) safe_malloc(sizeof(struct label_node));
	new_label_p = (struct label*) safe_malloc(sizeof(struct label));
	new_label_node_p->label_node_left = NULL;
	new_label_node_p->label_node_right = NULL;
	new_label_node_p->label_node_label_p = new_label_p;
	new_label_p->label_name = (char*) safe_malloc(strlen(label_name) + 1);
	new_label_p->label_id=label_id++;
	strcpy(new_label_p->label_name, label_name);
	new_label_p->label_module_n = module_n;
	new_label_p->label_offset = -1;

	*label_node_pp = new_label_node_p;
	return new_label_p;
}

int internal_label_id = 0;
struct label *new_internal_label(void) {
	char label_name[14];
	sprintf(label_name,"_internal%d",internal_label_id++);
	struct label *label = enter_label(label_name);
	return label;
}

struct label *new_label(uint32_t offset) {
	char name[16];
	snprintf(name, 16, "i%x", offset);
	name[15] = '\0';
	struct label *label = enter_label(name);
	label->label_offset = offset;
	return label;
}

void make_label_global(struct label *label) {
	if (label->label_module_n != -1) {
		label->label_module_n = -1;
		global_label_count++;
		global_label_string_count+=strlen(label->label_name);
	}
}

void store_data_l(uint64_t v) {
	if (pgrm.data_size >= allocated_data_size)
		realloc_data();
	*(uint64_t*)&pgrm.data[pgrm.data_size++] = v;
}

struct relocation *add_code_relocation(struct label *label, uint32_t offset) {
	if (pgrm.code_reloc_size>=allocated_code_relocations_size) {
		allocated_code_relocations_size *= 2;
		pgrm.code_relocations = (relocation*) safe_realloc(pgrm.code_relocations, allocated_code_relocations_size * sizeof(relocation));
	}

	struct relocation *relocation_p;

	relocation_p=&pgrm.code_relocations[pgrm.code_reloc_size++];
	relocation_p->relocation_offset=offset;
	relocation_p->relocation_label=label;

	return relocation_p;
}

struct relocation *add_data_relocation(struct label *label, uint32_t offset) {
	if (pgrm.data_reloc_size>=allocated_data_relocations_size) {
		allocated_data_relocations_size *= 2;
		pgrm.data_relocations = (relocation*) safe_realloc(pgrm.data_relocations, allocated_data_relocations_size * sizeof(relocation));
	}

	struct relocation *relocation_p;

	relocation_p=&pgrm.data_relocations[pgrm.data_reloc_size++];
	relocation_p->relocation_offset=offset;
	relocation_p->relocation_label=label;

	return relocation_p;
}

void store_code_internal_label_value(struct label *label,uint32_t offset) {
	add_code_relocation(label, pgrm.code_size);
	store_code_elem(8, offset);
}

void store_code_label_value(char *label_name,int32_t offset) {
	struct label *label;

	label=enter_label(label_name);

	add_code_relocation(label, pgrm.code_size);

	store_code_elem(4, offset);
}

static void store_data_label_value_of_label(struct label *label,uint32_t offset) {
	add_data_relocation(label, pgrm.data_size);
	store_data_l(offset);
}

void store_data_label_value(char *label_name,uint32_t offset) {
	store_data_label_value_of_label(enter_label (label_name),offset);
}

void add_data2_to_code(int i) {
	if (list_code)
		printf("%d\t.data2 %d\n",pgrm.code_size,i);

	store_code_elem(2, i);
}

void add_data4_to_code(int i) {
	if (list_code)
		printf("%d\t.data4 %d\n",pgrm.code_size,i);

	store_code_elem(4, i);
}

void add_data8_to_code(int i) {
	if (list_code)
		printf("%d\t.data8 %d\n",pgrm.code_size,i);

	store_code_elem(8, i);
}

void add_instruction(SS i) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s\n",pgrm.code_size,instruction_name (i));

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
}

struct word *add_add_arg_labels(void) {
	int i;

	for(i=0; i<N_ADD_ARG_LABELS; ++i)
		if (Fadd_arg_label_used[i]) {
			char label_name[11];
			sprintf(label_name,"_add_arg%d",i);
			struct label *label = enter_label(label_name);
			if (label->label_module_n != -1) {
				make_label_global(label);
				label->label_offset = pgrm.code_size<<2;
			}
			if (i>MAX_Cadd_arg_INSTRUCTION_N) {
				fprintf(stderr, "Error: Cadd_arg%d not yet implemented:\n",i);
				++i;
				while(i<N_ADD_ARG_LABELS) {
					if (Fadd_arg_label_used[i])
						fprintf(stderr, "Error: Cadd_arg%d not yet implemented:\n",i);
					++i;
				}
				exit(1);
			}
			add_instruction(Cadd_arg0+i);
		}

	return pgrm.code;
}

void add_code_and_data_offsets(void) {
	int i;
	size_t code_offset,data_offset;

	code_offset = (size_t) pgrm.code;
	data_offset = (size_t) pgrm.data;

	for(i=0; i<pgrm.code_reloc_size; ++i) {
		struct relocation *relocation_p;

		relocation_p=&pgrm.code_relocations[i];
		if ((relocation_p->relocation_label->label_offset & 1)==0)
			*(SI*)&pgrm.code[relocation_p->relocation_offset] += code_offset;
		else
			*(SI*)&pgrm.code[relocation_p->relocation_offset] += data_offset;
	}

	for(i=0; i<pgrm.data_reloc_size; ++i) {
		struct relocation *relocation_p;

		relocation_p=&pgrm.data_relocations[i];
		if ((relocation_p->relocation_label->label_offset & 1)==0)
			*(SI*)&pgrm.data[relocation_p->relocation_offset] += code_offset;
		else
			*(SI*)&pgrm.data[relocation_p->relocation_offset] += data_offset;
	}
}

void add_instruction_label(US i,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s\n",pgrm.code_size,instruction_name (i),label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name,0);
}

void add_instruction_label_offset(US i,char *label_name,uint32_t offset) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d\n",pgrm.code_size,instruction_name (i),label_name,offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name,offset);
}

void add_instruction_label_offset_label(SS i,char *label_name1,uint32_t offset,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d %s\n",pgrm.code_size,instruction_name (i),label_name1,offset,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name1,offset);
	store_code_label_value(label_name2,0);
}

void add_instruction_label_offset_w(US i,char *label_name,uint32_t offset,SI n1) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d %d\n",pgrm.code_size,instruction_name (i),label_name,offset,(int)n1);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n1);
}

void add_instruction_label_offset_w_label(US i,char *label_name1,uint32_t offset,SI n1,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d %d %s\n",pgrm.code_size,instruction_name (i),label_name1,offset,(int)n1,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name1,offset);
	store_code_elem(2, n1);
	store_code_label_value(label_name2,0);
}

void add_instruction_label_offset_label_label_offset_label
	(SS i,char *label_name1,uint32_t offset1,char *label_name2,char *label_name3,uint32_t offset2,char *label_name4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d %s %s%+d %s\n",pgrm.code_size,instruction_name (i),label_name1,offset1,label_name2,
																			label_name3,offset2,label_name4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name1,offset1);
	store_code_label_value(label_name2,0);
	store_code_label_value(label_name3,offset2);
	store_code_label_value(label_name4,0);
}

void add_instruction_label_w_w(SS i,char *label_name,SI n1,SI n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s %d %d\n",pgrm.code_size,instruction_name (i),label_name,(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name,0);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
}

void add_instruction_c(SS i,char n) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d\n",pgrm.code_size,instruction_name (i),(int)n);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(1, n);
}

void add_instruction_w(SS i,SI n) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d\n",pgrm.code_size,instruction_name (i),(int)n);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
}

void add_instruction_w_i(SS i,SI n1,int n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(8, n2);
}

void add_instruction_w_label(SS i,SI n,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s\n",pgrm.code_size,instruction_name (i),(int)n,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
	store_code_label_value(label_name,0);
}

void add_instruction_w_label_offset(SS i,SI n,char *label_name,uint32_t offset) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s%+d\n",pgrm.code_size,instruction_name (i),(int)n,label_name,offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
	store_code_label_value(label_name,offset);
}

void add_instruction_w_label_offset_label(SS i,SI n,char *label_name1,uint32_t offset,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s%+d %s\n",pgrm.code_size,instruction_name (i),(int)n,label_name1,offset,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
	store_code_label_value(label_name1,offset);
	store_code_label_value(label_name2,0);
}

void add_instruction_w_label_w(SS i,SI n1,char *label_name,SI n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s %d\n",pgrm.code_size,instruction_name (i),(int)n1,label_name,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name,0);
	store_code_elem(2, n2);
}

void add_instruction_w_label_offset_label_label_offset_label
	(SS i,SI n1,char *label_name1,uint32_t offset1,char *label_name2,char *label_name3,uint32_t offset2,char *label_name4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s%+d %s %s%+d %s\n",pgrm.code_size,instruction_name (i),n1,label_name1,offset1,label_name2,
																				  label_name3,offset2,label_name4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name1,offset1);
	store_code_label_value(label_name2,0);
	store_code_label_value(label_name3,offset2);
	store_code_label_value(label_name4,0);
}

void add_instruction_w_label_offset_w(SS i,SI n1,char *label_name,uint32_t offset,SI n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s+%d %d\n",pgrm.code_size,instruction_name (i),(int)n1,label_name,offset,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n2);
}

void add_instruction_w_label_offset_w_w(SS i,SI n1,char *label_name,uint32_t offset,SI n2,SI n3) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s+%d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,label_name,offset,(int)n2,(int)n3);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
}

void add_instruction_w_label_offset_w_w_w(SS i,SI n1,char *label_name,uint32_t offset,SI n2,SI n3,SI n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s+%d %d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,label_name,offset,(int)n2,(int)n3,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
}

void add_instruction_w_c(SS i,SI n1,char n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(1, n2);
}

void add_instruction_w_w(SS i,SI n1,SI n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
}

void add_instruction_i_w(SS i,SI n1,SI n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(8, n1);
	store_code_elem(2, n2);
}

void add_instruction_w_c_label(SS i,SI n1,char n2,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(1, n2);
	store_code_label_value(label_name,0);
}

void add_instruction_w_w_label(SS i,SI n1,SI n2,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,0);
}

void add_instruction_w_i_label(SS i,SI n1,SI n2,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(8, n2);
	store_code_label_value(label_name,0);
}

void add_instruction_w_w_label_offset(SI i,SI n1,SI n2,char *label_name,uint32_t offset) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s%+d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,offset);
}

void add_instruction_w_w_label_w(SS i,SI n1,SI n2,char *label_name,SI n3) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,(int)n3);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,0);
	store_code_elem(2, n3);
}

void add_instruction_w_c_label_c_label(SS i,SI n1,char n2,char *label_name1,char n3,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name1,(int)n3,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(1, n2);
	store_code_label_value(label_name1,0);
	store_code_elem(1, n3);
	store_code_label_value(label_name2,0);
}

void add_instruction_w_w_label_w_label(SS i,SI n1,SI n2,char *label_name1,SI n3,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name1,(int)n3,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name1,0);
	store_code_elem(2, n3);
	store_code_label_value(label_name2,0);
}

void add_instruction_w_i_label_i_label(SS i,SI n1,SI n2,char *label_name1,SI n3,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name1,(int)n3,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(8, n2);
	store_code_label_value(label_name1,0);
	store_code_elem(8, n3);
	store_code_label_value(label_name2,0);
}

void add_instruction_w_w_label_offset_w(SS i,SI n1,SI n2,char *label_name,uint32_t offset,SI n3) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s+%d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,offset,(int)n3);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n3);
}

void add_instruction_w_w_label_w_w(SS i,SI n1,SI n2,char *label_name,SI n3,SI n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,(int)n3,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,0);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
}

void add_instruction_w_w_label_offset_w_w(SS i,SI n1,SI n2,char *label_name,uint32_t offset,SI n3,SI n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s+%d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,offset,(int)n3,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
}

void add_instruction_w_w_w(SS i,SI n1,SI n2,SI n3) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
}

void add_instruction_w_w_w_label(SS i,SI n1,SI n2,SI n3,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,0);
}

void add_instruction_w_w_w_label_offset(SS i,SI n1,SI n2,SI n3,char *label_name,uint32_t offset) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %s+%d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,label_name,offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,offset);
}

void add_instruction_w_w_w_label_w(SS i,SI n1,SI n2,SI n3,char *label_name,SI n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("\t%s %d %d %d %s %d\n",instruction_name (i),(int)n1,(int)n2,(int)n3,label_name,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,0);
	store_code_elem(2, n4);
}

void add_instruction_w_w_w_label_offset_w(SS i,SI n1,SI n2,SI n3,char *label_name,uint32_t offset,SI n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %s+%d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,label_name,offset,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n4);
}

void add_instruction_w_w_w_label_w_w(SS i,SI n1,SI n2,SI n3,char *label_name,SI n4,SI n5) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,label_name,(int)n4,(int)n5);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,0);
	store_code_elem(2, n4);
	store_code_elem(2, n5);
}

void add_instruction_w_w_w_label_offset_w_w(SS i,SI n1,SI n2,SI n3,char *label_name,uint32_t offset,SI n4,SI n5) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %s+%d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,label_name,offset,(int)n4,(int)n5);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n4);
	store_code_elem(2, n5);
}

void add_instruction_w_w_w_w(SS i,SI n1,SI n2,SI n3,SI n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
}

void add_instruction_w_w_w_w_w(SS i,SI n1,SI n2,SI n3,SI n4,SI n5) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,(int)n4,(int)n5);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
	store_code_elem(2, n5);
}

void add_instruction_i(SS i,int n) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d\n",pgrm.code_size,instruction_name (i),n);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(8, n);
}

void add_instruction_r(SS i,double r) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %f\n",pgrm.code_size,instruction_name (i),r);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(8, *((uint64_t*)(&r)));
}

void add_instruction_internal_label(SS i,struct label *label) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d\n",pgrm.code_size,instruction_name (i),label->label_offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_internal_label_value(label,0);
}

void add_instruction_w_internal_label_label(SS i,SI n1,struct label *label,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s\n",pgrm.code_size,instruction_name (i),n1,label->label_offset,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_internal_label_value(label,0);
	store_code_label_value(label_name,0);
}

void add_label(char *label_name) {
	if (list_code)
		printf("%d\t.label %s\n",pgrm.code_size,label_name);

	store_code_label_value(label_name,0);
}

void code_absR(void) {
	add_instruction(CabsR);
}

void code_acosR(void) {
	add_instruction(CacosR);
}

void code_addI(void) {
	add_instruction(CaddI);
}

void code_addR(void) {
	add_instruction(CaddR);
}

void code_and(void) {
	add_instruction(CandI);
}

void code_asinR(void) {
	add_instruction(CasinR);
}

void code_atanR(void) {
	add_instruction(CatanR);
}

void code_buildh(char descriptor_name[],int arity);

void code_build(char descriptor_name[],int arity,char *code_name) {
	if (code_name[0]=='_' && code_name[1]=='_' && code_name[2]=='h' && code_name[3]=='n' && code_name[4]=='f' && code_name[5]=='\0') {
		fprintf(stderr, "Warning: build %s %d %s\n",descriptor_name,arity,code_name);
		code_buildh(descriptor_name,arity);
		return;
	}
/*
	if (! (descriptor_name[0]=='_' && descriptor_name[1]=='_' && descriptor_name[2]=='\0')) {
		fprintf(stderr, "Error: build %s %d %s\n",descriptor_name,arity,code_name);
		exit(1);
	}
*/
	switch(arity) {
		case 0:
			add_instruction_label(Cbuild0,code_name);
			break;
		case 1:
			add_instruction_label(Cbuild1,code_name);
			break;
		case 2:
			add_instruction_label(Cbuild2,code_name);
			break;
		case 3:
			add_instruction_label(Cbuild3,code_name);
			break;
		case 4:
			add_instruction_label(Cbuild4,code_name);
			break;
		default:
			if (arity<1)
				add_instruction_label(Cbuild1,code_name);
			else
				add_instruction_w_label(Cbuild,arity,code_name);
	}
}

void code_buildh(char descriptor_name[],int arity) {
	switch(arity) {
		case 0:
			add_instruction_label_offset(Cbuildh0,descriptor_name,-4);
			return;
		case 1:
			add_instruction_label_offset(Cbuildh1,descriptor_name,(1<<3)+2);
			return;
		case 2:
			add_instruction_label_offset(Cbuildh2,descriptor_name,(2<<3)+2);
			return;
		case 3:
			add_instruction_label_offset(Cbuildh3,descriptor_name,(3<<3)+2);
			return;
		case 4:
			add_instruction_label_offset(Cbuildh4,descriptor_name,(4<<3)+2);
			return;
		default:
			add_instruction_w_label_offset(Cbuildh,arity+2,descriptor_name,(arity<<3)+2);
			return;
	}
}

void code_buildB(int value) {
	if (value==0)
		add_instruction(CbuildBFALSE);
	else
		add_instruction(CbuildBTRUE);
}

void print_string_directive(char *string,int string_length) {
	int i;

	printf("%d\t.string \"",pgrm.data_size<<2);
	for(i=0; i<string_length; ++i)
		putchar(string[i]);
	printf("\"\n");
}

void add_words_in_strings(uint32_t val) {
	pgrm.words_in_strings += val;
}

void add_string_information(uint32_t data_offset) {
	if (pgrm.strings_size >= allocated_strings_size)
		realloc_strings();

	pgrm.strings[pgrm.strings_size++] = data_offset;
}

void store_string(char *string,int string_length) {
	add_string_information(pgrm.data_size);

	if (list_code)
		printf("%d\t.data4 %d\n",pgrm.data_size<<2,string_length);
	store_data_l(string_length);

	if (list_code)
		print_string_directive(string,string_length);

	int i=0;
	while(i+8<=string_length) {
		store_data_l(
				string[i] +
				((uint64_t)string[i+1]<<8) +
				((uint64_t)string[i+2]<<16) +
				((uint64_t)string[i+3]<<24) +
				((uint64_t)string[i+4]<<32) +
				((uint64_t)string[i+5]<<40) +
				((uint64_t)string[i+6]<<48) +
				((uint64_t)string[i+7]<<56));
		i+=8;
	}
	if (i!=string_length) {
		uint64_t n = 0;
		int s = 0;

		while(i<string_length) {
			n |= (uint64_t)string[i]<<s;
			s+=8;
			++i;
		}
		store_data_l(n);
	}

	pgrm.words_in_strings += string_length / 8 + (string_length % 8 == 0 ? 0 : 1);
}

void code_buildAC(char *string,int string_length) {
	struct label *string_label;

	string_label=new_internal_label();
	string_label->label_offset=(pgrm.data_size<<2)+1;

	add_instruction_internal_label(CbuildAC,string_label);
	if (list_code) {
		printf("\t.data\n");
		printf("\t.data4 __STRING__+2\n");
	}
	store_data_label_value("__STRING__",2);
	store_string(string,string_length);
	if (list_code)
		printf("\t.text\n");
}

void code_buildB_b(int b_offset) {
	add_instruction_w(CbuildB_b,b_offset);
}

void code_buildC(int value) {
	add_instruction_c(CbuildC,value);
}

void code_buildC_b(int b_offset) {
	add_instruction_w(CbuildC_b,b_offset);
}

void code_buildF_b(int b_offset) {
	add_instruction_w(CbuildF_b,b_offset);
}

void code_buildI(CleanInt value) {
	add_instruction_i(CbuildI,value);
}

void code_buildI_b(int b_offset) {
	add_instruction_w(CbuildI_b,b_offset);
}

void code_buildR(double value) {
	add_instruction_r(CbuildR,value);
}

void code_buildR_b(int b_offset) {
	add_instruction_w(CbuildR_b,b_offset);
}

void code_buildhr(char descriptor_name[],int a_size,int b_size) {
	if (a_size==0) {
		if (b_size==1) {
			add_instruction_label_offset(Cbuildhr01,descriptor_name,2);
			return;
		}
		if (b_size==2) {
			add_instruction_label_offset(Cbuildhr02,descriptor_name,2);
			return;
		}
		if (b_size==3) {
			add_instruction_label_offset(Cbuildhr03,descriptor_name,2);
			return;
		}
	} else if (a_size==1) {
		if (b_size==0) {
			add_instruction_label_offset(Cbuildhr10,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_label_offset(Cbuildhr11,descriptor_name,2);
			return;
		}
		if (b_size==2) {
			add_instruction_label_offset(Cbuildhr12,descriptor_name,2);
			return;
		}
		if (b_size==3) {
			add_instruction_label_offset(Cbuildhr13,descriptor_name,2);
			return;
		}
	} else if (a_size==2) {
		if (b_size==0) {
			add_instruction_label_offset(Cbuildhr20,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_label_offset(Cbuildhr21,descriptor_name,2);
			return;
		}
		if (b_size==2) {
			add_instruction_label_offset(Cbuildhr22,descriptor_name,2);
			return;
		}
	} else if (a_size==3) {
		if (b_size==0) {
			add_instruction_label_offset(Cbuildhr30,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_label_offset(Cbuildhr31,descriptor_name,2);
			return;
		}
	} else if (a_size==4) {
		if (b_size==0) {
			add_instruction_label_offset(Cbuildhr40,descriptor_name,2);
			return;
		}
	}

	if (a_size==0 && b_size>=5) {
		add_instruction_w_label_offset(Cbuildhr0b,b_size,descriptor_name,2);
		return;
	} else if (a_size==1 && b_size>=4) {
		add_instruction_w_label_offset(Cbuildhr1b,b_size,descriptor_name,2);
		return;
	} else if (a_size>=2 && b_size>=2) {
		add_instruction_w_label_offset_w(Cbuildhr,a_size+b_size,descriptor_name,2,a_size);
		return;
	} else if (a_size>=4 && b_size==1) {
		add_instruction_w_label_offset(Cbuildhra1,a_size,descriptor_name,2);
		return;
	} else if (a_size>=5 && b_size==0) {
		add_instruction_w_label_offset(Cbuildhra0,a_size+2,descriptor_name,2);
		return;
	}

	fprintf(stderr, "Error: buildhr %s %d %d\n",descriptor_name,a_size,b_size);
	exit(1);
}

void code_build_r(char descriptor_name[],int a_size,int b_size,int a_offset,int b_offset) {
	if (a_size==0) {
		if (b_size==1) {
			add_instruction_w_label_offset(Cbuild_r01,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==2) {
			add_instruction_w_label_offset(Cbuild_r02,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==3) {
			add_instruction_w_label_offset(Cbuild_r03,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==4) {
			add_instruction_w_label_offset(Cbuild_r04,b_offset,descriptor_name,2);
			return;
		}
	} else if (a_size==1) {
		if (b_size==0) {
			add_instruction_w_label_offset(Cbuild_r10,-a_offset,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_w_w_label_offset(Cbuild_r11,-a_offset,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==2) {
			add_instruction_w_w_label_offset(Cbuild_r12,-a_offset,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==3) {
			add_instruction_w_w_label_offset(Cbuild_r13,-a_offset,b_offset,descriptor_name,2);
			return;
		}
	} else if (a_size==2) {
		if (b_size==0) {
			add_instruction_w_label_offset(Cbuild_r20,-a_offset,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_w_w_label_offset(Cbuild_r21,-a_offset,b_offset,descriptor_name,2);
			return;
		}
	} else if (a_size==3) {
		if (b_size==0) {
			add_instruction_w_label_offset(Cbuild_r30,-a_offset,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_w_w_label_offset(Cbuild_r31,-a_offset,b_offset,descriptor_name,2);
			return;
		}
	} else if (a_size==4 && b_size==0) {
		add_instruction_w_label_offset(Cbuild_r40,-a_offset,descriptor_name,2);
		return;
	}

	if (a_size==0 && b_size>=5) {
		add_instruction_w_label_offset(Cbuild_r0b,b_size,descriptor_name,2);
		return;
	} else if (a_size==1 && b_size>=4) {
		add_instruction_w_w_label_offset_w(Cbuild_r1b,b_size,-a_offset,descriptor_name,2,b_offset);
		return;
	} else if (a_size>=2 && b_size>=2) {
		add_instruction_w_w_label_offset_w_w(Cbuild_r,a_size+b_size,-a_offset,descriptor_name,2,a_size,b_offset);
		return;
	} else if (a_size>=4 && b_size==1) {
		add_instruction_w_w_label_offset_w(Cbuild_ra1,a_size,-a_offset,descriptor_name,2,b_offset);
		return;
	} else if (a_size>=5 && b_size==0) {
		add_instruction_w_w_label_offset(Cbuild_ra0,a_size,-a_offset,descriptor_name,2);
		return;
	}

	fprintf(stderr, "Error: build_r %s %d %d %d %d\n",descriptor_name,a_size,b_size,a_offset,b_offset);
	exit(1);
}

void code_build_u(char descriptor_name[],int a_size,int b_size,char *code_name) {
	if (a_size==0) {
		if (b_size==1) {
			add_instruction_label(Cbuild_u01,code_name);
			return;
		}
		if (b_size==2) {
			add_instruction_label(Cbuild_u02,code_name);
			return;
		}
		if (b_size==3) {
			add_instruction_label(Cbuild_u03,code_name);
			return;
		}
	} else if (a_size==1) {
		if (b_size==1) {
			add_instruction_label(Cbuild_u11,code_name);
			return;
		}
		if (b_size==2) {
			add_instruction_label(Cbuild_u12,code_name);
			return;
		}
		if (b_size==3) {
			add_instruction_label(Cbuild_u13,code_name);
			return;
		}
	} else if (a_size==2) {
		if (b_size==1) {
			add_instruction_label(Cbuild_u21,code_name);
			return;
		}
		if (b_size==2) {
			add_instruction_label(Cbuild_u22,code_name);
			return;
		}
	} else if (a_size==3) {
		if (b_size==1) {
			add_instruction_label(Cbuild_u31,code_name);
			return;
		}
	}

	if (a_size>=2 && b_size>=2) {
		add_instruction_w_label_w(Cbuild_u,a_size+b_size,code_name,a_size);
		return;
	}
	if (a_size>=4 && b_size==1) {
		add_instruction_w_label(Cbuild_ua1,a_size,code_name);
		return;
	}

	fprintf(stderr, "Error: build_u %s %d %d %s\n",descriptor_name,a_size,b_size,code_name);
	exit(1);
}

void code_cosR(void) {
	add_instruction(CcosR);
}

void code_create(int n_arguments) {
	if (n_arguments<=2) {
		add_instruction(Ccreate);
		return;
	} else {
		add_instruction_w(Ccreates,n_arguments+1);
		return;
	}
}

static int is__rocid(char *element_descriptor) {
	return(element_descriptor[1]=='R' && element_descriptor[2]=='O' && element_descriptor[3]=='C' &&
			element_descriptor[4]=='I' && element_descriptor[5]=='D' && element_descriptor[6]=='\0');
}

static int is__orld(char *element_descriptor) {
	return(element_descriptor[1]=='O' && element_descriptor[2]=='R' && element_descriptor[3]=='L' &&
			element_descriptor[4]=='D' && element_descriptor[5]=='\0');
}

void code_create_array(char element_descriptor[],int a_size,int b_size) {
	switch(element_descriptor[0]) {
		case 'B':
			if (element_descriptor[1]=='O' && element_descriptor[2]=='O' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(Ccreate_arrayBOOL);
				return;
			}
		case 'C':
			if (element_descriptor[1]=='H' && element_descriptor[2]=='A' && element_descriptor[3]=='R' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(Ccreate_arrayCHAR);
				return;
			}
			break;
		case 'I':
			if (element_descriptor[1]=='N' && element_descriptor[2]=='T' && element_descriptor[3]=='\0') {
				add_instruction(Ccreate_arrayINT);
				return;
			}
			break;
		case 'R':
			if (element_descriptor[1]=='E' && element_descriptor[2]=='A' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(Ccreate_arrayREAL);
				return;
			}
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction(Ccreate_array);
				return;
			}
			break;
	}

	do {
		switch(element_descriptor[0]) {
			case 'P':
				if (is__rocid (element_descriptor))
					continue;
				break;
			case 'R':
				if (element_descriptor[1]=='E' && element_descriptor[2]=='A' && element_descriptor[3]=='L') {
					if (element_descriptor[4]=='\0')
						continue;
				}
				break;
			case 'A':
				if (element_descriptor[1]=='R' && element_descriptor[2]=='R' && element_descriptor[3]=='A' &&
					element_descriptor[4]=='Y' && element_descriptor[5]=='\0')
				{
					continue;
				}
				break;
			case 'S':
				if (element_descriptor[1]=='T' && element_descriptor[2]=='R' && element_descriptor[3]=='I' &&
					element_descriptor[4]=='N' && element_descriptor[5]=='G' && element_descriptor[6]=='\0')
				{
					continue;
				}
				break;
			case 'W':
				if (is__orld (element_descriptor))
					continue;
				break;
		}

		add_instruction_w_w_label(Ccreate_array_r,a_size+b_size,b_size,element_descriptor);
		return;
	} while(0);

	fprintf(stderr, "Error: create_array %s %d %d\n",element_descriptor,a_size,b_size);
	exit(1);
}

void code_create_array_(char element_descriptor[],int a_size,int b_size) {
	switch(element_descriptor[0]) {
		case 'B':
			if (element_descriptor[1]=='O' && element_descriptor[2]=='O' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(Ccreate_array_BOOL);
				return;
			}
		case 'C':
			if (element_descriptor[1]=='H' && element_descriptor[2]=='A' && element_descriptor[3]=='R' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(Ccreate_array_CHAR);
				return;
			}
			break;
		case 'I':
			if (element_descriptor[1]=='N' && element_descriptor[2]=='T' && element_descriptor[3]=='\0') {
				add_instruction(Ccreate_array_INT);
				return;
			}
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction(Ccreate_array_);
				return;
			}
	}

	if (b_size==0)
		add_instruction_w_label(Ccreate_array_r_a,a_size,element_descriptor);
	else if (a_size==0)
		add_instruction_w_label(Ccreate_array_r_b,b_size,element_descriptor);
	else
		add_instruction_w_w_label(Ccreate_array_r_,a_size+b_size,a_size,element_descriptor);
}

void code_decI(void) {
	add_instruction(CdecI);
}

void code_divI(void) {
	add_instruction(CdivI);
}

void code_divR(void) {
	add_instruction(CdivR);
}

void code_entierR(void) {
	add_instruction(CentierR);
}

void code_eqAC_a(char *string,int string_length) {
	struct label *string_label;

	string_label=new_internal_label();
	string_label->label_offset=(pgrm.data_size<<2)+1;

	add_instruction_internal_label(CeqAC_a,string_label);

	if (list_code)
		printf("\t.data\n");
	store_string(string,string_length);
	if (list_code)
		printf("\t.text\n");
}

void code_eqB(void) {
	add_instruction(CeqB);
}

void code_eqB_a(int value,int a_offset) {
	if (value==0)
		add_instruction_w(CeqB_aFALSE,-a_offset);
	else
		add_instruction_w(CeqB_aTRUE,-a_offset);
}

void code_eqB_b(int value,int b_offset) {
	if (value==0)
		add_instruction_w(CeqB_bFALSE,b_offset);
	else
		add_instruction_w(CeqB_bTRUE,b_offset);
}

void code_eqC(void) {
	add_instruction(CeqC);
}

void code_eqC_a(int value,int a_offset) {
	add_instruction_w_c(CeqC_a,-a_offset,value);
}

void code_eqC_b(int value,int b_offset) {
	add_instruction_w_c(CeqC_b,b_offset,value);
}

void code_eqD_b(char descriptor_name[],int arity) {
	add_instruction_label_offset(CeqD_b,descriptor_name,(arity<<3)+2);
}

void code_eqI(void) {
	add_instruction(CeqI);
}

void code_eqI_b(CleanInt value,int b_offset) {
	add_instruction_w_i(CeqI_b,b_offset,value);
}

void code_eqI_a(CleanInt value,int a_offset) {
	add_instruction_w_i(CeqI_a,-a_offset,value);
}

void code_eqR(void) {
	add_instruction(CeqR);
}

void code_eq_desc(char descriptor_name[],int arity,int a_offset) {
	add_instruction_w_label_offset(Ceq_desc,-a_offset,descriptor_name,(arity<<3)+2);
}

void code_eq_desc_b(char descriptor_name[],int arity) {
	if (arity == 0) {
		add_instruction_label_offset(Ceq_desc_b0,descriptor_name,2);
		return;
	}

	fprintf(stderr, "Error: eq_desc_b %d\n", arity);
	exit(1);
}

void code_eq_nulldesc(char descriptor_name[], int a_offset) {
	add_instruction_w_label_offset(Ceq_nulldesc,-a_offset,descriptor_name,2);
}

void code_exit_false(char label_name[]) {
	add_instruction_label(Cjmp_false,label_name);
}

void code_expR(void) {
	add_instruction(CexpR);
}

void code_fill(char descriptor_name[],int arity,char *code_name,int a_offset) {
	if (code_name[0]=='_' && code_name[1]=='_' && code_name[2]=='h' && code_name[3]=='n' && code_name[4]=='f' && code_name[5]=='\0') {
		fprintf(stderr, "Warning: fill %s %d %s\n",descriptor_name,arity,code_name);
		code_fillh(descriptor_name,arity,a_offset);
		return;
	}

	if (arity<0 || arity==1) {
		add_instruction_w_label(Cfill1,-a_offset,code_name);
		return;
	}
	if (arity==2) {
		add_instruction_w_label(Cfill2,-a_offset,code_name);
		return;
	}
	if (arity==3) {
		add_instruction_w_label(Cfill3,-a_offset,code_name);
		return;
	}
	if (arity==4) {
		add_instruction_w_label(Cfill4,-a_offset,code_name);
		return;
	}

	if (arity>4) {
		add_instruction_w_w_label(Cfill,-a_offset,arity,code_name);
		return;
	}

	fprintf(stderr, "Error: fill %s %d %s %d\n",descriptor_name,arity,code_name,a_offset);
	exit(1);
}

void code_fill1(char descriptor_name[],int arity,int a_offset,char bits[]) {
	if (arity==1) {
		if (bits[1]=='1') {
			if (bits[0]=='0')
				add_instruction_w(Cfill1010,-a_offset);
			else
				add_instruction_w_label_offset(Cfillh1,-a_offset,descriptor_name,(arity<<3)+2);
			return;
		}
	} else if (arity==2) {
		if (bits[1]=='0') {
			if (bits[0]=='0' && bits[2]=='1') {
				add_instruction_w(Cfill1001,-a_offset);
				return;
			}
		} else {
			if (bits[2]=='0') {
				if (bits[0]=='0') {
					add_instruction_w(Cfill1010,-a_offset);
					return;
				}
			} else {
				if (bits[0]=='0')
					add_instruction_w(Cfill1011,-a_offset);
				else
					add_instruction_w_label_offset(Cfillh2,-a_offset,descriptor_name,(arity<<3)+2);
				return;
			}
		}
	}

	fprintf(stderr, "Error: fill1 %s %d %d %s\n",descriptor_name,arity,a_offset,bits);
	exit(1);
}

void code_fill1_r(char descriptor_name[],int a_size,int b_size,int root_offset,char bits[]) {
	if (b_size==0) {
		if (a_size==1) {
			if (bits[0]=='0') {
				if (bits[1]=='0')
					return;
				add_instruction_w(Cfill1010,-root_offset);
				return;
			} else {
				if (bits[1]=='1') {
					add_instruction_w_label_offset(Cfillh1,-root_offset,descriptor_name,2);
					return;
				}
			}
		}

		if (a_size==2) {
			if (bits[0]=='0' && bits[1]=='1') {
				if (bits[2]=='0')
					add_instruction_w(Cfill1010,-root_offset);
				else
					add_instruction_w(Cfill1011,-root_offset);
				return;
			}
		}
	}

	fprintf(stderr, "Error: fill1_r %s %d %d %d %s\n",descriptor_name,a_size,b_size,root_offset,bits);
	exit(1);
}

void code_fill2(char descriptor_name[],int arity,int a_offset,char bits[]) {
	int n_bits,i;

	n_bits=0;
	for(i=0; i<=arity; ++i)
		 n_bits += bits[i]-'0';

	if (n_bits==0)
		return;

	if (n_bits==1) {
		int bit_n;

		for(bit_n=0; bit_n<=arity; ++bit_n)
			if (bits[bit_n]!='0')
				break;

		if (bit_n==1) {
			add_instruction_w(Cfill1010,-a_offset);
			return;
		}
		if (arity>2 && bit_n>=2) {
			add_instruction_w_w(Cfill2a001,-a_offset,bit_n-2);
			return;
		}
	}

	if (n_bits==2) {
		if (bits[0]=='0' && bits[1]=='1') {
			int bit_n;

			for(bit_n=2; bit_n<=arity; ++bit_n)
				if (bits[bit_n]!='0')
					break;

			if (arity>2) {
				add_instruction_w_w(Cfill2a011,-a_offset,bit_n-2);
				return;
			}
		}
	}

	if (n_bits==3) {
		if (bits[0]=='0' && bits[1]=='1') {
			int bit_n1,bit_n2;

			for(bit_n1=2; bit_n1<=arity; ++bit_n1)
				if (bits[bit_n1]!='0')
					break;

			for(bit_n2=bit_n1+1; bit_n2<=arity; ++bit_n2)
				if (bits[bit_n2]!='0')
					break;

			add_instruction_w_w_w(Cfill2a012,-a_offset,bit_n1-2,bit_n2-2);
			return;
		}
	}

	fprintf(stderr, "Error: fill2 %s %d %d %s\n",descriptor_name,arity,a_offset,bits);
	exit(1);
}

void code_fill2_r(char descriptor_name[],int a_size,int b_size,int root_offset,char bits[]) {
	int n_bits,i,ab_size;

	ab_size=a_size+b_size;

	n_bits=0;
	for(i=0; i<=ab_size; ++i)
		 n_bits += bits[i]-'0';

	if (n_bits==0)
		return;

	if (n_bits==1) {
		int bit_n;

		for(bit_n=0; bit_n<=ab_size; ++bit_n)
			if (bits[bit_n]!='0')
				break;

		if (bit_n==1) {
			add_instruction_w(Cfill1010,-root_offset);
			return;
		}

		if (ab_size>2 && bit_n>=2) {
			if (bit_n-1<a_size)
				add_instruction_w_w(Cfill2a001,-root_offset,bit_n-2);
			else
				add_instruction_w_w(Cfill2b001,-root_offset,bit_n-2);
			return;
		}
	}

	if (n_bits==2) {
		if (bits[0]=='0') {
			if (bits[1]=='0') {
				int bit_n1,bit_n2;

				for(bit_n1=2; bit_n1<=ab_size; ++bit_n1)
					if (bits[bit_n1]!='0')
						break;

				for(bit_n2=bit_n1+1; bit_n2<=ab_size; ++bit_n2)
					if (bits[bit_n2]!='0')
						break;

				if (bit_n1-1>=a_size) {
					add_instruction_w_w_w(Cfill2b002,-root_offset,bit_n1-2,bit_n2-2);
					return;
				} else if (bit_n2-1>=a_size) {
					add_instruction_w_w_w(Cfill2ab002,-root_offset,bit_n1-2,bit_n2-2);
					return;
				}
			} else {
				int bit_n;

				if (ab_size==2 && b_size==0) {
					add_instruction_w(Cfill1011,-root_offset);
					return;
				}

				for(bit_n=2; bit_n<=ab_size; ++bit_n)
					if (bits[bit_n]!='0')
						break;

				if (ab_size>2) {
					if (a_size!=0)
						if (bit_n-1<a_size)
							add_instruction_w_w(Cfill2a011,-root_offset,bit_n-2);
						else
							add_instruction_w_w(Cfill2ab011,-root_offset,bit_n-2);
					else
						add_instruction_w_w(Cfill2b011,-root_offset,bit_n-2);
					return;
				}
			}
		}
	}

	if (n_bits==3) {
		if (bits[0]=='0') {
			if (bits[1]=='0') {
				int bit_n1,bit_n2,bit_n3;

				for(bit_n1=1; bit_n1<ab_size; ++bit_n1)
					if (bits[bit_n1]!='0')
						break;

				for(bit_n2=bit_n1+1; bit_n2<ab_size; ++bit_n2)
					if (bits[bit_n2]!='0')
						break;

				for(bit_n3=bit_n2+1; bit_n3<ab_size; ++bit_n3)
					if (bits[bit_n3]!='0')
						break;

				if (bit_n2-1<a_size && bit_n3-1>=a_size) {
					add_instruction_w_w_w_w(Cfill2ab003,-root_offset,bit_n1-2,bit_n2-2,bit_n3-2);
					return;
				}
			} else if (bits[1]=='1') {
				int bit_n1,bit_n2;

				for(bit_n1=2; bit_n1<=ab_size; ++bit_n1)
					if (bits[bit_n1]!='0')
						break;

				for(bit_n2=bit_n1+1; bit_n2<=ab_size; ++bit_n2)
					if (bits[bit_n2]!='0')
						break;

				if (a_size==0) {
					add_instruction_w_w_w(Cfill2b012,-root_offset,bit_n1-2,bit_n2-2);
					return;
				}
			}
		}
	}

	if (n_bits==4) {
		if (bits[0]=='0' && bits[1]=='1') {
			int bit_n1,bit_n2,bit_n3;

			for(bit_n1=2; bit_n1<ab_size; ++bit_n1)
				if (bits[bit_n1]!='0')
					break;

			for(bit_n2=bit_n1+1; bit_n2<ab_size; ++bit_n2)
				if (bits[bit_n2]!='0')
					break;

			for(bit_n3=bit_n2+1; bit_n3<ab_size; ++bit_n3)
				if (bits[bit_n3]!='0')
					break;

			if (bit_n2-1<a_size && bit_n3-1>=a_size) {
				add_instruction_w_w_w_w(Cfill2ab013,-root_offset,bit_n1-2,bit_n2-2,bit_n3-2);
				return;
			}
		}
	}

	fprintf(stderr, "Error: fill2_r %s %d %d %d %s\n",descriptor_name,a_size,b_size,root_offset,bits);
	exit(1);
}

void code_fill3(char descriptor_name[],int arity,int a_offset,char bits[]) {
	int n_bits,i;

	n_bits=0;
	for(i=0; i<arity; ++i)
		 n_bits += bits[i]-'0';

	if (n_bits==3 && bits[0]=='1' && arity>2) {
		int bit_n1,bit_n2;

		for(bit_n1=1; bit_n1<arity; ++bit_n1)
			if (bits[bit_n1]!='0')
				break;

		for(bit_n2=bit_n1+1; bit_n2<arity; ++bit_n2)
			if (bits[bit_n2]!='0')
				break;

		add_instruction_w_label_offset_w_w(Cfill3a12,-a_offset,descriptor_name,(arity<<3)+2,bit_n1-1,bit_n2-1);
		return;
	}

	fprintf(stderr, "Error: fill3 %s %d %d %s\n",descriptor_name,arity,a_offset,bits);
	exit(1);
}

void code_fill3_r(char descriptor_name[],int a_size,int b_size,int root_offset,char bits[]) {
	int n_bits,i,ab_size;

	ab_size=a_size+b_size;

	n_bits=0;
	for(i=0; i<ab_size; ++i)
		 n_bits += bits[i]-'0';

	if (n_bits==1 && bits[0]=='1' && ab_size>2) {
		if (a_size>=1) {
			add_instruction_w_label_offset(Cfill3a10,-root_offset,descriptor_name,2);
			return;
		}
	}

	if (n_bits==2 && bits[0]=='1' && ab_size>2) {
		int bit_n;

		for(bit_n=1; bit_n<ab_size; ++bit_n)
			if (bits[bit_n]!='0')
				break;

		if (a_size>=2) {
			add_instruction_w_label_offset_w(Cfill3a11,-root_offset,descriptor_name,2,bit_n-1);
			return;
		}
	}

	if (n_bits==4 && bits[0]=='1' && ab_size>2) {
		int bit_n1,bit_n2,bit_n3;

		for(bit_n1=1; bit_n1<=ab_size; ++bit_n1)
			if (bits[bit_n1]!='0')
				break;

		for(bit_n2=bit_n1+1; bit_n2<=ab_size; ++bit_n2)
			if (bits[bit_n2]!='0')
				break;

		for(bit_n3=bit_n2+1; bit_n3<=ab_size; ++bit_n3)
			if (bits[bit_n3]!='0')
				break;

		if (bit_n2<a_size && bit_n3>=a_size) {
			add_instruction_w_label_offset_w_w_w(Cfill3aaab13,-root_offset,descriptor_name,2,bit_n1-1,bit_n2-1,bit_n3-1);
			return;
		}
	}

	fprintf(stderr, "Error: fill3_r %s %d %d %d %s\n",descriptor_name,a_size,b_size,root_offset,bits);
	exit(1);
}

void code_fillcaf(char *label_name,int a_stack_size,int b_stack_size) {
	add_instruction_label_w_w(Cfillcaf,label_name,a_stack_size,b_stack_size);
}

void code_fillh(char descriptor_name[],int arity,int a_offset) {
	switch(arity) {
		case 0:
			add_instruction_w_label_offset(Cfillh0,-a_offset,descriptor_name,2);
			break;
		case 1:
			add_instruction_w_label_offset(Cfillh1,-a_offset,descriptor_name,(1<<3)+2);
			break;
		case 2:
			add_instruction_w_label_offset(Cfillh2,-a_offset,descriptor_name,(2<<3)+2);
			break;
		case 3:
			add_instruction_w_label_offset(Cfillh3,-a_offset,descriptor_name,(3<<3)+2);
			break;
		case 4:
			add_instruction_w_label_offset(Cfillh4,-a_offset,descriptor_name,(4<<3)+2);
			break;
		default:
			add_instruction_w_w_label_offset(Cfillh,arity-1,-a_offset,descriptor_name,(arity<<3)+2);
			break;
	}
}

void code_fillB_b(int b_offset,int a_offset) {
	add_instruction_w_w(CfillB_b,-a_offset,b_offset);
}

void code_fillC_b(int b_offset,int a_offset) {
	add_instruction_w_w(CfillC_b,-a_offset,b_offset);
}

void code_fillF_b(int b_offset,int a_offset) {
	add_instruction_w_w(CfillF_b,-a_offset,b_offset);
}

void code_fillI_b(int b_offset,int a_offset) {
	add_instruction_w_w(CfillI_b,-a_offset,b_offset);
}

void code_fillR_b(int b_offset,int a_offset) {
	add_instruction_w_w(CfillR_b,-a_offset,b_offset);
}

void code_fill_a(int from_offset,int to_offset) {
	add_instruction_w_w(Cfill_a,-from_offset,-to_offset);
}

void code_fill_r(char descriptor_name[],int a_size,int b_size,int root_offset,int a_offset,int b_offset) {
	if (a_size==0) {
		if (b_size==1) {
			add_instruction_w_w_label_offset(Cfill_r01,-root_offset,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==2) {
			add_instruction_w_w_label_offset(Cfill_r02,-root_offset,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==3) {
			add_instruction_w_w_label_offset(Cfill_r03,-root_offset,b_offset,descriptor_name,2);
			return;
		}
	} else if (a_size==1) {
		if (b_size==0) {
			add_instruction_w_w_label_offset(Cfill_r10,-root_offset,-a_offset,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_w_w_w_label_offset(Cfill_r11,-root_offset,-a_offset,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==2) {
			add_instruction_w_w_w_label_offset(Cfill_r12,-root_offset,-a_offset,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==3) {
			add_instruction_w_w_w_label_offset(Cfill_r13,-root_offset,-a_offset,b_offset,descriptor_name,2);
			return;
		}
	} else if (a_size==2) {
		if (b_size==0) {
			add_instruction_w_w_label_offset(Cfill_r20,-root_offset,-a_offset,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_w_w_w_label_offset(Cfill_r21,-root_offset,-a_offset,b_offset,descriptor_name,2);
			return;
		}
		if (b_size==2) {
			add_instruction_w_w_w_label_offset(Cfill_r22,-root_offset,-a_offset,b_offset,descriptor_name,2);
			return;
		}
	} else if (a_size==3) {
		if (b_size==0) {
			add_instruction_w_w_label_offset(Cfill_r30,-root_offset,-a_offset,descriptor_name,2);
			return;
		}
		if (b_size==1) {
			add_instruction_w_w_w_label_offset(Cfill_r31,-root_offset,-a_offset,b_offset,descriptor_name,2);
			return;
		}
	} else if (a_size==4) {
		if (b_size==0) {
			add_instruction_w_w_label_offset(Cfill_r40,-root_offset,-a_offset,descriptor_name,2);
			return;
		}
	}

	if (a_size==1 && b_size>=4) {
		add_instruction_w_w_w_label_offset_w(Cfill_r1b,b_size,-root_offset,-a_offset,descriptor_name,2,b_offset);
		return;
	} else if (a_size>=2 && b_size>=2) {
		add_instruction_w_w_w_label_offset_w_w(Cfill_r,a_size+b_size,-root_offset,-a_offset,descriptor_name,2,a_size,b_offset);
		return;
	} else if (a_size>=4 && b_size==1) {
		add_instruction_w_w_w_label_offset_w(Cfill_ra1,a_size,-root_offset,-a_offset,descriptor_name,2,b_offset);
		return;
	} else if (a_size>=5 && b_size==0) {
		add_instruction_w_w_w_label_offset(Cfill_ra0,a_size,-root_offset,-a_offset,descriptor_name,2);
		return;
	}

	fprintf(stderr, "Error: code_fill_r %s %d %d %d %d %d\n",descriptor_name,a_size,b_size,root_offset,a_offset,b_offset);
	exit(1);
}

void code_get_node_arity(int a_offset) {
	add_instruction_w(Cget_node_arity,-a_offset);
}

void code_gtI(void) {
	add_instruction(CgtI);
}

void code_halt(void) {
	add_instruction(Chalt);
}

void code_incI(void) {
	add_instruction(CincI);
}

void code_is_record(int a_offset) {
	add_instruction_w(Cis_record, a_offset);
}

int last_da,last_db;

void code_jmp(char label_name[]) {
	last_d=0;

	if (!strcmp(label_name,"__driver")) {
		add_instruction_label(Cjsr,"__print__graph");
		add_instruction(Chalt);
	} else {
		add_instruction_label(Cjmp,label_name);
	}
}

void code_jmp_ap(int n_apply_args) {
	if (n_apply_args==1) {
		add_instruction(Cjmp_ap1);
		return;
	}
	if (n_apply_args==2) {
		add_instruction(Cjmp_ap2);
		return;
	}
	if (n_apply_args==3) {
		add_instruction(Cjmp_ap3);
		return;
	}

	fprintf(stderr, "Error: jmp_ap %d\n",n_apply_args);
	exit(1);
}

void code_jmp_eval(void) {
	add_instruction(Cjmp_eval);
}

void code_jmp_eval_upd(void) {
	add_instruction(Cjmp_eval_upd);
}

void code_jmp_false(char label_name[]) {
	add_instruction_label(Cjmp_false,label_name);
}

void code_jmp_true(char label_name[]) {
	add_instruction_label(Cjmp_true,label_name);
}

char *lib_functions[] = {
	/*0*/ "eqAC", /* CeqAC */
	/*1*/ "catAC",
	/*2*/ "sliceAC",
	/*3*/ "ItoAC", "BtoAC",
	/*5*/ "print__string__",
	/*6*/ "openF", "stdioF",
	/*8*/ "closeF",
	/*9*/ "readLineF",
	/*10*/ "endF",
	/*11*/ "cmpAC",
	/*12*/ "writeFI",
	/*13*/ "writeFS",
	/*14*/ "writeFC",
	"openSF"
};

static int get_lib_function_n(char label_name[]) {
	int lib_function_n,n_lib_functions;

	n_lib_functions = sizeof(lib_functions) / sizeof (char*);
	for(lib_function_n=0; lib_function_n<n_lib_functions; ++lib_function_n)
		if (!strcmp (label_name,lib_functions[lib_function_n]))
			return lib_function_n;

	return -1;
}

void code_jsr(char label_name[]) {
	int lib_function_n;

	if (last_jsr_with_d) {
		fprintf(stderr, "Error: .d or .o directive used incorrectly near jsr\n");
		exit(1);
	}

	if (last_d) {
		last_jsr_with_d=1;
		last_d=0;
	}

	lib_function_n = get_lib_function_n(label_name);

	if (lib_function_n>=0) {
		if (lib_function_n==0)
			add_instruction(CeqAC);
		else
			add_instruction_w(Cjesr,lib_function_n);

		last_jsr_with_d=0;
		return;
	}

	add_instruction_label(Cjsr,label_name);
}

void code_jsr_ap(int n_apply_args) {
	switch (n_apply_args) {
		case 1: add_instruction(Cjsr_ap1); return;
		case 2: add_instruction(Cjsr_ap2); return;
		case 3: add_instruction(Cjsr_ap3); return;
		case 4: add_instruction(Cjsr_ap4); return;
		case 5: add_instruction(Cjsr_ap5); return;
	}

	fprintf(stderr, "Error: jsr_ap %d\n",n_apply_args);
	exit(1);
}

void code_jsr_eval(int a_offset) {
	switch(a_offset) {
		case 0:
			add_instruction(Cjsr_eval0);
			break;
		case 1:
			add_instruction(Cjsr_eval1);
			add_instruction(Cswap_a1);
			break;
		case 2:
			add_instruction(Cjsr_eval2);
			add_instruction(Cswap_a2);
			break;
		case 3:
			add_instruction(Cjsr_eval3);
			add_instruction(Cswap_a3);
			break;
		default:
			add_instruction_w(Cjsr_eval,-a_offset);
			add_instruction_w(Cswap_a,-a_offset);
	}
}

void code_lnR(void) {
	add_instruction(ClnR);
}

void code_log10R(void) {
	add_instruction(Clog10R);
}

void code_ltC(void) {
	add_instruction(CltC);
}

void code_ltI(void) {
	add_instruction(CltI);
}

void code_ltR(void) {
	add_instruction(CltR);
}

void code_mulI(void) {
	add_instruction(CmulI);
}

void code_mulR(void) {
	add_instruction(CmulR);
}

void code_negI(void) {
	add_instruction(CnegI);
}

void code_negR(void) {
	add_instruction(CnegR);
}

void code_not(void) {
	add_instruction(CnotI);
}

void code_notB(void) {
	add_instruction(CnotB);
}

void code_or(void) {
	add_instruction(CorI);
}

void code_pop_a(int n) {
	add_instruction_w(Cpop_a,-n);
}

void code_pop_b(int n) {
	add_instruction_w(Cpop_b,n);
}

void code_powR(void) {
	add_instruction(CpowR);
}

void code_print(char *string,int length) {
	struct label *string_label;

	string_label=new_internal_label();
	string_label->label_offset=(pgrm.data_size<<2)+1;

	add_instruction_internal_label(Cprint,string_label);

	if (list_code)
		printf("\t.data\n");
	store_string(string,length);
	if (list_code)
		printf("\t.text\n");
}

void code_printD(void) {
	add_instruction(CprintD);
}

void code_print_char(void) {
	add_instruction(Cprint_char);
}

void code_print_int(void) {
	add_instruction(Cprint_int);
}

void code_print_real(void) {
	add_instruction(Cprint_real);
}

void code_print_sc(char *string,int length) {
	struct label *string_label;

	string_label=new_internal_label();
	string_label->label_offset=(pgrm.data_size<<2)+1;

	add_instruction_internal_label(Cprint,string_label);

	if (list_code)
		printf("\t.data\n");
	store_string(string,length);
	if (list_code)
		printf("\t.text\n");
}

void code_print_symbol_sc(int a_offset) {
	add_instruction_w(Cprint_symbol_sc,-a_offset);
}

void code_pushA_a(int a_offset) {
	add_instruction_w(CpushA_a,-a_offset);
}

void code_pushB(int b) {
	if (b==0)
		add_instruction(CpushBFALSE);
	else
		add_instruction(CpushBTRUE);
}

void code_pushB_a(int a_offset) {
	add_instruction_w(CpushB_a,-a_offset);
}

void code_pushB0_pop_a1() {
	add_instruction(CpushB0_pop_a1);
}

void code_pushC(int c) {
	add_instruction_c(CpushC,c);
}

void code_pushC_a(int a_offset) {
	add_instruction_w(CpushC_a,-a_offset);
}

void code_pushC0_pop_a1() {
	add_instruction(CpushC0_pop_a1);
}

void code_pushD(char *descriptor) {
	add_instruction_label(CpushD,descriptor);
}

void code_pushD_a(int a_offset) {
	add_instruction_w(CpushD_a,-a_offset);
}

void code_pushF_a(int a_offset) {
	add_instruction_w(CpushF_a,-a_offset);
}

void code_pushI(CleanInt i) {
	add_instruction_i(CpushI,i);
}

void code_pushI_a(int a_offset) {
	add_instruction_w(CpushI_a,-a_offset);
}

void code_pushI0_pop_a1() {
	add_instruction(CpushI0_pop_a1);
}

void code_pushR(double r) {
	add_instruction_r(CpushR,r);
}

void code_pushR_a(int a_offset) {
	add_instruction_w(CpushR_a,-a_offset);
}

void code_pushcaf(char *label_name,int a_size,int b_size) {
	if (a_size==1) {
		if (b_size==0) {
			add_instruction_label_offset(Cpushcaf10,label_name,4);
			return;
		}
		if (b_size==1) {
			add_instruction_label_offset(Cpushcaf11,label_name,4);
			return;
		}
	} else if (a_size==2) {
		if (b_size==0) {
			add_instruction_label_offset(Cpushcaf20,label_name,4);
			return;
		}
	} else if (a_size==3) {
		if (b_size==1) {
			add_instruction_label_offset(Cpushcaf31,label_name,4);
			return;
		}
	}
	fprintf(stderr, "Error: pushcaf %s %d %d\n",label_name,a_size,b_size);
	exit(1);
}

void code_push_a(int a_offset) {
	add_instruction_w(Cpush_a,-a_offset);
}

void code_push_a_r_args(void) {
	add_instruction(Cpush_a_r_args);
}

void code_push_a_b(int a_offset) {
	add_instruction_w(Cpush_a_b,-a_offset);
}

void code_push_arg(int a_offset,int arity,int argument_number) {
	if (argument_number==1) {
		add_instruction_w(Cpush_arg1,-a_offset);
		return;
	}
	if (argument_number==2) {
		if (arity<=2)
			add_instruction_w(Cpush_arg2,-a_offset);
		else
			add_instruction_w(Cpush_arg2l,-a_offset);
		return;
	}
	if (argument_number==3) {
		add_instruction_w(Cpush_arg3,-a_offset);
		return;
	}
	if (argument_number==4) {
		add_instruction_w(Cpush_arg4,-a_offset);
		return;
	}
	if (argument_number>4) {
		add_instruction_w_w(Cpush_arg,-a_offset,argument_number-2);
		return;
	}

	fprintf(stderr, "Error: push_arg %d %d %d\n",a_offset,arity,argument_number);
	exit(1);
}

void code_push_args(int a_offset,int arity,int n_arguments) {
	if (arity==n_arguments) {
		switch(arity) {
			case 1:
				add_instruction_w(Cpush_args1,-a_offset);
				return;
			case 2:
				add_instruction_w(Cpush_args2,-a_offset);
				return;
			case 3:
				add_instruction_w(Cpush_args3,-a_offset);
				return;
			case 4:
				add_instruction_w(Cpush_args4,-a_offset);
				return;
			default:
				add_instruction_w_w(Cpush_args,-a_offset,arity);
				return;
		}
	}
	fprintf(stderr, "Error: push_args %d %d %d\n",a_offset,arity,n_arguments);
	exit(1);
}

void code_push_args_u(int a_offset,int arity,int n_arguments) {
	if (arity==n_arguments) {
		code_push_args(a_offset,arity,n_arguments);
		return;
	} else {
		add_instruction_w_w_w(Cpush_args_u,a_offset,arity,n_arguments);
		return;
	}
}

void code_push_arg_b(int a_offset) {
	add_instruction_w(Cpush_arg_b,-a_offset);
}

void code_push_array(int a_offset) {
	add_instruction_w(Cpush_array,-a_offset);
}

void code_push_arraysize(char element_descriptor[],int a_size,int b_size) {
	add_instruction(Cpush_arraysize);
}

void code_push_b(int b_offset) {
	add_instruction_w(Cpush_b,b_offset);
}

void code_push_node(char *label_name,int n_arguments) {
	if (strcmp(label_name,"__cycle__in__spine") && strcmp(label_name,"__reserve"))
		fprintf(stderr, "Warning: push_node not implemented for '%s'\n", label_name);

	switch(n_arguments) {
		case 0:
			add_instruction_label(Cpush_node0,label_name);
			break;
		case 1:
			add_instruction_label(Cpush_node1,label_name);
			break;
		case 2:
			add_instruction_label(Cpush_node2,label_name);
			break;
		case 3:
			add_instruction_label(Cpush_node3,label_name);
			break;
		case 4:
			add_instruction_label(Cpush_node4,label_name);
			break;
		default:
			add_instruction_w_label(Cpush_node,n_arguments,label_name);
	}
}

void code_push_node_u(char *label_name,int a_size,int b_size) {
	if (a_size==0) {
		if (b_size==1) {
			add_instruction_label(Cpush_node_u01,label_name);
			return;
		}
		if (b_size==2) {
			add_instruction_label(Cpush_node_u02,label_name);
			return;
		}
		if (b_size==3) {
			add_instruction_label(Cpush_node_u03,label_name);
			return;
		}
	} else if (a_size==1) {
		if (b_size==1) {
			add_instruction_label(Cpush_node_u11,label_name);
			return;
		}
		if (b_size==2) {
			add_instruction_label(Cpush_node_u12,label_name);
			return;
		}
		if (b_size==3) {
			add_instruction_label(Cpush_node_u13,label_name);
			return;
		}
	} else if (a_size==2) {
		if (b_size==1) {
			add_instruction_label(Cpush_node_u21,label_name);
			return;
		}
		if (b_size==2) {
			add_instruction_label(Cpush_node_u22,label_name);
			return;
		}
	} else if (a_size==3 && b_size==1) {
		add_instruction_label(Cpush_node_u31,label_name);
		return;
	}

	if (a_size>=2 && b_size>=2) {
		add_instruction_w_label_w(Cpush_node_u,a_size,label_name,b_size);
		return;
	} else if (a_size>=3 && b_size==1) {
		add_instruction_w_label(Cpush_node_ua1,a_size,label_name);
		return;
	}
	fprintf(stderr, "Error: push_node_u %s %d %d\n",label_name,a_size,b_size);
	exit(1);
}

void code_remI(void) {
	add_instruction(CremI);
}

void code_push_r_arg_D(void) {
	add_instruction(Cpush_r_arg_D);
}

void code_push_r_arg_t(void) {
	add_instruction(Cpush_r_arg_t);
}

void code_push_r_arg_u(int a_offset,int a_size,int b_size,int a_arg_offset,int a_arg_size,int b_arg_offset,int b_arg_size) {
	if (b_arg_size==0) {
		code_push_r_args_a(-a_offset,a_size,b_size,a_arg_offset,a_arg_size);
		return;
	}
	if (a_arg_size==0) {
		code_push_r_args_b(a_offset,a_size,b_size,b_arg_offset,b_arg_size);
		return;
	}

	fprintf(stderr, "Error: push_r_arg_u %d %d %d %d %d %d %d\n",a_offset,a_size,b_size,a_arg_offset,a_arg_size,b_arg_offset,b_arg_size);
	exit(1);
}

void code_push_r_args(int a_offset,int a_size,int b_size) {
	if (a_size==0) {
		if (b_size==1) {
			add_instruction_w(Cpush_r_args01,-a_offset);
			return;
		}
		if (b_size==2) {
			add_instruction_w(Cpush_r_args02,-a_offset);
			return;
		}
		if (b_size==3) {
			add_instruction_w(Cpush_r_args03,-a_offset);
			return;
		}
		if (b_size==4) {
			add_instruction_w(Cpush_r_args04,-a_offset);
			return;
		}
	} else if (a_size==1) {
		if (b_size==0) {
			add_instruction_w(Cpush_r_args10,-a_offset);
			return;
		}
		if (b_size==1) {
			add_instruction_w(Cpush_r_args11,-a_offset);
			return;
		}
		if (b_size==2) {
			add_instruction_w(Cpush_r_args12,-a_offset);
			return;
		}
		if (b_size==3) {
			add_instruction_w(Cpush_r_args13,-a_offset);
			return;
		}
	} else if (a_size==2) {
		if (b_size==0) {
			add_instruction_w(Cpush_r_args20,-a_offset);
			return;
		}
		if (b_size==1) {
			add_instruction_w(Cpush_r_args21,-a_offset);
			return;
		}
		if (b_size==2) {
			add_instruction_w(Cpush_r_args22,-a_offset);
			return;
		}
	} else if (a_size==3) {
		if (b_size==0) {
			add_instruction_w(Cpush_r_args30,-a_offset);
			return;
		}
		if (b_size==1) {
			add_instruction_w(Cpush_r_args31,-a_offset);
			return;
		}
	} else if (a_size==4) {
		if (b_size==0) {
			add_instruction_w(Cpush_r_args40,-a_offset);
			return;
		}
	}

	if (a_size==0 && b_size>=5) {
		add_instruction_w_w(Cpush_r_args0b,-a_offset,b_size);
		return;
	} else if (a_size==1 && b_size>=4) {
		add_instruction_w_w(Cpush_r_args1b,-a_offset,b_size);
		return;
	} else if (a_size>=2 && b_size>=2) {
		add_instruction_w_w_w(Cpush_r_args,-a_offset,a_size,b_size);
		return;
	} else if (a_size>=4 && b_size==1) {
		add_instruction_w_w(Cpush_r_argsa1,-a_offset,a_size);
		return;
	} else if (a_size>=5 && b_size==0) {
		add_instruction_w_w(Cpush_r_argsa0,-a_offset,a_size);
		return;
	}

	fprintf(stderr, "Error: push_r_args %d %d %d\n",a_offset,a_size,b_size);
	exit(1);
}

void code_push_r_args_a(int a_offset,int a_size,int b_size,int argument_number,int n_arguments) {
	if (n_arguments==1) {
		if (argument_number==1) {
			add_instruction_w(Cpush_r_args_a1,-a_offset);
			return;
		}
		if (argument_number==2) {
			if (a_size+b_size==2) {
				add_instruction_w(Cpush_arg2,-a_offset);
				return;
			}
			if (a_size+b_size>2) {
				add_instruction_w(Cpush_r_args_a2l,-a_offset);
				return;
			}
		}
		if (argument_number==3) {
			add_instruction_w(Cpush_r_args_a3,-a_offset);
			return;
		}
		if (argument_number==4) {
			add_instruction_w(Cpush_r_args_a4,-a_offset);
			return;
		}
		add_instruction_w_w(Cpush_r_args_aa1,-a_offset,(argument_number-2)<<2);
		return;
	}

	fprintf(stderr, "Error: push_r_args_a %d %d %d %d %d\n",a_offset,a_size,b_size,argument_number,n_arguments);
	exit(1);
}

void code_push_r_args_b(int a_offset,int a_size,int b_size,int argument_number,int n_arguments) {
	if (n_arguments==1) {
		if (a_size==0) {
			if (argument_number==1) {
				add_instruction_w(Cpush_r_args_b0b11,-a_offset);
				return;
			}
			if (b_size==2 && argument_number==2) {
				add_instruction_w(Cpush_r_args_b0221,-a_offset);
				return;
			}
			if (argument_number==3) {
				add_instruction_w(Cpush_r_args_b31,-a_offset);
				return;
			}
		} else if (a_size==1) {
			if (b_size==1 && argument_number==1) {
				add_instruction_w(Cpush_r_args_b1111,-a_offset);
				return;
			}
		}
		if (a_size+b_size>2) {
			if (a_size+argument_number==2) {
				add_instruction_w(Cpush_r_args_b2l1,-a_offset);
				return;
			}
			if (a_size+argument_number==3) {
				add_instruction_w(Cpush_r_args_b31,-a_offset);
				return;
			}
			if (a_size+argument_number==4) {
				add_instruction_w(Cpush_r_args_b41,-a_offset);
				return;
			}
			if (a_size+argument_number>=5) {
				add_instruction_w_w(Cpush_r_args_b1,-a_offset,(a_size+argument_number-1-1)<<2);
				return;
			}
		}
	}

	if (n_arguments==2) {
		if (a_size+b_size>2) {
			if (a_size+argument_number==1) {
				add_instruction_w(Cpush_r_args_b1l2,-a_offset);
				return;
			}
			if (a_size+argument_number==2) {
				add_instruction_w(Cpush_r_args_b22,-a_offset);
				return;
			}
			if (a_size+argument_number>=5) {
				add_instruction_w_w(Cpush_r_args_b2,-a_offset,(a_size+argument_number-1-1)<<2);
				return;
			}
		}
	}

	add_instruction_w_w_w(Cpush_r_args_b,-a_offset,(a_size+argument_number-1-1)<<2,n_arguments);
	fprintf(stderr, "Warning: push_r_args_b %d %d %d %d %d was added by Camil\n",a_offset,a_size,b_size,argument_number,n_arguments);
}

void code_push_r_args_u(int a_offset,int a_size,int b_size) {
	code_push_r_args(a_offset,a_size,b_size);
}

void code_push_t_r_a(int a_offset) {
	add_instruction_w(Cpush_t_r_a,-a_offset);
}

void code_push_t_r_args(void) {
	add_instruction(Cpush_t_r_args);
}

void code_replace(char element_descriptor[],int a_size,int b_size) {
	switch(element_descriptor[0]) {
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction(Creplace);
				return;
			}
		case 'B':
			if (element_descriptor[1]=='O' && element_descriptor[2]=='O' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CreplaceBOOL);
				return;
			}
			break;
		case 'C':
			if (element_descriptor[1]=='H' && element_descriptor[2]=='A' && element_descriptor[3]=='R' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CreplaceCHAR);
				return;
			}
			break;
		case 'I':
			if (element_descriptor[1]=='N' && element_descriptor[2]=='T' && element_descriptor[3]=='\0') {
				add_instruction(CreplaceINT);
				return;
			}
		case 'R':
			if (element_descriptor[1]=='E' && element_descriptor[2]=='A' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CreplaceREAL);
				return;
			}
	}


	if (a_size>4 && b_size==0) {
		add_instruction_w(Creplace_ra,a_size);
		return;
	}

	fprintf(stderr, "Error: replace %s %d %d\n",element_descriptor,a_size,b_size);
	exit(1);
}

void code_repl_arg(int arity,int argument_n) {
	if (argument_n==1) {
		add_instruction(Crepl_r_args_aab11);
		return;
	}
	if (argument_n==2) {
		if (arity==2) {
			add_instruction(Crepl_r_args_a2021);
			return;
		} else if (arity>2) {
			add_instruction(Crepl_r_args_a21);
			return;
		}
	}
	if (argument_n==3) {
		add_instruction(Crepl_r_args_a31);
		return;
	}
	if (argument_n==4) {
		add_instruction(Crepl_r_args_a41);
		return;
	}
	add_instruction_w(Crepl_r_args_aa1,(argument_n-2)<<2);
	return;
}

void code_repl_args(int arity,int n_arguments) {
	if (arity==n_arguments) {
		switch(arity) {
			case 1:
				add_instruction(Crepl_args1);
				return;
			case 2:
				add_instruction(Crepl_args2);
				return;
			case 3:
				add_instruction(Crepl_args3);
				return;
			case 4:
				add_instruction(Crepl_args4);
				return;
			default:
				add_instruction_w(Crepl_args,arity-1);
				return;
		}
	}

	fprintf(stderr, "Error: repl_args %d %d\n",arity,n_arguments);
	exit(1);
}

void code_repl_args_b(void) {
	add_instruction(Crepl_args_b);
}

void code_repl_r_args(int a_size,int b_size) {
	if (a_size==0) {
		if (b_size==1) {
			add_instruction(Crepl_r_args01);
			return;
		}
		if (b_size==2) {
			add_instruction(Crepl_r_args02);
			return;
		}
		if (b_size==3) {
			add_instruction(Crepl_r_args03);
			return;
		}
		if (b_size==4) {
			add_instruction(Crepl_r_args04);
			return;
		}
	} else if (a_size==1) {
		if (b_size==0) {
			add_instruction(Crepl_r_args10);
			return;
		}
		if (b_size==1) {
			add_instruction(Crepl_r_args11);
			return;
		}
		if (b_size==2) {
			add_instruction(Crepl_r_args12);
			return;
		}
		if (b_size==3) {
			add_instruction(Crepl_r_args13);
			return;
		}
	} else if (a_size==2) {
		if (b_size==0) {
			add_instruction(Crepl_r_args20);
			return;
		}
		if (b_size==1) {
			add_instruction(Crepl_r_args21);
			return;
		}
		if (b_size==2) {
			add_instruction(Crepl_r_args22);
			return;
		}
	} else if (a_size==3) {
		if (b_size==0) {
			add_instruction(Crepl_r_args30);
			return;
		}
		if (b_size==1) {
			add_instruction(Crepl_r_args31);
			return;
		}
	} else if (a_size==4 && b_size==0) {
		add_instruction(Crepl_r_args40);
		return;
	}

	if (a_size==1 && b_size>=4) {
		add_instruction_w(Crepl_r_args1b,b_size);
		return;
	} else if (a_size>=2 && b_size>=2) {
		add_instruction_w_w(Crepl_r_args,a_size-1,b_size);
		return;
	} else if (a_size>=3 && b_size==1) {
		add_instruction_w(Crepl_r_argsa1,a_size-1);
		return;
	} else if (a_size>=4 && b_size==0) {
		add_instruction_w(Crepl_r_argsa0,a_size-1);
		return;
	}

	fprintf(stderr, "Error: repl_r_args %d %d\n",a_size,b_size);
	exit(1);
}

void code_repl_r_args_a(int a_size,int b_size,int argument_number,int n_arguments) {
	if (n_arguments==1) {
		if (argument_number==1) {
			if (a_size>0) {
				add_instruction(Crepl_r_args_aab11);
				return;
			}
		}
		if (argument_number==2) {
			if (a_size==2 && b_size==0) {
				add_instruction(Crepl_r_args_a2021);
				return;
			} else if (a_size>2) {
				add_instruction(Crepl_r_args_a21);
				return;
			}
		}
		if (argument_number==3) {
			add_instruction(Crepl_r_args_a31);
			return;
		}
		if (argument_number==4) {
			add_instruction(Crepl_r_args_a41);
			return;
		}
		add_instruction_w(Crepl_r_args_aa1,(argument_number-2)<<2);
		return;
	}

	fprintf(stderr, "Error: repl_r_args_a %d %d %d %d\n",a_size,b_size,argument_number,n_arguments);
	exit(1);
}

void code_rtn(void) {
	last_d=0;

	add_instruction(Crtn);
}

void code_select(char element_descriptor[],int a_size,int b_size) {
	switch(element_descriptor[0]) {
		case 'B':
			if (element_descriptor[1]=='O' && element_descriptor[2]=='O' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CselectBOOL);
				return;
			}
			break;
		case 'C':
			if (element_descriptor[1]=='H' && element_descriptor[2]=='A' && element_descriptor[3]=='R' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CselectCHAR);
				return;
			}
			break;
		case 'I':
			if (element_descriptor[1]=='N' && element_descriptor[2]=='T' && element_descriptor[3]=='\0') {
				add_instruction(CselectINT);
				return;
			}
			break;
		case 'R':
			if (element_descriptor[1]=='E' && element_descriptor[2]=='A' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CselectREAL);
				return;
			}
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction(Cselect);
				return;
			}
	}

	if (a_size==0 && b_size==2) {
		add_instruction(Cselect_r02);
		return;
	}
	if (a_size==1 && b_size==2) {
		add_instruction(Cselect_r12);
		return;
	}
	if (a_size==2) {
		if (b_size==0) {
			add_instruction(Cselect_r20);
			return;
		}
		if (b_size==1) {
			add_instruction(Cselect_r21);
			return;
		}
	}
	if (a_size==4) {
		if (b_size==0) {
			add_instruction(Cselect_r40);
			return;
		}
	}

	if (a_size>=2 && b_size>=2) {
		add_instruction_w_w_w(Cselect_r,a_size+b_size,a_size,b_size);
		return;
	}
	if (a_size>=4 && b_size==1) {
		add_instruction_w(Cselect_ra1,a_size+b_size);
		return;
	}
	if (a_size>=3 && b_size==0) {
		add_instruction_w(Cselect_ra,a_size);
		return;
	}

	fprintf(stderr, "Error: select %s %d %d\n",element_descriptor,a_size,b_size);
	exit(1);
}

void code_shiftl(void) {
	add_instruction(CshiftlI);
}

void code_shiftr(void) {
	add_instruction(CshiftrI);
}

void code_sinR(void) {
	add_instruction(CsinR);
}

void code_subI(void) {
	add_instruction(CsubI);
}

void code_subR(void) {
	add_instruction(CsubR);
}

void code_sqrtR(void) {
	add_instruction(CsqrtR);
}

void code_tanR(void) {
	add_instruction(CtanR);
}

void code_testcaf(char *label_name) {
	add_instruction_label(Ctestcaf,label_name);
}

void code_update(char element_descriptor[],int a_size,int b_size) {
	switch(element_descriptor[0]) {
		case 'B':
			if (element_descriptor[1]=='O' && element_descriptor[2]=='O' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CupdateBOOL);
				return;
			}
			break;
		case 'C':
			if (element_descriptor[1]=='H' && element_descriptor[2]=='A' && element_descriptor[3]=='R' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CupdateCHAR);
				return;
			}
			break;
		case 'I':
			if (element_descriptor[1]=='N' && element_descriptor[2]=='T' && element_descriptor[3]=='\0') {
				add_instruction(CupdateINT);
				return;
			}
			break;
		case 'R':
			if (element_descriptor[1]=='E' && element_descriptor[2]=='A' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(CupdateREAL);
				return;
			}
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction(Cupdate);
				return;
			}
	}

	if (a_size==0 && b_size==2) {
		add_instruction(Cupdate_r02);
		return;
	}
	if (a_size==1 && b_size==2) {
		add_instruction(Cupdate_r12);
		return;
	}
	if (a_size==2) {
		if (b_size==0) {
			add_instruction(Cupdate_r20);
			return;
		}
		if (b_size==1) {
			add_instruction(Cupdate_r21);
			return;
		}
	}
	if (a_size==3 && b_size==0) {
		add_instruction(Cupdate_r30);
		return;
	}
	if (a_size==4 && b_size==0) {
		add_instruction(Cupdate_r40);
		return;
	}

	if (a_size>=2 && b_size>=2) {
		add_instruction_w_w_w(Cupdate_r,a_size+b_size,a_size,b_size);
		return;
	} else if (a_size>3 && b_size==1) {
		add_instruction_w(Cupdate_ra1,a_size+b_size);
		return;
	} else if (a_size>4 && b_size==0) {
		add_instruction_w(Cupdate_ra,a_size);
		return;
	}

	fprintf(stderr, "Error: update %s %d %d\n",element_descriptor,a_size,b_size);
	exit(1);
}

void code_update_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cupdate_a,-a_offset_1,-a_offset_2);
}

void code_updatepop_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cupdatepop_a,-a_offset_1,-a_offset_2);
}

void code_update_b(int b_offset_1,int b_offset_2) {
	add_instruction_w_w(Cupdate_b,b_offset_1,b_offset_2);
}

void code_updatepop_b(int b_offset_1,int b_offset_2) {
	add_instruction_w_w(Cupdatepop_b,b_offset_1,b_offset_2);
}

void code_xor(void) {
	add_instruction(CxorI);
}

void code_CtoAC(void) {
	add_instruction(CCtoAC);
}

void code_CtoI(void) {
}

void code_ItoC(void) {
	add_instruction(CItoC);
}

void code_ItoR(void) {
	add_instruction(CItoR);
}

void code_RtoI(void) {
	add_instruction(CRtoI);
}


void code_addIi(CleanInt i) {
	add_instruction_i(CaddIi,i);
}

void code_andIi(CleanInt i) {
	add_instruction_i(CandIi,i);
}

void code_andIio(CleanInt i,int b_offset) {
	add_instruction_w_i(CandIio,b_offset,i);
}

void code_buildh0_dup_a(char descriptor_name[],int a_offset) {
	if (a_offset==0) {
		fprintf(stderr, "Error: code_buildh0_dup_a %s %d\n",descriptor_name,a_offset);
		exit(1);
	}

	add_instruction_label_offset_w(Cbuildh0_dup_a,descriptor_name,-4,-(a_offset-1));
}

void code_buildh0_dup2_a(char descriptor_name[],int a_offset) {
	if (a_offset==0) {
		fprintf(stderr, "Error: code_buildh0_dup2_a %s %d\n",descriptor_name,a_offset);
		exit(1);
	}

	add_instruction_label_offset_w(Cbuildh0_dup2_a,descriptor_name,-4,-(a_offset-1));
}

void code_buildh0_dup3_a(char descriptor_name[],int a_offset) {
	if (a_offset==0) {
		fprintf(stderr, "Error: code_buildh0_dup3_a %s %d\n",descriptor_name,a_offset);
		exit(1);
	}

	add_instruction_label_offset_w(Cbuildh0_dup3_a,descriptor_name,-4,-(a_offset-1));
}

void code_buildh0_put_a(char descriptor_name[],int a_offset) {
	if (a_offset==0) {
		fprintf(stderr, "Error: code_buildh0_put_a %s %d\n",descriptor_name,a_offset);
		exit(1);
	}

	add_instruction_label_offset_w(Cbuildh0_put_a,descriptor_name,-4,-(a_offset-1));
}

void code_buildh0_put_a_jsr(char descriptor_name[],int a_offset,char label_name[]) {
	int lib_function_n;

	if (a_offset==0) {
		fprintf(stderr, "Error: code_buildh0_put_a_jsr %s %d\n",descriptor_name,a_offset);
		exit(1);
	}

	if (last_jsr_with_d) {
		fprintf(stderr, "Error: .d or .o directive used incorrectly near buildh0_put_a_jsr\n");
		exit(1);
	}

	if (last_d) {
		last_jsr_with_d=1;
		last_d=0;
	}

	lib_function_n = get_lib_function_n(label_name);

	if (lib_function_n>=0) {
		code_buildh0_put_a(descriptor_name,a_offset);

		if (lib_function_n==0)
			add_instruction(CeqAC);
		else
			add_instruction_w(Cjesr,lib_function_n);

		last_jsr_with_d=0;
		return;
	}

	add_instruction_label_offset_w_label(Cbuildh0_put_a_jsr,descriptor_name,-4,-(a_offset-1),label_name);
}

void code_buildho2(char descriptor_name[],int a_offset1,int a_offset2) {
	if (a_offset1+1==a_offset2)
		add_instruction_w_label_offset(Cbuild_r20,-a_offset1,descriptor_name,(2<<3)+2);
	else
		add_instruction_w_w_label_offset(Cbuildho2,-a_offset1,-a_offset2,descriptor_name,(2<<3)+2);
}

void code_buildo1(char code_name[],int a_offset) {
	add_instruction_w_label(Cbuildo1,-a_offset,code_name);
}

void code_buildo2(char code_name[],int a_offset1,int a_offset2) {
	if (a_offset1+1==a_offset2)
		add_instruction_w_label(Cbuild_r20,-a_offset1,code_name);
	else
		add_instruction_w_w_label(Cbuildo2,-a_offset1,-a_offset2,code_name);
}

void code_dup_a(int a_offset) {
	add_instruction_w(Cdup_a,-a_offset);
}

void code_dup2_a(int a_offset) {
	add_instruction_w(Cdup2_a,-a_offset);
}

void code_dup3_a(int a_offset) {
	add_instruction_w(Cdup3_a,-a_offset);
}

void code_exchange_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cexchange_a,-a_offset_1,-a_offset_2);
}

void code_fill_a01_pop_rtn(void) {
	last_d=0;

	add_instruction(Cfill_a01_pop_rtn);
}

void code_geC(void) {
	add_instruction(CgeC);
}

void code_jmp_b_false(int b_offset,char label_name[]) {
	add_instruction_w_label(Cjmp_b_false,b_offset,label_name);
}

void code_jmp_eqACio(char *string,int string_length,int a_offset,char label_name[]) {
	struct label *string_label;

	string_label=new_internal_label();
	string_label->label_offset=(pgrm.data_size<<2)+1;

	add_instruction_w_internal_label_label(Cjmp_eqACio,-a_offset,string_label,label_name);

	if (list_code)
		printf("\t.data\n");
	store_string(string,string_length);
	if (list_code)
		printf("\t.text\n");
}

void code_jmp_eqC_b(int value,int b_offset,char label_name[]) {
	add_instruction_w_c_label(Cjmp_eqC_b,b_offset,value,label_name);
}

void code_jmp_eqC_b2(int value1,int value2,int b_offset,char label_name1[],char label_name2[]) {
	add_instruction_w_c_label_c_label(Cjmp_eqC_b2,b_offset,value1,label_name1,value2,label_name2);
}

void code_jmp_eqD_b(char descriptor_name[],int arity,char label_name[]) {
	add_instruction_label_offset_label(Cjmp_eqD_b,descriptor_name,(arity<<3)+2,label_name);
}

void code_jmp_eqD_b2(char descriptor_name1[],int arity1,char label_name1[],char descriptor_name2[],int arity2,char label_name2[]) {
	add_instruction_label_offset_label_label_offset_label(Cjmp_eqD_b2,descriptor_name1,(arity1<<3)+2,label_name1,
																	   descriptor_name2,(arity2<<3)+2,label_name2);
}

void code_jmp_eqI(char label_name[]) {
	add_instruction_label(Cjmp_eqI,label_name);
}

void code_jmp_eqI_b(int value,int b_offset,char label_name[]) {
	add_instruction_w_i_label(Cjmp_eqI_b,b_offset,value,label_name);
}

void code_jmp_eqI_b2(int value1,int value2,int b_offset,char label_name1[],char label_name2[]) {
	add_instruction_w_i_label_i_label(Cjmp_eqI_b2,b_offset,value1,label_name1,value2,label_name2);
}

void code_jmp_eq_desc(char descriptor_name[],int arity,int a_offset,char label_name[]) {
	add_instruction_w_label_offset_label(Cjmp_eq_desc,-a_offset,descriptor_name,(arity<<3)+2,label_name);
}

void code_jmp_geI(char label_name[]) {
	add_instruction_label(Cjmp_geI,label_name);
}

void code_jmp_ltI(char label_name[]) {
	add_instruction_label(Cjmp_ltI,label_name);
}

void code_jmp_neC_b(int value,int b_offset,char label_name[]) {
	add_instruction_w_c_label(Cjmp_neC_b,b_offset,value,label_name);
}

void code_jmp_neI(char label_name[]) {
	add_instruction_label(Cjmp_neI,label_name);
}

void code_jmp_neI_b(int value,int b_offset,char label_name[]) {
	add_instruction_w_i_label(Cjmp_neI_b,b_offset,value,label_name);
}

void code_jmp_ne_desc(char descriptor_name[],int arity,int a_offset,char label_name[]) {
	add_instruction_w_label_offset_label(Cjmp_ne_desc,-a_offset,descriptor_name,(arity<<3)+2,label_name);
}

void code_jmp_o_geI(int b_offset,char label_name[]) {
	add_instruction_w_label(Cjmp_o_geI,b_offset,label_name);
}

void code_jmp_o_geI_arraysize_a(char element_descriptor[],int b_offset,int a_offset,char label_name[]) {
	add_instruction_w_w_label(Cjmp_o_geI_arraysize_a,b_offset,-a_offset,label_name);
}

void code_ltIi(CleanInt i) {
	add_instruction_i(CltIi,i);
}

void code_neI(void) {
	add_instruction(CneI);
}

void code_swap_a1(void) {
	add_instruction(Cswap_a1);
}

void code_swap_a(int a_offset) {
	switch(a_offset) {
		case 1:
			add_instruction(Cswap_a1);
			return;
		case 2:
			add_instruction(Cswap_a2);
			return;
		case 3:
			add_instruction(Cswap_a3);
			return;
		default:
			add_instruction_w(Cswap_a,-a_offset);
			return;
	}
}

void code_swap_b1(void) {
	add_instruction(Cswap_b1);
}

void code_pop_a_jmp(int n,char label_name[]) {
	last_d=0;

	add_instruction_w_label(Cpop_a_jmp,-n,label_name);
}

void code_pop_a_jsr(int n,char label_name[]) {
	int lib_function_n;

	if (last_jsr_with_d) {
		fprintf(stderr, "Error: .d or .o directive used incorrectly near pop_a_jsr\n");
		exit(1);
	}

	if (last_d) {
		last_jsr_with_d=1;
		last_d=0;
	}

	lib_function_n = get_lib_function_n(label_name);

	if (lib_function_n>=0) {
		code_pop_a(n);

		if (lib_function_n==0)
			add_instruction(CeqAC);
		else
			add_instruction_w(Cjesr,lib_function_n);

		last_jsr_with_d=0;
		return;
	}

	add_instruction_w_label(Cpop_a_jsr,-n,label_name);
}

void code_pop_a_rtn(int n) {
	last_d=0;

	add_instruction_w(Cpop_a_rtn,-n);
}

void code_pop_ab_rtn(int na,int nb) {
	last_d=0;

	add_instruction_w_w(Cpop_ab_rtn,-na,nb);
}

void code_pop_b_jmp(int n,char label_name[]) {
	last_d=0;

	add_instruction_w_label(Cpop_b_jmp,n,label_name);
}

void code_pop_b_jsr(int n,char label_name[]) {
	int lib_function_n;

	if (last_jsr_with_d) {
		fprintf(stderr, "Error: .d or .o directive used incorrectly near pop_b_jsr\n");
		exit(1);
	}

	if (last_d) {
		last_jsr_with_d=1;
		last_d=0;
	}

	lib_function_n = get_lib_function_n(label_name);

	if (lib_function_n>=0) {
		code_pop_b(n);

		if (lib_function_n==0)
			add_instruction(CeqAC);
		else
			add_instruction_w(Cjesr,lib_function_n);

		last_jsr_with_d=0;
		return;
	}

	add_instruction_w_label(Cpop_b_jsr,n,label_name);
}

void code_pop_b_pushB(int n,int b) {
	if (b==0)
		add_instruction_w(Cpop_b_pushBFALSE,n-1);
	else
		add_instruction_w(Cpop_b_pushBTRUE,n-1);
}

void code_pop_b_rtn(int n) {
	last_d=0;

	add_instruction_w(Cpop_b_rtn,n);
}

void code_push_a_jsr(int a_offset,char label_name[]) {
	int lib_function_n;

	if (last_jsr_with_d) {
		fprintf(stderr, "Error: .d or .o directive used incorrectly near push_a_jsr\n");
		exit(1);
	}

	if (last_d) {
		last_jsr_with_d=1;
		last_d=0;
	}

	lib_function_n = get_lib_function_n(label_name);

	if (lib_function_n>=0) {
		code_push_a(a_offset);

		if (lib_function_n==0)
			add_instruction(CeqAC);
		else
			add_instruction_w(Cjesr,lib_function_n);

		last_jsr_with_d=0;
		return;
	}

	add_instruction_w_label(Cpush_a_jsr,-a_offset,label_name);
}

void code_push_arraysize_a(char element_descriptor[],int a_size,int b_size,int a_offset) {
	add_instruction_w(Cpush_arraysize_a,-a_offset);
}

void code_push_a2(int a_offset1,int a_offset2) {
	add_instruction_w_w(Cpush_a2,-a_offset1,-a_offset2);
}

void code_push_ab(int a_offset,int b_offset) {
	add_instruction_w_w(Cpush_ab,-a_offset,b_offset);
}

void code_push_b_incI(int b_offset) {
	add_instruction_w(Cpush_b_incI,b_offset);
}

void code_push_b_jsr(int b_offset,char label_name[]) {
	int lib_function_n;

	if (last_jsr_with_d) {
		fprintf(stderr, "Error: .d or .o directive used incorrectly near push_b_jsr\n");
		exit(1);
	}

	if (last_d) {
		last_jsr_with_d=1;
		last_d=0;
	}

	lib_function_n = get_lib_function_n(label_name);

	if (lib_function_n>=0) {
		code_push_b(b_offset);

		if (lib_function_n==0)
			add_instruction(CeqAC);
		else
			add_instruction_w(Cjesr,lib_function_n);

		last_jsr_with_d=0;
		return;
	}

	add_instruction_w_label(Cpush_b_jsr,b_offset,label_name);
}

void code_push_jsr_eval(int a_offset) {
	add_instruction_w(Cpush_jsr_eval,-a_offset);
}

void code_push_b2(int b_offset1,int b_offset2) {
	add_instruction_w_w(Cpush_b2,b_offset1,b_offset2);
}

void code_push2_a(int a_offset) {
	add_instruction_w(Cpush2_a,-a_offset);
}

void code_push2_b(int b_offset) {
	add_instruction_w(Cpush2_b,b_offset);
}

void code_push3_a(int a_offset) {
	add_instruction_w(Cpush3_a,-a_offset);
}

void code_push3_b(int b_offset) {
	add_instruction_w(Cpush3_b,b_offset);
}

void code_pushD_a_jmp_eqD_b2(int a_offset,char descriptor_name1[],int arity1,char label_name1[],
										   char descriptor_name2[],int arity2,char label_name2[]) {
	add_instruction_w_label_offset_label_label_offset_label
		(CpushD_a_jmp_eqD_b2,-a_offset,descriptor_name1,(arity1<<3)+2,label_name1,descriptor_name2,(arity2<<3)+2,label_name2);
}

void code_push_update_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cpush_update_a,-a_offset_1,-a_offset_2);
}

void code_put_a(int a_offset) {
	add_instruction_w(Cput_a,-a_offset);
}

void code_put_b(int b_offset) {
	add_instruction_w(Cput_b,b_offset);
}

void code_selectoo(char element_descriptor[],int a_size,int b_size,int a_offset,int b_offset) {
	switch(element_descriptor[0]) {
		case 'C':
			if (element_descriptor[1]=='H' && element_descriptor[2]=='A' && element_descriptor[3]=='R' &&
				element_descriptor[4]=='\0')
			{
				add_instruction_w_w(CselectCHARoo,-a_offset,b_offset);
				return;
			}
			break;
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction_w_w(Cselectoo,-a_offset,b_offset);
				return;
			}
	}

	code_push_ab(a_offset,b_offset);
	code_select(element_descriptor,a_size,b_size);
}

void code_update2_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cupdate2_a,-a_offset_1,-a_offset_2);
}

void code_update2_b(int b_offset_1,int b_offset_2) {
	add_instruction_w_w(Cupdate2_b,b_offset_1,b_offset_2);
}

void code_update2pop_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cupdate2pop_a,-a_offset_1,-a_offset_2);
}

void code_update2pop_b(int b_offset_1,int b_offset_2) {
	add_instruction_w_w(Cupdate2pop_b,b_offset_1,b_offset_2);
}

void code_update3_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cupdate3_a,-a_offset_1,-a_offset_2);
}

void code_update3_b(int b_offset_1,int b_offset_2) {
	add_instruction_w_w(Cupdate3_b,b_offset_1,b_offset_2);
}

void code_update3pop_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cupdate3pop_a,-a_offset_1,-a_offset_2);
}

void code_update3pop_b(int b_offset_1,int b_offset_2) {
	add_instruction_w_w(Cupdate3pop_b,b_offset_1,b_offset_2);
}

void code_update4_a(int a_offset_1,int a_offset_2) {
	add_instruction_w_w(Cupdate4_a,-a_offset_1,-a_offset_2);
}

void code_updates2_a(int a_offset_1,int a_offset_2,int a_offset_3) {
	add_instruction_w_w_w(Cupdates2_a,-a_offset_1,-a_offset_2,-a_offset_3);
}

void code_updates2_a_pop_a(int a_offset_1,int a_offset_2,int a_offset_3,int n) {
	add_instruction_w_w_w_w(Cupdates2_a_pop_a,-a_offset_1,-a_offset_2,-a_offset_3,n);
}

void code_updates2_b(int b_offset_1,int b_offset_2,int b_offset_3) {
	add_instruction_w_w_w(Cupdates2_b,b_offset_1,b_offset_2,b_offset_3);
}

void code_updates2pop_a(int a_offset_1,int a_offset_2,int a_offset_3) {
	add_instruction_w_w_w(Cupdates2pop_a,-a_offset_1,-a_offset_2,-a_offset_3);
}

void code_updates2pop_b(int b_offset_1,int b_offset_2,int b_offset_3) {
	add_instruction_w_w_w(Cupdates2pop_b,b_offset_1,b_offset_2,b_offset_3);
}

void code_updates3_a(int a_offset_1,int a_offset_2,int a_offset_3,int a_offset_4) {
	add_instruction_w_w_w_w(Cupdates3_a,-a_offset_1,-a_offset_2,-a_offset_3,-a_offset_4);
}

void code_updates3_b(int b_offset_1,int b_offset_2,int b_offset_3,int b_offset_4) {
	add_instruction_w_w_w_w(Cupdates3_b,b_offset_1,b_offset_2,b_offset_3,b_offset_4);
}

void code_updates3pop_a(int a_offset_1,int a_offset_2,int a_offset_3,int a_offset_4) {
	add_instruction_w_w_w_w(Cupdates3pop_a,-a_offset_1,-a_offset_2,-a_offset_3,-a_offset_4);
}

void code_updates3pop_b(int b_offset_1,int b_offset_2,int b_offset_3,int b_offset_4) {
	add_instruction_w_w_w_w(Cupdates3pop_b,b_offset_1,b_offset_2,b_offset_3,b_offset_4);
}

void code_updates4_a(int a_offset_1,int a_offset_2,int a_offset_3,int a_offset_4,int a_offset_5) {
	add_instruction_w_w_w_w_w(Cupdates4_a,-a_offset_1,-a_offset_2,-a_offset_3,-a_offset_4,-a_offset_5);
}

void code_a(int n_apply_args,char *ea_label_name) {
	if (n_apply_args==0) {
		add_instruction(CA_data_IlI);
		add_instruction_label(Cjmp,ea_label_name);
		add_instruction(Chalt);
	} else if (n_apply_args==2) {
		add_instruction(CA_data_IlI);
		add_instruction_label(Cadd_empty_node2,ea_label_name);
		add_instruction(Chalt);
	} else if (n_apply_args==3) {
		add_instruction(CA_data_IlI);
		add_instruction_label(Cadd_empty_node3,ea_label_name);
		add_instruction(Chalt);
	} else {
		if (ea_label_name!=NULL)
			fprintf(stderr, "Warning: .a %d %s\n",n_apply_args,ea_label_name);
		else
			fprintf(stderr, "Warning: .a %d\n",n_apply_args);
		/* to do add_empty_node_n */
		add_instruction(CA_data_IIl);
		add_instruction(Chalt);
		add_instruction_label(Cjmp,ea_label_name);
	}
}

void code_algtype(int n_constructors) {
}

void code_caf(char *label_name,int a_size,int b_size) {
	struct label *label;
	int s;

	if (list_code) {
		printf("\t.data\n");
		printf("%s:\n",label_name);
	}

	label=enter_label(label_name);

	if (a_size>0) {
		if (list_code)
			printf("%d\t.data4 %d\n",pgrm.data_size<<2,0);
		store_data_l(0);
	}

	if (label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",label_name);
		exit(1);
	}
	label->label_offset=(pgrm.data_size<<2)+1;

	s=a_size+b_size+1;
	while(s!=0) {
		if (list_code)
			printf("%d\t.data4 %d\n",pgrm.data_size<<2,0);
		store_data_l(0);
		--s;
	}

	if (list_code)
		printf("\t.text\n");

}

void code_comp(int version,char *options) {
}

void code_d(int da,int db,ULONG vector[]) {
	if (last_d) {
		fprintf(stderr, "Error: .d directive used incorrectly\n");
		exit(1);
	}
	last_d=1;
	last_da=da;
	last_db=db;
}

void code_depend(char *module_name,int module_name_length) {
}

struct label *code_descriptor
	(char label_name[],char *code_label_name,int arity,int lazy_record_flag,char descriptor_name[],int descriptor_name_length) {
	struct label *label;
	int n;

	/* Resolve descriptor address */
	store_data_l(-1); /* TODO check that this is correct in all cases */

	if (list_code) {
		printf("\t.data\n");
		printf("\t.data4 %s+2\n",label_name);
	}
	store_data_label_value(label_name,2);

	if (list_code)
		printf("%s:\n",label_name);
	label=enter_label(label_name);
	if (label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",label_name);
		exit(1);
	}
	label->label_offset=(pgrm.data_size<<2)+1;

	for(n=0; n<=arity; ++n) {
		if (list_code)
			printf("%d\t.data2 %d %d\n",pgrm.data_size<<2,n,(arity-n)<<3);
		store_data_l(n + (((arity-n)<<3)<<16));
		if (n<arity-1 || (n==arity-1 && !strcmp (code_label_name,"__add__arg"))) {
			if (list_code)
				printf("%d\t.data4 _add_arg%d\n",pgrm.data_size<<2,n);
			Fadd_arg_label_used[n]=1;
			char label_name[11];
			sprintf(label_name,"_add_arg%d",n);
			struct label *label = enter_label(label_name);
			store_data_label_value_of_label(label,0);
		} else if (n==arity-1) {
			if (list_code)
				printf("%d\t.data4 %s\n",pgrm.data_size<<2,code_label_name);
			store_data_label_value(code_label_name,0);
		}
	}

	if (list_code)
		printf("%d\t.data2 %d %d\n",pgrm.data_size<<2,lazy_record_flag,arity);
	store_data_l(lazy_record_flag+(arity<<16));
	if (list_code)
		printf("%d\t.data4 0\n",pgrm.data_size<<2);
	store_data_l(0);
	store_string(descriptor_name,descriptor_name_length);

	if (list_code)
		printf("\t.text\n");

	return label;
}

void code_desc(char label_name[],char node_entry_label_name[],char *code_label_name,
				   int arity,int lazy_record_flag,char descriptor_name[],int descriptor_name_length) {
	(void)code_descriptor(label_name,code_label_name,arity,lazy_record_flag,descriptor_name,descriptor_name_length);
}

void code_desc0(char label_name[],int desc0_number,char descriptor_name[],int descriptor_name_length) {
	struct label *label;

	if (list_code)
		printf("\t.data\n");

	/* Resolve descriptor address */
	store_data_l(-1);

	if (list_code)
		printf("%d\t.data4 %d\n",pgrm.data_size<<2,desc0_number);
	store_data_l(desc0_number);

	if (list_code)
		printf("\t.data4 %s+2\n",label_name);
	store_data_label_value(label_name,2);
	if (list_code)
		printf("%s:\n",label_name);

	label=enter_label(label_name);
	if (label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",label_name);
		exit(1);
	}
	label->label_offset=(pgrm.data_size<<2)+1;

	if (list_code)
		printf("%d\t.data2 %d %d\n",pgrm.data_size<<2,0,0);
	store_data_l(0);

	if (list_code)
		printf("%d\t.data2 %d %d\n",pgrm.data_size<<2,0,0);
	store_data_l(0);
	if (list_code)
		printf("%d\t.data4 0\n",pgrm.data_size<<2);
	store_data_l(0);
	store_string(descriptor_name,descriptor_name_length);

	if (list_code)
		printf("\t.text\n");
}

void code_descn(char label_name[],char node_entry_label_name[],int arity,int lazy_record_flag,char descriptor_name[],
				 int descriptor_name_length) {
	/* node_entry_label_name not used */
	struct label *label;

	/* Resolve descriptor address */
	store_data_l(-1);
	store_data_l(0);

	if (list_code) {
		printf("\t.data\n");
		printf("\t.data4 %s+2\n",label_name);
	}
	store_data_label_value(label_name,2);
	if (list_code)
		printf("%s:\n",label_name);

	label=enter_label(label_name);
	if (label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",label_name);
		exit(1);
	}
	label->label_offset=(pgrm.data_size<<2)+1;

	if (list_code)
		printf("%d\t.data2 %d %d\n",pgrm.data_size<<2,lazy_record_flag,arity);
	store_data_l(lazy_record_flag+(arity<<16));
	if (list_code)
		printf("%d\t.data4 0\n",pgrm.data_size<<2);
	store_data_l(0);
	store_string(descriptor_name,descriptor_name_length);

	if (list_code)
		printf("\t.text\n");
}

void code_descs(char label_name[],char node_entry_label_name[],char *result_descriptor_name,
				uint32_t offset1,uint32_t offset2,char descriptor_name[],uint32_t descriptor_name_length) {
	struct label *label;

	/* Resolve descriptor address */
	store_data_l(0);

	if (list_code) {
		printf("\t.data\n");
		printf("\t.data4 %s+2\n",label_name);
	}
	store_data_label_value(label_name,2);
	if (list_code)
		printf("%s:\n",label_name);

	label=enter_label(label_name);
	if (label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",label_name);
		exit(1);
	}
	label->label_offset=(pgrm.data_size<<2)+1;

	if (list_code)
		printf("%d\t.data2 0 8\n",pgrm.data_size<<2);
	store_data_l(8<<16);
	if (list_code)
		printf("%d\t.data2 %d %d\n",pgrm.data_size<<2,offset1<<2,offset2<<2);
	store_data_l((offset1<<2) | (offset2<<18));
	if (list_code)
		printf("%d\t.data2 1 0\n",pgrm.data_size<<2);
	store_data_l(1);

	if (list_code)
		printf("%d\t.data2 0 1\n",pgrm.data_size<<2);
	store_data_l(1<<16);
	if (list_code)
		printf("%d\t.data4 0\n",pgrm.data_size<<2);
	store_data_l(0);
	store_string(descriptor_name,descriptor_name_length);

	if (list_code)
		printf("\t.text\n");
}

void code_descexp(char label_name[],char node_entry_label_name[],char *code_label_name,
				   int32_t arity, int32_t lazy_record_flag, char descriptor_name[],uint32_t descriptor_name_length) {
	struct label *label,*node_entry_label;

	label = code_descriptor(label_name,code_label_name,arity,lazy_record_flag,descriptor_name,descriptor_name_length);
	make_label_global(label);

	node_entry_label=enter_label(node_entry_label_name);
	make_label_global(node_entry_label);
}

void code_label(char *label_name) {
	struct label *label;

	if (list_code)
		printf("%s:\n",label_name);

	label=enter_label(label_name);
	if (label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",label_name);
		exit(1);
	}
	label->label_offset=pgrm.code_size<<2;
}

void code_export(char *label_name) {
	struct label *label;

	label=enter_label(label_name);
	make_label_global(label);
}

void code_impdesc(char *label_name) {
}

void code_implab_node_entry(char *label_name,char *ea_label_name) {
}

void code_impmod(char *module_name) {
}

void code_implab(char *label_name) {
}

void code_module(char label_name[],char string[],uint32_t string_length) {
	struct label *label;

	if (list_code) {
		printf("\t.data\n");
		printf("%s:\n",label_name);
	}
	label=enter_label(label_name);
	if (label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",label_name);
		exit(1);
	}
	label->label_offset=(pgrm.data_size<<2)+1;

	store_string(string,string_length);

	if (list_code)
		printf("\t.text\n");
}

void code_n(int32_t number_of_arguments, char *descriptor_name, char *ea_label_name) {
#if 0
	if ((pgrm.code_size & 1)!=0)
		add_data2_to_code(0);
#endif
	if (descriptor_name[0]=='_' && descriptor_name[1]=='_' && descriptor_name[2]=='\0')
		descriptor_name=NULL;

	if (ea_label_name!=NULL) {
		if (ea_label_name[0]=='_' && ea_label_name[1]=='_' && ea_label_name[2]=='\0') {
			/* eval_fill */
			add_instruction(CA_data_IIIla);
			add_instruction(Cjsr_eval0);
			add_instruction(Cfill_a01_pop_rtn);
			add_instruction(Chalt);
		} else {
			add_instruction(CA_data_IlIla);

			if (number_of_arguments<-2)
				number_of_arguments=1;

			if (number_of_arguments==0) {
				add_instruction_label(Ceval_upd0,ea_label_name);
				add_instruction(Chalt);
			} else if (number_of_arguments==1) {
				add_instruction_label(Ceval_upd1,ea_label_name);
				add_instruction(Chalt);
			} else if (number_of_arguments==2) {
				add_instruction_label(Ceval_upd2,ea_label_name);
				add_instruction(Chalt);
			} else if (number_of_arguments==3) {
				add_instruction_label(Ceval_upd3,ea_label_name);
				add_instruction(Chalt);
			} else if (number_of_arguments==4) {
				add_instruction_label(Ceval_upd4,ea_label_name);
				add_instruction(Chalt);
			} else if (number_of_arguments==-1) {
				add_instruction_label(Cjmp,ea_label_name);
				add_instruction(Chalt);
			} else {
				/* to do eval_upd_n */
				if (ea_label_name!=NULL)
					fprintf(stderr, "Warning: .n %d %s\n",number_of_arguments,ea_label_name);
				else
					fprintf(stderr, "Warning: .n %d\n",number_of_arguments);
				add_instruction(Chalt);
				add_label(ea_label_name);
				add_instruction(Chalt);
			}
		}

		if (descriptor_name==NULL)
			add_data4_to_code(0);
		else
			add_label(descriptor_name);
	} else if (descriptor_name != NULL) {
		add_instruction(CA_data_la);
		add_label(descriptor_name);
	} else {
		add_instruction(CA_data_a);
	}

	add_data2_to_code(number_of_arguments);
}

void code_nu(int a_size,int b_size,char *descriptor_name,char *ea_label_name) {
#if 0
	if ((pgrm.code_size & 1)!=0)
		add_data2_to_code(0);
#endif
	if (descriptor_name[0]=='_' && descriptor_name[1]=='_' && descriptor_name[2]=='\0')
		descriptor_name=NULL;

	if (ea_label_name!=NULL) {
		/* eval_upd not yet implemented */
		/* eval_fill */
		add_instruction(CA_data_IIIla);
		add_instruction(Cjsr_eval0);
		add_instruction(Cfill_a01_pop_rtn);
		add_instruction(Chalt);

		if (descriptor_name==NULL)
			add_data4_to_code(0);
		else
			add_label(descriptor_name);
	} else if (descriptor_name!=NULL) {
		add_instruction(CA_data_la);
		add_label(descriptor_name);
	} else {
		add_instruction(CA_data_a);
	}

	add_data2_to_code(a_size+b_size+(b_size<<8));
}

void code_o(int oa,int ob,ULONG vector[]) {
	if (last_jsr_with_d) {
		if (pgrm.code[pgrm.code_size-2].value!=Cjsr) {
			int i;

			i=pgrm.code[pgrm.code_size-3].value;
			if (i!=Cpop_a_jsr && i!=Cpop_b_jsr && i!=Cpush_a_jsr && i!=Cpush_b_jsr
				&& pgrm.code[pgrm.code_size-4].value!=Cbuildh0_put_a_jsr)
			{
				fprintf(stderr, "Error: .o directive used incorrectly near jsr\n");
				exit(1);
			}
		}
#if 0
		code[pgrm.code_size-2]=Cjsr_stack_check;
		if (list_code)
			printf("%d\t%d %d\n",pgrm.code_size,oa-last_da,ob-last_db);
		store_code_elem(8, oa-last_da);
		store_code_elem(8, ob-last_db);

		add_instruction(Cstack_check);
#endif
	}
	last_jsr_with_d=0;
}

void code_start(char *label_name) {
	if (strcmp ("__nostart__",label_name)==0)
		return;

	add_instruction_label(Cjmp,label_name);
}

void code_record(char record_label_name[],char type[],int a_size,int b_size,char record_name[],int record_name_length) {
	struct label *record_label;

	if (list_code)
		printf("\t.data\n");

	/* Resolve descriptor address */
	store_data_l(-1);
	store_data_l(0);

	record_label=enter_label(record_label_name);
	if (record_label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",record_label_name);
		exit(1);
	}
	record_label->label_offset=(pgrm.data_size<<2)+1;

	if (list_code)
		printf("%d\t.data4 0\n",pgrm.data_size<<2);
	store_data_l((a_size + b_size + 256) | (a_size << 16));

	/* TODO: do we need the type string? */

	/* TODO: do we need the record name? */
	store_string(record_name,record_name_length);

	if (list_code)
		printf("\t.text\n");
}

void code_record_start(char record_label_name[],char type[],int a_size,int b_size) {
	struct label *record_label;

	if (list_code)
		printf("\t.data\n");

	/* Resolve descriptor address */
	store_data_l(0);
	store_data_l(0);

	record_label=enter_label(record_label_name);
	if (record_label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",record_label_name);
		exit(1);
	}
	record_label->label_offset=(pgrm.data_size<<2)+1;

	if (list_code)
		printf("%d\t.data4 0\n",pgrm.data_size<<2);
	store_data_l((a_size + b_size + 256) | (a_size << 16));
}

void code_record_descriptor_label(char descriptor_name[]) {
	store_data_label_value(descriptor_name,0);
}

void code_record_end(char record_name[],int record_name_length) {
	/* TODO: do we need the record name? */
	store_string(record_name,record_name_length);
	if (list_code)
		printf("\t.text\n");
}

void code_string(char label_name[],char string[],int string_length) {
	struct label *label;

	if (list_code) {
		printf("\t.data\n");
		printf("%s:\n",label_name);
	}

	label=enter_label(label_name);
	if (label->label_offset!=-1) {
		fprintf(stderr, "Error: label %s already defined\n",label_name);
		exit(1);
	}
	label->label_offset=(pgrm.data_size<<2)+1;

	store_string(string,string_length);

	if (list_code)
		printf("\t.text\n");
}

void code_dummy(void) {
}

static void print_code(int segment_size,struct word *segment,FILE *program_file) {
	if (segment_size <= 0)
		return;

	unsigned int i;
	for (i = 0; i < segment_size; i++) {
#if 0
		fprintf(stderr, "%u:\t%ld (%s: %s)\n", segment[i].width, segment[i].value, instruction_name(segment[i].value), instruction_type(segment[i].value));
#endif
		fwrite(&segment[i].value, segment[i].width, 1, program_file);
	}
}

static void print_strings(int segment_size,uint32_t *segment,FILE *program_file) {
	if (segment_size <= 0)
		return;

	unsigned int i;
	for (i = 0; i < segment_size; i++) {
		fwrite(&segment[i], sizeof(segment[i]), 1, program_file);
	}
}

static void print_data(int segment_size,uint64_t *segment,FILE *program_file) {
	if (segment_size <= 0)
		return;

	unsigned int i;
	for (i = 0; i < segment_size; i++) {
		fwrite(&segment[i], sizeof(uint64_t), 1, program_file);
	}
}

static void print_relocations(int n_relocations, struct relocation *relocations, FILE *program_file) {
	unsigned int i;
	for (i = 0; i < n_relocations; i++) {
		fwrite(&relocations[i].relocation_offset, sizeof(relocations[i].relocation_offset), 1, program_file);
		fwrite(&relocations[i].relocation_label->label_id, sizeof(relocations[i].relocation_label->label_id), 1, program_file);
	}
}

static int count_and_renumber_labels(struct label_node *node, int start) {
	if (node->label_node_left != NULL)
		start = count_and_renumber_labels(node->label_node_left, start);

	struct label *label = node->label_node_label_p;
	label->label_id = start++;
	if (label->label_offset < 0) {
		global_label_count++;
		global_label_string_count+=strlen(label->label_name);
	}

	if (node->label_node_right != NULL)
		start = count_and_renumber_labels(node->label_node_right, start);

	return start;
}

static void print_global_labels(struct label_node *node, FILE *program_file) {
	if (node->label_node_left != NULL)
		print_global_labels(node->label_node_left, program_file);

	struct label *label = node->label_node_label_p;
	fwrite(&label->label_offset, sizeof(label->label_offset), 1, program_file);
	if (label->label_module_n == - 1 || label->label_offset < 0)
		fprintf(program_file, "%s", label->label_name);
	fputc('\0', program_file);

	if (node->label_node_right != NULL)
		print_global_labels(node->label_node_right, program_file);
}

void write_program(FILE *program_file) {
	fwrite(&pgrm.code_size, sizeof(pgrm.code_size), 1, program_file);
	fwrite(&pgrm.words_in_strings, sizeof(pgrm.words_in_strings), 1, program_file);
	fwrite(&pgrm.strings_size, sizeof(pgrm.strings_size), 1, program_file);
	fwrite(&pgrm.data_size, sizeof(pgrm.data_size), 1, program_file);

	count_and_renumber_labels(labels, 0);
	fwrite(&label_id, sizeof(label_id), 1, program_file);
	fwrite(&global_label_string_count, sizeof(global_label_string_count), 1, program_file);

	fwrite(&pgrm.code_reloc_size, sizeof(pgrm.code_reloc_size), 1, program_file);
	fwrite(&pgrm.data_reloc_size, sizeof(pgrm.code_reloc_size), 1, program_file);

	print_code(pgrm.code_size,pgrm.code,program_file);
	print_strings(pgrm.strings_size,pgrm.strings,program_file);
	print_data(pgrm.data_size,pgrm.data,program_file);

	print_global_labels(labels, program_file);

	print_relocations(pgrm.code_reloc_size,pgrm.code_relocations,program_file);
	print_relocations(pgrm.data_reloc_size,pgrm.data_relocations,program_file);

	if (fclose(program_file)) {
		fprintf(stderr, "Error writing program file\n");
		exit(1);
	}
}
