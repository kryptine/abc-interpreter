#include "bc_gen.h"

// ### Definitions ###
#define IN_NAME_TABLE_SIZE 500
#define MAX_STRING_LENGTH 200
#define MEM_BLOCK_SIZE 4092

#define is_alpha_num_character(c) (alpha_num_table[(c)+1])

#define is_digit_character(c) ((unsigned)((c)-'0')<(unsigned)10)

#define is_hexdigit_character(c) ((unsigned)((c)-'0')<(unsigned)10 || (unsigned)((c | 0x20)-'a')<(unsigned)6)

#define is_octdigit_character(c) ((unsigned)((c)-'0')<(unsigned)8)

#define is_newline(c) ((c)=='\xa' || (c)=='\xd')

#define append_char(c) if (length<MAX_STRING_LENGTH) {label_string[length] = (c); ++length; };

#define print_error(s1) fprintf(stderr,"%s\n",s1)

// ### Data Structures/Types ###
struct memory_block_list {
	char                      mem_block[MEM_BLOCK_SIZE];
	struct memory_block_list* mem_next_block;
};

// Holds information on instructions
// Their name, and what functions to call for them
struct instruction {
	char* instruction_name;
	int (* instruction_parse_function)();
	void (* instruction_code_function)();
};

// Type alias instruction* as InstructionP
typedef struct instruction* InstructionP;

// String type
typedef char STRING [MAX_STRING_LENGTH+1];

// Linked List of Instruction Pointers
struct in_name {
	struct in_name* in_name_next;
	InstructionP in_name_instruction;
};

// ### Global Variables ###
static struct memory_block_list *heap;
static int first_mem_block_size;

static struct memory_block_list *memory_list;
static char *fast_mem_block;
static int fast_mem_block_size;

static FILE *abc_file;
static int last_char;

int line_number;
int last_d,last_jsr_with_d;

static unsigned char alpha_num_table[257];

static jmp_buf exit_abc_parser_buffer;

// Linked list of instruction names
static struct in_name *in_name_table;

// ### Functions ###
void *memory_allocate (int size)
{
	void *memory_block;

	memory_block=malloc (size);
	if (memory_block==NULL)
		perror ("Out of memory");
	return memory_block;
}

void warning_i (char *error_string,int integer)
{
	char error[300];

	sprintf (error,error_string,integer);
	print_error (error);
}

// Read all spaces and tabs
static int skip_spaces_and_tabs() {
	if (last_char==' ' || last_char=='\t'){
		do
			last_char = getc(abc_file);
		while (last_char == ' ' || last_char == '\t');
		return 1;
	} else
		return 0;
}

static void exit_abc_parser() {
	longjmp (exit_abc_parser_buffer, 1);
}

static void abc_parser_error_si(char *message, char *s,int i) {
	fprintf(stderr,message,s,i);
	exit_abc_parser();
}

static void abc_parser_error_i(char *message, int i) {
	fprintf(stderr,message,i);
	exit_abc_parser();
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
			last_char=getc (abc_file);
		} while (is_alpha_num_character (last_char));

	label_string[length]='\0';

	if (length==0)
		return 0;
	if (length==MAX_STRING_LENGTH)
		warning_i ("Label too long, extra characters ignored at line %d\n",line_number);
	return 1;
}


