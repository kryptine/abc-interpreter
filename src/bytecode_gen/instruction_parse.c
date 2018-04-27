#include <inttypes.h>
#include "instruction_parse.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

#include "../abci_types.h"
#include "../util.h"
#include "instruction_code.h"
#include "instruction_table.h"

void warning_i (char *error_string,int integer) {
	fprintf(stderr, error_string, integer);
}

#define MAX_STRING_LENGTH 200

typedef char STRING [MAX_STRING_LENGTH+1];

static unsigned char alpha_num_table[257];

static void initialize_alpha_num_table()
{
	int i;
	static unsigned char extra_alpha_nums[]="@#$%^&*-+/=<>\\_~.?\'\"|`!:{};";

	for (i=0; i<257; ++i)
		alpha_num_table[i]=0;

	for (i='a'; i<='z'; ++i)
		alpha_num_table[i+1]=1;
	for (i='A'; i<='Z'; ++i)
		alpha_num_table[1+i]=1;
	for (i='0'; i<='9'; ++i)
		alpha_num_table[1+i]=1;
	for (i=0; i<sizeof (extra_alpha_nums)-1; ++i)
		alpha_num_table[1+extra_alpha_nums[i]]=1;
}

#define is_alpha_num_character(c) (alpha_num_table[(c)+1])
#define is_digit_character(c) ((unsigned)((c)-'0')<(unsigned)10)
#define is_hexdigit_character(c) ((unsigned)((c)-'0')<(unsigned)10 || (unsigned)((c | 0x20)-'a')<(unsigned)6)
#define is_octdigit_character(c) ((unsigned)((c)-'0')<(unsigned)8)
#define append_char(c) if (length<MAX_STRING_LENGTH) {label_string[length] = (c); ++length; };

void init_parser(void) {
	initialize_alpha_num_table();
}

static char *current_line;
static char last_char;
static uint32_t line_number;

static char next_character() {
	return *current_line++;
}

static int skip_spaces_and_tabs (void)
{
	if (last_char==' ' || last_char=='\t'){
		do
			last_char=next_character();
		while (last_char==' ' || last_char=='\t');
		return 1;
	} else
		return 0;
}

static void parse_label (char *label_string);
void parse_line(char* line, unsigned int line_nr) {
	char *end, *token;

	line_number = line_nr;

	end = line;
	while (isspace(*end))
		end++;

	if ((end != line && *(end-1) == '\n') || *end == '|') {
		return;
	} else if (end == line && line[0] != '.' && line[0] != '|') {
		char s[MAX_STRING_LENGTH];
		strsep(&end, " \r\n\t");
		current_line = line;
		last_char = next_character();
		parse_label(s);
		code_label(s);
	} else if ((token = strsep(&end, " \r\n\t")) != NULL) {
		instruction* i = instruction_lookup(token);
		if (i == NULL) {
			fprintf(stderr, "Unknown instruction '%s' at line %d.\n", token, line_nr);
			exit(1);
		} else {
			current_line = end;
			last_char = next_character();
			skip_spaces_and_tabs();
			i->parse_function(i);
		}
	} else {
		fprintf(stderr, "Parse error at line %d.\n", line_nr);
		exit(1);
	}
}

