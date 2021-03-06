#include <inttypes.h>
#include <stdio.h>
#include <stdlib.h>
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

#define _2chars2int(a,b)             ((uint64_t) (a+(b<<8)))
#define _3chars2int(a,b,c)           ((uint64_t) (a+(b<<8)+(c<<16)))
#define _4chars2int(a,b,c,d)         ((uint64_t) (a+(b<<8)+(c<<16)+(d<<24)))
#define _5chars2int(a,b,c,d,e)       ((uint64_t) (a+(b<<8)+(c<<16)+(d<<24)+((uint64_t)e<<32)))
#define _7chars2int(a,b,c,d,e,f,g)   ((uint64_t) (a+(b<<8)+(c<<16)+(d<<24)+((uint64_t)e<<32)+((uint64_t)f<<40)+((uint64_t)g<<48)))
#define _8chars2int(a,b,c,d,e,f,g,h) ((uint64_t) (a+(b<<8)+(c<<16)+(d<<24)+((uint64_t)e<<32)+((uint64_t)f<<40)+((uint64_t)g<<48)+((uint64_t)h<<56)))
uint64_t prelinker_preamble[697] = {
	/*  0 */ 0, 0, 0, 7, _7chars2int('_','A','R','R','A','Y','_'),
	/*  5 */ 0, 0, 0, 8, _8chars2int('_','S','T','R','I','N','G','_'),
	/* 10 */ 0, 0, 0, 4, _4chars2int('B','O','O','L'),
	/* 15 */ 0, 0, 0, 4, _4chars2int('C','H','A','R'),
	/* 20 */ 0, 0, 0, 4, _4chars2int('R','E','A','L'),
	/* 25 */ 0, 0, 0, 3, _3chars2int('I','N','T'),
	/* 30 small integers */
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,
	/* 96 */ 0, 97*8, /* caf list */
	/* 98 */
		Cjmp_ap1,  Cjmp_ap2,  Cjmp_ap3,  Cjmp_ap4,  Cjmp_ap5,
		Cjmp_ap6,  Cjmp_ap7,  Cjmp_ap8,  Cjmp_ap9,  Cjmp_ap10,
		Cjmp_ap11, Cjmp_ap12, Cjmp_ap13, Cjmp_ap14, Cjmp_ap15,
		Cjmp_ap16, Cjmp_ap17, Cjmp_ap18, Cjmp_ap19, Cjmp_ap20,
		Cjmp_ap21, Cjmp_ap22, Cjmp_ap23, Cjmp_ap24, Cjmp_ap25,
		Cjmp_ap26, Cjmp_ap27, Cjmp_ap28, Cjmp_ap29, Cjmp_ap30,
		Cjmp_ap31, Cjmp_ap32,
	/* 130 unused; old */ 0,
	/* 131 */ Cjsr_eval0, Cfill_a01_pop_rtn, Chalt, Chalt, -2, /* indirection */
		Cpush_node1, (690+1)*8, Cjsr_eval0, Cfill_a01_pop_rtn,
	/* 140 */ 141*8+2, 0, 0, 0, 4, /* Nil */
		_4chars2int('_','N','i','l'),
	/* 146 static characters*/
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	/* 658 */ Cinstruction, 0, /* to return to JavaScript */
	/* 660 static booleans */
		10*8+2,0, 10*8+2,1,
	/* 664 */
		258, 2, _2chars2int('i','i'), 4, _4chars2int('F','I','L','E'),
	/* 669 power10_table (for RtoAC) */
		0,0,0,0,0,0,0,0, 0,0,0,0,0,0,0,0,
	/* 685 power10_table2 (for RtoAC) */
		0,0,0,0,0,
	/* 690 cycle in spine*/
		0, Chalt,
	/* 692 */
		0, 0, 0, 5, _5chars2int('D','R','E','A','L'),
	/* 697 */
};

void prepare_preamble(void) {
	/* small integers */
	for (int i=0; i<=32; i++) {
		prelinker_preamble[30+i*2]=25*8+2; /* INT+2 */
		prelinker_preamble[30+i*2+1]=i;
	}

	/* static characters */
	for (int i=0; i<256; i++) {
		prelinker_preamble[146+i*2]=15*8+2; /* CHAR+2 */
		prelinker_preamble[146+i*2+1]=i;
	}

	/* power10_table */
	*(double*)&prelinker_preamble[669]=1.0e00;
	*(double*)&prelinker_preamble[670]=1.0e01;
	*(double*)&prelinker_preamble[671]=1.0e02;
	*(double*)&prelinker_preamble[672]=1.0e03;
	*(double*)&prelinker_preamble[673]=1.0e04;
	*(double*)&prelinker_preamble[674]=1.0e05;
	*(double*)&prelinker_preamble[675]=1.0e06;
	*(double*)&prelinker_preamble[676]=1.0e07;
	*(double*)&prelinker_preamble[677]=1.0e08;
	*(double*)&prelinker_preamble[678]=1.0e09;
	*(double*)&prelinker_preamble[679]=1.0e10;
	*(double*)&prelinker_preamble[680]=1.0e11;
	*(double*)&prelinker_preamble[681]=1.0e12;
	*(double*)&prelinker_preamble[682]=1.0e13;
	*(double*)&prelinker_preamble[683]=1.0e14;
	*(double*)&prelinker_preamble[684]=1.0e15;

	/* power10_table2 */
	*(double*)&prelinker_preamble[685]=1.0e16;
	*(double*)&prelinker_preamble[686]=1.0e32;
	*(double*)&prelinker_preamble[687]=1.0e64;
	*(double*)&prelinker_preamble[688]=1.0e128;
	*(double*)&prelinker_preamble[689]=1.0e256;
}

void write_section(FILE *f, enum section_type type, uint32_t len, uint64_t *data) {
	fwrite(&type, 1, sizeof(uint32_t), f);
	fwrite(&len, 1, sizeof(uint32_t), f);
	fwrite(data, sizeof(uint64_t), len, f);
}

static uint32_t varwidth_encode(uint32_t len, uint64_t *data, uint64_t **dest) {
	char *ptr=safe_malloc (len*sizeof(uint64_t)*5/4); /* rough upper bound */
	*dest=(uint64_t*)ptr;

	*(uint32_t*)ptr=len;
	ptr+=4;

	while (len--) {
		uint64_t val=*data++;
		char byte=0x00;

		if ((int64_t)val<0) {
			val=0-(int64_t)val;
			byte=0x40;
		}

		byte|=val&0x3f;
		val>>=6;
		if (val)
			byte|=0x80;
		*ptr++=byte;

		while (val) {
			byte=val&0x7f;
			val>>=7;
			if (val)
				byte|=0x80;
			*ptr++=byte;
		}
	}

	return ((ptr-(char*)*dest)+7)>>3;
}

static void write_section_varwidth(FILE *f, enum section_type type, uint32_t len, uint64_t *data) {
	uint64_t *leb_encoded_data;
	uint32_t leb_encoded_len=varwidth_encode (len,data,&leb_encoded_data);
	write_section (f,type,leb_encoded_len,leb_encoded_data);
	free (leb_encoded_data);
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

	prepare_preamble();
	write_section_varwidth(output_file, ST_Preamble, sizeof(prelinker_preamble)/sizeof(uint64_t), prelinker_preamble);
	write_section_varwidth(output_file, ST_Code, program->code_size, program->code);
	write_section_varwidth(output_file, ST_Data, program->data_size, program->data);
	write_section(output_file, ST_Start, 1, &program->symbol_table[program->start_symbol_id].offset);

	fclose(output_file);

	return 0;
}
