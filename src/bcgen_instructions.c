#include "bcgen_instructions.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "bcgen.h"
#include "util.h"

#define max_implemented_instruction_n CMAX-1

int is_32_bit=0;

struct program pgrm;
uint32_t last_d, last_jsr_with_d;

struct label_node *labels;
uint32_t label_id;
uint32_t global_label_count;
uint32_t global_label_string_count;
static struct label *start_label;

uint32_t module_n;

uint32_t list_code = 0;

uint32_t allocated_code_size;
uint32_t allocated_data_size;
uint32_t allocated_strings_size;
uint32_t allocated_code_relocations_size;
uint32_t allocated_data_relocations_size;

#define N_ADD_ARG_LABELS 32
static uint32_t Fadd_arg_label_used[N_ADD_ARG_LABELS];
#ifndef LINK_CLEAN_RUNTIME
static int general_add_arg_label_used=0;
#endif

#ifndef LINK_CLEAN_RUNTIME
int16_t warned_unsupported_instructions[128]={-1};
int warned_unsupported_instructions_i=0;
void unsupported_instruction_warning(int16_t instruction) {
	for (int i=0; i<warned_unsupported_instructions_i; i++)
		if (warned_unsupported_instructions[i] == instruction)
			return;
	warned_unsupported_instructions[warned_unsupported_instructions_i++] = instruction;
	fprintf(stderr,"Warning: instruction %s is not supported by the interpreter\n",instruction_name(instruction));
}

char *used_ccalls[256]={NULL};
int used_ccalls_ptr=0;
void used_ccall_warning (char *c_function_name) {
	for (int i=0; i<used_ccalls_ptr; i++)
		if (!strcmp (used_ccalls[i],c_function_name))
			return;
	used_ccalls[used_ccalls_ptr]=safe_malloc (strlen (c_function_name)+1);
	strcpy (used_ccalls[used_ccalls_ptr],c_function_name);
	used_ccalls_ptr++;
}

void show_used_ccalls_warning (void) {
	if (used_ccalls_ptr==0)
		return;
	fprintf(stderr,"Warning: external C functions cannot be packaged into the bytecode: ");
	for (int i=0; i<used_ccalls_ptr-1; i++){
		fprintf (stderr,"%s; ",used_ccalls[i]);
		free (used_ccalls[i]);
	}
	fprintf (stderr,"%s\n",used_ccalls[used_ccalls_ptr-1]);
	free (used_ccalls[used_ccalls_ptr-1]);
}

int16_t requires_file_io[256]={-1};
int requires_file_io_ptr=0;
void requires_file_io_warning (int16_t instruction) {
	for (int i=0; i<requires_file_io_ptr; i++)
		if (requires_file_io[i]==instruction)
			return;
	requires_file_io[requires_file_io_ptr]=instruction;
	requires_file_io_ptr++;
}

void show_requires_file_io_warning (void) {
	if (requires_file_io_ptr==0)
		return;
	fprintf(stderr,"Warning: some instructions require file I/O: ");
	for (int i=0; i<requires_file_io_ptr-1; i++)
		fprintf (stderr,"%s; ",instruction_name (requires_file_io[i]));
	fprintf (stderr,"%s\n",instruction_name (requires_file_io[requires_file_io_ptr-1]));
}
#endif

struct program *initialize_code(void) {
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

	return &pgrm;
}

#ifndef LINK_CLEAN_RUNTIME
void code_next_module(void) {
	module_n++;

	last_d = 0;
	last_jsr_with_d = 0;
}
#endif

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
	pgrm.code_byte_size+=bytewidth;
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

#ifdef LINKER
struct label *find_label(char *label_name) {
	struct label_node *label_node_p=labels;

	while (label_node_p!=NULL) {
		int r=strcmp(label_name,label_node_p->label_node_label_p->label_name);
		if (r==0)
			return label_node_p->label_node_label_p;
		else if (r<0)
			label_node_p=label_node_p->label_node_left;
		else
			label_node_p=label_node_p->label_node_right;
	}

	return NULL;
}

static struct label_node **move_label_tree_to_list(
		struct label_node **dest, struct label_node *tree) {
	if (tree->label_node_left!=NULL)
		dest=move_label_tree_to_list(dest,tree->label_node_left);
	*dest++=tree;
	if (tree->label_node_right!=NULL)
		dest=move_label_tree_to_list(dest,tree->label_node_right);
	return dest;
}

static struct label_node **merge_label_tree_with_list(
		struct label_node **dest,struct label_node *tree,
		struct label_node **list,unsigned int *list_i,unsigned int list_size) {
	int i=*list_i;

	if (i>=list_size)
		return move_label_tree_to_list(dest,tree);

	int r=strcmp(list[i]->label_node_label_p->label_name,tree->label_node_label_p->label_name);
	if (r>0) { /* prune: left side does not need to consider label list */
		if (tree->label_node_left!=NULL)
			dest=move_label_tree_to_list(dest,tree->label_node_left);
	} else {
		if (tree->label_node_left!=NULL)
			dest=merge_label_tree_with_list(dest,tree->label_node_left,list,list_i,list_size);

		i=*list_i;
		while (i<list_size && (r=strcmp(list[i]->label_node_label_p->label_name,tree->label_node_label_p->label_name))<0)
			*dest++=list[i++];
		*list_i=i;

		if (r==0) {
			EPRINTF("internal error: %s redefined\n",tree->label_node_label_p->label_name);
			exit(-1);
		}
	}

	*dest++=tree;

	if (tree->label_node_right!=NULL)
		dest=merge_label_tree_with_list(dest,tree->label_node_right,list,list_i,list_size);

	return dest;
}

static struct label_node *update_label_node_pointers(
		struct label_node **sorted_list,int lft,int rgt) {
	if (rgt<lft)
		return NULL;

	if (rgt==lft) {
		sorted_list[lft]->label_node_left=NULL;
		sorted_list[lft]->label_node_right=NULL;
		return sorted_list[lft];
	}

	int middle=lft+(rgt-lft)/2;
	sorted_list[middle]->label_node_left =update_label_node_pointers(sorted_list,lft,middle-1);
	sorted_list[middle]->label_node_right=update_label_node_pointers(sorted_list,middle+1,rgt);
	return sorted_list[middle];
}

void merge_new_labels_and_rebalance(struct label_node **new_labels,unsigned int n_new_labels) {
	for (int i=0; i<n_new_labels; i++) {
		struct label *label=new_labels[i]->label_node_label_p;
		label->label_id=label_id++;
		make_label_global(label);
	}

	if (labels==NULL) {
		labels=update_label_node_pointers(new_labels,0,label_id-1);
		return;
	}

	struct label_node **temp_list=safe_malloc(label_id*sizeof(struct label_node*));
	unsigned int list_i=0;
	struct label_node **temp_list_end=merge_label_tree_with_list(temp_list,labels,new_labels,&list_i,n_new_labels);
	while (list_i<n_new_labels)
		*temp_list_end++=new_labels[list_i++];

	labels=update_label_node_pointers(temp_list,0,label_id-1);

	free(temp_list);
}
#endif

struct label *new_label_at_offset(uint32_t offset) {
	char name[16];
	snprintf(name, 16, "i%x", offset);
	name[15] = '\0';
	struct label *label = enter_label(name);
	label->label_offset = offset;
	return label;
}

int internal_label_id = 0;
struct label *new_internal_label(void) {
	char label_name[14];
	sprintf(label_name,"%%%x",internal_label_id++);
	struct label *label = enter_label(label_name);
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

#ifndef LINK_CLEAN_RUNTIME
void store_code_internal_label_value(struct label *label,uint32_t offset) {
	add_code_relocation(label, pgrm.code_size);
	store_code_elem(2, offset);
}

void store_code_label_value(char *label_name,int32_t offset) {
	struct label *label;

	label=enter_label(label_name);

	add_code_relocation(label, pgrm.code_size);

	store_code_elem(2, offset);
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

void add_data8_to_code(int i) {
	if (list_code)
		printf("%d\t.data8 %d\n",pgrm.code_size,i);

	store_code_elem(8, i);
}

void add_instruction(int16_t i) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s\n",pgrm.code_size,instruction_name (i));

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
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
			*(int32_t*)&pgrm.code[relocation_p->relocation_offset] += code_offset;
		else
			*(int32_t*)&pgrm.code[relocation_p->relocation_offset] += data_offset;
	}

	for(i=0; i<pgrm.data_reloc_size; ++i) {
		struct relocation *relocation_p;

		relocation_p=&pgrm.data_relocations[i];
		if ((relocation_p->relocation_label->label_offset & 1)==0)
			*(int32_t*)&pgrm.data[relocation_p->relocation_offset] += code_offset;
		else
			*(int32_t*)&pgrm.data[relocation_p->relocation_offset] += data_offset;
	}
}

void add_instruction_label(uint16_t i,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s\n",pgrm.code_size,instruction_name (i),label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name,0);
}

void add_instruction_label_offset(uint16_t i,char *label_name,uint32_t offset) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d\n",pgrm.code_size,instruction_name (i),label_name,offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name,offset);
}

void add_instruction_label_offset_label(int16_t i,char *label_name1,uint32_t offset,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d %s\n",pgrm.code_size,instruction_name (i),label_name1,offset,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name1,offset);
	store_code_label_value(label_name2,0);
}

void add_instruction_label_offset_w(uint16_t i,char *label_name,uint32_t offset,int32_t n1) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d %d\n",pgrm.code_size,instruction_name (i),label_name,offset,(int)n1);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n1);
}

void add_instruction_label_offset_w_label(uint16_t i,char *label_name1,uint32_t offset,int32_t n1,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d %d %s\n",pgrm.code_size,instruction_name (i),label_name1,offset,(int)n1,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name1,offset);
	store_code_elem(2, n1);
	store_code_label_value(label_name2,0);
}