static int try_parse_label (char *label_string)
{
	int length;

	length=0;
	if (is_alpha_num_character (last_char))
		do {
			switch (last_char){
				case '.':
					append_char ('_');
					append_char ('P');
					break;
				case '_':
					append_char ('_');
					append_char ('_');
					break;
				case '*':
					append_char ('_');
					append_char ('M');
					break;
				case '-':
					append_char ('_');
					append_char ('S');
					break;
				case '+':
					append_char ('_');
					append_char ('A');
					break;
				case '=':
					append_char ('_');
					append_char ('E');
					break;
				case '~':
					append_char ('_');
					append_char ('T');
					break;
				case '<':
					append_char ('_');
					append_char ('L');
					break;
				case '>':
					append_char ('_');
					append_char ('G');
					break;
				case '/':
					append_char ('_');
					append_char ('D');
					break;
				case '?':
					append_char ('_');
					append_char ('Q');
					break;
				case '#':
					append_char ('_');
					append_char ('H');
					break;
				case ':':
					append_char ('_');
					append_char ('C');
					break;
				case '$':
					append_char ('_');
					append_char ('N');
					append_char ('D');
					break;
				case '^':
					append_char ('_');
					append_char ('N');
					append_char ('C');
					break;
				case '@':
					append_char ('_');
					append_char ('N');
					append_char ('T');
					break;
				case '&':
					append_char ('_');
					append_char ('N');
					append_char ('A');
					break;
				case '%':
					append_char ('_');
					append_char ('N');
					append_char ('P');
					break;
				case '\'':
					append_char ('_');
					append_char ('N');
					append_char ('S');
					break;
				case '\"':
					append_char ('_');
					append_char ('N');
					append_char ('Q');
					break;
				case '|':
					append_char ('_');
					append_char ('O');
					break;
				case '\\':
					append_char ('_');
					append_char ('N');
					append_char ('B');
					break;
				case '`':
					append_char ('_');
					append_char ('B');
					break;
				case '!':
					append_char ('_');
					append_char ('N');
					append_char ('E');
					break;
				case ';':
					append_char ('_');
					append_char ('I');
					break;
				default:
					append_char (last_char);
			}
			last_char=next_character();
		} while (is_alpha_num_character (last_char));

	label_string[length]='\0';

	if (length==0)
		return 0;
	if (length==MAX_STRING_LENGTH)
		warning_i("Label too long, extra characters ignored at line %d\n",line_number);
	return 1;
}

#define is_newline(c) ((c)=='\n' || (c)=='\r')

static void abc_parser_error_i(char *message, int i) {
	fprintf(stderr,message,i);
	exit(-1);
}

static int parse_integer (LONG *integer_p)
{
	LONG integer;
	int minus_sign;

	minus_sign=0;
	if (last_char=='+' || last_char=='-'){
		if (last_char=='-')
			minus_sign=!minus_sign;
		last_char=next_character();
	}

	if (!is_digit_character (last_char))
		abc_parser_error_i ("Integer expected at line %d\n",line_number);

	integer=last_char-'0';
	last_char=next_character();

	while (is_digit_character (last_char)){
		integer*=10;
		integer+=last_char-'0';
		last_char=next_character();
	}

	skip_spaces_and_tabs();

	if (minus_sign)
		integer=-integer;
	*integer_p=integer;

	return 1;
}

static int parse_string_character (char *c_p)
{
	if (last_char!='\\'){
		if (last_char==EOF)
			return 0;
		*c_p=last_char;
		last_char=next_character();
	} else {
		last_char=next_character();
		if (last_char>='0' && last_char<='7'){
			int c=last_char-'0';
			last_char=next_character();
			if (last_char>='0' && last_char<='7'){
				c=(c<<3)+(last_char-'0');
				last_char=next_character();
				if (last_char>='0' && last_char<='7'){
					c=(c<<3)+(last_char-'0');
					last_char=next_character();
				}
			}
			*c_p=c;
		} else {
			switch (last_char){
				case 'b':	*c_p='\b';	break;
				case 'f':	*c_p='\f';	break;
#if (defined (M68000) && !defined (SUN)) || defined (__MWERKS__) || defined (__MRC__) || defined (POWER)
				case 'n':	*c_p='\xd';	break;
				case 'r':	*c_p='\xa';	break;
#else
				case 'n':	*c_p='\n';	break;
				case 'r':	*c_p='\r';	break;
#endif
				case 't':	*c_p='\t';	break;
				case EOF:	return 0;
				default:	*c_p=last_char;
			}
			last_char=next_character();
		}
	}
	return 1;
}

static int parse_string (char *string,int *string_length_p)
{
	int length;

	if (last_char!='"')
		abc_parser_error_i ("String expected at line %d",line_number);

	last_char=next_character();

	length=0;

	while (last_char!='"'){
		char c;

		if (!parse_string_character (&c))
			abc_parser_error_i ("Error in string at line %d\n",line_number);

		if (length<MAX_STRING_LENGTH)
			string[length++]=c;
	}

	if (length==MAX_STRING_LENGTH)
		warning_i("String too long, extra characters ignored at line %d\n",line_number);

	last_char=next_character();

	string[length]='\0';
	*string_length_p=length;

	skip_spaces_and_tabs();

	return 1;
}