static int parse_integer (LONG *integer_p)
{
	LONG integer;
	int minus_sign;

	minus_sign=0;
	if (last_char=='+' || last_char=='-'){
		if (last_char=='-')
			minus_sign=!minus_sign;
		last_char=getc (abc_file);
	}

	if (!is_digit_character (last_char))
		abc_parser_error_i ("Integer expected at line %d\n",line_number);

	integer=last_char-'0';
	last_char=getc (abc_file);

	while (is_digit_character (last_char)){
		integer*=10;
		integer+=last_char-'0';
		last_char=getc (abc_file);
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
		last_char=getc (abc_file);
	} else {
		last_char=getc (abc_file);
		if (last_char>='0' && last_char<='7'){
			int c=last_char-'0';
			last_char=getc (abc_file);
			if (last_char>='0' && last_char<='7'){
				c=(c<<3)+(last_char-'0');
				last_char=getc (abc_file);
				if (last_char>='0' && last_char<='7'){
					c=(c<<3)+(last_char-'0');
					last_char=getc (abc_file);
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
			last_char=getc (abc_file);
		}
	}
	return 1;
}

static int parse_string (char *string,int *string_length_p)
{
	int length;

	if (last_char!='"')
		abc_parser_error_i ("String expected at line %d",line_number);

	last_char=getc (abc_file);

	length=0;

	while (last_char!='"'){
		char c;

		if (!parse_string_character (&c))
			abc_parser_error_i ("Error in string at line %d\n",line_number);

		if (length<MAX_STRING_LENGTH)
			string[length++]=c;
	}

	if (length==MAX_STRING_LENGTH)
		warning_i ("String too long, extra characters ignored at line %d\n",line_number);

	last_char=getc (abc_file);

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

	last_char=getc (abc_file);

	length=0;

	while (last_char!='"'){
		char c;

		if (last_char==EOF)
			abc_parser_error_i ("Error in string at line %d\n",line_number);

		c=last_char;
		last_char=getc (abc_file);

		if (length<MAX_STRING_LENGTH)
			string[length++]=c;
	}

	if (length==MAX_STRING_LENGTH)
		warning_i ("String too long, extra characters ignored at line %d\n",line_number);

	last_char=getc (abc_file);

	string[length]='\0';
	*string_length_p=length;

	skip_spaces_and_tabs();

	return 1;
}

static void parse_label (char *label_string)
{
	if (!try_parse_label (label_string))
		abc_parser_error_i ("Label expected at line %d",line_number);
	skip_spaces_and_tabs();
}

static int try_parse_label_without_conversion (char *label_string)
{
	int length;

	length=0;
	if (is_alpha_num_character (last_char))
		do {
			append_char (last_char);
			last_char=getc (abc_file);
		} while (is_alpha_num_character (last_char));

	label_string[length]='\0';

	if (length==0)
		return 0;
	if (length==MAX_STRING_LENGTH)
		warning_i ("Label too long, extra characters ignored at line %d\n",line_number);
	return 1;
}

static void parse_label_without_conversion (char *label_string)
{
	if (!try_parse_label_without_conversion (label_string))
		abc_parser_error_i ("Label expected at line %d",line_number);
	skip_spaces_and_tabs();
}

static int try_parse_record_field_types (char *label_string)
{
	int length;

	length=0;
	if (is_alpha_num_character (last_char) || last_char==',' || last_char=='(' || last_char==')')
		do {
			append_char (last_char);
			last_char=getc (abc_file);
		} while (is_alpha_num_character (last_char) || last_char==',' || last_char=='(' || last_char==')');

	label_string[length]='\0';

	if (length==0)
		return 0;
	if (length==MAX_STRING_LENGTH)
		warning_i ("Label too long, extra characters ignored at line %d\n",line_number);
	return 1;
}

static void parse_record_field_types (char *label_string)
{
	if (!try_parse_record_field_types (label_string))
		abc_parser_error_i ("Record field types expected at line %d",line_number);
	skip_spaces_and_tabs();
}

// Calculate "hash" of string
static int instruction_hash(char *s) {
	int h = 0;
	while (*s != '\0')
		h += *s++;
	return h;
}

// Given a string return the instruction
static int parse_instruction_string(char *string) {
	int length;

	length=0;
	if (is_alpha_num_character (last_char)){
		do {
			if (length<MAX_STRING_LENGTH)
				string[length++] = last_char;
			last_char=getc (abc_file);
		} while (is_alpha_num_character (last_char));
	}
	string[length]='\0';

	skip_spaces_and_tabs();

	if (length==0)
		return 0;
	if (length==MAX_STRING_LENGTH)
		warning_i ("Instruction too long, extra characters ignored at line %d\n",line_number);
	return 1;
}

// Given a string, return the instructionpointer from the table
static InstructionP get_instruction(char *s) {
	struct in_name *in_name;

	in_name=&in_name_table [instruction_hash (s) % IN_NAME_TABLE_SIZE];

	while (in_name!=NULL && in_name->in_name_instruction!=NULL){
		if (strcmp (in_name->in_name_instruction->instruction_name,s)==0)
			return in_name->in_name_instruction;

		in_name=in_name->in_name_next;
	}
	return NULL;
}

// Reads untill end of line
static void skip_to_end_of_line() {
	while(last_char!='\xa' && last_char!='\xd' && last_char!=EOF)
		last_char = getc(abc_file);
}

// Parse a single line
static int parse_line() {
	InstructionP instruction;
	STRING s;

	if (!skip_spaces_and_tabs()
			&& last_char != '.' && last_char != '|' && last_char != '\xd' && last_char != '\xa' && last_char != EOF)
	{
		parse_label(s);
		skip_spaces_and_tabs();
		code_label(s);
	}

	if (last_char == '|'){
		skip_to_end_of_line();
		if (last_char == EOF)
			return 0;

		if (last_char == '\xd'){
			last_char = getc(abc_file);
			if (last_char == '\xa')
				last_char = getc(abc_file);
			return 1;
		}

		last_char = getc(abc_file);
		return 1;
	}

	if (!parse_instruction_string(s)){
		if (last_char == '|')
			skip_to_end_of_line();
		if (last_char == EOF)
			return 0;
		if (last_char == '\xd'){
			last_char = getc(abc_file);
			if (last_char == '\xa')
				last_char = getc(abc_file);
			return 1;
		} else if (last_char == '\xa'){
			last_char = getc(abc_file);
			return 1;
		} else {
			abc_parser_error_i("Instruction expected at line %d\n",line_number);
		}
	}

	instruction = get_instruction(s);
	if (instruction == NULL){
		fprintf (stderr,"Invalid instruction %s at line %d\n",s,line_number);
		skip_to_end_of_line();
	} else {
		if (!instruction->instruction_parse_function(instruction))
			return 0;
	}

	skip_spaces_and_tabs();

	if (last_char=='|')
		skip_to_end_of_line();

	if (last_char=='\xd'){
		last_char=getc (abc_file);
		if (last_char=='\xa')
			last_char=getc (abc_file);
	} else if (last_char=='\xa') {
		last_char=getc (abc_file);
	} else if (last_char!=EOF) {
		abc_parser_error_i ("Too many arguments for instruction at line %d\n",line_number);
	}

	return 1;	
}

// Sets initial variables for the parser
static void initialize_file_parsing() {
	line_number = 1;
	last_d = 0;
	last_jsr_with_d = 0;

	last_char = getc(abc_file);
}

// Parses the files line by line
int parse_file(FILE *file) {
	if (setjmp(exit_abc_parser_buffer) == 0){
		abc_file = file;

		initialize_file_parsing();

		while(parse_line()) {
			++line_number;
		}

		return 0;
	} else {
		return 1;
	}
}

// Allocate memory for rest of the program
static void initialize_memory() {
	memory_list = NULL;

	first_mem_block_size = 0;
	heap=memory_allocate(sizeof(struct memory_block_list));
	heap->mem_next_block = NULL;

	fast_mem_block_size = 0;
	memory_list=memory_allocate(sizeof(struct memory_block_list));
	memory_list->mem_next_block = NULL;
	fast_mem_block=memory_list->mem_block;
}

void *fast_memory_allocate (int size)
{
#ifdef sparc
	size=(size+7) & ~7;
#else
	size=(size+3) & ~3;
#endif
	if (fast_mem_block_size+size<=MEM_BLOCK_SIZE){
		char *memory_block;
		
		memory_block=&fast_mem_block[fast_mem_block_size];
		fast_mem_block_size+=size;
		return memory_block;
	} else {
		struct memory_block_list *new_memory_block;
		
#if USE_SYSTEM_ALLOC
		new_memory_block=system_memory_allocate_type (struct memory_block_list);
#else
		new_memory_block=memory_allocate (sizeof (struct memory_block_list));
#endif
		new_memory_block->mem_next_block=memory_list;
		memory_list=new_memory_block;
		
		fast_mem_block=new_memory_block->mem_block;
		fast_mem_block_size=size;
		
		return fast_mem_block;
	}
}

// Create Array to store if a character is a alpha_num + extra character
static void initialize_alpha_num_table() {
	uint8_t i;
	static unsigned char extra_alpha_nums[]="@#$%^&*-+/=<>\\_~.?\'\"|`!:{};";

	for (i=0; i<257; ++i)
		alpha_num_table[i]=0;

	for (i = 'a'; i<='z'; ++i)
		alpha_num_table[i+1] = 1;
	for (i = 'A'; i<='Z'; ++i)
		alpha_num_table[1+i] = 1;
	for (i = '0'; i<='9'; ++i)
		alpha_num_table[1+i] = 1;
	for (i = 0; i < sizeof(extra_alpha_nums) - 1; ++i)
		alpha_num_table[1 + extra_alpha_nums[i]] = 1;
}

// Store single instruction into table
static void put_instruction_name(char *name, int (*parse_function)(), void (code_function)()) {
	InstructionP instruction;
	struct in_name *in_name;

	// Create new element
	instruction = (InstructionP) malloc(sizeof(struct instruction));
	instruction->instruction_name = name;
	instruction->instruction_parse_function = parse_function;
	instruction->instruction_code_function = code_function;

	// Create hash table of Instructions
	in_name = &in_name_table[instruction_hash(instruction->instruction_name) % IN_NAME_TABLE_SIZE];

	// If hask table already has entry, add new entry to the front
	if(in_name->in_name_instruction != NULL){
		struct in_name *in_name_link;

		in_name_link = in_name;
		while(in_name_link->in_name_next != NULL)
			in_name_link = in_name_link->in_name_next;

		for(in_name = in_name_table; in_name < &in_name_table[IN_NAME_TABLE_SIZE]; ++in_name)
			if (in_name->in_name_instruction == NULL)
				break;

		if(in_name == &in_name_table[IN_NAME_TABLE_SIZE])
			perror ("Not enough entries in instruction hash table");

		in_name_link->in_name_next=in_name;
	}

	in_name->in_name_instruction = instruction;
}

static int parse_unsigned_integer (LONG *integer_p)
{
	LONG integer;
	
	if (!is_digit_character (last_char))
		abc_parser_error_i ("Integer expected at line %d\n",line_number);
		
	integer=last_char-'0';
	last_char=getc (abc_file);
	
	while (is_digit_character (last_char)){
		integer*=10;
		integer+=last_char-'0';
		last_char=getc (abc_file);
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
		last_char=getc (abc_file);
	}
	
	if (!is_digit_character (last_char))
		abc_parser_error_i ("Integer expected at line %d\n",line_number);
		
	integer=last_char-'0';
	last_char=getc (abc_file);

	if (last_char=='x' && integer==0){
		last_char=getc (abc_file);
		if (!is_hexdigit_character (last_char))
			abc_parser_error_i ("Integer expected at line %d\n",line_number);
		do {
			integer<<=4;
			integer+= last_char<='9' ? last_char-'0' : (last_char | 0x20)-('a'-10);
			last_char=getc (abc_file);
		} while (is_hexdigit_character (last_char));
	} else if (last_char=='o' && integer==0){
		last_char=getc (abc_file);
		if (!is_octdigit_character (last_char))
			abc_parser_error_i ("Integer expected at line %d\n",line_number);
		do {
			integer<<=3;
			integer+= last_char-'0';
			last_char=getc (abc_file);
		} while (is_octdigit_character (last_char));
	} else {
		while (is_digit_character (last_char)){
			integer*=10;
			integer+=last_char-'0';
			last_char=getc (abc_file);
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
		if ((last_char=getc (abc_file))=='A' && (last_char=getc (abc_file))=='L' &&
				(last_char=getc (abc_file))=='S' && (last_char=getc (abc_file))=='E'){
			last_char=getc (abc_file);
			skip_spaces_and_tabs();
			*boolean_p=0;
			return 1;
		}
	} else if (last_char=='T'){
		if ((last_char=getc (abc_file))=='R' && (last_char=getc (abc_file))=='U' &&
				(last_char=getc (abc_file))=='E'){
			last_char=getc (abc_file);
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

	last_char=getc (abc_file);
	
	if (!parse_string_character ((char*)character_p))
		abc_parser_error_i ("Error in character at line %d\n",line_number);

	if (last_char!='\'')
		abc_parser_error_i ("Character not terminated at line %d\n",line_number);

	last_char=getc (abc_file);
	
	skip_spaces_and_tabs();
	
	return 1;
}

static char *resize_string (char *string,int length,int max_length)
{
	if (length==MAX_STRING_LENGTH){
		char *new_string;
		int i;
		
		new_string=malloc (max_length);
		if (new_string==NULL)
			perror ("Out of memory");
		
		for (i=0; i<length; ++i)
			new_string[i]=string[i];

		return new_string;
	} else {
		string=realloc (string,max_length);
		if (string==NULL)	
			perror ("Out of memory");
		
		return string;
	}	
}

static int parse_0_or_1 (int *integer_p)
{
	int integer;
	
	if (!(last_char=='0' || last_char=='1'))
		abc_parser_error_i ("0 or 1 expected at line %d\n",line_number);
		
	integer=last_char-'0';
	
	last_char=getc (abc_file);
	skip_spaces_and_tabs();
	
	*integer_p=integer;
	
	return 1;
}

static char *parse_string2 (char *string,int *string_length_p)
{
	int length,max_length;

	if (last_char!='"')
		abc_parser_error_i ("String expected at line %d",line_number);

	last_char=getc (abc_file);
	
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

	last_char=getc (abc_file);

	if (length>=max_length){
		++max_length;
		string=resize_string (string,length,max_length);
	}
	string[length]='\0';
	*string_length_p=length;
	
	skip_spaces_and_tabs();

	return string;
}

static int parse_instruction (InstructionP instruction)
{
	instruction->instruction_code_function();
	return 1;
}

static int parse_instruction_a (InstructionP instruction)
{
	STRING a;
	
	parse_label (a);
	instruction->instruction_code_function (a);
	return 1;
}

static int parse_instruction_a_n (InstructionP instruction)
{
	STRING a;
	LONG n;
	
	parse_label (a);
	if (!parse_unsigned_integer (&n))
		return 0;
	instruction->instruction_code_function (a,(int)n);
	return 1;
}

static int parse_instruction_a_n_a (InstructionP instruction)
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

static int parse_instruction_a_n_a_a_n_a (InstructionP instruction)
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

static int parse_instruction_a_n_a_n (InstructionP instruction)
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

static int parse_instruction_a_n_n (InstructionP instruction)
{
	STRING a;
	LONG n1,n2;
	
	parse_label (a);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;
	instruction->instruction_code_function (a,(int)n1,(int)n2);
	return 1;
}

static int parse_instruction_a_n_n_a (InstructionP instruction)
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

static int parse_instruction_a_n_n_n (InstructionP instruction)
{
	STRING a;
	LONG n1,n2,n3;
	
	parse_label (a);
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || !parse_unsigned_integer (&n3))
		return 0;
	instruction->instruction_code_function (a,(int)n1,(int)n2,(int)n3);
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
		last_char=getc (abc_file);
	} while (last_char=='0' || last_char=='1');

	*s_p='\0';
}

static int parse_instruction_a_n_n_b (InstructionP instruction)
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

static int parse_instruction_a_n_n_n_b (InstructionP instruction)
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

static int parse_instruction_a_n_n_n_n (InstructionP instruction)
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

static int parse_instruction_a_n_n_n_n_n (InstructionP instruction)
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

static int parse_instruction_b (InstructionP instruction)
{
	int b;
	
	if (!parse_boolean (&b))
		return 0;
	instruction->instruction_code_function (b);
	return 1;
}

static int parse_instruction_b_n (InstructionP instruction)
{
	int b;
	LONG n;
	
	if (!parse_boolean (&b) || !parse_unsigned_integer (&n))
		return 0;
	instruction->instruction_code_function (b,(int)n);
	return 1;
}

static int parse_instruction_c (InstructionP instruction)
{
	unsigned char c;
	
	if (!parse_character (&c))
		return 0;
	instruction->instruction_code_function (c);
	return 1;
}

static int parse_instruction_c_c_n_a_a (InstructionP instruction)
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

static int parse_instruction_c_n (InstructionP instruction)
{
	unsigned char c;
	LONG n;
	
	if (!parse_character (&c) || !parse_unsigned_integer (&n))
		return 0;
	instruction->instruction_code_function (c,(int)n);
	return 1;
}

static int parse_instruction_c_n_a (InstructionP instruction)
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

static int parse_instruction_i (InstructionP instruction)
{
	CleanInt i;
	
	if (!parse_clean_integer (&i))
		return 0;
	instruction->instruction_code_function (i);
	return 1;
}

static int parse_instruction_i_i_n_a_a (InstructionP instruction)
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

static int parse_instruction_i_n (InstructionP instruction)
{
	CleanInt i;
	LONG n;

	if (!parse_clean_integer (&i) || !parse_unsigned_integer (&n))
		return 0;
	instruction->instruction_code_function (i,(int)n);
	return 1;
}

static int parse_instruction_i_n_a (InstructionP instruction)
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

static int parse_instruction_l (InstructionP instruction)
{
	STRING a;
	
	parse_label_without_conversion (a);
	instruction->instruction_code_function (a);
	return 1;
}

static int parse_instruction_n (InstructionP instruction)
{
	LONG n;
	
	if (!parse_unsigned_integer (&n))
		return 0;
	
	instruction->instruction_code_function ((int)n);
	return 1;
}

static int parse_instruction_n_a (InstructionP instruction)
{
	STRING a;
	LONG n;
	
	if (!parse_unsigned_integer (&n))
		return 0;
	parse_label (a);
	instruction->instruction_code_function ((int)n,a);
	return 1;
}

static int parse_instruction_n_a_n_a_a_n_a (InstructionP instruction)
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

static int parse_instruction_n_b (InstructionP instruction)
{
	int b;
	LONG n;
	
	if (!parse_unsigned_integer (&n) || !parse_boolean (&b))
		return 0;
	instruction->instruction_code_function ((int)n,b);
	return 1;
}

static int parse_instruction_n_n (InstructionP instruction)
{
	LONG n1,n2;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2);
	return 1;
}

static int parse_instruction_n_n_n (InstructionP instruction)
{
	LONG n1,n2,n3;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2,(int)n3);
	return 1;
}

static int parse_instruction_n_n_n_n (InstructionP instruction)
{
	LONG n1,n2,n3,n4;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2,(int)n3,(int)n4);
	return 1;
}

