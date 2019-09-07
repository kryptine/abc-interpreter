#ifdef MACH_O64
# define NEWLINE_CHAR '\r'
#else
# define NEWLINE_CHAR '\n'
#endif

#define F_READ_TEXT 0
#define F_WRITE_TEXT 1
#define F_APPEND_TEXT 2
#define F_READ_DATA 3
#define F_WRITE_DATA 4
#define F_APPEND_DATA 5

#define F_IS_TEXT_MODE(m) (0<=m && m<=2)
#define F_IS_DATA_MODE(m) (3<=m && m<=5)

static char *file_modes[]={"r","w","a","rb","wb","ab"};

struct file {
	FILE *file_handle;
	int file_mode;
};

static struct file clean_stdinout;
static struct file clean_stderr;

#define IO_error(s) do { EPRINTF("IO error: %s\n",s); exit(1); } while (0)

static int stdio_open=0;

static int last_readLineF_failed=0;
static long clean_get_line(char *dest,long max_length) {
	for (long length=1; length<=max_length; length++) {
		char c=getchar();
		*dest++=c;
		if (c==NEWLINE_CHAR)
			return length;
	}

	return -1;
}