static int parse_descriptor_string (char *string,int *string_length_p)
{
	int length;

	if (last_char!='"')
		abc_parser_error_i ("String expected at line %d",line_number);

	last_char=next_character();

	length=0;

	while (last_char!='"'){
		char c;

		if (last_char==EOF)
			abc_parser_error_i ("Error in string at line %d\n",line_number);

		c=last_char;
		last_char=next_character();

		if (length<MAX_STRING_LENGTH)
			string[length++]=c;
	}

	if (length==MAX_STRING_LENGTH)
		warning_i("String too long, extra characters ignored at line %d\n",line_number);

	last_char=next_character();

	string[length]='\0';
	*string_length_p=length;

	skip_spaces_and_tabs();

	return 1;
}

static void parse_label (char *label_string)
{
	if (!try_parse_label (label_string))
		abc_parser_error_i ("Label expected at line %d\n",line_number);
	skip_spaces_and_tabs();
}

static int try_parse_label_without_conversion (char *label_string)
{
	int length;

	length=0;
	if (is_alpha_num_character (last_char))
		do {
			append_char (last_char);
			last_char=next_character();
		} while (is_alpha_num_character (last_char));

	label_string[length]='\0';

	if (length==0)
		return 0;
	if (length==MAX_STRING_LENGTH)
		warning_i("Label too long, extra characters ignored at line %d\n",line_number);
	return 1;
}

static void parse_label_without_conversion (char *label_string)
{
	if (!try_parse_label_without_conversion (label_string))
		abc_parser_error_i ("Label expected at line %d\n",line_number);
	skip_spaces_and_tabs();
}

static int try_parse_record_field_types (char *label_string)
{
	int length;

	length=0;
	if (is_alpha_num_character (last_char) || last_char==',' || last_char=='(' || last_char==')')
		do {
			append_char (last_char);
			last_char=next_character();
		} while (is_alpha_num_character (last_char) || last_char==',' || last_char=='(' || last_char==')');

	label_string[length]='\0';

	if (length==0)
		return 0;
	if (length==MAX_STRING_LENGTH)
		warning_i("Label too long, extra characters ignored at line %d\n",line_number);
	return 1;
}

static void parse_record_field_types (char *label_string)
{
	if (!try_parse_record_field_types (label_string))
		abc_parser_error_i ("Record field types expected at line %d\n",line_number);
	skip_spaces_and_tabs();
}

static int parse_unsigned_integer (LONG *integer_p)
{
	LONG integer;

	if (!is_digit_character (last_char))
		abc_parser_error_i ("Integer expected at line %d\n",line_number);

	integer=last_char-'0';
	last_char=next_character();

	while (is_digit_character (last_char)){
		integer*=10;
		integer+=last_char-'0';
		last_char=next_character();
	}

	skip_spaces_and_tabs();

	*integer_p=integer;

	return 1;
}

static int parse_clean_integer (CleanInt *integer_p)
{
	CleanInt integer;
	int minus_sign;

	minus_sign=0;
	if (last_char=='+' || last_char=='-'){
		if (last_char=='-')
			minus_sign=!minus_sign;
		last_char=next_character();
	}

	if (!is_digit_character (last_char))
		abc_parser_error_i ("Integer expected at line %d\n",line_number);

	integer=last_char-'0';
	last_char=next_character();

	if (last_char=='x' && integer==0){
		last_char=next_character();
		if (!is_hexdigit_character (last_char))
			abc_parser_error_i ("Integer expected at line %d\n",line_number);
		do {
			integer<<=4;
			integer+= last_char<='9' ? last_char-'0' : (last_char | 0x20)-('a'-10);
			last_char=next_character();
		} while (is_hexdigit_character (last_char));
	} else if (last_char=='o' && integer==0){
		last_char=next_character();
		if (!is_octdigit_character (last_char))
			abc_parser_error_i ("Integer expected at line %d\n",line_number);
		do {
			integer<<=3;
			integer+= last_char-'0';
			last_char=next_character();
		} while (is_octdigit_character (last_char));
	} else {
		while (is_digit_character (last_char)){
			integer*=10;
			integer+=last_char-'0';
			last_char=next_character();
		}
	}

	skip_spaces_and_tabs();

	if (minus_sign)
		integer=-integer;
	*integer_p=integer;

	return 1;
}