static int parse_instruction_n_n_n_n_n (InstructionP instruction)
{
	LONG n1,n2,n3,n4,n5;
	
	if (!parse_unsigned_integer (&n1) || !parse_unsigned_integer (&n2) || 
		!parse_unsigned_integer (&n3) || !parse_unsigned_integer (&n4) ||
		!parse_unsigned_integer (&n5))
		return 0;
	instruction->instruction_code_function ((int)n1,(int)n2,(int)n3,(int)n4,(int)n5);
	return 1;
}

static int parse_instruction_n_n_n_n_n_n_n (InstructionP instruction)
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

static int parse_instruction_on (InstructionP instruction)
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

static int parse_instruction_s (InstructionP instruction)
{
	STRING s;
	int length;
	
	if (!parse_string (s,&length))
		return 0;
	instruction->instruction_code_function (s,length);
	return 1;
}

static int parse_instruction_s2 (InstructionP instruction)
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

static int parse_instruction_s2_n_a (InstructionP instruction)
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

static int parse_directive (InstructionP instruction)
{
	instruction->instruction_code_function();
	return 1;
}

static int parse_directive_a (InstructionP instruction)
{
	LONG n;
	STRING s;

	if (!parse_integer (&n))
		return 0;
	
	parse_label (s);
	instruction->instruction_code_function ((int)n,s);

	return 1;
}

