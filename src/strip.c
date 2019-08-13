#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "bcgen_instructions.h"
#include "bytecode.h"
#include "strip.h"
#include "util.h"

enum data_label_type {
	DLT_NORMAL,
	DLT_STRING,
	DLT_STRING_WITH_DESCRIPTOR,
	DLT_CAF
};

struct s_label { /* source label */
	int32_t offset;
	char *name;
	struct label *bcgen_label;
	enum data_label_type label_type;
};

struct s_relocation { /* source relocation */
	int32_t relocation_offset;
	uint32_t relocation_label;
	struct s_label *relocation_belongs_to_label;
};

struct label_queue { /* source labels that are to be activated */
	uint32_t writer;
	uint32_t reader;
	uint32_t size;
	struct s_label *labels[0];
};

struct incoming_labels {
	uint32_t label_id;
	struct incoming_labels *next;
};

struct code_index { /* auxiliary information about code indices */
	int32_t byte_index; /* index in width-optimised bytecode */
	struct incoming_labels *incoming_labels;
};

static struct program *pgrm;

static uint8_t *code;
static struct code_index *code_indices;
static uint64_t *data;

static uint32_t n_labels;
static uint32_t n_global_labels;
static struct s_label *labels;

static uint32_t code_reloc_size;
static struct s_relocation *code_relocations;
static uint32_t data_reloc_size;
static struct s_relocation *data_relocations;

struct label_queue *queue;

static int export_all_labels;
static int include_symbol_table=0;
static int export_label(const char *label) {
	if (export_all_labels)
		return 1;
	else if (!strcmp(label, "__ARRAY__"))
		return 1;
	else if (!strcmp(label, "__STRING__"))
		return 1;
	else if (!strcmp(label, "INT") || !strcmp(label, "dINT"))
		return 1;
	else if (!strcmp(label, "BOOL"))
		return 1;
	else if (!strcmp(label, "CHAR"))
		return 1;
	else if (!strcmp(label, "REAL"))
		return 1;
	else if (!include_symbol_table)
		return 0;
	else if (!label[0])
		return 0;
	else if (!strncmp(label, "_internal", 9))
		return 0;
	else
		return 1;
}

static void init_label_queue(void) {
	queue=safe_malloc(sizeof(struct label_queue)+2*sizeof(char*));
	queue->writer=0;
	queue->reader=0;
	queue->size=2;
}

static void add_label_to_queue(struct s_label *label) {
	if (label->bcgen_label!=NULL)
		return;

	if (label->name[0]!='\0') {
		label->bcgen_label=enter_label(label->name);
		if (export_label(label->name))
			make_label_global(label->bcgen_label);
	} else
		label->bcgen_label=new_internal_label();

	queue->labels[queue->writer++]=label;
	queue->writer%=queue->size;

	if (queue->writer==queue->reader) {
		queue->writer=queue->size;
		queue->size*=2;
		queue->reader=0;
		queue=safe_realloc(queue, sizeof(struct label_queue)+queue->size*sizeof(struct s_label*));
	}
}

static struct s_label *next_label_from_queue(void) {
	if (queue->reader==queue->writer)
		return NULL;
	struct s_label *label=queue->labels[queue->reader];
	queue->reader++;
	queue->reader%=queue->size;
	return label;
}