static int parse_boolean (int *boolean_p)
{
	if (last_char=='F'){
		if ((last_char=next_character())=='A' && (last_char=next_character())=='L' &&
				(last_char=next_character())=='S' && (last_char=next_character())=='E'){
			last_char=next_character();
			skip_spaces_and_tabs();
			*boolean_p=0;
			return 1;
		}
	} else if (last_char=='T'){
		if ((last_char=next_character())=='R' && (last_char=next_character())=='U' &&
				(last_char=next_character())=='E'){
			last_char=next_character();
			skip_spaces_and_tabs();
			*boolean_p=1;
			return 1;
		}
	}
	abc_parser_error_i ("Boolean expected at line %d\n",line_number);
	return 0;
}

static int parse_character (unsigned char *character_p)
{
	if (last_char!='\'')
		abc_parser_error_i ("Character expected at line %d\n",line_number);

	last_char=next_character();

	if (!parse_string_character ((char*)character_p))
		abc_parser_error_i ("Error in character at line %d\n",line_number);

	if (last_char!='\'')
		abc_parser_error_i ("Character not terminated at line %d\n",line_number);

	last_char=next_character();

	skip_spaces_and_tabs();

	return 1;
}

static char *resize_string (char *string,int length,int max_length)
{
	if (length==MAX_STRING_LENGTH){
		char *new_string;
		int i;

		new_string=safe_malloc(max_length);

		for (i=0; i<length; ++i)
			new_string[i]=string[i];

		return new_string;
	} else {
		string=safe_realloc (string,max_length);

		return string;
	}
}

static int parse_0_or_1 (int *integer_p)
{
	int integer;

	if (!(last_char=='0' || last_char=='1'))
		abc_parser_error_i ("0 or 1 expected at line %d\n",line_number);

	integer=last_char-'0';

	last_char=next_character();
	skip_spaces_and_tabs();

	*integer_p=integer;

	return 1;
}

static char *parse_string2 (char *string,int *string_length_p)
{
	int length,max_length;

	if (last_char!='"')
		abc_parser_error_i ("String expected at line %d",line_number);

	last_char=next_character();

	length=0;
	max_length=MAX_STRING_LENGTH;

	while (last_char!='"'){
		char c;

		if (!parse_string_character (&c))
			abc_parser_error_i ("Error in string at line %d\n",line_number);

		if (length<max_length)
			string[length++]=c;
		else {
			max_length=max_length<<1;
			string=resize_string (string,length,max_length);
			string[length++]=c;
		}
	}

	last_char=next_character();

	if (length>=max_length){
		++max_length;
		string=resize_string (string,length,max_length);
	}
	string[length]='\0';
	*string_length_p=length;

	skip_spaces_and_tabs();

	return string;
}

static int parse_hex_real (double *real_p)
{
	int s1,s2;
	unsigned int i1,i2;

	last_char=next_character();
	if (!is_hexdigit_character (last_char))
		abc_parser_error_i ("Hex digit expected in real at line %d\n",line_number);

	while (last_char=='0')
		last_char=next_character();

	i1=0;
	s1=0;
	while (s1<7){
		if (!is_hexdigit_character (last_char)){
			*real_p=(double)i1;
			return 1;
		}
		i1<<=4;
		i1+=last_char<='9' ? last_char-'0' : (last_char | 0x20)-('a'-10);
		++s1;
		last_char=next_character();
	}

	i2=0;
	s2=0;
	while (s2<7){
		if (!is_hexdigit_character (last_char)){
			double r;

			r=(double)i1;
			while (s2>=0){
				r*=16.0;
				--s2;
			}
			*real_p=r+(double)i2;
			return 1;
		}
		i2<<=4;
		i2+=last_char<='9' ? last_char-'0' : (last_char | 0x20)-('a'-10);
		++s2;
		last_char=next_character();
	}

	{
	int extra_non_zero_char;
	double p,r1;
	char char14;

	p=1.0;

	r1=(double)i1*(double)0x10000000;

	char14=last_char;
	if (i1<0x2000000){
		if (!is_hexdigit_character (last_char)){
			*real_p=r1+(double)i2;
			return 1;
		}
		p*=16.0;
		last_char=next_character();
	}

	while (last_char=='0'){
		p*=16.0;
		last_char=next_character();
	}

	extra_non_zero_char=0;
	if (is_hexdigit_character (last_char)){
		extra_non_zero_char=1;
		do {
			p*=16.0;
			last_char=next_character();
		} while (is_hexdigit_character (last_char));
	}

	/* round to 53 bits */
	if (i1<0x2000000){
		if (char14>'8' || (char14=='8' && !(extra_non_zero_char==0 && (i2 & 1)==0)))
			++i2;
	} else if (i1<0x4000000){
		if ((i2 & 1)!=0 && !(extra_non_zero_char==0 && (i2 & 2)==0))
			i2 += 2;
		i2 &= -2;
	} else if (i1<0x8000000){
		if ((i2 & 2)>2 || ((i2 & 2)==2 && !(extra_non_zero_char==0 && (i2 & 4)==0)))
			i2 += 4;
		i2 &= -4;
	} else {
		if ((i2 & 4)>4 || ((i2 & 4)==4 && !(extra_non_zero_char==0 && (i2 & 8)==0)))
			i2 += 8;
		i2 &= -8;
	}

	*real_p=(r1+(double)i2)*p;
	return 1;
	}
}

