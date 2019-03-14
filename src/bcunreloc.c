#include <inttypes.h>
#include <stdio.h>
#include <string.h>

#include "abc_instructions.h"
#include "util.h"
#include "parse.h"

const char usage[] = "Usage: %s FILE [-o FILE]\n";

enum section_type {
	ST_Preamble,
	ST_Code,
	ST_Data,
	ST_Start
};

#define _3chars2int(a,b,c)           ((uint64_t) (a+(b<<8)+(c<<16)))
#define _4chars2int(a,b,c,d)         ((uint64_t) (a+(b<<8)+(c<<16)+(d<<24)))
#define _7chars2int(a,b,c,d,e,f,g)   ((uint64_t) (a+(b<<8)+(c<<16)+(d<<24)+((uint64_t)e<<32)+((uint64_t)f<<40)+((uint64_t)g<<48)))
#define _8chars2int(a,b,c,d,e,f,g,h) ((uint64_t) (a+(b<<8)+(c<<16)+(d<<24)+((uint64_t)e<<32)+((uint64_t)f<<40)+((uint64_t)g<<48)+((uint64_t)h<<56)))
uint64_t unrelocator_preamble[131] = {
	/*  0 */ 0, 0, 0, 7, _7chars2int('_','A','R','R','A','Y','_'),
	/*  5 */ 0, 0, 0, 8, _8chars2int('_','S','T','R','I','N','G','_'),
	/* 10 */ 0, 0, 0, 4, _4chars2int('B','O','O','L'),
	/* 15 */ 0, 0, 0, 4, _4chars2int('C','H','A','R'),
	/* 20 */ 0, 0, 0, 4, _4chars2int('R','E','A','L'),
	/* 25 */ 0, 0, 0, 3, _3chars2int('I','N','T'),
	/* 30 */ 26*8+2,  0, /* small integers */
		26*8+2,  1, 26*8+2,  2, 26*8+2,  3, 26*8+2,  4, 26*8+2,  5,
		26*8+2,  6, 26*8+2,  7, 26*8+2,  8, 26*8+2,  9, 26*8+2, 10,
		26*8+2, 11, 26*8+2, 12, 26*8+2, 13, 26*8+2, 14, 26*8+2, 15,
		26*8+2, 16, 26*8+2, 17, 26*8+2, 18, 26*8+2, 19, 26*8+2, 20,
		26*8+2, 21, 26*8+2, 22, 26*8+2, 23, 26*8+2, 24, 26*8+2, 25,
		26*8+2, 26, 26*8+2, 27, 26*8+2, 28, 26*8+2, 29, 26*8+2, 30,
		26*8+2, 31, 26*8+2, 32,
	/* 96 */ 0, 97*8, /* caf list */
	/* 98 */
		Cjmp_ap1,  Cjmp_ap2,  Cjmp_ap3,  Cjmp_ap4,  Cjmp_ap5,
		Cjmp_ap6,  Cjmp_ap7,  Cjmp_ap8,  Cjmp_ap9,  Cjmp_ap10,
		Cjmp_ap11, Cjmp_ap12, Cjmp_ap13, Cjmp_ap14, Cjmp_ap15,
		Cjmp_ap16, Cjmp_ap17, Cjmp_ap18, Cjmp_ap19, Cjmp_ap20,
		Cjmp_ap21, Cjmp_ap22, Cjmp_ap23, Cjmp_ap24, Cjmp_ap25,
		Cjmp_ap26, Cjmp_ap27, Cjmp_ap28, Cjmp_ap29, Cjmp_ap30,
		Cjmp_ap31, Cjmp_ap32,
	/* 130 */ Chalt, /* cycle in spine */
	/* 131 */
};

void write_section(FILE *f, enum section_type type, uint32_t len, uint64_t *data) {
	fwrite(&type, 1, sizeof(uint32_t), f);
	fwrite(&len, 1, sizeof(uint32_t), f);
	fwrite(data, sizeof(uint64_t), len, f);
}

int main(int argc, char **argv) {
	char *output_file_name=NULL;
	FILE *input_file=NULL;

	for (int i=1; i<argc; i++) {
		if(!strcmp("-o", argv[i]) && i <= argc-1) {
			output_file_name=argv[i+1];
			i++;
		} else if (input_file!=NULL) {
			fprintf(stderr, usage, argv[0]);
			return -1;
		} else {
			input_file=fopen(argv[i], "rb");
			if (input_file==NULL) {
				fprintf(stderr, "Error: Could not open input file: %s\n", argv[i]);
				return -1;
			}
		}
	}

	if (input_file==NULL) {
		fprintf(stderr, usage, argv[0]);
		return -1;
	}

	struct parser state;
	init_parser(&state);

	struct char_provider cp;
	new_file_char_provider(&cp, input_file);
	int res=parse_program(&state, &cp);
	free_parser(&state);
	free_char_provider(&cp);
	if (res) {
		EPRINTF("Parsing failed (%d)\n", res);
		return res;
	}

	FILE *output_file=stdout;
	if(output_file_name!=NULL && (output_file=fopen(output_file_name, "wb"))==NULL) {
		fprintf(stderr, "Error: Could not open output file: %s\n", output_file_name);
		return -1;
	}

	struct program *program=state.program;

	write_section(output_file, ST_Preamble, sizeof(unrelocator_preamble)/sizeof(uint64_t), unrelocator_preamble);
	write_section(output_file, ST_Code, program->code_size, program->code);
	write_section(output_file, ST_Data, program->data_size, program->data);
	write_section(output_file, ST_Start, 1, &program->symbol_table[program->start_symbol_id].offset);

	fclose(output_file);

	return 0;
}