static int parse_directive_depend (InstructionP instruction)
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

static int parse_directive_desc (InstructionP instruction)
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

static int parse_directive_desc0 (InstructionP instruction)
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

static int parse_directive_descn (InstructionP instruction)
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

static int parse_directive_implab (InstructionP instruction)
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

static int parse_directive_labels (InstructionP instruction)
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

static int parse_directive_module (InstructionP instruction)
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

static int parse_directive_n (InstructionP instruction)
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

static int parse_directive_nu (InstructionP instruction)
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

static int parse_directive_n_l (InstructionP instruction)
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

static int parse_directive_n_n_t (InstructionP instruction)
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

static int parse_directive_label (InstructionP instruction)
{
	STRING s;
	
	parse_label (s);
	instruction->instruction_code_function (s);
	skip_spaces_and_tabs();
	
	return 1;
}

static int parse_directive_record (InstructionP instruction)
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

static int parse_directive_string (InstructionP instruction)
{
	STRING a,s;
	int l;

	parse_label (a);
	if (!parse_string (s,&l))
		return 0;
	instruction->instruction_code_function (a,s,l);
	return 1;
}

// Load database of instructions
static void put_instructions_in_table() {
	put_instruction_name ("addI",                  parse_instruction,               code_addI );
	put_instruction_name ("and%",                  parse_instruction,               code_and );
	put_instruction_name ("build",                 parse_instruction_a_n_a,         code_build );
	put_instruction_name ("buildB",                parse_instruction_b,             code_buildB );
	put_instruction_name ("buildAC",               parse_instruction_s2,            code_buildAC );
	put_instruction_name ("buildh",                parse_instruction_a_n,           code_buildh );
	put_instruction_name ("buildB_b",              parse_instruction_n,             code_buildB_b );
	put_instruction_name ("buildC",                parse_instruction_c,             code_buildC );
	put_instruction_name ("buildC_b",              parse_instruction_n,             code_buildC_b );
	put_instruction_name ("buildF_b",              parse_instruction_n,             code_buildF_b );
	put_instruction_name ("buildI",                parse_instruction_i,             code_buildI );
	put_instruction_name ("buildI_b",              parse_instruction_n,             code_buildI_b );
	put_instruction_name ("buildhr",               parse_instruction_a_n_n,         code_buildhr );
	put_instruction_name ("build_r",               parse_instruction_a_n_n_n_n,     code_build_r );
	put_instruction_name ("build_u",               parse_instruction_a_n_n_a,       code_build_u );
	put_instruction_name ("create",                parse_instruction_on,            code_create );
	put_instruction_name ("create_array",          parse_instruction_a_n_n,         code_create_array );
	put_instruction_name ("create_array_",         parse_instruction_a_n_n,         code_create_array_ );
	put_instruction_name ("decI",                  parse_instruction,               code_decI );
	put_instruction_name ("divI",                  parse_instruction,               code_divI );
	put_instruction_name ("eqAC_a",                parse_instruction_s2,            code_eqAC_a );
	put_instruction_name ("eqD_b",                 parse_instruction_a_n,           code_eqD_b );
	put_instruction_name ("eqB",                   parse_instruction,               code_eqB );
	put_instruction_name ("eqB_a",                 parse_instruction_b_n,           code_eqB_a );
	put_instruction_name ("eqB_b",                 parse_instruction_b_n,           code_eqB_b );
	put_instruction_name ("eqC",                   parse_instruction,               code_eqC );
	put_instruction_name ("eqC_a",                 parse_instruction_c_n,           code_eqC_a );
	put_instruction_name ("eqC_b",                 parse_instruction_c_n,           code_eqC_b );
	put_instruction_name ("eqI",                   parse_instruction,               code_eqI );
	put_instruction_name ("eqI_a",                 parse_instruction_i_n,           code_eqI_a );
	put_instruction_name ("eqI_b",                 parse_instruction_i_n,           code_eqI_b );
	put_instruction_name ("eq_desc",               parse_instruction_a_n_n,         code_eq_desc );
	put_instruction_name ("exit_false",            parse_instruction_a,             code_exit_false );
	put_instruction_name ("fill",                  parse_instruction_a_n_a_n,       code_fill );
	put_instruction_name ("fill1",                 parse_instruction_a_n_n_b,       code_fill1 );
	put_instruction_name ("fill1_r",               parse_instruction_a_n_n_n_b,     code_fill1_r );
	put_instruction_name ("fill2",                 parse_instruction_a_n_n_b,       code_fill2 );
	put_instruction_name ("fill2_r",               parse_instruction_a_n_n_n_b,     code_fill2_r );
	put_instruction_name ("fill3",                 parse_instruction_a_n_n_b,       code_fill3 );
	put_instruction_name ("fill3_r",               parse_instruction_a_n_n_n_b,     code_fill3_r );
	put_instruction_name ("fillB_b",               parse_instruction_n_n,           code_fillB_b );
	put_instruction_name ("fillC_b",               parse_instruction_n_n,           code_fillC_b );
	put_instruction_name ("fillF_b",               parse_instruction_n_n,           code_fillF_b );
	put_instruction_name ("fillI_b",               parse_instruction_n_n,           code_fillI_b );
	put_instruction_name ("fillcaf",               parse_instruction_a_n_n,         code_fillcaf );
	put_instruction_name ("fillh",                 parse_instruction_a_n_n,         code_fillh );
	put_instruction_name ("fill_a",                parse_instruction_n_n,           code_fill_a );
	put_instruction_name ("fill_r",                parse_instruction_a_n_n_n_n_n,   code_fill_r );
	put_instruction_name ("get_node_arity",        parse_instruction_n,             code_get_node_arity );
	put_instruction_name ("gtI",                   parse_instruction,               code_gtI );
	put_instruction_name ("halt",                  parse_instruction,               code_halt );
	put_instruction_name ("incI",                  parse_instruction,               code_incI );
	put_instruction_name ("jmp",                   parse_instruction_a,             code_jmp );
	put_instruction_name ("jmp_ap",                parse_instruction_n,             code_jmp_ap );
	put_instruction_name ("jmp_eval",              parse_instruction,               code_jmp_eval );
	put_instruction_name ("jmp_eval_upd",          parse_instruction,               code_jmp_eval_upd );
	put_instruction_name ("jmp_false",             parse_instruction_a,             code_jmp_false );
	put_instruction_name ("jmp_true",              parse_instruction_a,             code_jmp_true );
	put_instruction_name ("jsr",                   parse_instruction_a,             code_jsr );
	put_instruction_name ("jsr_eval",              parse_instruction_n,             code_jsr_eval );
	put_instruction_name ("jsr_ap",                parse_instruction_n,             code_jsr_ap );
	put_instruction_name ("ltC",                   parse_instruction,               code_ltC );
	put_instruction_name ("ltI",                   parse_instruction,               code_ltI );
	put_instruction_name ("mulI",                  parse_instruction,               code_mulI );
	put_instruction_name ("negI",                  parse_instruction,               code_negI );
	put_instruction_name ("notB",                  parse_instruction,               code_notB );
	put_instruction_name ("not%",                  parse_instruction,               code_not );
	put_instruction_name ("or%",                   parse_instruction,               code_or );
	put_instruction_name ("pop_a",                 parse_instruction_n,             code_pop_a );
	put_instruction_name ("pop_b",                 parse_instruction_n,             code_pop_b );
	put_instruction_name ("print",                 parse_instruction_s,             code_print );
	put_instruction_name ("print_sc",              parse_instruction_s,             code_print_sc );
	put_instruction_name ("print_symbol_sc",       parse_instruction_n,             code_print_symbol_sc );
	put_instruction_name ("pushB",                 parse_instruction_b,             code_pushB );
	put_instruction_name ("pushC",                 parse_instruction_c,             code_pushC );
	put_instruction_name ("pushB_a",               parse_instruction_n,             code_pushB_a );
	put_instruction_name ("pushC_a",               parse_instruction_n,             code_pushC_a );
	put_instruction_name ("pushD",                 parse_instruction_a,             code_pushD );
	put_instruction_name ("pushD_a",               parse_instruction_n,             code_pushD_a );
	put_instruction_name ("pushF_a",               parse_instruction_n,             code_pushF_a );
	put_instruction_name ("pushI",                 parse_instruction_i,             code_pushI );
	put_instruction_name ("pushI_a",               parse_instruction_n,             code_pushI_a );
	put_instruction_name ("pushcaf",               parse_instruction_a_n_n,         code_pushcaf );
	put_instruction_name ("push_a",                parse_instruction_n,             code_push_a );
	put_instruction_name ("push_a_b",              parse_instruction_n,             code_push_a_b );
	put_instruction_name ("push_b",                parse_instruction_n,             code_push_b );
	put_instruction_name ("push_arg",              parse_instruction_n_n_n,         code_push_arg );
	put_instruction_name ("push_args",             parse_instruction_n_n_n,         code_push_args );
	put_instruction_name ("push_args_u",           parse_instruction_n_n_n,         code_push_args_u );
	put_instruction_name ("push_array",            parse_instruction_n,             code_push_array );
	put_instruction_name ("push_arraysize",        parse_instruction_a_n_n,         code_push_arraysize );
	put_instruction_name ("push_arg_b",            parse_instruction_n,             code_push_arg_b );
	put_instruction_name ("push_node",             parse_instruction_a_n,           code_push_node );
	put_instruction_name ("push_node_u",           parse_instruction_a_n_n,         code_push_node_u );
	put_instruction_name ("push_r_arg_u",          parse_instruction_n_n_n_n_n_n_n, code_push_r_arg_u );
	put_instruction_name ("push_r_args",           parse_instruction_n_n_n,         code_push_r_args );
	put_instruction_name ("push_r_args_a",         parse_instruction_n_n_n_n_n,     code_push_r_args_a );
	put_instruction_name ("push_r_args_b",         parse_instruction_n_n_n_n_n,     code_push_r_args_b );
	put_instruction_name ("push_r_args_u",         parse_instruction_n_n_n,         code_push_r_args_u );
	put_instruction_name ("remI",                  parse_instruction,               code_remI );
	put_instruction_name ("replace",               parse_instruction_a_n_n,         code_replace );
	put_instruction_name ("repl_arg",              parse_instruction_n_n,           code_repl_arg );
	put_instruction_name ("repl_args",             parse_instruction_n_n,           code_repl_args );
	put_instruction_name ("repl_r_args",           parse_instruction_n_n,           code_repl_r_args );
	put_instruction_name ("repl_r_args_a",         parse_instruction_n_n_n_n,       code_repl_r_args_a );
	put_instruction_name ("select",                parse_instruction_a_n_n,         code_select );
	put_instruction_name ("shiftl%",               parse_instruction,               code_shiftl );
	put_instruction_name ("shiftr%",               parse_instruction,               code_shiftr );
	put_instruction_name ("subI",                  parse_instruction,               code_subI );
	put_instruction_name ("rtn",                   parse_instruction,               code_rtn );
	put_instruction_name ("testcaf",               parse_instruction_a,             code_testcaf );
	put_instruction_name ("update",                parse_instruction_a_n_n,         code_update );
	put_instruction_name ("update_a",              parse_instruction_n_n,           code_update_a );
	put_instruction_name ("updatepop_a",           parse_instruction_n_n,           code_updatepop_a );
	put_instruction_name ("update_b",              parse_instruction_n_n,           code_update_b );
	put_instruction_name ("updatepop_b",           parse_instruction_n_n,           code_updatepop_b );
	put_instruction_name ("xor%",                  parse_instruction,               code_xor );
	put_instruction_name ("CtoAC",                 parse_instruction,               code_CtoAC );
	put_instruction_name ("CtoI",                  parse_instruction,               code_CtoI );
	put_instruction_name ("ItoC",                  parse_instruction,               code_ItoC );

	put_instruction_name ("addIi",                 parse_instruction_i,             code_addIi );
	put_instruction_name ("andIi",                 parse_instruction_i,             code_andIi );
	put_instruction_name ("andIio",                parse_instruction_i_n,           code_andIio );
	put_instruction_name ("buildh0_dup_a",         parse_instruction_a_n,           code_buildh0_dup_a );
	put_instruction_name ("buildh0_dup2_a",        parse_instruction_a_n,           code_buildh0_dup2_a );
	put_instruction_name ("buildh0_dup3_a",        parse_instruction_a_n,           code_buildh0_dup3_a );
	put_instruction_name ("buildh0_put_a",         parse_instruction_a_n,           code_buildh0_put_a );
	put_instruction_name ("buildh0_put_a_jsr",     parse_instruction_a_n_a,         code_buildh0_put_a_jsr );
	put_instruction_name ("buildho2",              parse_instruction_a_n_n,         code_buildho2 );
	put_instruction_name ("buildo1",               parse_instruction_a_n,           code_buildo1 );
	put_instruction_name ("buildo2",               parse_instruction_a_n_n,         code_buildo2 );
	put_instruction_name ("dup_a",                 parse_instruction_n,             code_dup_a );
	put_instruction_name ("dup2_a",                parse_instruction_n,             code_dup2_a );
	put_instruction_name ("dup3_a",                parse_instruction_n,             code_dup3_a );
	put_instruction_name ("fill_a01_pop_rtn",      parse_instruction,               code_fill_a01_pop_rtn );
	put_instruction_name ("exchange_a",            parse_instruction_n_n,           code_exchange_a );
	put_instruction_name ("geC",                   parse_instruction,               code_geC );
	put_instruction_name ("jmp_b_false",           parse_instruction_n_a,           code_jmp_b_false );
	put_instruction_name ("jmp_eqACio",            parse_instruction_s2_n_a,        code_jmp_eqACio );
	put_instruction_name ("jmp_eqC_b",             parse_instruction_c_n_a,         code_jmp_eqC_b );
	put_instruction_name ("jmp_eqC_b2",            parse_instruction_c_c_n_a_a,     code_jmp_eqC_b2 );
	put_instruction_name ("jmp_eqD_b",             parse_instruction_a_n_a,         code_jmp_eqD_b );
	put_instruction_name ("jmp_eqD_b2",            parse_instruction_a_n_a_a_n_a,   code_jmp_eqD_b2 );
	put_instruction_name ("jmp_eqI",               parse_instruction_a,             code_jmp_eqI );
	put_instruction_name ("jmp_eqI_b",             parse_instruction_i_n_a,         code_jmp_eqI_b );
	put_instruction_name ("jmp_eqI_b2",            parse_instruction_i_i_n_a_a,     code_jmp_eqI_b2 );
	put_instruction_name ("jmp_eq_desc",           parse_instruction_a_n_n_a,       code_jmp_eq_desc );
	put_instruction_name ("jmp_geI",               parse_instruction_a,             code_jmp_geI );
	put_instruction_name ("jmp_ltI",               parse_instruction_a,             code_jmp_ltI );
	put_instruction_name ("jmp_neC_b",             parse_instruction_c_n_a,         code_jmp_neC_b );
	put_instruction_name ("jmp_neI",               parse_instruction_a,             code_jmp_neI );
	put_instruction_name ("jmp_neI_b",             parse_instruction_i_n_a,         code_jmp_neI_b );
	put_instruction_name ("jmp_ne_desc",           parse_instruction_a_n_n_a,       code_jmp_ne_desc );
	put_instruction_name ("jmp_o_geI",             parse_instruction_n_a,           code_jmp_o_geI );
	put_instruction_name ("jmp_o_geI_arraysize_a", parse_instruction_a_n_n_a,       code_jmp_o_geI_arraysize_a );
	put_instruction_name ("ltIi",                  parse_instruction_i,             code_ltIi );
	put_instruction_name ("neI",                   parse_instruction,               code_neI );
	put_instruction_name ("swap_a",                parse_instruction_n,             code_swap_a );
	put_instruction_name ("swap_a1",               parse_instruction,               code_swap_a1 );
	put_instruction_name ("swap_b1",               parse_instruction,               code_swap_b1 );
	put_instruction_name ("pop_a_jmp",             parse_instruction_n_a,           code_pop_a_jmp );
	put_instruction_name ("pop_a_jsr",             parse_instruction_n_a,           code_pop_a_jsr );
	put_instruction_name ("pop_a_rtn",             parse_instruction_n,             code_pop_a_rtn );
	put_instruction_name ("pop_ab_rtn",            parse_instruction_n_n,           code_pop_ab_rtn );
	put_instruction_name ("pop_b_jmp",             parse_instruction_n_a,           code_pop_b_jmp );
	put_instruction_name ("pop_b_jsr",             parse_instruction_n_a,           code_pop_b_jsr );
	put_instruction_name ("pop_b_pushB",           parse_instruction_n_b,           code_pop_b_pushB );
	put_instruction_name ("pop_b_rtn",             parse_instruction_n,             code_pop_b_rtn );
	put_instruction_name ("pushD_a_jmp_eqD_b2",    parse_instruction_n_a_n_a_a_n_a, code_pushD_a_jmp_eqD_b2 );
	put_instruction_name ("push_a_jsr",            parse_instruction_n_a,           code_push_a_jsr );
	put_instruction_name ("push_arraysize_a",      parse_instruction_a_n_n_n,       code_push_arraysize_a );
	put_instruction_name ("push_b_incI",           parse_instruction_n,             code_push_b_incI );
	put_instruction_name ("push_b_jsr",            parse_instruction_n_a,           code_push_b_jsr );
	put_instruction_name ("push_jsr_eval",         parse_instruction_n,             code_push_jsr_eval );
	put_instruction_name ("push_a2",               parse_instruction_n_n,           code_push_a2 );
	put_instruction_name ("push_ab",               parse_instruction_n_n,           code_push_ab );
	put_instruction_name ("push_b2",               parse_instruction_n_n,           code_push_b2 );
	put_instruction_name ("push2_a",               parse_instruction_n,             code_push2_a );
	put_instruction_name ("push2_b",               parse_instruction_n,             code_push2_b );
	put_instruction_name ("push3_a",               parse_instruction_n,             code_push3_a );
	put_instruction_name ("push3_b",               parse_instruction_n,             code_push3_b );
	put_instruction_name ("push_update_a",         parse_instruction_n_n,           code_push_update_a );
	put_instruction_name ("put_a",                 parse_instruction_n,             code_put_a );
	put_instruction_name ("put_b",                 parse_instruction_n,             code_put_b );
	put_instruction_name ("selectoo",              parse_instruction_a_n_n_n_n,     code_selectoo );
	put_instruction_name ("update2_a",             parse_instruction_n_n,           code_update2_a );
	put_instruction_name ("update2_b",             parse_instruction_n_n,           code_update2_b );
	put_instruction_name ("update2pop_a",          parse_instruction_n_n,           code_update2pop_a );
	put_instruction_name ("update2pop_b",          parse_instruction_n_n,           code_update2pop_b );
	put_instruction_name ("update3_a",             parse_instruction_n_n,           code_update3_a );
	put_instruction_name ("update3_b",             parse_instruction_n_n,           code_update3_b );
	put_instruction_name ("update3pop_a",          parse_instruction_n_n,           code_update3pop_a );
	put_instruction_name ("update3pop_b",          parse_instruction_n_n,           code_update3pop_b );
	put_instruction_name ("update4_a",             parse_instruction_n_n,           code_update4_a );
	put_instruction_name ("updates2_a",            parse_instruction_n_n_n,         code_updates2_a );
	put_instruction_name ("updates2_a_pop_a",      parse_instruction_n_n_n_n,       code_updates2_a_pop_a );
	put_instruction_name ("updates2_b",            parse_instruction_n_n_n,         code_updates2_b );
	put_instruction_name ("updates2pop_a",         parse_instruction_n_n_n,         code_updates2pop_a );
	put_instruction_name ("updates2pop_b",         parse_instruction_n_n_n,         code_updates2pop_b );
	put_instruction_name ("updates3_a",            parse_instruction_n_n_n_n,       code_updates3_a );
	put_instruction_name ("updates3_b",            parse_instruction_n_n_n_n,       code_updates3_b );
	put_instruction_name ("updates3pop_a",         parse_instruction_n_n_n_n,       code_updates3pop_a );
	put_instruction_name ("updates3pop_b",         parse_instruction_n_n_n_n,       code_updates3pop_b );
	put_instruction_name ("updates4_a",            parse_instruction_n_n_n_n_n,     code_updates4_a );

	put_instruction_name (".a",                    parse_directive_a,               code_a );
	put_instruction_name (".algtype",              parse_instruction_n,             code_algtype );
	put_instruction_name (".caf",                  parse_instruction_a_n_n,         code_caf );
	put_instruction_name (".comp",                 parse_directive_n_l,             code_comp    );
	put_instruction_name (".d",                    parse_directive_n_n_t,           code_d );
	put_instruction_name (".depend",               parse_directive_depend,          code_depend );
	put_instruction_name (".desc",                 parse_directive_desc,            code_desc );
	put_instruction_name (".desc0",                parse_directive_desc0,           code_desc0 );
	put_instruction_name (".descn",                parse_directive_descn,           code_descn );
	put_instruction_name (".descs",                parse_directive_desc,            code_descs );
	put_instruction_name (".descexp",              parse_directive_desc,            code_descexp );
	put_instruction_name (".end",                  parse_directive,                 code_dummy );
	put_instruction_name (".endinfo",              parse_directive,                 code_dummy    );
	put_instruction_name (".export",               parse_directive_labels,          code_export );
	put_instruction_name (".impdesc",              parse_directive_label,           code_impdesc );
	put_instruction_name (".impmod",               parse_instruction_l,             code_impmod );
	put_instruction_name (".implab",               parse_directive_implab,          code_implab );
	put_instruction_name (".inline",               parse_directive_label,           code_dummy );
	put_instruction_name (".keep",                 parse_instruction_n_n,           code_dummy );
	put_instruction_name (".module",               parse_directive_module,          code_module );
	put_instruction_name (".n",                    parse_directive_n,               code_n );
	put_instruction_name (".nu",                   parse_directive_nu,              code_nu );
	put_instruction_name (".o",                    parse_directive_n_n_t,           code_o );
	put_instruction_name (".record",               parse_directive_record,          code_record );
	put_instruction_name (".start",                parse_directive_label,           code_start );
	put_instruction_name (".string",               parse_directive_string,          code_string );
}

