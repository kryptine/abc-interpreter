#include "instruction_parse.h"

int parse_instruction(instruction* instruction) {
	instruction->code_function();
	return 1;
}

int parse_instruction_a (instruction* instruction)
{
	STRING a;
	
	parse_label(a);
	instruction->instruction_code_function (a);
	return 1;
}

int parse_instruction_a_n (instruction* instruction)
{
	STRING a;
	LONG n;
	
	parse_label (a);
	if (!parse_unsigned_integer (&n))
		return 0;
	instruction->instruction_code_function (a,(int)n);
	return 1;
}

int parse_instruction_a_n_a (instruction* instruction)
{
	STRING a1,a2;
	LONG n1;
	
	parse_label (a1);
	if (!parse_integer (&n1))
		return 0;
	parse_label (a2);
	instruction->instruction_code_function (a1,(int)n1,a2);
	return 1;
}

int parse_instruction_a_n_a_a_n_a (instruction* instruction)
{
	STRING a1,a2,a3,a4;
	LONG n1,n2;
	
	parse_label (a1);
	if (!parse_integer (&n1))
		return 0;
	parse_label (a2);
	parse_label (a3);
	if (!parse_unsigned_integer (&n2))
		return 0;
	parse_label (a4);
	instruction->instruction_code_function (a1,(int)n1,a2,a3,(int)n2,a4);
	return 1;
}

int parse_instruction_a_n_a_n (instruction* instruction)
{
	STRING a1,a2;
	LONG n1,n2;
	
	parse_label (a1);
	if (!parse_integer (&n1))
		return 0;
	parse_label (a2);
	if (!parse_unsigned_integer (&n2))
		return 0;
	instruction->instruction_code_function (a1,(int)n1,a2,(int)n2);
	return 1;
}

int parse_instruction_a_n_n (instruction* instruction)
{
	STRING a;
	LONG n1,n2;
	
	parse_label (a);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;
	instruction->instruction_code_function (a,(int)n1,(int)n2);
	return 1;
}

int parse_instruction_a_n_n_a (instruction* instruction)
{
	STRING a1,a2;
	LONG n1,n2;
	
	parse_label (a1);
	if (!parse_integer (&n1))
		return 0;
	if (!parse_integer (&n2))
		return 0;
	parse_label (a2);
	instruction->instruction_code_function (a1,(int)n1,(int)n2,a2);
	return 1;
}

int parse_instruction_a_n_n_n (instruction* instruction)
{
	STRING a;
	LONG n1,n2,n3;
	
	parse_label (a);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || !parse_unsigned_integer (&n3))
		return 0;
	instruction->instruction_code_function (a,(int)n1,(int)n2,(int)n3);
	return 1;
}

void parse_bit_string (char *s_p)
{
	if (last_char!='0' && last_char!='1'){
		abc_parser_error_i ("0 or 1 expected at line %d\n",line_number);
		*s_p='\0';
	}

	do {
		*s_p++=last_char;
		last_char=getc (abc_file);
	} while (last_char=='0' || last_char=='1');

	*s_p='\0';
}

int parse_instruction_a_n_n_b (instruction* instruction)
{
	STRING a1,a2;
	LONG n1,n2;
	
	parse_label (a1);
	if (!parse_unsigned_integer (&n1))
		return 0;
	if (!parse_unsigned_integer (&n2))
		return 0;
	
	parse_bit_string (a2);
	
	skip_spaces_and_tabs();
	
	instruction->instruction_code_function (a1,(int)n1,(int)n2,a2);
	return 1;
}

int parse_instruction_a_n_n_n_b (instruction* instruction)
{
	STRING a1,a2;
	LONG n1,n2,n3;
	
	parse_label (a1);
	if (!parse_unsigned_integer (&n1))
		return 0;
	if (!parse_unsigned_integer (&n2))
		return 0;
	if (!parse_unsigned_integer (&n3))
		return 0;
	
	parse_bit_string (a2);
	
	skip_spaces_and_tabs();
	
	instruction->instruction_code_function (a1,(int)n1,(int)n2,(int)n3,a2);
	return 1;
}

int parse_instruction_a_n_n_n_n (instruction* instruction)
{
	STRING a1;
	LONG n1,n2,n3,n4;
	
	parse_label (a1);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4))
		return 0;
	instruction->instruction_code_function (a1,(int)n1,(int)n2,(int)n3,(int)n4);
	return 1;
}

int parse_instruction_a_n_n_n_n_n (instruction* instruction)
{
	STRING a1;
	LONG n1,n2,n3,n4,n5;
	
	parse_label (a1);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4) ||
		!parse_unsigned_integer (&n5))
		return 0;
	instruction->instruction_code_function (a1,(int)n1,(int)n2,(int)n3,(int)n4,(int)n5);
	return 1;
}