static int parse_real (double *real_p)
{
	unsigned int real_string_length=0;
	char real_string[64];

	if (last_char=='+' || last_char=='-'){
		if (last_char=='-')
			real_string[real_string_length++]='-';
		last_char=next_character();
	}

	if (last_char=='0'){
		real_string[real_string_length++]='0';
		last_char=next_character();

		if (last_char=='x'){
			int r;

			r=parse_hex_real (real_p);

			if (real_string[0]=='-')
				*real_p = - *real_p;

			skip_spaces_and_tabs();

			return r;
		}

		while (is_digit_character (last_char)) {
			real_string[real_string_length++]=last_char;
			last_char=next_character();
		}
	} else if (!is_digit_character(last_char)) {
		abc_parser_error_i ("Digit expected in real at line %d\n",line_number);
		return 0;
	} else {
		do {
			real_string[real_string_length++]=last_char;
			last_char=next_character();
		} while (is_digit_character(last_char));
	}

	if (last_char=='.'){
		real_string[real_string_length++]=last_char;
		last_char=next_character();

		if (!is_digit_character(last_char)) {
			abc_parser_error_i ("Digit expected in real at line %d\n",line_number);
			return 0;
		} else {
			do {
				real_string[real_string_length++]=last_char;
				last_char=next_character();
			} while (is_digit_character(last_char));
		}
	} else if (last_char!='e' && last_char!='E')
		abc_parser_error_i ("'.' or 'E' expected in real at line %d\n",line_number);

	if (last_char=='e' || last_char=='E'){
		real_string[real_string_length++]=last_char;
		last_char=next_character();

		if (last_char=='+' || last_char=='-'){
			if (last_char=='-')
				real_string[real_string_length++]='-';
			last_char=next_character();
		}

		if (!is_digit_character(last_char)) {
			abc_parser_error_i ("Digit expected in real at line %d\n",line_number);
			return 0;
		} else {
			do {
				real_string[real_string_length++]=last_char;
				last_char=next_character();
			} while (is_digit_character(last_char));
		}
	}

	if (real_string_length==MAX_STRING_LENGTH)
		abc_parser_error_i ("Real denotation too long at line %d\n",line_number);

	real_string[real_string_length]='\0';

	skip_spaces_and_tabs();

	*real_p=atof (real_string);

	return 1;
}

int parse_instruction (instruction *instruction)
{
	instruction->code_function();
	return 1;
}

int parse_instruction_a (instruction *instruction)
{
	STRING a;

	parse_label (a);
	instruction->code_function (a);
	return 1;
}

int parse_instruction_a_n (instruction *instruction)
{
	STRING a;
	LONG n;

	parse_label (a);
	if (!parse_unsigned_integer (&n))
		return 0;
	instruction->code_function (a,(int)n);
	return 1;
}

int parse_instruction_a_n_a (instruction *instruction)
{
	STRING a1,a2;
	LONG n1;

	parse_label (a1);
	if (!parse_integer (&n1))
		return 0;
	parse_label (a2);
	instruction->code_function (a1,(int)n1,a2);
	return 1;
}

int parse_instruction_a_n_a_a_n_a (instruction *instruction)
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
	instruction->code_function (a1,(int)n1,a2,a3,(int)n2,a4);
	return 1;
}

int parse_instruction_a_n_a_n (instruction *instruction)
{
	STRING a1,a2;
	LONG n1,n2;

	parse_label (a1);
	if (!parse_integer (&n1))
		return 0;
	parse_label (a2);
	if (!parse_unsigned_integer (&n2))
		return 0;
	instruction->code_function (a1,(int)n1,a2,(int)n2);
	return 1;
}

