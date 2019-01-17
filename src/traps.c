#include "interpret.h"
#include "traps.h"
#include "util.h"

void clean_readLineF (void)
{
	BC_WORD *bsp,*hp;
	unsigned int l,max_l;
	unsigned char *s;
	FILE *file;
/*
	printf ("clean_readLineF\n");
*/
	bsp=g_bsp;
	hp=g_hp;

	if ( (g_heap_free -= 2) < 0){
		EPRINTF("clean_readLineF gc\n");
		EXIT(NULL,1);
	}

	file=(FILE*)bsp[1];

	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=0;
	*++g_asp = (BC_WORD)hp;
	hp+=2;

	s=(unsigned char *)hp;
	max_l = g_heap_free<<2;
	l=0;

	while (l<max_l){
		int c;

		c=getc (file);
		if (c==EOF){
			unsigned int lw;

			hp[-1]=l;
			bsp[0]+=l;

			lw=(l+3)>>2;
			g_heap_free -= lw;
			g_hp = hp+lw;
			return;
		}
/*
		putchar (c);
*/
		if (c=='\n'){
			unsigned int lw;

			s[l++]=c;
			hp[-1]=l;
			bsp[0]+=l;

			lw=(l+3)>>2;
			g_heap_free -= lw;
			g_hp = hp+lw;
			return;
		}
		s[l++]=c;
	}

	EPRINTF("clean_readLineF gc\n");
	EXIT(NULL,1);
}

void clean_sliceAC (void)
{
	uint32_t *s,n_words;
	int32_t l,first_i,end_i,i;
	unsigned char *s_p,*new_s_p;

	s=(uint32_t*)g_asp[0];
	l=s[IF_INT_64_OR_32(2,1)];
	s_p=(unsigned char*)&s[IF_INT_64_OR_32(4,2)];

	first_i=g_bsp[0];
	end_i=g_bsp[1]+1;

	if (first_i<0)
		first_i=0;
	if (end_i>l)
		end_i=l;
	l=end_i-first_i;

	n_words=IF_INT_64_OR_32((l+7)/8, (l+3)/4);
	if ((g_heap_free -= 2+n_words) < 0){
		g_heap_free += 2+n_words;
		trap_needs_gc = 1;
		return;
	}

	g_bsp += 2;
	g_hp[0]=(BC_WORD)&__STRING__+2;
	g_hp[1]=0;
	g_asp[0]=(BC_WORD)g_hp;
	g_hp+=2;
	g_hp[-1]=l;

	new_s_p=(unsigned char*)g_hp;
	s_p=&s_p[first_i];
	g_hp+=n_words;

	for (i=0; i<l; ++i)
		new_s_p[i]=s_p[i];
}

void clean_ItoAC (void)
{
	BC_WORD *asp,*bsp,*hp;
	BC_WORD_S i,l,lw;
	BC_WORD ui,t;
	unsigned char *p;

	asp=g_asp;
	bsp=g_bsp;
	hp=g_hp;

	i=bsp[0];
	bsp+=1;

	l=1;
	if (i<0){
		ui = -i;
		++l;
	} else {
		ui = i;
	}

	t=ui;
	while (t>=1000){
		t=t/1000;
		l+=3;
	}
	while (t>=10){
		t=t/10;
		++l;
	}

	lw=(l+11)>>2;

	if ( (g_heap_free -= lw) < 0){
		g_heap_free += lw;
		trap_needs_gc = 1;
		return;
	}

	*++asp=(BC_WORD)hp;

	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=l;
	p=(unsigned char*)&hp[2];
	p[0] = '-'; /* overwritten by digit if >= 0 */

	p+=l;
	while (ui>=10){
		t=ui/10;
		*--p='0'+(ui-10*t);
		ui=t;
	}

	*--p='0'+ui;

	g_hp=hp+lw;

	g_asp=asp;
	g_bsp=bsp;
}