void add_instruction_label_offset_label_label_offset_label
	(int16_t i,char *label_name1,uint32_t offset1,char *label_name2,char *label_name3,uint32_t offset2,char *label_name4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s%+d %s %s%+d %s\n",pgrm.code_size,instruction_name (i),label_name1,offset1,label_name2,
																			label_name3,offset2,label_name4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name1,offset1);
	store_code_label_value(label_name2,0);
	store_code_label_value(label_name3,offset2);
	store_code_label_value(label_name4,0);
}

void add_instruction_label_w_w(int16_t i,char *label_name,int32_t n1,int32_t n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %s %d %d\n",pgrm.code_size,instruction_name (i),label_name,(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_label_value(label_name,0);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
}

void add_instruction_c(int16_t i,char n) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d\n",pgrm.code_size,instruction_name (i),(int)n);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(1, n);
}

void add_instruction_c_label(int16_t i,char n,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s\n",pgrm.code_size,instruction_name (i),(int)n,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(1, n);
	store_code_label_value(label_name,0);
}

void add_instruction_w(int16_t i,int32_t n) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d\n",pgrm.code_size,instruction_name (i),(int)n);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
}

void add_instruction_w_i(int16_t i,int32_t n1,int64_t n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(8, n2);
}

void add_instruction_w_label(int16_t i,int32_t n,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s\n",pgrm.code_size,instruction_name (i),(int)n,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
	store_code_label_value(label_name,0);
}

void add_instruction_w_label_offset(int16_t i,int32_t n,char *label_name,uint32_t offset) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s%+d\n",pgrm.code_size,instruction_name (i),(int)n,label_name,offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
	store_code_label_value(label_name,offset);
}

void add_instruction_w_label_offset_label(int16_t i,int32_t n,char *label_name1,uint32_t offset,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s%+d %s\n",pgrm.code_size,instruction_name (i),(int)n,label_name1,offset,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
	store_code_label_value(label_name1,offset);
	store_code_label_value(label_name2,0);
}

void add_instruction_w_label_w(int16_t i,int32_t n1,char *label_name,int32_t n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s %d\n",pgrm.code_size,instruction_name (i),(int)n1,label_name,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name,0);
	store_code_elem(2, n2);
}

void add_instruction_w_label_offset_label_label_offset_label
	(int16_t i,int32_t n1,char *label_name1,uint32_t offset1,char *label_name2,char *label_name3,uint32_t offset2,char *label_name4) {
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

void add_instruction_w_label_offset_w(int16_t i,int32_t n1,char *label_name,uint32_t offset,int32_t n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s+%d %d\n",pgrm.code_size,instruction_name (i),(int)n1,label_name,offset,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n2);
}

void add_instruction_w_label_offset_w_w(int16_t i,int32_t n1,char *label_name,uint32_t offset,int32_t n2,int32_t n3) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s+%d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,label_name,offset,(int)n2,(int)n3);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
}

void add_instruction_w_label_offset_w_w_w(int16_t i,int32_t n1,char *label_name,uint32_t offset,int32_t n2,int32_t n3,int32_t n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s+%d %d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,label_name,offset,(int)n2,(int)n3,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
}

void add_instruction_w_c(int16_t i,int32_t n1,char n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(1, n2);
}

void add_instruction_w_w(int16_t i,int32_t n1,int32_t n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
}

void add_instruction_i_w(int16_t i,int64_t n1,int32_t n2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(8, n1);
	store_code_elem(2, n2);
}

void add_instruction_i_label(int16_t i,int64_t n,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %s\n",pgrm.code_size,instruction_name (i),(int)n,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(8, n);
	store_code_label_value(label_name,0);
}

void add_instruction_w_c_label(int16_t i,int32_t n1,char n2,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(1, n2);
	store_code_label_value(label_name,0);
}

void add_instruction_w_w_label(int16_t i,int32_t n1,int32_t n2,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,0);
}

void add_instruction_w_i_label(int16_t i,int32_t n1,int64_t n2,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(8, n2);
	store_code_label_value(label_name,0);
}

void add_instruction_w_w_label_offset(int32_t i,int32_t n1,int32_t n2,char *label_name,uint32_t offset) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s%+d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,offset);
}

void add_instruction_w_w_label_w(int16_t i,int32_t n1,int32_t n2,char *label_name,int32_t n3) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,(int)n3);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,0);
	store_code_elem(2, n3);
}

void add_instruction_w_c_label_c_label(int16_t i,int32_t n1,char n2,char *label_name1,char n3,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name1,(int)n3,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(1, n2);
	store_code_label_value(label_name1,0);
	store_code_elem(1, n3);
	store_code_label_value(label_name2,0);
}

void add_instruction_w_w_label_w_label(int16_t i,int32_t n1,int32_t n2,char *label_name1,int32_t n3,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name1,(int)n3,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name1,0);
	store_code_elem(2, n3);
	store_code_label_value(label_name2,0);
}

void add_instruction_w_i_label_i_label(int16_t i,int32_t n1,int64_t n2,char *label_name1,int64_t n3,char *label_name2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name1,(int)n3,label_name2);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(8, n2);
	store_code_label_value(label_name1,0);
	store_code_elem(8, n3);
	store_code_label_value(label_name2,0);
}

void add_instruction_w_w_label_offset_w(int16_t i,int32_t n1,int32_t n2,char *label_name,uint32_t offset,int32_t n3) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s+%d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,offset,(int)n3);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n3);
}

void add_instruction_w_w_label_w_w(int16_t i,int32_t n1,int32_t n2,char *label_name,int32_t n3,int32_t n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,(int)n3,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,0);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
}

void add_instruction_w_w_label_offset_w_w(int16_t i,int32_t n1,int32_t n2,char *label_name,uint32_t offset,int32_t n3,int32_t n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s+%d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,label_name,offset,(int)n3,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
}

void add_instruction_w_w_w(int16_t i,int32_t n1,int32_t n2,int32_t n3) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
}

void add_instruction_w_w_w_label(int16_t i,int32_t n1,int32_t n2,int32_t n3,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %s\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,0);
}

void add_instruction_w_w_w_label_offset(int16_t i,int32_t n1,int32_t n2,int32_t n3,char *label_name,uint32_t offset) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %s+%d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,label_name,offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,offset);
}

void add_instruction_w_w_w_label_w(int16_t i,int32_t n1,int32_t n2,int32_t n3,char *label_name,int32_t n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("\t%s %d %d %d %s %d\n",instruction_name (i),(int)n1,(int)n2,(int)n3,label_name,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,0);
	store_code_elem(2, n4);
}

void add_instruction_w_w_w_label_offset_w(int16_t i,int32_t n1,int32_t n2,int32_t n3,char *label_name,uint32_t offset,int32_t n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %s+%d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,label_name,offset,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_label_value(label_name,offset);
	store_code_elem(2, n4);
}

void add_instruction_w_w_w_label_w_w(int16_t i,int32_t n1,int32_t n2,int32_t n3,char *label_name,int32_t n4,int32_t n5) {
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

void add_instruction_w_w_w_label_offset_w_w(int16_t i,int32_t n1,int32_t n2,int32_t n3,char *label_name,uint32_t offset,int32_t n4,int32_t n5) {
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

void add_instruction_w_w_w_w(int16_t i,int32_t n1,int32_t n2,int32_t n3,int32_t n4) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,(int)n4);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
}

void add_instruction_w_w_w_w_w(int16_t i,int32_t n1,int32_t n2,int32_t n3,int32_t n4,int32_t n5) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %d %d %d\n",pgrm.code_size,instruction_name (i),(int)n1,(int)n2,(int)n3,(int)n4,(int)n5);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_elem(2, n2);
	store_code_elem(2, n3);
	store_code_elem(2, n4);
	store_code_elem(2, n5);
}

void add_instruction_i(int16_t i,int64_t n) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d\n",pgrm.code_size,instruction_name (i),(int)n);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(8, n);
}

void add_instruction_r(int16_t i,double r) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %f\n",pgrm.code_size,instruction_name (i),r);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	if (is_32_bit){
		store_code_elem (4,((uint32_t*)&r)[0]);
		store_code_elem (4,((uint32_t*)&r)[1]);
	} else {
		store_code_elem (8,*(uint64_t*)&r);
	}
}

void add_instruction_w_r(int16_t i,int32_t n,double r) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %f\n",pgrm.code_size,instruction_name (i),r);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n);
	if (is_32_bit){
		store_code_elem (4,((uint32_t*)&r)[0]);
		store_code_elem (4,((uint32_t*)&r)[1]);
	} else {
		store_code_elem (8,*(uint64_t*)&r);
	}
}

void add_instruction_internal_label(int16_t i,struct label *label) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d\n",pgrm.code_size,instruction_name (i),label->label_offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_internal_label_value(label,0);
}

void add_instruction_internal_label_internal_label(int16_t i,struct label *label1,struct label *label2) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d\n",pgrm.code_size,instruction_name (i),label1->label_offset,label2->label_offset);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_internal_label_value(label1,0);
	store_code_internal_label_value(label2,0);
}

void add_instruction_w_internal_label_label(int16_t i,int32_t n1,struct label *label,char *label_name) {
	if (list_code || i>max_implemented_instruction_n)
		printf("%d\t%s %d %d %s\n",pgrm.code_size,instruction_name (i),n1,label->label_offset,label_name);

	store_code_elem(BYTEWIDTH_INSTRUCTION, i);
	store_code_elem(2, n1);
	store_code_internal_label_value(label,0);
	store_code_label_value(label_name,0);
}

struct word *add_add_arg_labels(void) {
	int i;

	for(i=0; i<N_ADD_ARG_LABELS; ++i)
		if (Fadd_arg_label_used[i]) {
			if (i>0) {
				/* Three instructions above add_arg is the code used for fast
				 * applies of constructors. build_node returns; notB is not used. */
				if (i==1) {
					add_instruction(Cbuild_node2_rtn);
					add_instruction(CnotB);
				} else {
					add_instruction_w(Cbuild_node_rtn,i-1);
				}
				add_instruction(CnotB);
			}
			char label_name[11];
			sprintf(label_name,"_add_arg%d",i);
			struct label *label = enter_label(label_name);
			if (label->label_module_n != -1) {
				make_label_global(label);
				label->label_offset = pgrm.code_size<<2;
			}
			add_instruction(Cadd_arg0+i);
		}