int parse_instruction_a_n_n (instruction *instruction)
{
	STRING a;
	LONG n1,n2;

	parse_label (a);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;
	instruction->code_function (a,(int)n1,(int)n2);
	return 1;
}

int parse_instruction_a_n_n_a (instruction *instruction)
{
	STRING a1,a2;
	LONG n1,n2;

	parse_label (a1);
	if (!parse_integer (&n1))
		return 0;
	if (!parse_integer (&n2))
		return 0;
	parse_label (a2);
	instruction->code_function (a1,(int)n1,(int)n2,a2);
	return 1;
}

int parse_instruction_a_n_n_n (instruction *instruction)
{
	STRING a;
	LONG n1,n2,n3;

	parse_label (a);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || !parse_unsigned_integer (&n3))
		return 0;
	instruction->code_function (a,(int)n1,(int)n2,(int)n3);
	return 1;
}

static void parse_bit_string (char *s_p)
{
	if (last_char!='0' && last_char!='1'){
		abc_parser_error_i ("0 or 1 expected at line %d\n",line_number);
		*s_p='\0';
	}

	do {
		*s_p++=last_char;
		last_char=next_character();
	} while (last_char=='0' || last_char=='1');

	*s_p='\0';
}

int parse_instruction_a_n_n_b (instruction *instruction)
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

	instruction->code_function (a1,(int)n1,(int)n2,a2);
	return 1;
}

int parse_instruction_a_n_n_n_b (instruction *instruction)
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

	instruction->code_function (a1,(int)n1,(int)n2,(int)n3,a2);
	return 1;
}

int parse_instruction_a_n_n_n_n (instruction *instruction)
{
	STRING a1;
	LONG n1,n2,n3,n4;

	parse_label (a1);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) ||
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4))
		return 0;
	instruction->code_function (a1,(int)n1,(int)n2,(int)n3,(int)n4);
	return 1;
}

int parse_instruction_a_n_n_n_n_n (instruction *instruction)
{
	STRING a1;
	LONG n1,n2,n3,n4,n5;

	parse_label (a1);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) ||
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4) ||
		!parse_unsigned_integer (&n5))
		return 0;
	instruction->code_function (a1,(int)n1,(int)n2,(int)n3,(int)n4,(int)n5);
	return 1;
}

int parse_instruction_b (instruction *instruction)
{
	int b;

	if (!parse_boolean (&b))
		return 0;
	instruction->code_function (b);
	return 1;
}

int parse_instruction_b_n (instruction *instruction)
{
	int b;
	LONG n;

	if (!parse_boolean (&b) || !parse_unsigned_integer (&n))
		return 0;
	instruction->code_function (b,(int)n);
	return 1;
}

int parse_instruction_c (instruction *instruction)
{
	unsigned char c;

	if (!parse_character (&c))
		return 0;
	instruction->code_function (c);
	return 1;
}

int parse_instruction_c_c_n_a_a (instruction *instruction)
{
	unsigned char c1,c2;
	LONG n;
	STRING a1,a2;

	if (!parse_character (&c1) || !parse_character (&c2) || !parse_unsigned_integer (&n))
		return 0;
	parse_label (a1);
	parse_label (a2);
	instruction->code_function (c1,c2,(int)n,a1,a2);
	return 1;
}

int parse_instruction_c_n (instruction *instruction)
{
	unsigned char c;
	LONG n;

	if (!parse_character (&c) || !parse_unsigned_integer (&n))
		return 0;
	instruction->code_function (c,(int)n);
	return 1;
}

int parse_instruction_c_n_a (instruction *instruction)
{
	unsigned char c;
	LONG n;
	STRING a;

	if (!parse_character (&c) || !parse_unsigned_integer (&n))
		return 0;
	parse_label (a);
	instruction->code_function (c,(int)n,a);
	return 1;
}

int parse_instruction_i (instruction *instruction)
{
	CleanInt i;

	if (!parse_clean_integer (&i))
		return 0;
	instruction->code_function (i);
	return 1;
}

int parse_instruction_i_i_n_a_a (instruction *instruction)
{
	CleanInt i1,i2;
	LONG n;
	STRING a1,a2;

	if (!parse_clean_integer (&i1) || !parse_clean_integer (&i2) || !parse_unsigned_integer (&n))
		return 0;
	parse_label (a1);
	parse_label (a2);
	instruction->code_function (i1,i2,(int)n,a1,a2);
	return 1;
}