void clean_openF (void)
{
	BC_WORD *asp,*bsp;
	char *file_name,*c_file_name;
	int *file_name_string,file_name_length,mode;
	int i;
	FILE *file;

	asp=g_asp;
	bsp=g_bsp;

	mode=bsp[0];
	file_name_string=(int*)asp[0];
	file_name_length=file_name_string[1];
	file_name=(char*)&file_name_string[2];
/*
	printf ("clean_openF %d %d %d\n",mode,(int)file_name_string,file_name_length);
*/
	c_file_name=safe_malloc (file_name_length+1);
	if (c_file_name==NULL){
		EPRINTF("clean_openF malloc failed\n");
		EXIT(NULL,1);
	}

	for (i=0; i<file_name_length; ++i)
		c_file_name[i]=file_name[i];
	c_file_name[file_name_length]='\0';
/*
	printf ("%s\n",c_file_name);
*/
	if (mode==0){
		file=fopen (c_file_name,"r");
		if (file==NULL){
			EPRINTF("clean_openF could not open file %s\n",c_file_name);
			EXIT(NULL,1);
		}
	} else if (mode==1){
		file=fopen (c_file_name,"w");
		if (file==NULL){
			EPRINTF("clean_openF could not create file %s\n",c_file_name);
			EXIT(NULL,1);
			return;
		}
	} else if (mode==3){
		file=fopen (c_file_name,"rb");
		if (file==NULL){
			EPRINTF("clean_openF could not open file %s\n",c_file_name);
			EXIT(NULL,1);
			return;
		}
	} else {
		EPRINTF("clean_openF mode not implemented %d\n",mode);
		EXIT(NULL,1);
		return;
	}

	asp-=1;
	bsp-=2;

	bsp[0]=1;
	bsp[1]=0;
	bsp[2]=(BC_WORD)file;

	g_asp=asp;
	g_bsp=bsp;
}

void clean_endF (void)
{
	BC_WORD *bsp;
	FILE *file;
	int c;

	bsp=g_bsp;

	file=(FILE*)bsp[1];
	c=getc (file);
	if (c==EOF){
		*--bsp = 1;
	} else {
		ungetc (c,file);
		*--bsp = 1;
	}
	g_bsp=bsp;
	return;
}

void clean_writeFI (void)
{
	BC_WORD *bsp;
	FILE *file;
	int i;

	bsp=g_bsp;
	i=bsp[0];
	file=(FILE*)bsp[2];
	g_bsp=bsp+1;

	fprintf (file,"%d",i);
}

void clean_writeFS (void)
{
	BC_WORD *asp,*bsp,*s;
	FILE *file;
	int n;
	unsigned char *p;

	asp=g_asp;
	bsp=g_bsp;

	s=(BC_WORD*)asp[0];
	g_asp=asp-1;

	file=(FILE*)bsp[1];

	n=s[1];
	p=(unsigned char*)&s[2];

	fwrite (p,1,n,file);
}

void clean_writeFC (void)
{
	BC_WORD *bsp;
	FILE *file;
	int c;

	bsp=g_bsp;

	c=bsp[0];
	file=(FILE*)bsp[2];

	putc (c,file);

	g_bsp=bsp+1;
}

void clean_closeF (void)
{
	BC_WORD *bsp;
	FILE *file;

	bsp=g_bsp;

	file=(FILE*)bsp[1];

	bsp[1] = fclose (file)==0;
/*
	printf ("clean_closeF\n");
*/
	g_bsp=bsp+1;
}

void clean_print_string_ (void)
{
	BC_WORD *asp;
	char *s;
	int l;

	asp=g_asp;
	s=(char*)asp[0];
	g_asp=asp-1;

	l=*(BC_WORD_S*)(s+IF_INT_64_OR_32(8,4));
	s+=IF_INT_64_OR_32(16,8);

	while (l!=0){
		PUTCHAR (*s);
		++s;
		--l;
	}
}