	if (general_add_arg_label_used) {
		fprintf(stderr,"Warning: currying of functions with more than 32 arguments is not implemented.\n");

		struct label *label=enter_label("_add_arg");
		if (label->label_module_n!=-1) {
			make_label_global(label);
			label->label_offset=pgrm.code_size<<2;
		}
		add_instruction(Cadd_arg);
	}

	return pgrm.code;
}

struct specialized_jsr {
	const char *label;
	int instruction;
	int instruction_32;
	int flags;
};

#define SPECIALIZED(instr,flags) {#instr, C ## instr, C ## instr, flags},
#define SPECIALIZED_32(instr,flags) {#instr, C ## instr, C ## instr ## _32, flags},
#define S_UNSUPPORTED 1
#define S_IO 2

static struct specialized_jsr specialized_jsr_labels[]={
	SPECIALIZED(eqAC,0)
	SPECIALIZED(cmpAC,0)
	SPECIALIZED(catAC,0)
	SPECIALIZED(sliceAC,0)
	SPECIALIZED(updateAC,0)
	SPECIALIZED(ItoAC,0)
	SPECIALIZED(BtoAC,0)
	SPECIALIZED_32(RtoAC,0)
	{"print__string__",Cprint_string,Cprint_string},

	SPECIALIZED(closeF,       S_IO)
	SPECIALIZED(endF,         S_IO)
	SPECIALIZED(endSF,        S_IO | S_UNSUPPORTED)
	SPECIALIZED(errorF,       S_IO)
	SPECIALIZED(flushF,       S_IO)
	SPECIALIZED(openF,        S_IO)
	SPECIALIZED(openSF,       S_IO | S_UNSUPPORTED)
	SPECIALIZED(positionF,    S_IO)
	SPECIALIZED(positionSF,   S_IO | S_UNSUPPORTED)
	SPECIALIZED(readFC,       S_IO)
	SPECIALIZED(readFI,       S_IO)
	SPECIALIZED_32(readFR,    S_IO)
	SPECIALIZED(readFS,       S_IO)
	SPECIALIZED(readFString,  S_IO | S_UNSUPPORTED)
	SPECIALIZED(readLineF,    S_IO)
	SPECIALIZED(readLineSF,   S_IO | S_UNSUPPORTED)
	SPECIALIZED(readSFC,      S_IO | S_UNSUPPORTED)
	SPECIALIZED(readSFI,      S_IO | S_UNSUPPORTED)
	SPECIALIZED_32(readSFR,   S_IO | S_UNSUPPORTED)
	SPECIALIZED(readSFS,      S_IO | S_UNSUPPORTED)
	SPECIALIZED(reopenF,      S_IO | S_UNSUPPORTED)
	SPECIALIZED(seekF,        S_IO)
	SPECIALIZED(seekSF,       S_IO | S_UNSUPPORTED)
	SPECIALIZED(shareF,       S_IO | S_UNSUPPORTED)
	SPECIALIZED(stderrF,      S_IO)
	SPECIALIZED(stdioF,       S_IO)
	SPECIALIZED(writeFC,      S_IO)
	SPECIALIZED(writeFI,      S_IO)
	SPECIALIZED_32(writeFR,   S_IO)
	SPECIALIZED(writeFS,      S_IO)
	SPECIALIZED(writeFString, S_IO)
};

static int get_specialized_jsr_label_n(char label_name[]) {
	int i,n;

	n=sizeof(specialized_jsr_labels)/sizeof(struct specialized_jsr);
	for(i=0; i<n; ++i)
		if (!strcmp(label_name,specialized_jsr_labels[i].label))
			return i;

	return -1;
}

void add_specialized_jsr_instruction(unsigned int n) {
	if (n>=sizeof(specialized_jsr_labels)/sizeof(struct specialized_jsr)) {
		fprintf(stderr,"internal error in add_specialized_jsr_instruction: %d\n",n);
		exit(1);
	}

	struct specialized_jsr *entry=&specialized_jsr_labels[n];
	if (entry->flags & S_UNSUPPORTED)
		unsupported_instruction_warning(entry->instruction);
	else if (entry->flags & S_IO)
		requires_file_io_warning (entry->instruction);

	add_instruction(is_32_bit ? entry->instruction_32 : entry->instruction);
}

void add_label(char *label_name) {
	if (list_code)
		printf("%d\t.label %s\n",pgrm.code_size,label_name);

	store_code_label_value(label_name,0);
}

void code_absR(void) {
	add_instruction(is_32_bit ? CabsR_32 : CabsR);
}

void code_acosR(void) {
	add_instruction(is_32_bit ? CacosR_32 : CacosR);
}

void code_addI(void) {
	add_instruction(CaddI);
}

void code_addIo(void) {
	add_instruction(CaddIo);
}

void code_addLU(void) {
	add_instruction(CaddLU);
}

void code_addR(void) {
	add_instruction(is_32_bit ? CaddR_32 : CaddR);
}

void code_and(void) {
	add_instruction(CandI);
}

void code_asinR(void) {
	add_instruction(is_32_bit ? CasinR_32 : CasinR);
}

void code_atanR(void) {
	add_instruction(is_32_bit ? CatanR_32 : CatanR);
}

void code_buildh(char descriptor_name[],int arity);

