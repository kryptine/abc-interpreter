#include <stdlib.h>
#include <string.h>

#include "abc_instructions.h"
#include "bytecode.h"
#include "strip.h"
#include "util.h"

struct label {
	int32_t offset;
	char *name;

	int8_t copy_offset;
	uint32_t active_bytes;

	uint32_t new_label_id;
	int32_t new_offset;
};

struct relocation {
	int32_t relocation_offset;
	uint32_t relocation_label;
	struct label *relocation_belongs_to_label;
};

static uint32_t code_size;
static uint8_t *code;
static int32_t *code_indices;
static uint32_t data_size;
static uint64_t *data;

static uint32_t n_labels;
static struct label *labels;

static uint32_t code_reloc_size;
static struct relocation *code_relocations;
static uint32_t data_reloc_size;
static struct relocation *data_relocations;

static uint32_t new_code_size;
static uint32_t new_data_size;
static uint32_t new_n_code_relocs;
static uint32_t new_n_data_relocs;

static void activate_code_relocation(struct label *belongs_to, uint32_t offset);
static void activate_data_relocation(struct label *belongs_to, uint32_t offset);

static inline int instruction_ends_block(int16_t instr) {
	switch (instr) {
		case Cjmp:
		case Cjmp_ap1:
		case Cjmp_ap2:
		case Cjmp_ap3:
		case Cjmp_ap4:
		case Cjmp_ap5:
		case Cjmp_ap:
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

static void activate_label(struct label *label) {
	/* TODO: it would be better if this were not recursive */
	if (label->active_bytes>0) {
		EPRINTF("%s already active\n",label->name);
		return;
	}

	EPRINTF("activating 0x%x %s\n",label->offset,label->name);

	if (label->offset & 1) { /* data */
		uint32_t arity=data[(label->offset-1)>>2];
		if ((arity & 0xffff) > 256) { /* record */
			uint32_t type_string_length=*((uint32_t*)&data[((label->offset-1)>>2)+1]);
			uint32_t name_length=*((uint32_t*)&data[((label->offset-1)>>2)+2+(type_string_length+sizeof(uint64_t)-1)/sizeof(uint64_t)]);
			EPRINTF("\trecord with lengths %d/%d\n",type_string_length,name_length);

			/* TODO records with unboxed records */

			label->copy_offset=(int8_t)(-2*sizeof(uint64_t));
			label->active_bytes=5*sizeof(uint64_t) +
				(type_string_length+sizeof(uint64_t)-1)/sizeof(uint64_t) +
				(name_length+sizeof(uint64_t)-1)/sizeof(uint64_t);
		} else { /* no record */
			arity>>=3+16;
			uint32_t name_length=*((uint32_t*)&data[((label->offset-1)>>2)+arity*2+3]);
			EPRINTF("\tarity %d, name %d\n",arity,name_length);

			if (arity==0) { /* desc0 */
				label->copy_offset=(int8_t)(-3*sizeof(uint64_t));
				label->active_bytes=3*sizeof(uint64_t) + 2*sizeof(uint64_t)*arity + 4*sizeof(uint64_t) + name_length;
			} else {
				label->copy_offset=(int8_t)(-2*sizeof(uint64_t));
				label->active_bytes=2*sizeof(uint64_t) + 2*sizeof(uint64_t)*arity + 4*sizeof(uint64_t) + name_length;

				for (int i=0; i<arity; i++) {
					activate_data_relocation(label, ((label->offset-1)>>2)+1+2*i);
				}
			}
		}

		new_data_size+=(label->active_bytes+sizeof(uint64_t)-1)/sizeof(uint64_t);
	} else {
		int ci=label->offset>>2;
		uint8_t *code_block=&code[code_indices[ci]];
		uint8_t *start_code_block=code_block;
		int in_block=1;

		label->active_bytes=1; /* prevent infinite recursion */

		while (in_block) {
			int16_t instr=*(int16_t*)code_block;
			code_block+=2;
			const char *type=instruction_type(instr);
			EPRINTF("\t%s %s\n",instruction_name(instr),type);
			ci++;
			for (; *type; type++) {
				switch (*type) {
					case 'c': /* Char */
						code_block+=1;
						break;
					case 'I': /* Instruction */
					case 'n': /* Stack index */
					case 'N': /* Stack index, optimised to byte width */
					case 'a': /* Arity */
						code_block+=2;
						break;
					case 'l': /* Label */
						activate_code_relocation(label, ci);
						code_block+=4;
						break;
					case 'S': /* String label */
					case 's': /* String label */
						/* TODO: activate */
						code_block+=4;
						break;
					case 'r': /* Real */
					case 'i': /* Int */
						code_block+=8;
						break;
					default:
						EPRINTF("error in activate_label\n");
						exit(-1);
				}
				ci++;
			}
			in_block=!instruction_ends_block(instr);
		}

		label->active_bytes=code_block-start_code_block;
		new_code_size+=ci-(label->offset>>2);
	}
}

static void activate_code_relocation(struct label *belongs_to, uint32_t offset) {
	for (int i=0; i<code_reloc_size; i++) { /* TODO log search */
		if (code_relocations[i].relocation_offset==offset) {
			code_relocations[i].relocation_belongs_to_label=belongs_to;
			new_n_code_relocs++;
			activate_label(&labels[code_relocations[i].relocation_label]);
			return;
		}
	}
	EPRINTF("reloc %d not found!\n",offset);
}

static void activate_data_relocation(struct label *belongs_to, uint32_t offset) {
	for (int i=0; i<data_reloc_size; i++) { /* TODO log search */
		if (data_relocations[i].relocation_offset==offset) {
			code_relocations[i].relocation_belongs_to_label=belongs_to;
			new_n_data_relocs++;
			activate_label(&labels[data_relocations[i].relocation_label]);
			return;
		}
	}
	EPRINTF("reloc %d not found!\n",offset);
}

static void activate_label_by_name(const char *name, int name_length) {
	for (int i=0; i<n_labels; i++) {
		struct label *label=&labels[i];
		if (!strncmp(label->name, name, name_length) && label->name[name_length]=='\0') {
			activate_label(label);
			return;
		}
	}
	EPRINTF("not found!\n");
}

static uint32_t count_active_labels(void) {
	uint32_t id=0;
	for (int i=0; i<n_labels; i++) {
		if (labels[i].active_bytes==0)
			continue;
		labels[i].new_label_id=id++;
	}
	return id;
}

static uint32_t *write_active_relocations(struct relocation *relocs, uint32_t n_relocs, uint32_t *bytecode) {
	for (int i=0; i<n_relocs; i++) {
		struct relocation *reloc=&relocs[i];
		if (reloc->relocation_belongs_to_label==NULL)
			continue;
		bytecode[0]=reloc->relocation_offset-
			(reloc->relocation_belongs_to_label->offset>>2)+
			(reloc->relocation_belongs_to_label->new_offset>>2);
		EPRINTF("offset %d, label %s, old label %d, new label %d, new offset %d\n",
				reloc->relocation_offset,
				reloc->relocation_belongs_to_label->name,
				reloc->relocation_belongs_to_label->offset,
				reloc->relocation_belongs_to_label->new_offset,
				bytecode[0]);
		bytecode[1]=labels[reloc->relocation_label].new_label_id;
		bytecode+=2;
	}
	return bytecode;
}

static char *collect_active_labels(uint32_t *bytecode_size) {
	uint32_t n_new_labels=count_active_labels();

	char *new_bytecode=safe_malloc(1000000); /* TODO */
	((uint32_t*)new_bytecode)[0]=new_code_size;
	((uint32_t*)new_bytecode)[1]=0; /* TODO */
	((uint32_t*)new_bytecode)[2]=0; /* TODO */
	((uint32_t*)new_bytecode)[3]=new_data_size;

	EPRINTF("new data size is %d; new code size %d\n",new_data_size,new_code_size);

	((uint32_t*)new_bytecode)[4]=n_new_labels;
	((uint32_t*)new_bytecode)[5]=0; /* filled in while adding labels below */

	((uint32_t*)new_bytecode)[6]=new_n_code_relocs;
	((uint32_t*)new_bytecode)[7]=new_n_data_relocs;

	char *new_code=&new_bytecode[8*sizeof(uint32_t)];
	char *start_code=new_code;
	for (int i=0; i<n_labels; i++) {
		struct label *label=&labels[i];
		if (label->active_bytes==0 || (label->offset & 1))
			continue;
		label->new_offset=((new_code-start_code-label->copy_offset)/sizeof(uint64_t))<<2;
		fprintf(stderr,"copying code: %s %x+%d, %d bytes\n",label->name,label->offset>>2,label->copy_offset,label->active_bytes);
		memcpy(new_code, ((uint8_t*)&code[code_indices[label->offset>>2]])+label->copy_offset, label->active_bytes);
		new_code+=label->active_bytes;
	}

	/* TODO strings */

	char *new_data=new_code;
	for (int i=0; i<n_labels; i++) {
		struct label *label=&labels[i];
		if (label->active_bytes==0 || !(label->offset & 1))
			continue;
		label->new_offset=(((new_data-new_code-label->copy_offset)/sizeof(uint64_t))<<2)+1;
		fprintf(stderr,"copying data: %s %x to %x\n",label->name,(label->offset-1)>>2,label->new_offset);
		memcpy(new_data, ((uint8_t*)&data[(label->offset-1)>>2])+label->copy_offset, label->active_bytes);
		new_data+=(label->active_bytes+sizeof(uint64_t)-1)/sizeof(uint64_t)*sizeof(uint64_t);
	}

	char *new_labels=new_data;
	for (int i=0; i<n_labels; i++) {
		struct label *label=&labels[i];
		if (label->active_bytes==0)
			continue;
		//fprintf(stderr,"copying label: %s\n",label->name);
		int len=strlen(labels[i].name);
		memcpy(new_labels, &labels[i].new_offset, sizeof(uint32_t));
		memcpy(new_labels+sizeof(uint32_t), labels[i].name, len+1);
		new_labels+=sizeof(uint32_t)+len+1;
		((uint32_t*)new_bytecode)[5]+=len;
	}

	uint32_t *new_relocs=(uint32_t*)new_labels;
	new_relocs=write_active_relocations(code_relocations, code_reloc_size, new_relocs);
	new_relocs=write_active_relocations(data_relocations, data_reloc_size, new_relocs);

	*bytecode_size=(char*)new_relocs-new_bytecode;
	EPRINTF("new bytecode at %p is %d bytes long\n",new_bytecode,*bytecode_size);
	return new_bytecode;
}

void strip_bytecode(uint32_t *bytecode, struct clean_string **descriptors,
		uint32_t *result_size, char **result) {
	code_size=bytecode[0];
	/*uint32_t words_in_strings=bytecode[1];*/
	uint32_t strings_size=bytecode[2];
	data_size=bytecode[3];
	n_labels=bytecode[4];
	/*uint32_t global_label_string_count=bytecode[5];*/
	code_reloc_size=bytecode[6];
	data_reloc_size=bytecode[7];

	fprintf(stderr,"%x %x %x %x %x\n",code_size,data_size,n_labels,code_reloc_size,data_reloc_size);

	code_indices=safe_malloc(sizeof(uint32_t)*code_size);
	code=(uint8_t*)&bytecode[8];

	int ci=0;
	int i=0;
	while (ci<code_size) {
		uint16_t instr=*(uint16_t*)&code[i];
		code_indices[ci++]=i;
		i+=2;
		const char *type=instruction_type(instr);
		for (; *type; type++) {
			code_indices[ci++]=-1;
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
					exit(-1);
			}
		}
	}

	bytecode=(uint32_t*)&code[i];
	bytecode+=strings_size;
	data=(uint64_t*)bytecode;
	bytecode=(uint32_t*)&data[data_size];

	char *labels_in_bytecode=(char*)bytecode;
	labels=safe_malloc(sizeof(struct label)*n_labels);
	for (int i=0; i<n_labels; i++) {
		labels[i].offset=*(int32_t*)labels_in_bytecode;
		labels[i].name=&labels_in_bytecode[4];
		labels[i].copy_offset=0;
		labels[i].active_bytes=0;
		labels[i].new_label_id=0;
		labels[i].new_offset=-1;
		//EPRINTF("label %d:\t%d %s\n",i,labels[i].offset,labels[i].name);
		labels_in_bytecode+=4;
		while (*labels_in_bytecode++);
	}
	bytecode=(uint32_t*)labels_in_bytecode;

	code_relocations=safe_malloc(sizeof(struct relocation)*code_reloc_size);
	for (int i=0; i<code_reloc_size; i++) {
		code_relocations[i].relocation_offset=bytecode[0];
		code_relocations[i].relocation_label=bytecode[1];
		code_relocations[i].relocation_belongs_to_label=NULL;
		//EPRINTF("code reloc %d %s\n",bytecode[0],labels[bytecode[1]].name);
		bytecode+=2;
	}
	data_relocations=safe_malloc(sizeof(struct relocation)*data_reloc_size);
	for (int i=0; i<data_reloc_size; i++) {
		data_relocations[i].relocation_offset=bytecode[0];
		data_relocations[i].relocation_label=bytecode[1];
		data_relocations[i].relocation_belongs_to_label=NULL;
		//EPRINTF("data reloc %d %s\n",bytecode[0],labels[bytecode[1]].name);
		bytecode+=2;
	}

	new_code_size=0;
	new_data_size=0;
	new_n_code_relocs=0;
	new_n_data_relocs=0;

	fprintf(stderr,"%ld descriptors\n",((BC_WORD*)descriptors)[-2]);
	for (int i=0; i<((BC_WORD*)descriptors)[-2]; i++) {
		activate_label_by_name(descriptors[i]->cs_characters, descriptors[i]->cs_size);
	}

	*result=collect_active_labels(result_size);

	fprintf(stderr,"done\n");
}
