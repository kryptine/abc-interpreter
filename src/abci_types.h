
typedef unsigned char UB;
typedef unsigned short US;
typedef unsigned int UI;
typedef short SS;
typedef int SI;

#if defined (LINUX) && defined (G_A64)
# define LONG int
# define ULONG unsigned int
#else
# define LONG long
# define ULONG unsigned long
#endif

#define CleanInt LONG