// Create and Load instruction table
static void init_instruction_name_table() {
	// Create empty instruction list/table
	struct in_name *local_in_name_table;

	local_in_name_table = (struct in_name*) safe_malloc(sizeof(struct in_name) * IN_NAME_TABLE_SIZE);

	uint8_t i;
	for (i = 0; i < IN_NAME_TABLE_SIZE; ++i){
		local_in_name_table[i].in_name_next = NULL;
		local_in_name_table[i].in_name_instruction = NULL;
	}

	in_name_table = local_in_name_table;

	// Load Instructions into table
	put_instructions_in_table();
}

// Set up datastructures for the parser
void init_parser() {
	init_instruction_name_table();
	initialize_alpha_num_table();
}

int main(int argc,char **argv)
{
	int arg_n;

	if(argc < 2){
		printf ("abc file name missing\n");
		return 0;
	}

	init_parser();

	initialize_memory();
	initialize_code();

	for(arg_n=1; arg_n<argc; ++arg_n) {
		char *file_name;
		FILE *abc_file;

		file_name=argv[arg_n];
		if(!strcmp (file_name,"-l")) {
			list_code=1;
			continue;
		}

		abc_file=fopen (file_name,"r");

		if(abc_file==NULL) {
			fprintf (stderr,"file \"%s\" could not be opened.\n", file_name);
			return 0;
		}

		setvbuf(abc_file, NULL,_IOFBF,8192);

		parse_file(abc_file);
		fclose(abc_file);
		code_next_module();
	}

	(void)relocate_code_and_data(0);

	write_program();
	return 0;
}
