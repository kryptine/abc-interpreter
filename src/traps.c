#include <stdio.h>
#include <stdlib.h>

#include "interpret.h"
#include "traps.h"

void clean_catAC (void)
{
	BC_WORD *asp,*hp;
	BC_WORD *s1,*s2;
	unsigned int l1,l2,l,lw,i;
	unsigned char *s1_p,*s2_p,*s_p;

	asp=g_asp;
	hp=g_hp;
	
	s1=(BC_WORD*)asp[0];
	s2=(BC_WORD*)asp[-1];
	l1=s1[1];
	l2=s2[1];

/*	printf ("clean_catAC %d %d\n",l1,l2); */

	s1_p=(unsigned char*)&s1[2];
	s2_p=(unsigned char*)&s2[2];

	l=l1+l2;
	lw=(l+3)>>2;
	
	if ( (g_heap_free -= (int)(lw+2)) < 0){
		printf("clean_catAC %d %d\n", l1, l2);
		printf("clean_catAC gc %d %d\n", (int) g_heap_free, lw+2);
		exit(1);
	}
	
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=l;

	asp-=1;
	asp[0] = (BC_WORD)hp;
	g_asp=asp;

	s_p=(unsigned char *)&hp[2];
	hp+=2+lw;
	g_hp=hp;
	
	for (i=0; i<l1; ++i)
		s_p[i]=s1_p[i];

	s_p+=l1;

	for (i=0; i<l2; ++i)
		s_p[i]=s2_p[i];

}

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
		printf ("clean_readLineF gc\n");
		exit (1);
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
		
	printf ("clean_readLineF gc\n");
	exit (1);
}

void clean_sliceAC (void)
{
	BC_WORD *asp,*bsp,*hp;
	unsigned int *s,n_words;
	unsigned int l,first_i,end_i,i;
	unsigned char *s_p,*new_s_p;

	asp=g_asp;
	bsp=g_bsp;
	hp=g_hp;

	if ( (g_heap_free -= 2) < 0){
		printf ("clean_sliceAC gc\n");
		exit (1);
	}

	s=(unsigned int*)asp[0];
	l=s[1];
	s_p=(unsigned char*)&s[2];

	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=0;
	asp[0]=(BC_WORD)hp;
	hp+=2;

	first_i=bsp[0];
	end_i=bsp[1]+1;
	g_bsp = bsp+2;
	
	if (first_i<0)
		first_i=0;
	if (end_i>l)
		end_i=l;
	l=end_i-first_i;
	
	hp[-1]=l;
	n_words=(l+3)>>2;
	if ( (g_heap_free -= n_words) < 0){
		printf ("clean_sliceAC gc\n");
		exit (1);
	}

	g_hp=hp+n_words;
	
	new_s_p=(unsigned char*)hp;
	s_p=&s_p[first_i];
	
	for (i=0; i<l; ++i)
		new_s_p[i]=s_p[i];
}

void clean_ItoAC (void)
{
	BC_WORD *asp,*bsp,*hp;
	int i,l,lw;
	unsigned int ui,t;
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
		printf ("clean_sliceAC gc\n");
		exit (1);
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
	c_file_name=malloc (file_name_length+1);
	if (c_file_name==NULL){
		printf ("clean_openF malloc failed\n");
		exit (1);
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
			printf ("clean_openF could not open file %s\n",c_file_name);
			exit (1);
		}
	} else if (mode==1){
		file=fopen (c_file_name,"w");
		if (file==NULL){
			printf ("clean_openF could not create file %s\n",c_file_name);
			exit (1);
		}
	} else if (mode==3){
		file=fopen (c_file_name,"rb");
		if (file==NULL){
			printf ("clean_openF could not open file %s\n",c_file_name);
			exit (1);
		}
	} else {
		printf ("clean_openF mode not implemented %d\n",mode);
		exit (1);
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

void clean_cmpAC (void)
{
	BC_WORD *asp,*bsp;
	BC_WORD *a;
	BC_WORD *b;
	BC_WORD s;

	asp=g_asp;
	bsp=g_bsp;

	a=(BC_WORD*)asp[0];
	b=(BC_WORD*)asp[-1];
	
	g_asp=asp-2;

	--bsp;
	g_bsp=bsp;

	s=b[1];
	if (a[1]!=s){
		*bsp = a[1]<s;
		return;
	}
	a=(BC_WORD*)(BC_WORD)&a[2];
	b=(BC_WORD*)(BC_WORD)&b[2];
	// TODO make 64/32-bit agnostic
	while ((short int)s>=(short int)4){
		if (a[0]!=b[0]){
			*bsp = a[0]<b[0] ? -1 : 1;
			return;
		}
		++a;
		++b;
		s-=4;
	}

	if ((short int)s>=(short int)2){
		if (*(uint16_t*)a!=*(uint16_t*)b){
			*bsp = *(uint16_t*)a < *(uint16_t*)b ? -1 : 1;
			return;
		}
		if ((short int)s>(short int)2){
			if (((uint8_t*)a)[2]!=((uint8_t*)b)[2]){
				*bsp = ((uint8_t*)a)[2] < ((uint8_t*)b)[2] ? -1 : 1;
			 	return;
			}
		}
	} else if ((short int)s>(short int)0){
		if (*(uint8_t*)a!=*(uint8_t*)b){
			*bsp = *(uint8_t*)a < *(uint8_t*)b ? -1 : 1;
			return;
		}
	}
	
	*bsp = 1;
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
	
	l=*(int*)(s+4);
	s+=8;
	
	while (l!=0){
		putchar (*s);
		++s;
		--l;
	}		
}