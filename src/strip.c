#include "bytecode.h"
#include "strip.h"
#include "util.h"

struct label {
	int32_t offset;
	char *name;
};

static uint8_t *code;
static int32_t *code_indices;
static uint64_t *data;
static struct label *inactive_labels;

static void activate_label() {
}

void strip_bytecode(uint32_t *bytecode, struct clean_string **descriptors,
		uint32_t *result_size, uint32_t **result) {
	uint32_t code_size=bytecode[0];
	uint32_t words_in_strings=bytecode[1];
	uint32_t strings_size=bytecode[2];
	uint32_t data_size=bytecode[3];
	uint32_t n_labels=bytecode[4];
	uint32_t global_label_string_count=bytecode[5];
	uint32_t code_reloc_size=bytecode[6];
	uint32_t data_reloc_size=bytecode[7];

	code_indices=safe_malloc(sizeof(uint32_t)*code_size);
	code=(uint8_t*)&bytecode[8];

	int ci=0;
	int i=0;
	while (ci<code_size) {
		uint16_t instr=*(uint16_t*)&code[i];
		code_indices[ci++]=i;
		i+=2;
		char *type=instruction_type(instr);
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
				case 'd': /* Descriptor */
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
	inactive_labels=safe_malloc(sizeof(struct label)*n_labels);
	for (int i=0; i<n_labels; i++) {
		inactive_labels[i].offset=*(int32_t*)labels_in_bytecode;
		inactive_labels[i].name=&labels_in_bytecode[4];
		labels_in_bytecode+=4;
		while (*labels_in_bytecode++);
	}

	fprintf(stderr,"%ld descriptors\n",((BC_WORD*)descriptors)[-2]);
	for (int i=0; i<((BC_WORD*)descriptors)[-2]; i++) {
		char desc[256];
		strncpy(desc, &descriptors[i]->cs_characters, descriptors[i]->cs_size);
		desc[descriptors[i]->cs_size]='\0';
		fprintf(stderr,"desc: %3d  %s\n",i,desc);
	}

	result=&bytecode;
	*result_size=0;
}