void code_build(char descriptor_name[],int arity,char *code_name) {
	if (code_name[0]=='_' && code_name[1]=='_' && code_name[2]=='h' && code_name[3]=='n' && code_name[4]=='f' && code_name[5]=='\0') {
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
#endif

void print_string_directive(char *string,int string_length) {
	int i;

	PRINTF("%d\t.string \"",pgrm.data_size<<2);
	for(i=0; i<string_length; ++i)
		putchar(string[i]);
	PRINTF("\"\n");
}

void add_words_in_strings(uint32_t val) {
	pgrm.words_in_strings += val;
}

void add_string_information(uint32_t data_offset) {
	if (pgrm.strings_size >= allocated_strings_size)
		realloc_strings();

	pgrm.strings[pgrm.strings_size++] = data_offset;
}

void store_string(char *string,int string_length,int include_terminator) {
	add_string_information(pgrm.data_size);

	if (list_code)
		PRINTF("%d\t.data4 %d\n",pgrm.data_size<<2,string_length);
	store_data_l(string_length+include_terminator);

	if (list_code)
		print_string_directive(string,string_length);

	int i=0;
	while(i+8<=string_length) {
		store_data_l(
				(unsigned char)string[i] |
				((uint64_t)(unsigned char)string[i+1]<<8) |
				((uint64_t)(unsigned char)string[i+2]<<16) |
				((uint64_t)(unsigned char)string[i+3]<<24) |
				((uint64_t)(unsigned char)string[i+4]<<32) |
				((uint64_t)(unsigned char)string[i+5]<<40) |
				((uint64_t)(unsigned char)string[i+6]<<48) |
				((uint64_t)(unsigned char)string[i+7]<<56));
		i+=8;
	}
	if (i!=string_length) {
		uint64_t n = 0;
		int s = 0;

		while(i<string_length) {
			n |= (uint64_t)(unsigned char)string[i]<<s;
			s+=8;
			++i;
		}
		store_data_l(n);
	} else if (include_terminator) {
		store_data_l(0);
	}

	pgrm.words_in_strings += string_length / 8 + (string_length % 8 == 0 ? 0 : 1);
}

#ifndef LINK_CLEAN_RUNTIME
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
	store_string(string,string_length,0);
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
	add_instruction_r(is_32_bit ? CbuildR_32 : CbuildR,value);
}

void code_buildR_b(int b_offset) {
	add_instruction_w(is_32_bit ? CbuildR_b_32 : CbuildR_b,b_offset);
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
		if (b_size==4) {
			add_instruction_label_offset(Cbuildhr04,descriptor_name,2);
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
		add_instruction_w_w_label_offset(Cbuild_r0b,b_size,b_offset,descriptor_name,2);
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
	switch (a_size) {
		case 0: switch (b_size) {
			case 1:  add_instruction_label(Cbuild_u01,code_name); return;
			case 2:  add_instruction_label(Cbuild_u02,code_name); return;
			case 3:  add_instruction_label(Cbuild_u03,code_name); return;
			default: add_instruction_w_label(Cbuild_u0b,b_size,code_name); return;
		}
		case 1: switch (b_size) {
			case 1:  add_instruction_label(Cbuild_u11,code_name); return;
			case 2:  add_instruction_label(Cbuild_u12,code_name); return;
			case 3:  add_instruction_label(Cbuild_u13,code_name); return;
			default: add_instruction_w_label(Cbuild_u1b,b_size,code_name); return;
		}
		case 2: switch (b_size) {
			case 1:  add_instruction_label(Cbuild_u21,code_name); return;
			case 2:  add_instruction_label(Cbuild_u22,code_name); return;
			default: add_instruction_w_label(Cbuild_u2b,b_size,code_name); return;
		}
		default:
			if (b_size==1) {
				if (a_size==3)
					add_instruction_label(Cbuild_u31,code_name);
				else
					add_instruction_w_label(Cbuild_ua1,a_size,code_name);
				return;
			} else {
				add_instruction_w_label_w(Cbuild_u,a_size+b_size,code_name,a_size);
			}
	}
}

void code_cosR(void) {
	add_instruction(is_32_bit ? CcosR_32 : CcosR);
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
				add_instruction(is_32_bit ? Ccreate_arrayREAL_32 : Ccreate_arrayREAL);
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

		add_instruction_w_w_label_offset(Ccreate_array_r,a_size+b_size,b_size,element_descriptor,2);
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
		case 'R':
			if (element_descriptor[1]=='E' && element_descriptor[2]=='A' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0')
			{
				add_instruction(is_32_bit ? Ccreate_array_REAL_32 : Ccreate_array_REAL);
				return;
			}
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction(Ccreate_array_);
				return;
			}
	}

	if (b_size==0)
		add_instruction_w_label_offset(Ccreate_array_r_a,a_size,element_descriptor,2);
	else if (a_size==0)
		add_instruction_w_label_offset(Ccreate_array_r_b,b_size,element_descriptor,2);
	else
		add_instruction_w_w_label_offset(Ccreate_array_r_,a_size+b_size,a_size,element_descriptor,2);
}

void code_decI(void) {
	add_instruction(CdecI);
}

void code_divI(void) {
	add_instruction(CdivI);
}

void code_divLU(void) {
	add_instruction(CdivLU);
}

void code_divR(void) {
	add_instruction(is_32_bit ? CdivR_32 : CdivR);
}

void code_entierR(void) {
	add_instruction(is_32_bit ? CentierR_32 : CentierR);
}

void code_eqAC_a(char *string,int string_length) {
	struct label *string_label;

	string_label=new_internal_label();
	string_label->label_offset=(pgrm.data_size<<2)+1;

	add_instruction_internal_label(CeqAC_a,string_label);

	if (list_code)
		printf("\t.data\n");
	store_string(string,string_length,0);
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

void code_eqCc(int value) {
	add_instruction_c(CeqCc,value);
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

void code_eqIi(CleanInt value) {
	add_instruction_i(CeqIi,value);
}

void code_eqR(void) {
	add_instruction(is_32_bit ? CeqR_32 : CeqR);
}

void code_eqR_b(double value,int b_offset) {
	add_instruction_w_r(is_32_bit ? CeqR_b_32 : CeqR_b,b_offset,value);
}

void code_eq_desc(char descriptor_name[],int arity,int a_offset) {
	add_instruction_w_label_offset(Ceq_desc,-a_offset,descriptor_name,(arity<<3)+2);
}

void code_eq_desc_b(char descriptor_name[],int arity) {
	if (is_32_bit && descriptor_name[0]=='R' && descriptor_name[1]=='E' &&
			descriptor_name[2]=='A' && descriptor_name[3]=='L' && descriptor_name[4]=='\0')
		add_instruction_label_offset(Ceq_desc_b,"DREAL",(arity<<3)+2);
	else
		add_instruction_label_offset(Ceq_desc_b,descriptor_name,(arity<<3)+2);
	return;
}

void code_eq_nulldesc(char descriptor_name[], int a_offset) {
	add_instruction_w_label_offset(Ceq_nulldesc,-a_offset,descriptor_name,2);
}

void code_exit_false(char label_name[]) {
	add_instruction_label(Cjmp_false,label_name);
}

void code_expR(void) {
	add_instruction(is_32_bit ? CexpR_32 : CexpR);
}

void code_fill(char descriptor_name[],int arity,char *code_name,int a_offset) {
	if (code_name[0]=='_' && code_name[1]=='_' && code_name[2]=='h' && code_name[3]=='n' && code_name[4]=='f' && code_name[5]=='\0') {
		code_fillh(descriptor_name,arity,a_offset);
		return;
	}

	if (arity==0) {
		add_instruction_w_label(Cfill0,-a_offset,code_name);
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
		} else {
			if (bits[0]=='1')
				add_instruction_w_label_offset(Cfillh0,-a_offset,descriptor_name,(arity<<3)+2);
			else
				{} /* nop */
			return;
		}
	} else if (arity==2) {
		if (bits[1]=='0') {
			if (bits[0]=='0') {
				if (bits[2]=='1')
					add_instruction_w(Cfill1001,-a_offset);
				else
					{} /* nop */
				return;
			} else if (bits[0]=='1' && bits[2]=='1') {
				add_instruction_w_label_offset(Cfill1101,-a_offset,descriptor_name,(arity<<3)+2);
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
	if (b_size==0 && bits[0]=='0') {
		code_fill1(descriptor_name,a_size,root_offset,bits);
		return;
	}

	switch (a_size+b_size) {
		case 1:
			if (bits[0]=='0' && bits[1]=='0') {
				/* nop */
				return;
			}
			if (b_size==1) {
				if (bits[0]=='0' && bits[1]=='1') {
					add_instruction_w(Cfill1_r0101,-root_offset);
					return;
				} else if (bits[0]=='1' && bits[1]=='1') {
					add_instruction_w_label_offset(Cfill1_r0111,-root_offset,descriptor_name,2);
					return;
				}
			} else {
				if (bits[0]=='0' && bits[1]=='1') {
					add_instruction_w(Cfill1010,-root_offset);
					return;
				} else if (bits[0]=='1' && bits[1]=='1') {
					add_instruction_w_label_offset(Cfill1,-root_offset,descriptor_name,2);
					return;
				}
			}
			break;
		case 2:
			if (bits[0]=='0' && bits[1]=='0' && bits[2]=='0') {
				/* nop */
				return;
			}
			if (a_size==2) {
				if (bits[0]=='1' && bits[1]=='0' && bits[2]=='0') {
					add_instruction_w_label_offset(Cfill0,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='0' && bits[2]=='1') {
					add_instruction_w_label_offset(Cfill1101,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='1' && bits[2]=='0') {
					add_instruction_w_label_offset(Cfill1,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='1' && bits[2]=='1') {
					add_instruction_w_label_offset(Cfill1_r20111,-root_offset,descriptor_name,2);
					return;
				}
			} else if (b_size==1 && a_size==1) {
				if (bits[0]=='0' && bits[1]=='0' && bits[2]=='1') {
					add_instruction_w(Cfill1_r11001,-root_offset);
					return;
				} else if (bits[0]=='0' && bits[1]=='1' && bits[2]=='0') {
					add_instruction_w(Cfill1010,-root_offset);
					return;
				} else if (bits[0]=='0' && bits[1]=='1' && bits[2]=='1') {
					add_instruction_w(Cfill1_r11011,-root_offset);
					return;
				} else if (bits[0]=='1' && bits[1]=='0' && bits[2]=='0') {
					add_instruction_w_label_offset(Cfill0,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='0' && bits[2]=='1') {
					add_instruction_w_label_offset(Cfill1_r11101,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='1' && bits[2]=='0') {
					add_instruction_w_label_offset(Cfill1,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='1' && bits[2]=='1') {
					add_instruction_w_label_offset(Cfill1_r11111,-root_offset,descriptor_name,2);
					return;
				}
			} else if (b_size==2) {
				if (bits[0]=='0' && bits[1]=='0' && bits[2]=='1') {
					add_instruction_w(Cfill1_r02001,-root_offset);
					return;
				} else if (bits[0]=='0' && bits[1]=='1' && bits[2]=='0') {
					add_instruction_w(Cfill1_r02010,-root_offset);
					return;
				} else if (bits[0]=='0' && bits[1]=='1' && bits[2]=='1') {
					add_instruction_w(Cfill1_r02011,-root_offset);
					return;
				} else if (bits[0]=='1' && bits[1]=='0' && bits[2]=='0') {
					add_instruction_w_label_offset(Cfill0,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='0' && bits[2]=='1') {
					add_instruction_w_label_offset(Cfill1_r02101,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='1' && bits[2]=='0') {
					add_instruction_w_label_offset(Cfill1_r02110,-root_offset,descriptor_name,2);
					return;
				} else if (bits[0]=='1' && bits[1]=='1' && bits[2]=='1') {
					add_instruction_w_label_offset(Cfill1_r02111,-root_offset,descriptor_name,2);
					return;
				}
			}
			break;
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
		if (bits[0]=='0') {
			int bit_n1,bit_n2;

			for(bit_n1=1; bit_n1<=arity; ++bit_n1)
				if (bits[bit_n1]!='0')
					break;

			for(bit_n2=bit_n1+1; bit_n2<=arity; ++bit_n2)
				if (bits[bit_n2]!='0')
					break;

			if (bit_n1==1) {
				add_instruction_w_w(Cfill2a011,-a_offset,bit_n2-2);
				return;
			} else {
				add_instruction_w_w_w(Cfill2a002,-a_offset,bit_n1-2,bit_n2-2);
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

	uint32_t bitvec=0;
	for (i=ab_size; i>1; i--)
		bitvec=(bitvec<<1)|(bits[i]=='1' ? 1 : 0);

	if (bits[0]=='0') {
		if (bits[1]=='0')
			add_instruction_w_w_w(Cfill2_r00,-root_offset,a_size-1,bitvec);
		else
			add_instruction_w_w_w(Cfill2_r01,-root_offset,a_size,bitvec);
	} else {
		if (bits[1]=='0')
			add_instruction_w_label_offset_w_w(Cfill2_r10,-root_offset,descriptor_name,2,a_size-1,bitvec);
		else
			add_instruction_w_label_offset_w_w(Cfill2_r11,-root_offset,descriptor_name,2,a_size,bitvec);
	}
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

	if (n_bits==1 && ab_size>2) {
		if (bits[0]=='1' && a_size>=1) {
			add_instruction_w_label_offset(Cfill3a10,-root_offset,descriptor_name,2);
			return;
		}

		int bit_n;
		for(bit_n=1; bit_n<ab_size; ++bit_n)
			if (bits[bit_n]!='0')
				break;

		if (bit_n>=a_size) {
			add_instruction_w_label_offset_w(Cfill3_r01b,-root_offset,descriptor_name,2,bit_n);
			return;
		} else {
			add_instruction_w_label_offset_w(Cfill3_r01a,-root_offset,descriptor_name,2,bit_n);
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

	uint32_t bitvec=0;
	for (i=0; i<ab_size; i++)
		bitvec=(bitvec<<1)|(bits[i]=='1' ? 1 : 0);

	add_instruction_w_label_offset_w_w_w(Cfill3_r,-root_offset,descriptor_name,2,a_size,b_size,bitvec);
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

void code_fillI(CleanInt value,int a_offset) {
	add_instruction_i_w(CfillI,value,-a_offset);
}

void code_fillI_b(int b_offset,int a_offset) {
	add_instruction_w_w(CfillI_b,-a_offset,b_offset);
}

void code_fillR_b(int b_offset,int a_offset) {
	add_instruction_w_w(is_32_bit ? CfillR_b_32 : CfillR_b,-a_offset,b_offset);
}

void code_fill_a(int from_offset,int to_offset) {
	add_instruction_w_w(Cfill_a,-from_offset,-to_offset);
}

void code_fill_u(char descriptor_name[], int a_size, int b_size, char code_name[], int a_offset) {
	if (!(descriptor_name[0]=='_' && descriptor_name[1]=='_' && descriptor_name[2]=='\0')) {
		fprintf(stderr,"Error: fill_u %s %d %d %s %d\n",descriptor_name,a_size,b_size,code_name,a_offset);
		exit(1);
	}

	add_instruction_w_w_label_offset_w(Cfill_u,a_size,b_size,code_name,0,a_offset);
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

	if (a_size==0 && b_size>=4) {
		add_instruction_w_w_label_offset_w(Cfill_r0b,b_size,-root_offset,descriptor_name,2,b_offset);
		return;
	} else if (a_size==1 && b_size>=4) {
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

void code_get_desc_arity_offset(void) {
	unsupported_instruction_warning(Cget_desc_arity_offset);
	add_instruction(Cget_desc_arity_offset);
}

void code_get_node_arity(int a_offset) {
	add_instruction_w(Cget_node_arity,-a_offset);
}

void code_get_thunk_arity(void) {
	unsupported_instruction_warning(Cget_thunk_arity);
	add_instruction(Cget_thunk_arity);
}

void code_get_thunk_desc(void) {
	unsupported_instruction_warning(Cget_thunk_desc);
	add_instruction(Cget_thunk_desc);
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

void code_instruction(CleanInt value) {
	unsupported_instruction_warning(Cinstruction);
	add_instruction_i(Cinstruction,value);
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
	switch (n_apply_args) {
		case 1:  add_instruction(Cjmp_ap1);  return;
		case 2:  add_instruction(Cjmp_ap2);  return;
		case 3:  add_instruction(Cjmp_ap3);  return;
		case 4:  add_instruction(Cjmp_ap4);  return;
		case 5:  add_instruction(Cjmp_ap5);  return;
		case 6:  add_instruction(Cjmp_ap6);  return;
		case 7:  add_instruction(Cjmp_ap7);  return;
		case 8:  add_instruction(Cjmp_ap8);  return;
		case 9:  add_instruction(Cjmp_ap9);  return;
		case 10: add_instruction(Cjmp_ap10); return;
		case 11: add_instruction(Cjmp_ap11); return;
		case 12: add_instruction(Cjmp_ap12); return;
		case 13: add_instruction(Cjmp_ap13); return;
		case 14: add_instruction(Cjmp_ap14); return;
		case 15: add_instruction(Cjmp_ap15); return;
		case 16: add_instruction(Cjmp_ap16); return;
		case 17: add_instruction(Cjmp_ap17); return;
		case 18: add_instruction(Cjmp_ap18); return;
		case 19: add_instruction(Cjmp_ap19); return;
		case 20: add_instruction(Cjmp_ap20); return;
		case 21: add_instruction(Cjmp_ap21); return;
		case 22: add_instruction(Cjmp_ap22); return;
		case 23: add_instruction(Cjmp_ap23); return;
		case 24: add_instruction(Cjmp_ap24); return;
		case 25: add_instruction(Cjmp_ap25); return;
		case 26: add_instruction(Cjmp_ap26); return;
		case 27: add_instruction(Cjmp_ap27); return;
		case 28: add_instruction(Cjmp_ap28); return;
		case 29: add_instruction(Cjmp_ap29); return;
		case 30: add_instruction(Cjmp_ap30); return;
		case 31: add_instruction(Cjmp_ap31); return;
		case 32: add_instruction(Cjmp_ap32); return;
	}
	fprintf(stderr, "Error: jmp_ap %d not yet implemented\n",n_apply_args);
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

void code_jmp_i(int n_apply_args) {
	last_d=0;

	switch (n_apply_args) {
		case 0: add_instruction(Cjmp_i0); break;
		case 1: add_instruction(Cjmp_i1); break;
		case 2: add_instruction(Cjmp_i2); break;
		case 3: add_instruction(Cjmp_i3); break;
		default: add_instruction_w(Cjmp_i,n_apply_args);
	}
}

void code_jmp_true(char label_name[]) {
	add_instruction_label(Cjmp_true,label_name);
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

	lib_function_n = get_specialized_jsr_label_n(label_name);

	if (lib_function_n>=0) {
		add_specialized_jsr_instruction(lib_function_n);
		last_jsr_with_d=0;
		return;
	}

	add_instruction_label(Cjsr,label_name);
}

void code_jsr_ap(int n_apply_args) {
	switch (n_apply_args) {
		case 1:  add_instruction(Cjsr_ap1);  return;
		case 2:  add_instruction(Cjsr_ap2);  return;
		case 3:  add_instruction(Cjsr_ap3);  return;
		case 4:  add_instruction(Cjsr_ap4);  return;
		case 5:  add_instruction(Cjsr_ap5);  return;
		case 6:  add_instruction(Cjsr_ap6);  return;
		case 7:  add_instruction(Cjsr_ap7);  return;
		case 8:  add_instruction(Cjsr_ap8);  return;
		case 9:  add_instruction(Cjsr_ap9);  return;
		case 10: add_instruction(Cjsr_ap10); return;
		case 11: add_instruction(Cjsr_ap11); return;
		case 12: add_instruction(Cjsr_ap12); return;
		case 13: add_instruction(Cjsr_ap13); return;
		case 14: add_instruction(Cjsr_ap14); return;
		case 15: add_instruction(Cjsr_ap15); return;
		case 16: add_instruction(Cjsr_ap16); return;
		case 17: add_instruction(Cjsr_ap17); return;
		case 18: add_instruction(Cjsr_ap18); return;
		case 19: add_instruction(Cjsr_ap19); return;
		case 20: add_instruction(Cjsr_ap20); return;
		case 21: add_instruction(Cjsr_ap21); return;
		case 22: add_instruction(Cjsr_ap22); return;
		case 23: add_instruction(Cjsr_ap23); return;
		case 24: add_instruction(Cjsr_ap24); return;
		case 25: add_instruction(Cjsr_ap25); return;
		case 26: add_instruction(Cjsr_ap26); return;
		case 27: add_instruction(Cjsr_ap27); return;
		case 28: add_instruction(Cjsr_ap28); return;
		case 29: add_instruction(Cjsr_ap29); return;
		case 30: add_instruction(Cjsr_ap30); return;
		case 31: add_instruction(Cjsr_ap31); return;
		case 32: add_instruction(Cjsr_ap32); return;
	}
	fprintf(stderr, "Error: jsr_ap %d not yet implemented\n",n_apply_args);
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

void code_jsr_i(int n_apply_args) {
	if (last_d) {
		last_jsr_with_d=1;
		last_d=0;
	}

	switch (n_apply_args) {
		case 0: add_instruction(Cjsr_i0); break;
		case 1: add_instruction(Cjsr_i1); break;
		case 2: add_instruction(Cjsr_i2); break;
		case 3: add_instruction(Cjsr_i3); break;
		default: add_instruction_w(Cjsr_i,n_apply_args);
	}
}

void code_lnR(void) {
	add_instruction(is_32_bit ? ClnR_32 : ClnR);
}

void code_load_i(CleanInt value) {
	add_instruction_i(Cload_i,value);
}

void code_load_module_name(void) {
	unsupported_instruction_warning(Cload_module_name);
	add_instruction(Cload_module_name);
}

void code_load_si16(CleanInt value) {
	add_instruction_i(Cload_si16,value);
}

void code_load_si32(CleanInt value) {
	add_instruction_i(Cload_si32,value);
}

void code_load_ui8(CleanInt value) {
	add_instruction_i(Cload_ui8,value);
}

void code_log10R(void) {
	add_instruction(is_32_bit ? Clog10R_32 : Clog10R);
}

void code_ltC(void) {
	add_instruction(CltC);
}

void code_ltI(void) {
	add_instruction(CltI);
}

void code_ltR(void) {
	add_instruction(is_32_bit ? CltR_32 : CltR);
}

void code_ltU(void) {
	add_instruction(CltU);
}

void code_mulI(void) {
	add_instruction(CmulI);
}

void code_mulIo(void) {
	add_instruction(CmulIo);
}

void code_mulR(void) {
	add_instruction(is_32_bit ? CmulR_32 : CmulR);
}

void code_mulUUL(void) {
	add_instruction(CmulUUL);
}

void code_negI(void) {
	add_instruction(CnegI);
}

void code_negR(void) {
	add_instruction(is_32_bit ? CnegR_32 : CnegR);
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
	add_instruction(is_32_bit ? CpowR_32 : CpowR);
}

void code_print(char *string,int length) {
	struct label *string_label;

	string_label=new_internal_label();
	string_label->label_offset=(pgrm.data_size<<2)+1;

	add_instruction_internal_label(Cprint,string_label);

	if (list_code)
		printf("\t.data\n");
	store_string(string,length,0);
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
	add_instruction(is_32_bit ? Cprint_real_32 : Cprint_real);
}

void code_print_sc(char *string,int length) {
	struct label *string_label;

	string_label=new_internal_label();
	string_label->label_offset=(pgrm.data_size<<2)+1;

	add_instruction_internal_label(Cprint,string_label);

	if (list_code)
		printf("\t.data\n");
	store_string(string,length,0);
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

void code_pushL(char *label) {
	unsupported_instruction_warning(CpushL);
	add_instruction(CpushL);
}

void code_pushLc(char *label) {
	unsupported_instruction_warning(CpushLc);
	add_instruction(CpushLc);
}

void code_pushR(double r) {
	add_instruction_r(is_32_bit ? CpushR_32 : CpushR,r);
}

void code_pushR_a(int a_offset) {
	add_instruction_w(is_32_bit ? CpushR_a_32 : CpushR_a,-a_offset);
}

void code_pushcaf(char *label_name,int a_size,int b_size) {
	if (a_size==1) {
		if (b_size==0) {
			add_instruction_label(Cpushcaf10,label_name);
			return;
		}
		if (b_size==1) {
			add_instruction_label(Cpushcaf11,label_name);
			return;
		}
	} else if (a_size==2) {
		if (b_size==0) {
			add_instruction_label(Cpushcaf20,label_name);
			return;
		}
	} else if (a_size==3) {
		if (b_size==1) {
			add_instruction_label(Cpushcaf31,label_name);
			return;
		}
	}

	add_instruction_w_w_label(Cpushcaf,a_size,a_size+b_size,label_name);
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

void code_push_finalizers(void) {
	unsupported_instruction_warning(Cpush_finalizers);
	add_instruction(Cpush_finalizers);
}

void code_push_node(char *label_name,int n_arguments) {
	if (!strcmp(label_name,"__")) {
		add_instruction_w(Cpush_node_,n_arguments);
		return;
	}

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
	switch (a_size) {
		case 0: switch (b_size) {
			case 1:  add_instruction_label(Cpush_node_u01,label_name); return;
			case 2:  add_instruction_label(Cpush_node_u02,label_name); return;
			case 3:  add_instruction_label(Cpush_node_u03,label_name); return;
			default: add_instruction_w_label(Cpush_node_u0b,b_size,label_name); return;
		}
		case 1: switch (b_size) {
			case 1:  add_instruction_label(Cpush_node_u11,label_name); return;
			case 2:  add_instruction_label(Cpush_node_u12,label_name); return;
			case 3:  add_instruction_label(Cpush_node_u13,label_name); return;
			default: add_instruction_w_label(Cpush_node_u1b,b_size,label_name); return;
		}
		case 2: switch (b_size) {
			case 1:  add_instruction_label(Cpush_node_u21,label_name); return;
			case 2:  add_instruction_label(Cpush_node_u22,label_name); return;
			default: add_instruction_w_label_w(Cpush_node_u,a_size,label_name,b_size); return;
		}
		default:
			if (a_size==3 && b_size==1)
				add_instruction_label(Cpush_node_u31,label_name);
			else if (a_size>3 && b_size==1)
				add_instruction_w_label(Cpush_node_ua1,a_size,label_name);
			else
				add_instruction_w_label_w(Cpush_node_u,a_size,label_name,b_size);
	}
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
		add_instruction_w_w(Cpush_r_args_aa1,-a_offset,argument_number-2);
		return;
	}

	add_instruction_w_w_w_w(Cpush_r_args_a,-a_offset,a_size+b_size,argument_number,n_arguments);
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
				add_instruction_w_w(Cpush_r_args_b1,-a_offset,a_size+argument_number-1-1);
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
				add_instruction_w_w(Cpush_r_args_b2,-a_offset,a_size+argument_number-1-1);
				return;
			}
		}
	}

	add_instruction_w_w_w(Cpush_r_args_b,-a_offset,a_size+argument_number-1-1,n_arguments);
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
				add_instruction(is_32_bit ? CreplaceREAL_32 : CreplaceREAL);
				return;
			}
	}

	add_instruction_w_w(Creplace_r,a_size,b_size);
	return;
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
	add_instruction_w(Crepl_r_args_aa1,argument_n-2);
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
	switch (a_size) {
		case 0: switch (b_size) {
			case 1:  add_instruction(Crepl_r_args01); return;
			case 2:  add_instruction(Crepl_r_args02); return;
			case 3:  add_instruction(Crepl_r_args03); return;
			case 4:  add_instruction(Crepl_r_args04); return;
			default: add_instruction_w(Crepl_r_args0b,b_size); return;
		}
		case 1: switch (b_size) {
			case 0:  add_instruction(Crepl_r_args10); return;
			case 1:  add_instruction(Crepl_r_args11); return;
			case 2:  add_instruction(Crepl_r_args12); return;
			case 3:  add_instruction(Crepl_r_args13); return;
			case 4:  add_instruction(Crepl_r_args14); return;
			default: add_instruction_w(Crepl_r_args1b,b_size); return;
		}
		case 2: switch (b_size) {
			case 0:  add_instruction(Crepl_r_args20); return;
			case 1:  add_instruction(Crepl_r_args21); return;
			case 2:  add_instruction(Crepl_r_args22); return;
			case 3:  add_instruction(Crepl_r_args23); return;
			case 4:  add_instruction(Crepl_r_args24); return;
			default: add_instruction_w(Crepl_r_args2b,b_size); return;
		}
		case 3: switch (b_size) {
			case 0:  add_instruction(Crepl_r_args30); return;
			case 1:  add_instruction(Crepl_r_args31); return;
			case 2:  add_instruction(Crepl_r_args32); return;
			case 3:  add_instruction(Crepl_r_args33); return;
			case 4:  add_instruction(Crepl_r_args34); return;
			default: add_instruction_w(Crepl_r_args3b,b_size); return;
		}
		default: switch (b_size) {
			case 0:
				if (a_size==4)
					add_instruction(Crepl_r_args40);
				else
					add_instruction_w(Crepl_r_argsa0,a_size-1);
				return;
			case 1: add_instruction_w(Crepl_r_argsa1,a_size-1); return;
			default: add_instruction_w_w(Crepl_r_args,a_size-1,b_size); return;
		}
	}
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
		add_instruction_w(Crepl_r_args_aa1,argument_number-2);
		return;
	}

	add_instruction_w_w_w(Crepl_r_args_a,a_size+b_size,argument_number,n_arguments);
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
				add_instruction(is_32_bit ? CselectREAL_32 : CselectREAL);
				return;
			}
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction(Cselect);
				return;
			}
	}

	switch (a_size) {
		case 0: switch (b_size) {
			case 1:  add_instruction(Cselect_r01); return;
			case 2:  add_instruction(Cselect_r02); return;
			case 3:  add_instruction(Cselect_r03); return;
			case 4:  add_instruction(Cselect_r04); return;
			default: add_instruction_w(Cselect_r0b,b_size); return;
		}
		case 1: switch (b_size) {
			case 0:  add_instruction(Cselect_r10); return;
			case 1:  add_instruction(Cselect_r11); return;
			case 2:  add_instruction(Cselect_r12); return;
			case 3:  add_instruction(Cselect_r13); return;
			case 4:  add_instruction(Cselect_r14); return;
			default: add_instruction_w(Cselect_r1b,b_size); return;
		}
		case 2: switch (b_size) {
			case 0:  add_instruction(Cselect_r20); return;
			case 1:  add_instruction(Cselect_r21); return;
			case 2:  add_instruction(Cselect_r22); return;
			case 3:  add_instruction(Cselect_r23); return;
			case 4:  add_instruction(Cselect_r24); return;
			default: add_instruction_w(Cselect_r2b,b_size); return;
		}
		default: add_instruction_w_w(Cselect_r,a_size,b_size); return;
	}
}

void code_set_finalizers(void) {
	unsupported_instruction_warning(Cset_finalizers);
	add_instruction(Cset_finalizers);
}

void code_shiftlI(void) {
	add_instruction(CshiftlI);
}

void code_shiftrI(void) {
	add_instruction(CshiftrI);
}

void code_shiftrU(void) {
	add_instruction(CshiftrU);
}

void code_sinR(void) {
	add_instruction(is_32_bit ? CsinR_32 : CsinR);
}

void code_subI(void) {
	add_instruction(CsubI);
}

void code_subIo(void) {
	add_instruction(CsubIo);
}

void code_subLU(void) {
	add_instruction(CsubLU);
}

void code_subR(void) {
	add_instruction(is_32_bit ? CsubR_32 : CsubR);
}

void code_sqrtR(void) {
	add_instruction(is_32_bit ? CsqrtR_32 : CsqrtR);
}

void code_tanR(void) {
	add_instruction(is_32_bit ? CtanR_32 : CtanR);
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
				add_instruction(is_32_bit ? CupdateREAL_32 : CupdateREAL);
				return;
			}
		case '_':
			if (element_descriptor[1]=='_' && element_descriptor[2]=='\0') {
				add_instruction(Cupdate);
				return;
			}
	}

	switch (a_size) {
		case 0: switch (b_size) {
			case 1:  add_instruction(Cupdate_r01); return;
			case 2:  add_instruction(Cupdate_r02); return;
			case 3:  add_instruction(Cupdate_r03); return;
			case 4:  add_instruction(Cupdate_r04); return;
			default: add_instruction_w(Cupdate_r0b,b_size); return;
		}
		case 1: switch (b_size) {
			case 0:  add_instruction(Cupdate_r10); return;
			case 1:  add_instruction(Cupdate_r11); return;
			case 2:  add_instruction(Cupdate_r12); return;
			case 3:  add_instruction(Cupdate_r13); return;
			case 4:  add_instruction(Cupdate_r14); return;
			default: add_instruction_w(Cupdate_r1b,b_size); return;
		}
		case 2: switch (b_size) {
			case 0:  add_instruction(Cupdate_r20); return;
			case 1:  add_instruction(Cupdate_r21); return;
			case 2:  add_instruction(Cupdate_r22); return;
			case 3:  add_instruction(Cupdate_r23); return;
			case 4:  add_instruction(Cupdate_r24); return;
			default: add_instruction_w(Cupdate_r2b,b_size); return;
		}
		case 3: switch (b_size) {
			case 0:  add_instruction(Cupdate_r30); return;
			case 1:  add_instruction(Cupdate_r31); return;
			case 2:  add_instruction(Cupdate_r32); return;
			case 3:  add_instruction(Cupdate_r33); return;
			case 4:  add_instruction(Cupdate_r34); return;
			default: add_instruction_w(Cupdate_r3b,b_size); return;
		}
		default: add_instruction_w_w(Cupdate_r,a_size,b_size); return;
	}
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
	add_instruction(is_32_bit ? CItoR_32 : CItoR);
}

void code_RtoI(void) {
	add_instruction(is_32_bit ? CRtoI_32 : CRtoI);
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

	lib_function_n = get_specialized_jsr_label_n(label_name);

	if (lib_function_n>=0) {
		code_buildh0_put_a(descriptor_name,a_offset);
		add_specialized_jsr_instruction(lib_function_n);
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

void code_ccall (char *c_function_name,char *type,int type_length) {
#ifndef LINK_CLEAN_RUNTIME
	used_ccall_warning (c_function_name);
#endif

	struct label *function_label;
	function_label=new_internal_label();
	function_label->label_offset=(pgrm.data_size<<2)+1;
	store_string(c_function_name,strlen(c_function_name),0);

	struct label *type_label;
	type_label=new_internal_label();
	type_label->label_offset=(pgrm.data_size<<2)+1;
	for (int i=0; i<type_length; i++) {
		switch (type[i]) {
			case '-': type[i]=':'; break;
			case ':': break;
			case 'p': type[i]='I'; break;
			case 'I': break;
			default:
				//fprintf(stderr,"Warning: '%c' type in ccall for %s not supported by interpreter\n",type[i],c_function_name);
				break;
		}
	}
	store_string(type,type_length,0);

	add_instruction_internal_label_internal_label(Cccall,function_label,type_label);
}

void code_centry (char *c_function_name,char *clean_function_label,char *s,int length) {
	unsupported_instruction_warning(Ccentry);
	add_instruction(Ccentry);
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
	store_string(string,string_length,0);
	if (list_code)
		printf("\t.text\n");
}

void code_jmp_eqC_b(int value,int b_offset,char label_name[]) {
	add_instruction_w_c_label(Cjmp_eqC_b,b_offset,value,label_name);
}

void code_jmp_eqC_b2(int value1,int value2,int b_offset,char label_name1[],char label_name2[]) {
	add_instruction_w_c_label_c_label(Cjmp_eqC_b2,b_offset,value1,label_name1,value2,label_name2);
}

void code_jmp_eqCc(int value,char label_name[]) {
	add_instruction_c_label(Cjmp_eqCc,value,label_name);
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

void code_jmp_eqI_b(CleanInt value,int b_offset,char label_name[]) {
	add_instruction_w_i_label(Cjmp_eqI_b,b_offset,value,label_name);
}

void code_jmp_eqI_b2(CleanInt value1,CleanInt value2,int b_offset,char label_name1[],char label_name2[]) {
	add_instruction_w_i_label_i_label(Cjmp_eqI_b2,b_offset,value1,label_name1,value2,label_name2);
}

void code_jmp_eqIi(CleanInt value,char label_name[]) {
	add_instruction_i_label(Cjmp_eqIi,value,label_name);
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

void code_jmp_neCc(int value,char label_name[]) {
	add_instruction_c_label(Cjmp_neCc,value,label_name);
}

void code_jmp_neI(char label_name[]) {
	add_instruction_label(Cjmp_neI,label_name);
}

void code_jmp_neI_b(CleanInt value,int b_offset,char label_name[]) {
	add_instruction_w_i_label(Cjmp_neI_b,b_offset,value,label_name);
}

void code_jmp_neIi(CleanInt value,char label_name[]) {
	add_instruction_i_label(Cjmp_neIi,value,label_name);
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

	lib_function_n = get_specialized_jsr_label_n(label_name);

	if (lib_function_n>=0) {
		code_pop_a(n);
		add_specialized_jsr_instruction(lib_function_n);
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

	lib_function_n = get_specialized_jsr_label_n(label_name);

	if (lib_function_n>=0) {
		code_pop_b(n);
		add_specialized_jsr_instruction(lib_function_n);
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

	lib_function_n = get_specialized_jsr_label_n(label_name);

	if (lib_function_n>=0) {
		code_push_a(a_offset);
		add_specialized_jsr_instruction(lib_function_n);
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

void code_push_b_decI(int b_offset) {
	add_instruction_w(Cpush_b_decI,b_offset);
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

	lib_function_n = get_specialized_jsr_label_n(label_name);

	if (lib_function_n>=0) {
		code_push_b(b_offset);
		add_specialized_jsr_instruction(lib_function_n);
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

void code_put_a_jmp(int a_offset, char label_name[]) {
	last_d=0;

	add_instruction_w_label(Cput_a_jmp,-a_offset,label_name);
}

void code_put_b(int b_offset) {
	add_instruction_w(Cput_b,b_offset);
}

void code_put_b_jmp(int b_offset, char label_name[]) {
	last_d=0;

	add_instruction_w_label(Cput_b_jmp,b_offset,label_name);
}

void code_selectoo(char element_descriptor[],int a_size,int b_size,int a_offset,int b_offset) {
	switch(element_descriptor[0]) {
		case 'B':
			if (element_descriptor[1]=='O' && element_descriptor[2]=='O' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0') {
				add_instruction_w_w(CselectBOOLoo,-a_offset,b_offset);
				return;
			}
			break;
		case 'C':
			if (element_descriptor[1]=='H' && element_descriptor[2]=='A' && element_descriptor[3]=='R' &&
				element_descriptor[4]=='\0') {
				add_instruction_w_w(CselectCHARoo,-a_offset,b_offset);
				return;
			}
			break;
		case 'I':
			if (element_descriptor[1]=='N' && element_descriptor[2]=='T' && element_descriptor[3]=='\0') {
				add_instruction_w_w(CselectINToo,-a_offset,b_offset);
				return;
			}
			break;
		case 'R':
			if (element_descriptor[1]=='E' && element_descriptor[2]=='A' && element_descriptor[3]=='L' &&
				element_descriptor[4]=='\0') {
				add_instruction_w_w(is_32_bit ? CselectREALoo_32 : CselectREALoo,-a_offset,b_offset);
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
	} else if (n_apply_args<=32) {
		add_instruction(CA_data_IlI);
		add_instruction_label(Cadd_empty_node3+(n_apply_args-3),ea_label_name);
		add_instruction(Chalt);
	} else {
		if (ea_label_name!=NULL)
			fprintf(stderr, "Error: .a %d %s\n",n_apply_args,ea_label_name);
		else
			fprintf(stderr, "Error: .a %d\n",n_apply_args);
	}
}

void code_ai(int n_apply_args,char *ea_label_name,char *instance_member_code_name) {
	if (n_apply_args==0) {
		add_instruction_label(CA_data_lIlI,instance_member_code_name);
		add_instruction_label(Cjmp,ea_label_name);
		add_instruction(Chalt);
	} else if (n_apply_args<=32) {
		add_instruction_label(CA_data_lIlI,instance_member_code_name);
		add_instruction_label(Cadd_empty_node3+(n_apply_args-3),ea_label_name);
		add_instruction(Chalt);
	} else {
		if (ea_label_name!=NULL)
			fprintf(stderr, "Error: .ai %d %s %s\n",n_apply_args,ea_label_name,instance_member_code_name);
		else
			fprintf(stderr, "Error: .ai %d %s\n",n_apply_args,instance_member_code_name);
	}
}

void code_algtype(int n_constructors) {
}

void code_caf(char *label_name,int a_size,int b_size) {
	struct label *label;
	int s=a_size+b_size+1;

	if (list_code) {
		printf("\t.data\n");
		printf("%s:\n",label_name);
	}

	label=enter_label(label_name);

	if (list_code)
		printf("%d\t.data4 %d\n",pgrm.data_size<<2,s);
	store_data_l(s);

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
	if (strlen (options)<11 || options[10]=='0')
		is_32_bit=1;
}

void code_d(int da,int db,uint32_t vector[]) {
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
	store_data_l(-1);

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
			struct label *label;
			if (n>N_ADD_ARG_LABELS) {
				general_add_arg_label_used=1;
				label=enter_label("_add_arg");
			} else {
				Fadd_arg_label_used[n]=1;
				char label_name[19];
				sprintf(label_name,"_add_arg%d",n);
				label = enter_label(label_name);
			}
			if (list_code)
				printf("%d\t.data4 %s\n",pgrm.data_size<<2,label->label_name);
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
	store_string(descriptor_name,descriptor_name_length,0);

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

	if (list_code)
		printf("%d\t.data4 %d\n",pgrm.data_size<<2,desc0_number);
	store_data_l(desc0_number);

	/* Resolve descriptor address */
	store_data_l(-1);

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
	store_string(descriptor_name,descriptor_name_length,0);

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
	store_string(descriptor_name,descriptor_name_length,0);

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
	store_string(descriptor_name,descriptor_name_length,0);

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

	store_string(string,string_length,0);

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

			if (number_of_arguments>=0 && number_of_arguments<=32) {
				add_instruction_label(Ceval_upd0+number_of_arguments,ea_label_name);
				add_instruction(Chalt);
			} else if (number_of_arguments<0) { /* selectors and indirections */
				add_instruction_label(Cjmp,ea_label_name);
				add_instruction(Chalt);
			} else {
				fprintf(stderr, "Warning: .n %d %s is not implemented\n",number_of_arguments,ea_label_name);
				add_instruction(Chalt);
				add_label(ea_label_name);
				add_instruction(Chalt);
			}
		}

		if (descriptor_name==NULL)
			add_data2_to_code(0);
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
			add_data2_to_code(0);
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

void code_o(int oa,int ob,uint32_t vector[]) {
	while (last_jsr_with_d) {
		int i=pgrm.code[pgrm.code_size-2].value;
		if (i==Cjsr || i==Cjsr_i)
			break;

		i=pgrm.code[pgrm.code_size-1].value;
		if ((Cjsr_ap1<=i && i<=Cjsr_ap32) ||
				(Cjsr_i0<=i && i<=Cjsr_i3))
			break;

		i=pgrm.code[pgrm.code_size-3].value;
		if (i==Cpop_a_jsr || i==Cpop_b_jsr || i==Cpush_a_jsr || i==Cpush_b_jsr)
			break;

		if (pgrm.code[pgrm.code_size-4].value==Cbuildh0_put_a_jsr)
			break;

		fprintf(stderr, "Error: .o directive used incorrectly near jsr\n");
		exit(1);
	}

	last_jsr_with_d=0;
}
#endif

void code_start(char *label_name) {
	if (strcmp ("__nostart__",label_name)==0)
		return;

	if (start_label!=NULL)
		EPRINTF("Warning: overriding start label from '%s' to '%s'\n",start_label->label_name,label_name);

	start_label=enter_label(label_name);
}

#ifndef LINK_CLEAN_RUNTIME
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

	store_string(type,strlen(type),1);

	store_string(record_name,record_name_length,0);

	if (list_code)
		printf("\t.text\n");
}

void code_record_start(char record_label_name[],char type[],int a_size,int b_size) {
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

	store_string(type,strlen(type),1);
}

void code_record_descriptor_label(char descriptor_name[]) {
	store_data_label_value(descriptor_name,0);
}

void code_record_end(char record_name[],int record_name_length) {
	store_string(record_name,record_name_length,0);
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

	store_string(string,string_length,0);

	if (list_code)
		printf("\t.text\n");
}

void code_dummy(void) {
}
#endif

static void count_and_renumber_labels(struct label_node *node, int *start, int *end) {
	if (node->label_node_left != NULL)
		count_and_renumber_labels(node->label_node_left, start, end);

	struct label *label = node->label_node_label_p;
	if (label->label_offset < 0 || label->label_module_n < 0) {
		label->label_id=*start;
		*start=*start+1;
		if (label->label_offset < 0) {
			global_label_count++;
			global_label_string_count+=strlen(label->label_name);
		}
	} else {
		label->label_id=*end;
		*end=*end-1;
	}

	if (node->label_node_right != NULL)
		count_and_renumber_labels(node->label_node_right, start, end);
}

#ifndef LINK_CLEAN_RUNTIME
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

static void print_global_labels(struct label_node *node, FILE *program_file) {
	if (node->label_node_left != NULL)
		print_global_labels(node->label_node_left, program_file);

	struct label *label = node->label_node_label_p;
	if (label->label_module_n == -1 || label->label_offset < 0) {
		fwrite(&label->label_offset, sizeof(label->label_offset), 1, program_file);
		fprintf(program_file, "%s", label->label_name);
		fputc('\0', program_file);
	}

	if (node->label_node_right != NULL)
		print_global_labels(node->label_node_right, program_file);
}

static void print_local_labels(struct label_node *node, FILE *program_file) {
	if (node->label_node_right != NULL)
		print_local_labels(node->label_node_right, program_file);

	struct label *label = node->label_node_label_p;
	if (!(label->label_module_n == - 1 || label->label_offset < 0)) {
		fwrite(&label->label_offset, sizeof(label->label_offset), 1, program_file);
		fputc('\0', program_file);
	}

	if (node->label_node_left != NULL)
		print_local_labels(node->label_node_left, program_file);
}

void write_program(FILE *program_file) {
	uint32_t temp;
	temp=ABC_MAGIC_NUMBER;
	fwrite(&temp, sizeof(temp), 1, program_file);
	temp=10*4;
	fwrite(&temp, sizeof(temp), 1, program_file);
	temp=ABC_VERSION;
	fwrite(&temp, sizeof(temp), 1, program_file);
	fwrite(&pgrm.code_size, sizeof(pgrm.code_size), 1, program_file);
	fwrite(&pgrm.words_in_strings, sizeof(pgrm.words_in_strings), 1, program_file);
	fwrite(&pgrm.strings_size, sizeof(pgrm.strings_size), 1, program_file);
	fwrite(&pgrm.data_size, sizeof(pgrm.data_size), 1, program_file);

	show_used_ccalls_warning();
	show_requires_file_io_warning();

	int start=0, end=label_id-1;
	count_and_renumber_labels(labels, &start, &end);
	fwrite(&label_id, sizeof(label_id), 1, program_file);
	fwrite(&global_label_string_count, sizeof(global_label_string_count), 1, program_file);

	fwrite(&pgrm.code_reloc_size, sizeof(pgrm.code_reloc_size), 1, program_file);
	fwrite(&pgrm.data_reloc_size, sizeof(pgrm.code_reloc_size), 1, program_file);

	if (start_label!=NULL) {
		fwrite(&start_label->label_id, sizeof(uint32_t), 1, program_file);
	} else {
		temp=-1;
		fwrite(&temp, sizeof(temp), 1, program_file);
	}

	print_code(pgrm.code_size,pgrm.code,program_file);
	print_strings(pgrm.strings_size,pgrm.strings,program_file);
	print_data(pgrm.data_size,pgrm.data,program_file);

	print_global_labels(labels, program_file);
	print_local_labels(labels, program_file);

	print_relocations(pgrm.code_reloc_size,pgrm.code_relocations,program_file);
	print_relocations(pgrm.data_reloc_size,pgrm.data_relocations,program_file);

	if (fclose(program_file)) {
		fprintf(stderr, "Error writing program file\n");
		exit(1);
	}
}
#endif

static char *print_global_labels_to_string(struct label_node *node, char *ptr) {
	if (node->label_node_left != NULL)
		ptr=print_global_labels_to_string(node->label_node_left, ptr);

	struct label *label = node->label_node_label_p;
	if (label->label_module_n == - 1 || label->label_offset < 0) {
		memcpy(ptr, &label->label_offset, sizeof(label->label_offset));
		ptr+=sizeof(label->label_offset);
		for (char *name_ptr=label->label_name; *name_ptr; name_ptr++)
			*ptr++=*name_ptr;
		*ptr++='\0';
	}

	if (node->label_node_right != NULL)
		ptr=print_global_labels_to_string(node->label_node_right, ptr);

	return ptr;
}

static char *print_local_labels_to_string(struct label_node *node, char *ptr) {
	if (node->label_node_right != NULL)
		ptr=print_local_labels_to_string(node->label_node_right, ptr);

	struct label *label = node->label_node_label_p;
	if (!(label->label_module_n == - 1 || label->label_offset < 0)) {
		memcpy(ptr, &label->label_offset, sizeof(label->label_offset));
		ptr+=sizeof(label->label_offset);
		*ptr++='\0';
	}

	if (node->label_node_left != NULL)
		ptr=print_local_labels_to_string(node->label_node_left, ptr);

	return ptr;
}

char *write_program_to_string(uint32_t *bytes_needed) {
	int start=0, end=label_id-1;

	count_and_renumber_labels(labels, &start, &end);

	*bytes_needed =
			12*sizeof(uint32_t) +
			pgrm.code_byte_size +
			sizeof(uint32_t)*pgrm.strings_size +
			sizeof(uint64_t)*pgrm.data_size +
			(sizeof(uint32_t)+1)*label_id +
			global_label_string_count +
			sizeof(uint32_t)*2*(pgrm.code_reloc_size+pgrm.data_reloc_size);
	char *bytecode=safe_malloc(*bytes_needed);

	((uint32_t*)bytecode)[ 0]=ABC_MAGIC_NUMBER;
	((uint32_t*)bytecode)[ 1]=10*4;
	((uint32_t*)bytecode)[ 2]=ABC_VERSION;
	((uint32_t*)bytecode)[ 3]=pgrm.code_size;
	((uint32_t*)bytecode)[ 4]=pgrm.words_in_strings;
	((uint32_t*)bytecode)[ 5]=pgrm.strings_size;
	((uint32_t*)bytecode)[ 6]=pgrm.data_size;
	((uint32_t*)bytecode)[ 7]=label_id;
	((uint32_t*)bytecode)[ 8]=global_label_string_count;
	((uint32_t*)bytecode)[ 9]=pgrm.code_reloc_size;
	((uint32_t*)bytecode)[10]=pgrm.data_reloc_size;
	((uint32_t*)bytecode)[11]=start_label==NULL ? -1 : start_label->label_id;

	char *ptr=(char*)&((uint32_t*)bytecode)[12];
	for (int i=0; i<pgrm.code_size; i++) {
		memcpy(ptr, &pgrm.code[i].value, pgrm.code[i].width);
		ptr+=pgrm.code[i].width;
	}

	for (int i=0; i<pgrm.strings_size; i++) {
		memcpy(ptr, &pgrm.strings[i], sizeof(uint32_t));
		ptr+=sizeof(uint32_t);
	}

	for (int i=0; i<pgrm.data_size; i++) {
		memcpy(ptr, &pgrm.data[i], sizeof(uint64_t));
		ptr+=sizeof(uint64_t);
	}

	ptr=print_global_labels_to_string(labels, ptr);
	ptr=print_local_labels_to_string(labels, ptr);

	for (int i=0; i<pgrm.code_reloc_size; i++) {
		memcpy(ptr, &pgrm.code_relocations[i].relocation_offset, sizeof(uint32_t));
		memcpy(ptr+4, &pgrm.code_relocations[i].relocation_label->label_id, sizeof(uint32_t));
		ptr+=8;
	}

	for (int i=0; i<pgrm.data_reloc_size; i++) {
		memcpy(ptr, &pgrm.data_relocations[i].relocation_offset, sizeof(uint32_t));
		memcpy(ptr+4, &pgrm.data_relocations[i].relocation_label->label_id, sizeof(uint32_t));
		ptr+=8;
	}

	return bytecode;
}

void free_label_node(struct label_node *node) {
	free(node->label_node_label_p->label_name);
	free(node->label_node_label_p);
	if (node->label_node_left!=NULL)
		free_label_node(node->label_node_left);
	if (node->label_node_right!=NULL)
		free_label_node(node->label_node_right);
	free(node);
}

void free_generated_program(void) {
	free_program(&pgrm);
	if (labels!=NULL)
		free_label_node(labels);
}