static inline int instruction_ends_block(int16_t instr) {
	switch (instr) {
		case Cjmp:
		case Cjmp_ap1:
		case Cjmp_ap2:
		case Cjmp_ap3:
		case Cjmp_ap4:
		case Cjmp_ap5:
		case Cjmp_ap6:
		case Cjmp_ap7:
		case Cjmp_ap8:
		case Cjmp_ap9:
		case Cjmp_ap10:
		case Cjmp_ap11:
		case Cjmp_ap12:
		case Cjmp_ap13:
		case Cjmp_ap14:
		case Cjmp_ap15:
		case Cjmp_ap16:
		case Cjmp_ap17:
		case Cjmp_ap18:
		case Cjmp_ap19:
		case Cjmp_ap20:
		case Cjmp_ap21:
		case Cjmp_ap22:
		case Cjmp_ap23:
		case Cjmp_ap24:
		case Cjmp_ap25:
		case Cjmp_ap26:
		case Cjmp_ap27:
		case Cjmp_ap28:
		case Cjmp_ap29:
		case Cjmp_ap30:
		case Cjmp_ap31:
		case Cjmp_ap32:
		case Cjmp_eval:
		case Cjmp_eval_upd:
		case Cpop_a_jmp:
		case Cpop_b_jmp:

		case Cfill_a01_pop_rtn:
		case Cpop_a_rtn:
		case Cpop_ab_rtn:
		case Cpop_b_rtn:
		case Crtn:

		case Cadd_arg0:
		case Cadd_arg1:
		case Cadd_arg2:
		case Cadd_arg3:
		case Cadd_arg4:
		case Cadd_arg5:
		case Cadd_arg6:
		case Cadd_arg7:
		case Cadd_arg8:
		case Cadd_arg9:
		case Cadd_arg10:
		case Cadd_arg11:
		case Cadd_arg12:
		case Cadd_arg13:
		case Cadd_arg14:
		case Cadd_arg15:
		case Cadd_arg16:
		case Cadd_arg17:
		case Cadd_arg18:
		case Cadd_arg19:
		case Cadd_arg20:
		case Cadd_arg21:
		case Cadd_arg22:
		case Cadd_arg23:
		case Cadd_arg24:
		case Cadd_arg25:
		case Cadd_arg26:
		case Cadd_arg27:
		case Cadd_arg28:
		case Cadd_arg29:
		case Cadd_arg30:
		case Cadd_arg31:

		case Chalt:

			return 1;

		default:
			return 0;
	}
}

static struct s_relocation *find_relocation_by_offset(struct s_relocation *relocs, uint32_t n_relocs, uint32_t offset) {
	int l=0;
	int r=n_relocs-1;
	while (l<=r) {
		int i=(l+r)/2;
		if (relocs[i].relocation_offset<offset)
			l=i+1;
		else if (relocs[i].relocation_offset>offset)
			r=i-1;
		else
			return &relocs[i];
	}
	return NULL;
}