int parse_instruction_b (instruction* instruction)
{
	int b;
	
	if (!parse_boolean (&b))
		return 0;
	instruction->instruction_code_function (b);
	return 1;
}

int parse_instruction_b_n (instruction* instruction)
{
	int b;
	LONG n;
	
	if (!parse_boolean (&b) || !parse_unsigned_integer (&n))
		return 0;
	instruction->instruction_code_function (b,(int)n);
	return 1;
}

int parse_instruction_c (instruction* instruction)
{
	unsigned char c;
	
	if (!parse_character (&c))
		return 0;
	instruction->instruction_code_function (c);
	return 1;
}

int parse_instruction_c_c_n_a_a (instruction* instruction)
{
	unsigned char c1,c2;
	LONG n;
	STRING a1,a2;
	
	if (!parse_character (&c1) || !parse_character (&c2) || !parse_unsigned_integer (&n))
		return 0;
	parse_label (a1);
	parse_label (a2);
	instruction->instruction_code_function (c1,c2,(int)n,a1,a2);
	return 1;
}

int parse_instruction_c_n (instruction* instruction)
{
	unsigned char c;
	LONG n;
	
	if (!parse_character (&c) || !parse_unsigned_integer (&n))
		return 0;
	instruction->instruction_code_function (c,(int)n);
	return 1;
}

int parse_instruction_c_n_a (instruction* instruction)
{
	unsigned char c;
	LONG n;
	STRING a;
	
	if (!parse_character (&c) || !parse_unsigned_integer (&n))
		return 0;
	parse_label (a);
	instruction->instruction_code_function (c,(int)n,a);
	return 1;
}

int parse_instruction_i (instruction* instruction)
{
	CleanInt i;
	
	if (!parse_clean_integer (&i))
		return 0;
	instruction->instruction_code_function (i);
	return 1;
}

int parse_instruction_i_i_n_a_a (instruction* instruction)
{
	CleanInt i1,i2;
	LONG n;
	STRING a1,a2;

	if (!parse_clean_integer (&i1) || !parse_clean_integer (&i2) || !parse_unsigned_integer (&n))
		return 0;
	parse_label (a1);
	parse_label (a2);
	instruction->instruction_code_function (i1,i2,(int)n,a1,a2);
	return 1;
}

int parse_instruction_i_n (instruction* instruction)
{
	CleanInt i;
	LONG n;

	if (!parse_clean_integer (&i) || !parse_unsigned_integer (&n))
		return 0;
	instruction->instruction_code_function (i,(int)n);
	return 1;
}

int parse_instruction_i_n_a (instruction* instruction)
{
	CleanInt i;
	LONG n;
	STRING a;

	if (!parse_clean_integer (&i) || !parse_unsigned_integer (&n))
		return 0;
	parse_label (a);
	instruction->instruction_code_function (i,(int)n,a);
	return 1;
}

int parse_instruction_l (instruction* instruction)
{
	STRING a;
	
	parse_label_without_conversion (a);
	instruction->instruction_code_function (a);
	return 1;
}

int parse_instruction_n (instruction* instruction)
{
	LONG n;
	
	if (!parse_unsigned_integer (&n))
		return 0;
	
	instruction->instruction_code_function ((int)n);
	return 1;
}

int parse_instruction_n_a (instruction* instruction)
{
	STRING a;
	LONG n;
	
	if (!parse_unsigned_integer (&n))
		return 0;
	parse_label (a);
	instruction->instruction_code_function ((int)n,a);
	return 1;
}

int parse_instruction_n_a_n_a_a_n_a (instruction* instruction)
{
	STRING a1,a2,a3,a4;
	LONG n0,n1,n2;
	
	if (!parse_unsigned_integer (&n0))
		return 0;
	parse_label (a1);
	if (!parse_integer (&n1))
		return 0;
	parse_label (a2);
	parse_label (a3);
	if (!parse_unsigned_integer (&n2))
		return 0;
	parse_label (a4);
	instruction->instruction_code_function ((int)n0,a1,(int)n1,a2,a3,(int)n2,a4);
	return 1;
}

int parse_instruction_n_b (instruction* instruction)
{
	int b;
	LONG n;
	
	if (!parse_unsigned_integer (&n) || !parse_boolean (&b))
		return 0;
	instruction->instruction_code_function ((int)n,b);
	return 1;
}

int parse_instruction_n_n (instruction* instruction)
{
	LONG n1,n2;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2);
	return 1;
}

int parse_instruction_n_n_n (instruction* instruction)
{
	LONG n1,n2,n3;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2,(int)n3);
	return 1;
}

int parse_instruction_n_n_n_n (instruction* instruction)
{
	LONG n1,n2,n3,n4;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2,(int)n3,(int)n4);
	return 1;
}