int parse_instruction_i_n (instruction *instruction)
{
	CleanInt i;
	LONG n;

	if (!parse_clean_integer (&i) || !parse_unsigned_integer (&n))
		return 0;
	instruction->code_function (i,(int)n);
	return 1;
}

int parse_instruction_i_n_a (instruction *instruction)
{
	CleanInt i;
	LONG n;
	STRING a;

	if (!parse_clean_integer (&i) || !parse_unsigned_integer (&n))
		return 0;
	parse_label (a);
	instruction->code_function (i,(int)n,a);
	return 1;
}

int parse_instruction_r (instruction *instruction)
{
	double r;

	if (!parse_real (&r))
		return 0;
	instruction->code_function (r);
	return 1;
}

int parse_instruction_l (instruction *instruction)
{
	STRING a;

	parse_label_without_conversion (a);
	instruction->code_function (a);
	return 1;
}

int parse_instruction_n (instruction *instruction)
{
	LONG n;

	if (!parse_unsigned_integer (&n))
		return 0;

	instruction->code_function ((int)n);
	return 1;
}

int parse_instruction_n_a (instruction *instruction)
{
	STRING a;
	LONG n;

	if (!parse_unsigned_integer (&n))
		return 0;
	parse_label (a);
	instruction->code_function ((int)n,a);
	return 1;
}

int parse_instruction_n_a_n_a_a_n_a (instruction *instruction)
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
	instruction->code_function ((int)n0,a1,(int)n1,a2,a3,(int)n2,a4);
	return 1;
}

int parse_instruction_n_b (instruction *instruction)
{
	int b;
	LONG n;

	if (!parse_unsigned_integer (&n) || !parse_boolean (&b))
		return 0;
	instruction->code_function ((int)n,b);
	return 1;
}

int parse_instruction_n_n (instruction *instruction)
{
	LONG n1,n2;

	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;
	instruction->code_function ((int)n1,(int)n2);
	return 1;
}

int parse_instruction_n_n_n (instruction *instruction)
{
	LONG n1,n2,n3;

	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) ||
		!parse_unsigned_integer (&n3))
		return 0;
	instruction->code_function ((int)n1,(int)n2,(int)n3);
	return 1;
}

int parse_instruction_n_n_n_n (instruction *instruction)
{
	LONG n1,n2,n3,n4;

	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) ||
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4))
		return 0;
	instruction->code_function ((int)n1,(int)n2,(int)n3,(int)n4);
	return 1;
}

int parse_instruction_n_n_n_n_n (instruction *instruction)
{
	LONG n1,n2,n3,n4,n5;

	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) ||
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4) ||
		!parse_unsigned_integer (&n5))
		return 0;
	instruction->code_function ((int)n1,(int)n2,(int)n3,(int)n4,(int)n5);
	return 1;
}

int parse_instruction_n_n_n_n_n_n_n (instruction *instruction)
{
	LONG n1,n2,n3,n4,n5,n6,n7;

	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) ||
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4) ||
		!parse_unsigned_integer (&n5) || !parse_unsigned_integer (&n6) ||
		!parse_unsigned_integer (&n7))
		return 0;
	instruction->code_function ((int)n1,(int)n2,(int)n3,(int)n4,(int)n5,(int)n6,(int)n7);
	return 1;
}

int parse_instruction_on (instruction *instruction)
{
	LONG n;

	if (!is_digit_character (last_char))
		n=-1;
	else
		if (!parse_unsigned_integer (&n))
			return 0;

	instruction->code_function ((int)n);
	return 1;
}

int parse_instruction_s (instruction *instruction)
{
	STRING s;
	int length;

	if (!parse_string (s,&length))
		return 0;
	instruction->code_function (s,length);
	return 1;
}

int parse_instruction_s2 (instruction *instruction)
{
	STRING s;
	int length;
	char *string;

	string=parse_string2 (s,&length);

	instruction->code_function (string,length);

	if (string!=s)
		free (string);

	return 1;
}

int parse_instruction_s2_n_a (instruction *instruction)
{
	STRING s1,s2;
	int length;
	char *string;
	LONG n;

	string=parse_string2 (s1,&length);
	if (!parse_unsigned_integer (&n))
		return 0;
	parse_label (s2);

	instruction->code_function (string,length,(int)n,s2);

	if (string!=s1)
		free (string);

	return 1;
}