static void activate_label(struct s_label *label) {
	struct s_relocation *reloc;
	if (label->bcgen_label->label_offset!=-1)
		return;

	if (label->offset==-1) {
		return;
	} else if (label->offset & 1) { /* data */
		uint64_t *block=&data[(label->offset-1)>>2];
		uint32_t arity=block[0];

		switch (label->label_type) {
			case DLT_NORMAL:
				if ((arity & 0xffff) > 256) { /* record */
					uint64_t *type_string=&block[2];

					store_data_l(block[-2]);
					store_data_l(block[-1]);
					label->bcgen_label->label_offset=(pgrm->data_size<<2)+1;
					store_data_l(block[0]);

					store_string((char*)type_string,type_string[-1]-1,1);

					int n_desc_labels=0;
					for (char *ts=(char*)type_string; *ts; ts++)
						if (*ts=='{')
							n_desc_labels++;
					uint64_t *desc_labels=&block[2+(type_string[-1]+sizeof(uint64_t)-1)/sizeof(uint64_t)];
					for (; n_desc_labels; n_desc_labels--) {
						reloc=find_relocation_by_offset(data_relocations, data_reloc_size, desc_labels-data);
						add_label_to_queue(&labels[reloc->relocation_label]);
						add_data_relocation(labels[reloc->relocation_label].bcgen_label, pgrm->data_size);
						store_data_l(*desc_labels++);
					}

					uint64_t *name_string=desc_labels+1;
					store_string((char*)name_string,name_string[-1],0);
				} else if ((arity & 0xffff) > 0 && (arity>>16)==0) { /* string */
					label->bcgen_label->label_offset=(pgrm->data_size<<2)+1;
					store_string((char*)&block[1], block[0], 0);
				} else { /* no record */
					arity>>=3+16;

					if (arity==0) { /* desc0, descn or string */
						if (block[0]==0 || block[0]==1 || (block[0]>>16)>0) { /* desc0 or descn */
							store_data_l(block[-3]);
							store_data_l(block[-2]);
							reloc=find_relocation_by_offset(data_relocations, data_reloc_size, &block[-1]-data);
							add_label_to_queue(&labels[reloc->relocation_label]);
							add_data_relocation(labels[reloc->relocation_label].bcgen_label, pgrm->data_size);
							store_data_l(block[-1]);
							label->bcgen_label->label_offset=(pgrm->data_size<<2)+1;
							store_data_l(block[0]);
							store_data_l(block[1]);
							if (block[2]!=0) { /* descn */
								store_string((char*)&block[3], block[2], 0);
							} else { /* desc0 */
								store_data_l(block[2]);
								store_string((char*)&block[4], block[3], 0);
							}
						} else { /* string */
							store_string((char*)&block[1], block[0], 0);
						}
					} else { /* descs or normal descriptor */
						store_data_l(block[-2]);
						reloc=find_relocation_by_offset(data_relocations, data_reloc_size, &block[-1]-data);
						add_label_to_queue(&labels[reloc->relocation_label]);
						add_data_relocation(labels[reloc->relocation_label].bcgen_label, pgrm->data_size);
						store_data_l(block[-1]);
						label->bcgen_label->label_offset=(pgrm->data_size<<2)+1;

						int include_last_words=1;

						if (arity==1 && (block[1]>>2) > 0) { /* descs */
							store_data_l(block[0]);
							store_data_l(block[1]);
						} else {
							for (int i=0; i<arity; i++) {
								store_data_l(block[i*2]);
								reloc=find_relocation_by_offset(data_relocations, data_reloc_size, &block[i*2+1]-data);
								if (reloc==NULL && i==0) { /* descn */
									store_data_l(block[1]);
									store_string((char*)&block[3], block[2], 0);
									include_last_words=0;
									break;
								}
								add_label_to_queue(&labels[reloc->relocation_label]);
								add_data_relocation(labels[reloc->relocation_label].bcgen_label, pgrm->data_size);
								store_data_l(block[i*2+1]);
							}
						}

						if (include_last_words) {
							store_data_l(block[2*arity]);
							store_data_l(block[2*arity+1]);
							store_data_l(block[2*arity+2]);
							store_string((char*)&block[2*arity+4],block[2*arity+3],0);
						}
					}
				}
				break;
			case DLT_STRING:
				label->bcgen_label->label_offset=(pgrm->data_size<<2)+1;
				store_string((char*)&block[1], block[0], 0);
				break;
			case DLT_STRING_WITH_DESCRIPTOR:
				label->bcgen_label->label_offset=(pgrm->data_size<<2)+1;
				reloc=find_relocation_by_offset(data_relocations, data_reloc_size, block-data);
				add_label_to_queue(&labels[reloc->relocation_label]);
				add_data_relocation(labels[reloc->relocation_label].bcgen_label, pgrm->data_size);
				store_data_l(block[0]);
				store_string((char*)&block[2], block[1], 0);
				break;
			case DLT_CAF:
				{
					int s=block[-1];
					if (s==0) {
						s=block[-2];
						store_data_l(block[-2]);
					}
					store_data_l(block[-1]);
					label->bcgen_label->label_offset=(pgrm->data_size<<2)+1;
					for (; s; s--)
						store_data_l(0);
				}
				break;
		}
	} else { /* code */
		int ci=label->offset>>2;

		/* since the label may point to the middle of a block, first find the beginning */
		do {
			do { ci--; } while (ci>=0 && code_indices[ci].byte_index==-1);
		} while (ci>0 && !instruction_ends_block(*(int16_t*)&code[code_indices[ci].byte_index]));
		if (ci>0)
			do { ci++; } while (code_indices[ci].byte_index==-1);
		else
			ci=0;

		uint8_t *code_block=&code[code_indices[ci].byte_index];

		while (1) {
			for (struct incoming_labels *ilabs=code_indices[ci].incoming_labels; ilabs!=NULL; ilabs=ilabs->next) {
				struct s_label *lab=&labels[ilabs->label_id];
				if (lab->bcgen_label==NULL) {
					if (lab->name[0] != '\0') {
						lab->bcgen_label=enter_label(lab->name);
						if (export_label(lab->name))
							make_label_global(lab->bcgen_label);
					} else
						lab->bcgen_label=new_label_at_offset(pgrm->code_size<<2);
				} else if (lab->bcgen_label->label_offset!=-1 && lab->bcgen_label->label_offset!=(pgrm->code_size<<2)) {
					EPRINTF("Error: overwriting label '%s'\n",lab->bcgen_label->label_name);
					EXIT(NULL,1);
				}
				lab->bcgen_label->label_offset=pgrm->code_size<<2;
			}

			int16_t instr=*(int16_t*)code_block;
			store_code_elem(2, instr);
			code_block+=2;
			const char *type=instruction_type(instr);
			ci++;
			for (; *type; type++) {
				switch (*type) {
					case 'c': /* Char */
						store_code_elem(1, *(uint8_t*)code_block);
						code_block+=1;
						break;
					case 'I': /* Instruction */
					case 'n': /* Stack index */
					case 'N': /* Stack index, optimised to byte width */
					case 'a': /* Arity */
						store_code_elem(2, *(uint16_t*)code_block);
						code_block+=2;
						break;
					case 'l': /* Label */
						reloc=find_relocation_by_offset(code_relocations, code_reloc_size, ci);
						/* Only labels in .n/.nu directives (i.e., CA_data_*)
						 * may be NULL; otherwise, throw an error. */
						if (instr<CA_data_IIIla && reloc==NULL) {
							EPRINTF("Error: label for %s was NULL\n",instruction_name(instr));
							EXIT(NULL,1);
						}
						if (reloc!=NULL) {
							add_label_to_queue(&labels[reloc->relocation_label]);
							add_code_relocation(labels[reloc->relocation_label].bcgen_label, pgrm->code_size);
						}
						store_code_elem(4, *(uint32_t*)code_block);
						code_block+=4;
						break;
					case 'C': /* CAF label */
						reloc=find_relocation_by_offset(code_relocations, code_reloc_size, ci);
						add_label_to_queue(&labels[reloc->relocation_label]);
						labels[reloc->relocation_label].label_type=DLT_CAF;
						add_code_relocation(labels[reloc->relocation_label].bcgen_label, pgrm->code_size);
						store_code_elem(4, *(uint32_t*)code_block);
						code_block+=4;
						break;
					case 'S': /* String with __STRING__ label */
					case 's': /* Plain string label */
						reloc=find_relocation_by_offset(code_relocations, code_reloc_size, ci);
						add_label_to_queue(&labels[reloc->relocation_label]);
						labels[reloc->relocation_label].label_type=*type=='S' ? DLT_STRING_WITH_DESCRIPTOR : DLT_STRING;
						add_code_relocation(labels[reloc->relocation_label].bcgen_label, pgrm->code_size);
						store_code_elem(4, *(uint32_t*)code_block);
						code_block+=4;
						break;
					case 'r': /* Real */
					case 'i': /* Int */
						store_code_elem(8, *(uint64_t*)code_block);
						code_block+=8;
						break;
					default:
						EPRINTF("error in activate_label\n");
						EXIT(NULL,-1);
				}
				ci++;
			}

			if (instruction_ends_block(instr))
				break;
		}
	}
}