int parse_instruction_n_n_n_n_n (instruction* instruction)
{
	LONG n1,n2,n3,n4,n5;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4) ||
		!parse_unsigned_integer (&n5))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2,(int)n3,(int)n4,(int)n5);
	return 1;
}

int parse_instruction_n_n_n_n_n_n_n (instruction* instruction)
{
	LONG n1,n2,n3,n4,n5,n6,n7;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4) ||
		!parse_unsigned_integer (&n5) || !parse_unsigned_integer (&n6) ||
		!parse_unsigned_integer (&n7))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2,(int)n3,(int)n4,(int)n5,(int)n6,(int)n7);
	return 1;
}

int parse_instruction_on (instruction* instruction)
{
	LONG n;
	
	if (!is_digit_character (last_char))
		n=-1;
	else
		if (!parse_unsigned_integer (&n))
			return 0;
	
	instruction->instruction_code_function ((int)n);
	return 1;
}

int parse_instruction_s (instruction* instruction)
{
	STRING s;
	int length;
	
	if (!parse_string (s,&length))
		return 0;
	instruction->instruction_code_function (s,length);
	return 1;
}

int parse_instruction_s2 (instruction* instruction)
{
	STRING s;
	int length;
	char *string;

	string=parse_string2 (s,&length);

	instruction->instruction_code_function (string,length);
	
	if (string!=s)
		free (string);

	return 1;
}

int parse_instruction_s2_n_a (instruction* instruction)
{
	STRING s1,s2;
	int length;
	char *string;
	LONG n;
	
	string=parse_string2 (s1,&length);
	if (!parse_unsigned_integer (&n))
		return 0;
	parse_label (s2);

	instruction->instruction_code_function (string,length,(int)n,s2);
	
	if (string!=s1)
		free (string);

	return 1;
}

int parse_directive (instruction* instruction)
{
	instruction->instruction_code_function();
	return 1;
}

int parse_directive_a (instruction* instruction)
{
	LONG n;
	STRING s;

	if (!parse_integer (&n))
		return 0;
	
	parse_label (s);
	instruction->instruction_code_function ((int)n,s);

	return 1;
}

int parse_directive_depend (instruction* instruction)
{
	STRING a;
	int l;
	
	if (!parse_string (a,&l))
		return 0;

	 /* skip optional last modification time */
	if (last_char=='"'){
		STRING t;
		int tl;
	
		parse_string (t,&tl);
	}

	instruction->instruction_code_function (a,l);

#if 0
	while (last_char==','){
		last_char=getc (abc_file);
		skip_spaces_and_tabs();
		if (!parse_string (a,&l))
			return 0;
		instruction->instruction_code_function (a,l);
	}
#endif

	return 1;
}

int parse_directive_desc (instruction* instruction)
{
	STRING a1,a2,a3,s;
	int l;
	LONG n;
	LONG f;

	parse_label (a1);
	parse_label (a2);
	parse_label (a3);

	if (!parse_unsigned_integer (&n) || !parse_unsigned_integer (&f) || !parse_descriptor_string (s,&l))
		return 0;
	instruction->instruction_code_function (a1,a2,a3,(int)n,(int)f,s,l);
	return 1;
}

int parse_directive_desc0 (instruction* instruction)
{
	STRING a1,s;
	int l;
	LONG n;

	parse_label (a1);

	if (!parse_unsigned_integer (&n) || !parse_descriptor_string (s,&l))
		return 0;
	instruction->instruction_code_function (a1,(int)n,s,l);
	return 1;
}

int parse_directive_descn (instruction* instruction)
{
	STRING a1,a2,s;
	int l;
	LONG n;
	int f;

	parse_label (a1);
	parse_label (a2);
	if (!parse_unsigned_integer (&n) || !parse_0_or_1 (&f) || !parse_descriptor_string (s,&l))
		return 0;
	instruction->instruction_code_function (a1,a2,(int)n,f,s,l);
	return 1;
}

int parse_directive_implab (instruction* instruction)
{
	STRING s;
	
	parse_label (s);
	skip_spaces_and_tabs();

	if (!is_newline (last_char)){
		STRING s2;

		parse_label (s2);
		code_implab_node_entry (s,s2);
	} else
		code_implab (s);

	return 1;
}

int parse_directive_labels (instruction* instruction)
{
	STRING s;
	
	parse_label (s);
	instruction->instruction_code_function (s);
	skip_spaces_and_tabs();
	
	while (!is_newline (last_char)){
		if (!try_parse_label (s)){
			abc_parser_error_i ("Label expected at line %d\n",line_number);
			return 0;
		}
		skip_spaces_and_tabs();
		instruction->instruction_code_function (s);
		skip_spaces_and_tabs();
	}

	return 1;
}