int parse_directive (instruction *instruction)
{
	instruction->code_function();
	return 1;
}

int parse_directive_a (instruction *instruction)
{
	LONG n;
	STRING s;

	if (!parse_integer (&n))
		return 0;

	parse_label (s);
	instruction->code_function ((int)n,s);

	return 1;
}

int parse_directive_depend (instruction *instruction)
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

	instruction->code_function (a,l);

#if 0
	while (last_char==','){
		last_char=next_character();
		skip_spaces_and_tabs();
		if (!parse_string (a,&l))
			return 0;
		instruction->code_function (a,l);
	}
#endif

	return 1;
}

int parse_directive_desc (instruction *instruction)
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
	instruction->code_function (a1,a2,a3,(int)n,(int)f,s,l);
	return 1;
}

int parse_directive_desc0 (instruction *instruction)
{
	STRING a1,s;
	int l;
	LONG n;

	parse_label (a1);

	if (!parse_unsigned_integer (&n) || !parse_descriptor_string (s,&l))
		return 0;
	instruction->code_function (a1,(int)n,s,l);
	return 1;
}

int parse_directive_descn (instruction *instruction)
{
	STRING a1,a2,s;
	int l;
	LONG n;
	int f;

	parse_label (a1);
	parse_label (a2);
	if (!parse_unsigned_integer (&n) || !parse_0_or_1 (&f) || !parse_descriptor_string (s,&l))
		return 0;
	instruction->code_function (a1,a2,(int)n,f,s,l);
	return 1;
}

int parse_directive_implab (instruction *instruction)
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

int parse_directive_labels (instruction *instruction)
{
	STRING s;

	parse_label (s);
	instruction->code_function (s);
	skip_spaces_and_tabs();

	while (!is_newline (last_char)){
		if (!try_parse_label (s)){
			abc_parser_error_i ("Label expected at line %d\n",line_number);
			return 0;
		}
		skip_spaces_and_tabs();
		instruction->code_function (s);
		skip_spaces_and_tabs();
	}

	return 1;
}

int parse_directive_module (instruction *instruction)
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

	instruction->code_function (a,s,l);
	return 1;
}

int parse_directive_n (instruction *instruction)
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
		instruction->code_function ((int)n,s,s2);
	} else
		instruction->code_function ((int)n,s,NULL);

	return 1;
}

int parse_directive_nu (instruction *instruction)
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
		instruction->code_function ((int)n1,(int)n2,s,s2);
	} else
		instruction->code_function ((int)n1,(int)n2,s,NULL);

	return 1;
}

int parse_directive_n_l (instruction *instruction)
{
	LONG n;
	STRING s;

	if (!parse_unsigned_integer (&n))
		return 0;

	/* parse the options string as a label */
	parse_label (s);

	instruction->code_function ((int)n,(char *)s);
	return 1;
}

#define SMALL_VECTOR_SIZE 32
#define LOG_SMALL_VECTOR_SIZE 5
#define MASK_SMALL_VECTOR_SIZE 31

int parse_directive_n_n_t (instruction *instruction)
{
	LONG n1,n2;
	int i;
	static ULONG small_vector;
	ULONG *vector_p;
	int vector_size=0;

	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;

	vector_size=n2;
	if (vector_size+1<=SMALL_VECTOR_SIZE)
		vector_p=&small_vector;
	else
		vector_p=(ULONG*)safe_malloc(((vector_size+1+SMALL_VECTOR_SIZE-1)>>LOG_SMALL_VECTOR_SIZE) * sizeof (ULONG));

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
				break;
			default:
				fprintf(stderr,"B, C, F, I, P or R expected instead of '%c' (0x%02x) at line %d\n",last_char,last_char,line_number);
				exit(1);
		}
		last_char=next_character();
		skip_spaces_and_tabs();
	}

	instruction->code_function ((int)n1,(int)n2,vector_p);
	return 1;
}

int parse_directive_label (instruction *instruction)
{
	STRING s;

	parse_label (s);
	instruction->code_function (s);
	skip_spaces_and_tabs();

	return 1;
}

int parse_directive_record (instruction *instruction)
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

		instruction->code_function (a1,a2,(int)n1,(int)n2,s,l);
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

int parse_directive_string (instruction *instruction)
{
	STRING a,s;
	int l;

	parse_label (a);
	if (!parse_string (s,&l))
		return 0;
	instruction->code_function (a,s,l);
	return 1;
}