static struct s_label *find_label_by_name(const char *name) {
	int l=0;
	int r=n_global_labels-1;
	while (l<=r) {
		int i=(l+r)/2;
		struct s_label *label=&labels[i];
		int c=strcmp(label->name, name);
		if (c<0)
			l=i+1;
		else if (c>0)
			r=i-1;
		else
			return label;
	}
	EPRINTF("error in find_label_by_name (%s)\n",name);
	EXIT(NULL,1);
	return NULL;
}

void prepare_strip_bytecode(uint32_t *bytecode,
		int _include_symbol_table, int activate_start_label) {
	include_symbol_table=_include_symbol_table;
	export_all_labels=0;

	if (bytecode[0]!=ABC_MAGIC_NUMBER) {
		EPRINTF("file to strip does not start with right magic number\n");
		EXIT(NULL,-1);
	}
	code=&((uint8_t*)bytecode)[bytecode[1]+8];
	if (bytecode[2]!=ABC_VERSION) {
		EPRINTF("file to strip ABC* version does not match\n");
		EXIT(NULL,-1);
	}

	uint32_t code_size=bytecode[3];
	/*uint32_t words_in_strings=bytecode[4];*/
	uint32_t strings_size=bytecode[5];
	uint32_t data_size=bytecode[6];
	n_labels=bytecode[7];
	/*uint32_t global_label_string_count=bytecode[8];*/
	code_reloc_size=bytecode[9];
	data_reloc_size=bytecode[10];
	uint32_t start_label_id=bytecode[11];

	code_indices=safe_malloc(sizeof(struct code_index)*code_size);

	int ci=0;
	int i=0;
	while (ci<code_size) {
		uint16_t instr=*(uint16_t*)&code[i];
		code_indices[ci].byte_index=i;
		code_indices[ci].incoming_labels=NULL;
		ci++;
		i+=2;
		const char *type=instruction_type(instr);
		for (; *type; type++) {
			code_indices[ci].byte_index=-1;
			code_indices[ci].incoming_labels=NULL;
			ci++;
			switch (*type) {
				case 'c': /* Char */
					i+=1;
					break;
				case 'I': /* Instruction */
				case 'n': /* Stack index */
				case 'N': /* Stack index, optimised to byte width */
				case 'a': /* Arity */
					i+=2;
					break;
				case 'l': /* Label */
				case 'C': /* CAF label */
				case 'S': /* String label */
				case 's': /* String label */
					i+=4;
					break;
				case 'r': /* Real */
				case 'i': /* Int */
					i+=8;
					break;
				default:
					EPRINTF("error in strip_bytecode\n");
					EXIT(NULL,-1);
			}
		}
	}

	bytecode=(uint32_t*)&code[i];
	bytecode+=strings_size;
	data=(uint64_t*)bytecode;
	bytecode=(uint32_t*)&data[data_size];

	init_label_queue();
	pgrm=initialize_code();

	char *labels_in_bytecode=(char*)bytecode;
	labels=safe_malloc(sizeof(struct s_label)*n_labels);
	for (int i=0; i<n_labels; i++) {
		labels[i].offset=*(int32_t*)labels_in_bytecode;
		labels[i].name=&labels_in_bytecode[4];
		if (labels[i].name[0]!='\0')
			n_global_labels=i;
		labels[i].bcgen_label=NULL;
		labels[i].label_type=DLT_NORMAL;
		if (!(labels[i].offset & 1)) {
			struct incoming_labels *ilabs=safe_malloc(sizeof(struct incoming_labels));
			ilabs->next=code_indices[labels[i].offset>>2].incoming_labels;
			ilabs->label_id=i;
			code_indices[labels[i].offset>>2].incoming_labels=ilabs;
		}
		if (activate_start_label && i==start_label_id) {
			add_label_to_queue(&labels[i]);
			code_start(labels[i].name);
		}
		labels_in_bytecode+=4;
		while (*labels_in_bytecode++);
	}
	bytecode=(uint32_t*)labels_in_bytecode;

	code_relocations=safe_malloc(sizeof(struct s_relocation)*code_reloc_size);
	for (int i=0; i<code_reloc_size; i++) {
		code_relocations[i].relocation_offset=bytecode[0];
		code_relocations[i].relocation_label=bytecode[1];
		code_relocations[i].relocation_belongs_to_label=NULL;
		bytecode+=2;
	}
	data_relocations=safe_malloc(sizeof(struct s_relocation)*data_reloc_size);
	for (int i=0; i<data_reloc_size; i++) {
		data_relocations[i].relocation_offset=bytecode[0];
		data_relocations[i].relocation_label=bytecode[1];
		data_relocations[i].relocation_belongs_to_label=NULL;
		bytecode+=2;
	}

	/* Give unnamed labels a name when there is a label with the same offset to
	 * avoid generating superfluous labels. */
	for (int ci=0; ci<code_size; ci++) {
		if (code_indices[ci].byte_index==-1)
			continue;
		char *name=NULL;
		for (struct incoming_labels *ilabs=code_indices[ci].incoming_labels; ilabs!=NULL; ilabs=ilabs->next)
			if (labels[ilabs->label_id].name[0]!='\0') {
				name=labels[ilabs->label_id].name;
				break;
			}
		if (name!=NULL)
			for (struct incoming_labels *ilabs=code_indices[ci].incoming_labels; ilabs!=NULL; ilabs=ilabs->next)
				if (labels[ilabs->label_id].name[0]=='\0')
					labels[ilabs->label_id].name=name;
	}
}

char *finish_strip_bytecode(uint32_t *result_size) {
	struct s_label *label;
	while ((label=next_label_from_queue())!=NULL)
		activate_label(label);

	return write_program_to_string(result_size);
}

void strip_bytecode(int _include_symbol_table,
		uint32_t *bytecode, struct clean_string **descriptors,
		uint32_t *result_size, char **result) {
	prepare_strip_bytecode(bytecode, _include_symbol_table, 0);
	export_all_labels=1;

	for (int i=0; i<((BC_WORD*)descriptors)[-2]; i++)
		add_label_to_queue(find_label_by_name(descriptors[i]->cs_characters));

	*result=finish_strip_bytecode(result_size);
}