int parse_directive_module (instruction* instruction)
{
	STRING a,s;
	int l;

	parse_label (a);
	if (!parse_string (s,&l))
		return 0;
	
	 /* skip optional last modification time */
	if (last_char=='"'){
		STRING t;
		int tl;
	
		parse_string (t,&tl);
	}

	instruction->instruction_code_function (a,s,l);
	return 1;
}

int parse_directive_n (instruction* instruction)
{
	LONG n;
	STRING s;

	if (!parse_integer (&n))
		return 0;
	
	parse_label (s);

	skip_spaces_and_tabs();

	if (!is_newline (last_char)){
		STRING s2;

		parse_label (s2);
		instruction->instruction_code_function ((int)n,s,s2);
	} else
		instruction->instruction_code_function ((int)n,s,NULL);

	return 1;
}

int parse_directive_nu (instruction* instruction)
{
	LONG n1,n2;
	STRING s;

	if (!parse_integer (&n1) || !parse_integer (&n2))
		return 0;
	
	parse_label (s);

	skip_spaces_and_tabs();

	if (!is_newline (last_char)){
		STRING s2;

		parse_label (s2);
		instruction->instruction_code_function ((int)n1,(int)n2,s,s2);
	} else
		instruction->instruction_code_function ((int)n1,(int)n2,s,NULL);

	return 1;
}

int parse_directive_n_l (instruction* instruction)
{
	LONG n;
	STRING s;

	if (!parse_unsigned_integer (&n))
		return 0;

	/* parse the options string as a label */
	parse_label (s);
	
	instruction->instruction_code_function ((int)n,(char *)s);
	return 1;
}

#define SMALL_VECTOR_SIZE 32
#define LOG_SMALL_VECTOR_SIZE 5
#define MASK_SMALL_VECTOR_SIZE 31

int parse_directive_n_n_t (instruction* instruction)
{
	LONG n1,n2;
	int i;
	ULONG small_vector;
	ULONG *vector_p;
	int vector_size=0;

	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;
	
	vector_size=n2;
	if (vector_size+1<=SMALL_VECTOR_SIZE)
		vector_p=&small_vector;
	else
		vector_p=(ULONG*)fast_memory_allocate 
					(((vector_size+1+SMALL_VECTOR_SIZE-1)>>LOG_SMALL_VECTOR_SIZE) * sizeof (ULONG));
	
	i=0;
	while (i!=n2){
		switch (last_char){
			case 'i':	case 'I':
			case 'b':	case 'B':
			case 'c':	case 'C':
			case 'p':	case 'P':
				vector_p[i>>LOG_SMALL_VECTOR_SIZE] &= ~ (1<< (i & MASK_SMALL_VECTOR_SIZE));
				i+=1;	
				break;
			case 'f':	case 'F':
				vector_p[i>>LOG_SMALL_VECTOR_SIZE] &= ~ (1<< (i & MASK_SMALL_VECTOR_SIZE));
				i+=1;
				vector_p[i>>LOG_SMALL_VECTOR_SIZE] &= ~ (1<< (i & MASK_SMALL_VECTOR_SIZE));
				i+=1;
				break;
			case 'r':	case 'R':
				vector_p[i>>LOG_SMALL_VECTOR_SIZE] |= (1<< (i & MASK_SMALL_VECTOR_SIZE));
				i+=1;
#ifndef G_A64
				vector_p[i>>LOG_SMALL_VECTOR_SIZE] |= (1<< (i & MASK_SMALL_VECTOR_SIZE));
				i+=1;	
#endif
				break;
			default:
				abc_parser_error_i ("B, C, F, I, P or R expected at line %d\n",line_number);
		}
		last_char=getc (abc_file);
		skip_spaces_and_tabs();
	}
	
	instruction->instruction_code_function ((int)n1,(int)n2,vector_p);
	return 1;
}

int parse_directive_label (instruction* instruction)
{
	STRING s;
	
	parse_label (s);
	instruction->instruction_code_function (s);
	skip_spaces_and_tabs();
	
	return 1;
}

int parse_directive_record (instruction* instruction)
{
	STRING a1,a2,s;
	LONG n1,n2;
	int l;
	
	parse_label (a1);
	parse_record_field_types (a2);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;

	if (last_char=='"'){
		if (!parse_descriptor_string (s,&l))
			return 0;

		instruction->instruction_code_function (a1,a2,(int)n1,(int)n2,s,l);
		return 1;
	} else {
		code_record_start (a1,a2,(int)n1,(int)n2);

		do {
			STRING a3;
			
			parse_label (a3);

			code_record_descriptor_label (a3);
		} while (last_char!='"');
		
		if (!parse_descriptor_string (s,&l))
			return 0;

		code_record_end (s,l);
		return 1;
	}
}

int parse_directive_string (instruction* instruction)
{
	STRING a,s;
	int l;

	parse_label (a);
	if (!parse_string (s,&l))
		return 0;
	instruction->instruction_code_function (a,s,l);
	return 1;
}
