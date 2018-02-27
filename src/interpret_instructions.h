#include "abc_instructions.h"

case CaddI:
	bsp[1]=bsp[0] + bsp[1];
	++bsp;
	pc+=1;
	continue;
case Cadd_empty_node2:
	if ((heap_free-=3)<0)
		break;
	pc=*(BC_WORD**)&pc[1];
	hp[0]=(BC_WORD)&__cycle__in__spine;
	asp[1]=asp[0];
	asp[0]=asp[-1];
	asp[-1]=(BC_WORD)hp;
	asp+=1;
	hp+=3;
	continue;			
case Cadd_empty_node3:
	if ((heap_free-=3)<0)
		break;
	pc=*(BC_WORD**)&pc[1];
	hp[0]=(BC_WORD)&__cycle__in__spine;
	asp[1]=asp[0];
	asp[0]=asp[-1];
	asp[-1]=asp[-2];
	asp[-2]=(BC_WORD)hp;
	asp+=1;
	hp+=3;
	continue;			
case CandI:
	bsp[1]=bsp[0] & bsp[1];
	++bsp;
	pc+=1;
	continue;
case Cbuild:
{
	BC_WORD s;
	
	s=pc[1];
	if ((heap_free-=s+1)<0)
		break;
	hp[0]=pc[2];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=asp[-2];
	hp[4]=asp[-3];
	hp[5]=asp[-4];
	pc+=3;
	do {
		if (s< 6) break; hp[ 6]=asp[ -5];
		if (s< 7) break; hp[ 7]=asp[ -6];
		if (s< 8) break; hp[ 8]=asp[ -7];
		if (s< 9) break; hp[ 9]=asp[ -8];
		if (s<10) break; hp[10]=asp[ -9];
		if (s<11) break; hp[11]=asp[-10];
		if (s<12) break; hp[12]=asp[-11];
		if (s<13) break; hp[13]=asp[-12];
		if (s<14) break; hp[14]=asp[-13];
		if (s<15) break; hp[15]=asp[-14];
		if (s<16) break; hp[16]=asp[-15];
		if (s<17) break; hp[17]=asp[-16];
		if (s<18) break; hp[18]=asp[-17];
		if (s<19) break; hp[19]=asp[-18];
		if (s<20) break; hp[20]=asp[-19];
		if (s<21) break; hp[21]=asp[-20];
		if (s<22) break; hp[22]=asp[-21];
		if (s<23) break; hp[23]=asp[-22];
		if (s<24) break; hp[24]=asp[-23];
		if (s<25) break; hp[25]=asp[-24];
		if (s<26) break; hp[26]=asp[-25];
		if (s<27) break; hp[27]=asp[-26];
		if (s<28) break; hp[28]=asp[-27];
		if (s<29) break; hp[29]=asp[-28];
		if (s<30) break; hp[30]=asp[-29];
		if (s<31) break; hp[31]=asp[-30];
		if (s<32) break; hp[32]=asp[-31];
	} while (0);
	asp-=s;
	*++asp=(BC_WORD)hp;
	hp+=s+1;
	continue;
}
case Cbuild0:
	if ((heap_free-=3)<0)
		break;
	hp[0]=pc[1];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	continue;
case Cbuild1:
	if ((heap_free-=3)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	asp[0]=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	continue;
case Cbuild2:
case Cbuildh2:
case Cbuildhr20:
#if 0
	printf ("%d %d %d\n",hp-heap,heap_free,(hp-heap)+heap_free);
#endif
	if ((heap_free-=3)<0)
		break;
#if 0
	printf ("Cbuild(h)2 %d %d %d %d\n",(int)hp,(int)pc[1],(int)asp[0],(int)asp[-1]);
#endif
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	*--asp=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	continue;
case Cbuild3:
	if ((heap_free-=4)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=asp[-2];
	asp[-2]=(BC_WORD)hp;
	asp-=2;
	hp+=4;
	pc+=2;
	continue;
case Cbuild4:
	if ((heap_free-=5)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=asp[-2];
	hp[4]=asp[-3];
	asp[-3]=(BC_WORD)hp;
	asp-=3;
	hp+=5;
	pc+=2;
	continue;
case Cbuildh0:
case CbuildAC:
	*++asp=pc[1];
	pc+=2;
	continue;
case Cbuildh:
case Cbuildhra0:
{
	BC_WORD s_p_2;

	s_p_2=pc[1]; /* >=5+2 */
	if ((heap_free-=s_p_2)<0)
		break;
	hp[0]=pc[2];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	pc+=3;
	hp[ 3]=asp[- 1];
	hp[ 4]=asp[- 2];
	hp[ 5]=asp[- 3];
	hp[ 6]=asp[- 4];
	do {
		if (s_p_2< 8) break; hp[ 7]=asp[- 5];
		if (s_p_2< 9) break; hp[ 8]=asp[- 6];
		if (s_p_2<10) break; hp[ 9]=asp[- 7];
		if (s_p_2<11) break; hp[10]=asp[- 8];
		if (s_p_2<12) break; hp[11]=asp[- 9];
		if (s_p_2<13) break; hp[12]=asp[-10];
		if (s_p_2<14) break; hp[13]=asp[-11];
		if (s_p_2<15) break; hp[14]=asp[-12];
		if (s_p_2<16) break; hp[15]=asp[-13];
		if (s_p_2<17) break; hp[16]=asp[-14];
		if (s_p_2<18) break; hp[17]=asp[-15];
		if (s_p_2<19) break; hp[18]=asp[-16];
		if (s_p_2<20) break; hp[19]=asp[-17];
		if (s_p_2<21) break; hp[20]=asp[-18];
		if (s_p_2<22) break; hp[21]=asp[-19];
		if (s_p_2<23) break; hp[22]=asp[-20];
		if (s_p_2<24) break; hp[23]=asp[-21];
		if (s_p_2<25) break; hp[24]=asp[-22];
		if (s_p_2<26) break; hp[25]=asp[-23];
		if (s_p_2<27) break; hp[26]=asp[-24];
		if (s_p_2<28) break; hp[27]=asp[-25];
		if (s_p_2<29) break; hp[28]=asp[-26];
		if (s_p_2<30) break; hp[29]=asp[-27];
		if (s_p_2<31) break; hp[30]=asp[-28];
		if (s_p_2<32) break; hp[31]=asp[-29];
		if (s_p_2<33) break; hp[32]=asp[-30];
		if (s_p_2<34) break; hp[33]=asp[-31];
	} while (0);
	asp-=s_p_2-3;
	*asp=(BC_WORD)hp;
	hp+=s_p_2;
	continue;
}
case Cbuildh1:
case Cbuildhr10:
	if ((heap_free-=2)<0)
		break;
	hp[0]=pc[1];
	hp[1]=*asp;
	*asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	continue;
case Cbuildh3:
case Cbuildhr30:
	if ((heap_free-=5)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[-1];
	hp[4]=asp[-2];
	asp[-2]=(BC_WORD)hp;
	asp-=2;
	hp+=5;
	pc+=2;
	continue;
case Cbuildh4:
case Cbuildhr40:
	if ((heap_free-=6)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[-1];
	hp[4]=asp[-2];
	hp[5]=asp[-3];
	asp[-3]=(BC_WORD)hp;
	asp-=3;
	hp+=6;
	pc+=2;
	continue;
case Cbuildhr0b:
{
	BC_WORD n_b;

	n_b=pc[1];
	if ((heap_free-=n_b+2)<0)
		break;
	hp[0]=pc[2];
	hp[1]=bsp[0];
	hp[2]=(BC_WORD)&hp[3];
	pc+=3;
	hp[3]=bsp[1];
	hp[4]=bsp[2];
	hp[5]=bsp[3];
	hp[6]=bsp[4];
	*++asp=(BC_WORD)hp;
	do {
		if (n_b< 6) break; hp[ 7]=bsp[ 5];
		if (n_b< 7) break; hp[ 8]=bsp[ 6];
		if (n_b< 8) break; hp[ 9]=bsp[ 7];
		if (n_b< 9) break; hp[10]=bsp[ 8];
		if (n_b<10) break; hp[11]=bsp[ 9];
		if (n_b<11) break; hp[12]=bsp[10];
		if (n_b<12) break; hp[13]=bsp[11];
		if (n_b<13) break; hp[14]=bsp[12];
		if (n_b<14) break; hp[15]=bsp[13];
		if (n_b<15) break; hp[16]=bsp[14];
		if (n_b<16) break; hp[17]=bsp[15];
		if (n_b<17) break; hp[18]=bsp[16];
		if (n_b<18) break; hp[19]=bsp[17];
		if (n_b<19) break; hp[20]=bsp[18];
		if (n_b<20) break; hp[21]=bsp[19];
		if (n_b<21) break; hp[22]=bsp[20];
		if (n_b<22) break; hp[23]=bsp[21];
		if (n_b<23) break; hp[24]=bsp[22];
		if (n_b<24) break; hp[25]=bsp[23];
		if (n_b<25) break; hp[26]=bsp[24];
		if (n_b<26) break; hp[27]=bsp[25];
		if (n_b<27) break; hp[28]=bsp[26];
		if (n_b<28) break; hp[29]=bsp[27];
		if (n_b<29) break; hp[30]=bsp[28];
		if (n_b<30) break; hp[31]=bsp[29];
		if (n_b<31) break; hp[32]=bsp[30];
		if (n_b<32) break; hp[33]=bsp[31];
	} while (0);
	bsp+=n_b;
	hp+=n_b+2;
	continue;
}
case Cbuildhr1b:
{
	BC_WORD n_b;

	n_b=pc[1];
	if ((heap_free-=n_b+3)<0)
		break;
	hp[0]=pc[2];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	pc+=3;
	hp[3]=bsp[0];
	hp[4]=bsp[1];
	hp[5]=bsp[2];
	hp[6]=bsp[3];
	*asp=(BC_WORD)hp;
	do {
		if (n_b< 5) break; hp[ 7]=bsp[ 4];
		if (n_b< 6) break; hp[ 8]=bsp[ 5];
		if (n_b< 7) break; hp[ 9]=bsp[ 6];
		if (n_b< 8) break; hp[10]=bsp[ 7];
		if (n_b< 9) break; hp[11]=bsp[ 8];
		if (n_b<10) break; hp[12]=bsp[ 9];
		if (n_b<11) break; hp[13]=bsp[10];
		if (n_b<12) break; hp[14]=bsp[11];
		if (n_b<13) break; hp[15]=bsp[12];
		if (n_b<14) break; hp[16]=bsp[13];
		if (n_b<15) break; hp[17]=bsp[14];
		if (n_b<16) break; hp[18]=bsp[15];
		if (n_b<17) break; hp[19]=bsp[16];
		if (n_b<18) break; hp[20]=bsp[17];
		if (n_b<19) break; hp[21]=bsp[18];
		if (n_b<20) break; hp[22]=bsp[19];
		if (n_b<21) break; hp[23]=bsp[20];
		if (n_b<22) break; hp[24]=bsp[21];
		if (n_b<23) break; hp[25]=bsp[22];
		if (n_b<24) break; hp[26]=bsp[23];
		if (n_b<25) break; hp[27]=bsp[24];
		if (n_b<26) break; hp[28]=bsp[25];
		if (n_b<27) break; hp[29]=bsp[26];
		if (n_b<28) break; hp[30]=bsp[27];
		if (n_b<29) break; hp[31]=bsp[28];
		if (n_b<30) break; hp[32]=bsp[29];
		if (n_b<31) break; hp[33]=bsp[30];
	} while (0);
	bsp+=n_b;
	hp+=n_b+3;
	continue;
}
case CbuildBFALSE:
	if ((heap_free-=2)<0)
		break;
	hp[0]=(BC_WORD)&BOOL+2;
	hp[1]=0;
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=1;
	continue;
case CbuildBTRUE:
	if ((heap_free-=2)<0)
		break;
	hp[0]=(BC_WORD)&BOOL+2;
	hp[1]=1;
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=1;
	continue;
case CbuildB_b:
{
	BC_WORD_S bo;
	
	if ((heap_free-=2)<0)
		break;
	bo = pc[1];
	hp[0]=(BC_WORD)&BOOL+2;
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	continue;
}
case CbuildC:
	if ((heap_free-=2)<0)
		break;
	hp[0]=(BC_WORD)&CHAR+2;
	hp[1]=pc[1];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	continue;
case CbuildC_b:
{
	BC_WORD_S bo;
	
	if ((heap_free-=2)<0)
		break;
	bo = pc[1];
	hp[0]=(BC_WORD)&CHAR+2;
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	continue;
}
case CbuildF_b:
{
	BC_WORD_S bo;
	
	if ((heap_free-=3)<0)
		break;
	bo = pc[1];
	hp[0]=(BC_WORD)&dFILE+2;
	hp[1]=bsp[bo];
	hp[2]=bsp[bo+1];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	continue;
}
case CbuildI:
	if ((heap_free-=2)<0)
		break;
	hp[0]=(BC_WORD)&INT+2;
	hp[1]=pc[1];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	continue;
case CbuildI_b:
{
	BC_WORD_S bo;
	
	if ((heap_free-=2)<0)
		break;
	bo = pc[1];
	hp[0]=(BC_WORD)&INT+2;
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	continue;
}
case Cbuildhr:
{
	BC_WORD n_a,n_b,n_ab;

	n_ab=pc[1];
	if ((heap_free-=n_ab+2)<0)
		break;
	hp[0]=pc[2];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	n_a=pc[3];
	n_b=n_ab-n_a;
	pc+=4;
	hp[3]=asp[-1];
	do {
		if (n_a< 3) break; hp[ 4]=asp[- 2];
		if (n_a< 4) break; hp[ 5]=asp[- 3];
		if (n_a< 5) break; hp[ 6]=asp[- 4];
		if (n_a< 6) break; hp[ 7]=asp[- 5];
		if (n_a< 7) break; hp[ 8]=asp[- 6];
		if (n_a< 8) break; hp[ 9]=asp[- 7];
		if (n_a< 9) break; hp[10]=asp[- 8];
		if (n_a<10) break; hp[11]=asp[- 9];
		if (n_a<11) break; hp[12]=asp[-10];
		if (n_a<12) break; hp[13]=asp[-11];
		if (n_a<13) break; hp[14]=asp[-12];
		if (n_a<14) break; hp[15]=asp[-13];
		if (n_a<15) break; hp[16]=asp[-14];
		if (n_a<16) break; hp[17]=asp[-15];
		if (n_a<17) break; hp[18]=asp[-16];
		if (n_a<18) break; hp[19]=asp[-17];
		if (n_a<19) break; hp[20]=asp[-18];
		if (n_a<20) break; hp[21]=asp[-19];
		if (n_a<21) break; hp[22]=asp[-20];
		if (n_a<22) break; hp[23]=asp[-21];
		if (n_a<23) break; hp[24]=asp[-22];
		if (n_a<24) break; hp[25]=asp[-23];
		if (n_a<25) break; hp[26]=asp[-24];
		if (n_a<26) break; hp[27]=asp[-25];
		if (n_a<27) break; hp[28]=asp[-26];
		if (n_a<28) break; hp[29]=asp[-27];
		if (n_a<29) break; hp[30]=asp[-28];
		if (n_a<30) break; hp[31]=asp[-29];
	} while (0);
	asp-=n_a;
	*++asp=(BC_WORD)hp;
	hp+=n_a+2;
	hp[0]=bsp[0];
	hp[1]=bsp[1];
	do {
		if (n_b< 3) break; hp[ 2]=bsp[ 2];
		if (n_b< 4) break; hp[ 3]=bsp[ 3];
		if (n_b< 5) break; hp[ 4]=bsp[ 4];
		if (n_b< 6) break; hp[ 5]=bsp[ 5];
		if (n_b< 7) break; hp[ 6]=bsp[ 6];
		if (n_b< 8) break; hp[ 7]=bsp[ 7];
		if (n_b< 9) break; hp[ 8]=bsp[ 8];
		if (n_b<10) break; hp[ 9]=bsp[ 9];
		if (n_b<11) break; hp[10]=bsp[10];
		if (n_b<12) break; hp[11]=bsp[11];
		if (n_b<13) break; hp[12]=bsp[12];
		if (n_b<14) break; hp[13]=bsp[13];
		if (n_b<15) break; hp[14]=bsp[14];
		if (n_b<16) break; hp[15]=bsp[15];
		if (n_b<17) break; hp[16]=bsp[16];
		if (n_b<18) break; hp[17]=bsp[17];
		if (n_b<19) break; hp[18]=bsp[18];
		if (n_b<20) break; hp[19]=bsp[19];
		if (n_b<21) break; hp[20]=bsp[20];
		if (n_b<22) break; hp[21]=bsp[21];
		if (n_b<23) break; hp[22]=bsp[22];
		if (n_b<24) break; hp[23]=bsp[23];
		if (n_b<25) break; hp[24]=bsp[24];
		if (n_b<26) break; hp[25]=bsp[25];
		if (n_b<27) break; hp[26]=bsp[26];
		if (n_b<28) break; hp[27]=bsp[27];
		if (n_b<29) break; hp[28]=bsp[28];
		if (n_b<30) break; hp[29]=bsp[29];
	} while (0);
	bsp+=n_b;
	hp+=n_b;
	continue;
}
case Cbuildhra1:
{
	BC_WORD n_a;

	n_a=pc[1];
	if ((heap_free-=n_a+3)<0)
		break;
	hp[0]=pc[2];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	pc+=3;
	hp[3]=asp[-1];
	hp[4]=asp[-2];
	hp[5]=asp[-3];
	do {
		if (n_a< 5) break; hp[ 6]=asp[- 4];
		if (n_a< 6) break; hp[ 7]=asp[- 5];
		if (n_a< 7) break; hp[ 8]=asp[- 6];
		if (n_a< 8) break; hp[ 9]=asp[- 7];
		if (n_a< 9) break; hp[10]=asp[- 8];
		if (n_a<10) break; hp[11]=asp[- 9];
		if (n_a<11) break; hp[12]=asp[-10];
		if (n_a<12) break; hp[13]=asp[-11];
		if (n_a<13) break; hp[14]=asp[-12];
		if (n_a<14) break; hp[15]=asp[-13];
		if (n_a<15) break; hp[16]=asp[-14];
		if (n_a<16) break; hp[17]=asp[-15];
		if (n_a<17) break; hp[18]=asp[-16];
		if (n_a<18) break; hp[19]=asp[-17];
		if (n_a<19) break; hp[20]=asp[-18];
		if (n_a<20) break; hp[21]=asp[-19];
		if (n_a<21) break; hp[22]=asp[-20];
		if (n_a<22) break; hp[23]=asp[-21];
		if (n_a<23) break; hp[24]=asp[-22];
		if (n_a<24) break; hp[25]=asp[-23];
		if (n_a<25) break; hp[26]=asp[-24];
		if (n_a<26) break; hp[27]=asp[-25];
		if (n_a<27) break; hp[28]=asp[-26];
		if (n_a<28) break; hp[29]=asp[-27];
		if (n_a<29) break; hp[30]=asp[-28];
		if (n_a<30) break; hp[31]=asp[-29];
		if (n_a<31) break; hp[32]=asp[-30];
	} while (0);
	asp-=n_a;
	*++asp=(BC_WORD)hp;
	hp+=n_a+3;
	hp[-1]=*bsp++;
	continue;
}
case Cbuildhr01:
{
	if ((heap_free-=2)<0)
		break;
	hp[0]=pc[1];
	hp[1]=*bsp++;
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	continue;
}
case Cbuildhr02:
{
	if ((heap_free-=3)<0)
		break;
	hp[0]=pc[1];
	hp[1]=bsp[0];
	hp[2]=bsp[1];
	*++asp=(BC_WORD)hp;
	bsp+=2;
	hp+=3;
	pc+=2;
	continue;
}
case Cbuildhr03:
{
	if ((heap_free-=5)<0)
		break;
	hp[0]=pc[1];
	hp[1]=bsp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[1];
	hp[4]=bsp[2];
	*++asp=(BC_WORD)hp;
	bsp+=3;
	hp+=5;
	pc+=2;
	continue;
}
case Cbuildhr11:
	if ((heap_free-=3)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=bsp[0];
	*asp=(BC_WORD)hp;
	++bsp;
	hp+=3;
	pc+=2;
	continue;
case Cbuildhr12:
	if ((heap_free-=5)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[0];
	hp[4]=bsp[1];
	*asp=(BC_WORD)hp;
	bsp+=2;
	hp+=5;
	pc+=2;
	continue;
case Cbuildhr13:
	if ((heap_free-=6)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[0];
	hp[4]=bsp[1];
	hp[5]=bsp[2];
	*asp=(BC_WORD)hp;
	bsp+=3;
	hp+=6;
	pc+=2;
	continue;
case Cbuildhr21:
	if ((heap_free-=5)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[-1];
	hp[4]=bsp[0];
	asp[-1]=(BC_WORD)hp;
	asp-=1;
	++bsp;
	hp+=5;
	pc+=2;
	continue;
case Cbuildhr22:
	if ((heap_free-=6)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[-1];
	hp[4]=bsp[0];
	hp[5]=bsp[1];
	asp[-1]=(BC_WORD)hp;
	asp-=1;
	bsp+=2;
	hp+=6;
	pc+=2;
	continue;
case Cbuildhr31:
	if ((heap_free-=6)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[-1];
	hp[4]=asp[-2];
	hp[5]=bsp[0];
	asp[-2]=(BC_WORD)hp;
	asp-=2;
	bsp+=1;
	hp+=6;
	pc+=2;
	continue;
case Cbuild_r:
{
	BC_WORD_S ao,bo;
	BC_WORD n_a,n_b,n_ab,*ao_p,*bo_p;

	n_ab=pc[1];
	if ((heap_free-=n_ab+2)<0)
		break;
	ao=pc[2];
	hp[0]=pc[3];
	hp[1]=asp[ao];
	ao_p=&asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	n_a=pc[4];
	n_b=n_ab-n_a;
	hp[3]=ao_p[-1];
	do {
		if (n_a< 3) break; hp[ 4]=ao_p[- 2];
		if (n_a< 4) break; hp[ 5]=ao_p[- 3];
		if (n_a< 5) break; hp[ 6]=ao_p[- 4];
		if (n_a< 6) break; hp[ 7]=ao_p[- 5];
		if (n_a< 7) break; hp[ 8]=ao_p[- 6];
		if (n_a< 8) break; hp[ 9]=ao_p[- 7];
		if (n_a< 9) break; hp[10]=ao_p[- 8];
		if (n_a<10) break; hp[11]=ao_p[- 9];
		if (n_a<11) break; hp[12]=ao_p[-10];
		if (n_a<12) break; hp[13]=ao_p[-11];
		if (n_a<13) break; hp[14]=ao_p[-12];
		if (n_a<14) break; hp[15]=ao_p[-13];
		if (n_a<15) break; hp[16]=ao_p[-14];
		if (n_a<16) break; hp[17]=ao_p[-15];
		if (n_a<17) break; hp[18]=ao_p[-16];
		if (n_a<18) break; hp[19]=ao_p[-17];
		if (n_a<19) break; hp[20]=ao_p[-18];
		if (n_a<20) break; hp[21]=ao_p[-19];
		if (n_a<21) break; hp[22]=ao_p[-20];
		if (n_a<22) break; hp[23]=ao_p[-21];
		if (n_a<23) break; hp[24]=ao_p[-22];
		if (n_a<24) break; hp[25]=ao_p[-23];
		if (n_a<25) break; hp[26]=ao_p[-24];
		if (n_a<26) break; hp[27]=ao_p[-25];
		if (n_a<27) break; hp[28]=ao_p[-26];
		if (n_a<28) break; hp[29]=ao_p[-27];
		if (n_a<29) break; hp[30]=ao_p[-28];
		if (n_a<30) break; hp[31]=ao_p[-29];
	} while (0);
	bo=pc[5];
	pc+=6;
	*++asp=(BC_WORD)hp;
	hp+=n_a+2;
	hp[0]=bsp[bo];
	bo_p=&bsp[bo];
	hp[1]=bo_p[1];
	do {
		if (n_b< 3) break; hp[ 2]=bo_p[ 2];
		if (n_b< 4) break; hp[ 3]=bo_p[ 3];
		if (n_b< 5) break; hp[ 4]=bo_p[ 4];
		if (n_b< 6) break; hp[ 5]=bo_p[ 5];
		if (n_b< 7) break; hp[ 6]=bo_p[ 6];
		if (n_b< 8) break; hp[ 7]=bo_p[ 7];
		if (n_b< 9) break; hp[ 8]=bo_p[ 8];
		if (n_b<10) break; hp[ 9]=bo_p[ 9];
		if (n_b<11) break; hp[10]=bo_p[10];
		if (n_b<12) break; hp[11]=bo_p[11];
		if (n_b<13) break; hp[12]=bo_p[12];
		if (n_b<14) break; hp[13]=bo_p[13];
		if (n_b<15) break; hp[14]=bo_p[14];
		if (n_b<16) break; hp[15]=bo_p[15];
		if (n_b<17) break; hp[16]=bo_p[16];
		if (n_b<18) break; hp[17]=bo_p[17];
		if (n_b<19) break; hp[18]=bo_p[18];
		if (n_b<20) break; hp[19]=bo_p[19];
		if (n_b<21) break; hp[20]=bo_p[20];
		if (n_b<22) break; hp[21]=bo_p[21];
		if (n_b<23) break; hp[22]=bo_p[22];
		if (n_b<24) break; hp[23]=bo_p[23];
		if (n_b<25) break; hp[24]=bo_p[24];
		if (n_b<26) break; hp[25]=bo_p[25];
		if (n_b<27) break; hp[26]=bo_p[26];
		if (n_b<28) break; hp[27]=bo_p[27];
		if (n_b<29) break; hp[28]=bo_p[28];
		if (n_b<30) break; hp[29]=bo_p[29];
	} while (0);				
	hp+=n_b;
	continue;
}
case Cbuild_ra1:
{
	BC_WORD_S ao,bo;
	BC_WORD n_a,*ao_p;

	n_a=pc[1];
	if ((heap_free-=n_a+3)<0)
		break;
	ao=pc[2];
	hp[0]=pc[3];
	hp[1]=asp[ao];
	ao_p=&asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=ao_p[-1];
	hp[4]=ao_p[-2];
	hp[5]=ao_p[-3];
	do {
		if (n_a< 5) break; hp[ 6]=ao_p[ -4];
		if (n_a< 6) break; hp[ 7]=ao_p[ -5];
		if (n_a< 7) break; hp[ 8]=ao_p[ -6];
		if (n_a< 8) break; hp[ 9]=ao_p[ -7];
		if (n_a< 9) break; hp[10]=ao_p[ -8];
		if (n_a<10) break; hp[11]=ao_p[ -9];
		if (n_a<11) break; hp[12]=ao_p[-10];
		if (n_a<12) break; hp[13]=ao_p[-11];
		if (n_a<13) break; hp[14]=ao_p[-12];
		if (n_a<14) break; hp[15]=ao_p[-13];
		if (n_a<15) break; hp[16]=ao_p[-14];
		if (n_a<16) break; hp[17]=ao_p[-15];
		if (n_a<17) break; hp[18]=ao_p[-16];
		if (n_a<18) break; hp[19]=ao_p[-17];
		if (n_a<19) break; hp[20]=ao_p[-18];
		if (n_a<20) break; hp[21]=ao_p[-19];
		if (n_a<21) break; hp[22]=ao_p[-20];
		if (n_a<22) break; hp[23]=ao_p[-21];
		if (n_a<23) break; hp[24]=ao_p[-22];
		if (n_a<24) break; hp[25]=ao_p[-23];
		if (n_a<25) break; hp[26]=ao_p[-24];
		if (n_a<26) break; hp[27]=ao_p[-25];
		if (n_a<27) break; hp[28]=ao_p[-26];
		if (n_a<28) break; hp[29]=ao_p[-27];
		if (n_a<29) break; hp[30]=ao_p[-28];
		if (n_a<30) break; hp[31]=ao_p[-29];
		if (n_a<31) break; hp[32]=ao_p[-30];
	} while (0);
	bo=pc[4];
	*++asp=(BC_WORD)hp;
	pc+=5;
	hp+=n_a+3;
	hp[-1]=bsp[bo];
	continue;
}
case Cbuild_ra0:
{
	BC_WORD_S ao;
	BC_WORD n_a,*ao_p;

	n_a=pc[1];
	if ((heap_free-=n_a+2)<0)
		break;
	ao=pc[2];
	hp[0]=pc[3];
	hp[1]=asp[ao];
	ao_p=&asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=ao_p[-1];
	hp[4]=ao_p[-2];
	hp[5]=ao_p[-3];
	hp[6]=ao_p[-4];
	pc+=4;
	do {
		if (n_a< 6) break; hp[ 7]=ao_p[ -5];
		if (n_a< 7) break; hp[ 8]=ao_p[ -6];
		if (n_a< 8) break; hp[ 9]=ao_p[ -7];
		if (n_a< 9) break; hp[10]=ao_p[ -8];
		if (n_a<10) break; hp[11]=ao_p[ -9];
		if (n_a<11) break; hp[12]=ao_p[-10];
		if (n_a<12) break; hp[13]=ao_p[-11];
		if (n_a<13) break; hp[14]=ao_p[-12];
		if (n_a<14) break; hp[15]=ao_p[-13];
		if (n_a<15) break; hp[16]=ao_p[-14];
		if (n_a<16) break; hp[17]=ao_p[-15];
		if (n_a<17) break; hp[18]=ao_p[-16];
		if (n_a<18) break; hp[19]=ao_p[-17];
		if (n_a<19) break; hp[20]=ao_p[-18];
		if (n_a<20) break; hp[21]=ao_p[-19];
		if (n_a<21) break; hp[22]=ao_p[-20];
		if (n_a<22) break; hp[23]=ao_p[-21];
		if (n_a<23) break; hp[24]=ao_p[-22];
		if (n_a<24) break; hp[25]=ao_p[-23];
		if (n_a<25) break; hp[26]=ao_p[-24];
		if (n_a<26) break; hp[27]=ao_p[-25];
		if (n_a<27) break; hp[28]=ao_p[-26];
		if (n_a<28) break; hp[29]=ao_p[-27];
		if (n_a<29) break; hp[30]=ao_p[-28];
		if (n_a<30) break; hp[31]=ao_p[-29];
		if (n_a<31) break; hp[32]=ao_p[-30];
		if (n_a<32) break; hp[33]=ao_p[-31];
	} while (0);
	*++asp=(BC_WORD)hp;
	hp+=n_a+2;
	continue;
}
case Cbuild_r01:
{
	BC_WORD_S bo;

	if ((heap_free-=2)<0)
		break;
	bo=pc[1];
	hp[0]=pc[2];
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=3;
	continue;
}
case Cbuild_r02:
{
	BC_WORD_S bo;

	if ((heap_free-=3)<0)
		break;
	bo=pc[1];
	hp[0]=pc[2];
	hp[1]=bsp[bo];
	hp[2]=bsp[bo+1];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=3;
	continue;
}
case Cbuild_r03:
{
	BC_WORD_S bo;

	if ((heap_free-=5)<0)
		break;
	bo=pc[1];
	hp[0]=pc[2];
	hp[1]=bsp[bo];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[bo+1];
	hp[4]=bsp[bo+2];
	*++asp=(BC_WORD)hp;
	hp+=5;
	pc+=3;
	continue;
}
case Cbuild_r04:
{
	BC_WORD_S bo;

	if ((heap_free-=6)<0)
		break;
	bo=pc[1];
	hp[0]=pc[2];
	hp[1]=bsp[bo];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[bo+1];
	hp[4]=bsp[bo+2];
	hp[5]=bsp[bo+3];
	*++asp=(BC_WORD)hp;
	hp+=6;
	pc+=3;
	continue;
}
case Cbuild_r0b:
{
	BC_WORD_S bo;
	BC_WORD n_b,*bo_p;

	n_b=pc[1];
	if ((heap_free-=n_b+2)<0)
		break;
	bo=pc[2];
	hp[0]=pc[3];
	hp[1]=bsp[bo];
	bo_p=&bsp[bo];
	hp[2]=(BC_WORD)&hp[3];
	pc+=4;
	hp[3]=bo_p[1];
	hp[4]=bo_p[2];
	hp[5]=bo_p[3];
	hp[6]=bo_p[4];
	do {
		if (n_b< 6) break; hp[ 7]=bo_p[ 5];
		if (n_b< 7) break; hp[ 8]=bo_p[ 6];
		if (n_b< 8) break; hp[ 9]=bo_p[ 7];
		if (n_b< 9) break; hp[10]=bo_p[ 8];
		if (n_b<10) break; hp[11]=bo_p[ 9];
		if (n_b<11) break; hp[12]=bo_p[10];
		if (n_b<12) break; hp[13]=bo_p[11];
		if (n_b<13) break; hp[14]=bo_p[12];
		if (n_b<14) break; hp[15]=bo_p[13];
		if (n_b<15) break; hp[16]=bo_p[14];
		if (n_b<16) break; hp[17]=bo_p[15];
		if (n_b<17) break; hp[18]=bo_p[16];
		if (n_b<18) break; hp[19]=bo_p[17];
		if (n_b<19) break; hp[20]=bo_p[18];
		if (n_b<20) break; hp[21]=bo_p[19];
		if (n_b<21) break; hp[22]=bo_p[20];
		if (n_b<22) break; hp[23]=bo_p[21];
		if (n_b<23) break; hp[24]=bo_p[22];
		if (n_b<24) break; hp[25]=bo_p[23];
		if (n_b<25) break; hp[26]=bo_p[24];
		if (n_b<26) break; hp[27]=bo_p[25];
		if (n_b<27) break; hp[28]=bo_p[26];
		if (n_b<28) break; hp[29]=bo_p[27];
		if (n_b<29) break; hp[30]=bo_p[28];
		if (n_b<30) break; hp[31]=bo_p[29];
		if (n_b<31) break; hp[32]=bo_p[30];
		if (n_b<32) break; hp[33]=bo_p[31];
	} while (0);
	hp+=n_b+2;
	continue;
}
case Cbuild_r10:
{
	BC_WORD_S ao;

	if ((heap_free-=2)<0)
		break;
	ao=pc[1];
	hp[0]=pc[2];
	hp[1]=asp[ao];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=3;
	continue;
}
case Cbuild_r11:
{
	BC_WORD_S ao,bo;

	if ((heap_free-=3)<0)
		break;
	ao=((BC_WORD_S*)pc)[1];
	bo=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao];
	hp[2]=asp[bo];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=4;
	continue;
}
case Cbuild_r12:
{
	BC_WORD_S ao,bo;

	if ((heap_free-=5)<0)
		break;
	ao=((BC_WORD_S*)pc)[1];
	bo=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[bo];
	hp[4]=asp[bo+1];
	*++asp=(BC_WORD)hp;
	hp+=5;
	pc+=4;
	continue;
}
case Cbuild_r13:
{
	BC_WORD_S ao,bo;

	if ((heap_free-=6)<0)
		break;
	ao=((BC_WORD_S*)pc)[1];
	bo=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[bo];
	hp[4]=asp[bo+1];
	hp[5]=bsp[bo+2];
	*++asp=(BC_WORD)hp;
	hp+=6;
	pc+=4;
	continue;
}
case Cbuild_r1b:
{
	BC_WORD_S ao,bo;
	BC_WORD n_b,*bo_p;

	n_b=pc[1];
	if ((heap_free-=n_b+3)<0)
		break;
	ao=pc[2];
	hp[0]=pc[3];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	bo=pc[4];
	*++asp=(BC_WORD)hp;
	pc+=5;
	hp[3]=bsp[bo];
	bo_p=&bsp[bo];
	hp[4]=bo_p[1];
	hp[5]=bo_p[2];
	hp[6]=bo_p[3];
	do {
		if (n_b< 5) break; hp[ 7]=bo_p[ 4];
		if (n_b< 6) break; hp[ 8]=bo_p[ 5];
		if (n_b< 7) break; hp[ 9]=bo_p[ 6];
		if (n_b< 8) break; hp[10]=bo_p[ 7];
		if (n_b< 9) break; hp[11]=bo_p[ 8];
		if (n_b<10) break; hp[12]=bo_p[ 9];
		if (n_b<11) break; hp[13]=bo_p[10];
		if (n_b<12) break; hp[14]=bo_p[11];
		if (n_b<13) break; hp[15]=bo_p[12];
		if (n_b<14) break; hp[16]=bo_p[13];
		if (n_b<15) break; hp[17]=bo_p[14];
		if (n_b<16) break; hp[18]=bo_p[15];
		if (n_b<17) break; hp[19]=bo_p[16];
		if (n_b<18) break; hp[20]=bo_p[17];
		if (n_b<19) break; hp[21]=bo_p[18];
		if (n_b<20) break; hp[22]=bo_p[19];
		if (n_b<21) break; hp[23]=bo_p[20];
		if (n_b<22) break; hp[24]=bo_p[21];
		if (n_b<23) break; hp[25]=bo_p[22];
		if (n_b<24) break; hp[26]=bo_p[23];
		if (n_b<25) break; hp[27]=bo_p[24];
		if (n_b<26) break; hp[28]=bo_p[25];
		if (n_b<27) break; hp[29]=bo_p[26];
		if (n_b<28) break; hp[30]=bo_p[27];
		if (n_b<29) break; hp[31]=bo_p[28];
		if (n_b<30) break; hp[32]=bo_p[29];
		if (n_b<31) break; hp[33]=bo_p[30];
	} while (0);				
	hp+=n_b+3;
	continue;
}
case Cbuild_r20:
{
	BC_WORD_S ao;

	if ((heap_free-=3)<0)
		break;
	ao=((BC_WORD_S*)pc)[1];
	hp[0]=pc[2];
	hp[1]=asp[ao];
	hp[2]=asp[ao-1];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=3;
	continue;
}
case Cbuild_r21:
{
	BC_WORD_S ao,bo;

	if ((heap_free-=5)<0)
		break;
	ao=((BC_WORD_S*)pc)[1];
	bo=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[ao-1];
	hp[4]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=5;
	pc+=4;
	continue;
}
case Cbuild_r30:
{
	BC_WORD_S ao;

	if ((heap_free-=5)<0)
		break;
	ao=((BC_WORD_S*)pc)[1];
	hp[0]=*(BC_WORD*)&pc[2];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[ao-1];
	hp[4]=asp[ao-2];
	*++asp=(BC_WORD)hp;
	hp+=5;
	pc+=3;
	continue;
}
case Cbuild_r31:
{
	BC_WORD_S ao,bo;

	if ((heap_free-=6)<0)
		break;
	ao=((BC_WORD_S*)pc)[1];
	bo=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[ao-1];
	hp[4]=asp[ao-2];
	hp[5]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=6;
	pc+=4;
	continue;
}
case Cbuild_r40:
{
	BC_WORD_S ao;

	if ((heap_free-=6)<0)
		break;
	ao=((BC_WORD_S*)pc)[1];
	hp[0]=*(BC_WORD*)&pc[2];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[ao-1];
	hp[4]=asp[ao-2];
	hp[5]=asp[ao-3];
	*++asp=(BC_WORD)hp;
	hp+=6;
	pc+=3;
	continue;
}
case Cbuild_u:
{
	BC_WORD n_a,n_b,n_ab;

	n_ab=pc[1];
	if ((heap_free-=n_ab+1)<0)
		break;
	hp[0]=*(BC_WORD*)&pc[2];
	hp[1]=asp[0];
	n_a=pc[3];
	n_b=n_ab-n_a;
	hp[2]=asp[-1];
	do {
		if (n_a< 3) break; hp[ 3]=asp[- 2];
		if (n_a< 4) break; hp[ 4]=asp[- 3];
		if (n_a< 5) break; hp[ 5]=asp[- 4];
		if (n_a< 6) break; hp[ 6]=asp[- 5];
		if (n_a< 7) break; hp[ 7]=asp[- 6];
		if (n_a< 8) break; hp[ 8]=asp[- 7];
		if (n_a< 9) break; hp[ 9]=asp[- 8];
		if (n_a<10) break; hp[10]=asp[- 9];
		if (n_a<11) break; hp[11]=asp[-10];
		if (n_a<12) break; hp[12]=asp[-11];
		if (n_a<13) break; hp[13]=asp[-12];
		if (n_a<14) break; hp[14]=asp[-13];
		if (n_a<15) break; hp[15]=asp[-14];
		if (n_a<16) break; hp[16]=asp[-15];
		if (n_a<17) break; hp[17]=asp[-16];
		if (n_a<18) break; hp[18]=asp[-17];
		if (n_a<19) break; hp[19]=asp[-18];
		if (n_a<20) break; hp[20]=asp[-19];
		if (n_a<21) break; hp[21]=asp[-20];
		if (n_a<22) break; hp[22]=asp[-21];
		if (n_a<23) break; hp[23]=asp[-22];
		if (n_a<24) break; hp[24]=asp[-23];
		if (n_a<25) break; hp[25]=asp[-24];
		if (n_a<26) break; hp[26]=asp[-25];
		if (n_a<27) break; hp[27]=asp[-26];
		if (n_a<28) break; hp[28]=asp[-27];
		if (n_a<29) break; hp[29]=asp[-28];
		if (n_a<30) break; hp[30]=asp[-29];
	} while (0);
	asp-=n_a;
	pc+=4;
	*++asp=(BC_WORD)hp;
	hp+=n_a+1;
	hp[0]=bsp[0];
	hp[1]=bsp[1];
	do {
		if (n_b< 3) break; hp[ 2]=bsp[ 2];
		if (n_b< 4) break; hp[ 3]=bsp[ 3];
		if (n_b< 5) break; hp[ 4]=bsp[ 4];
		if (n_b< 6) break; hp[ 5]=bsp[ 5];
		if (n_b< 7) break; hp[ 6]=bsp[ 6];
		if (n_b< 8) break; hp[ 7]=bsp[ 7];
		if (n_b< 9) break; hp[ 8]=bsp[ 8];
		if (n_b<10) break; hp[ 9]=bsp[ 9];
		if (n_b<11) break; hp[10]=bsp[10];
		if (n_b<12) break; hp[11]=bsp[11];
		if (n_b<13) break; hp[12]=bsp[12];
		if (n_b<14) break; hp[13]=bsp[13];
		if (n_b<15) break; hp[14]=bsp[14];
		if (n_b<16) break; hp[15]=bsp[15];
		if (n_b<17) break; hp[16]=bsp[16];
		if (n_b<18) break; hp[17]=bsp[17];
		if (n_b<19) break; hp[18]=bsp[18];
		if (n_b<20) break; hp[19]=bsp[19];
		if (n_b<21) break; hp[20]=bsp[20];
		if (n_b<22) break; hp[21]=bsp[21];
		if (n_b<23) break; hp[22]=bsp[22];
		if (n_b<24) break; hp[23]=bsp[23];
		if (n_b<25) break; hp[24]=bsp[24];
		if (n_b<26) break; hp[25]=bsp[25];
		if (n_b<27) break; hp[26]=bsp[26];
		if (n_b<28) break; hp[27]=bsp[27];
		if (n_b<29) break; hp[28]=bsp[28];
		if (n_b<30) break; hp[29]=bsp[29];
	} while (0);
	bsp+=n_b;
	hp+=n_b;
	continue;
}
case Cbuild_u01:
	if ((heap_free-=3)<0)
		break;
	hp[0]=pc[1];
	hp[1]=*bsp++;
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	continue;
case Cbuild_u02:
	if ((heap_free-=3)<0)
		break;
	hp[0]=pc[1];
	hp[1]=bsp[0];
	hp[2]=bsp[1];
	*++asp=(BC_WORD)hp;
	bsp+=2;
	hp+=3;
	pc+=2;
	continue;
case Cbuild_u11:
	if ((heap_free-=3)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=*bsp++;
	asp[0]=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	continue;
case Cbuild_u12:
	if ((heap_free-=4)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=bsp[0];
	hp[3]=bsp[1];
	asp[0]=(BC_WORD)hp;
	bsp+=2;
	hp+=4;
	pc+=2;
	continue;
case Cbuild_u13:
	if ((heap_free-=5)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=bsp[0];
	hp[3]=bsp[1];
	hp[4]=bsp[2];
	asp[0]=(BC_WORD)hp;
	bsp+=3;
	hp+=5;
	pc+=2;
	continue;
case Cbuild_u21:
	if ((heap_free-=4)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=bsp[0];
	asp[-1]=(BC_WORD)hp;
	asp-=1;
	bsp+=1;
	hp+=4;
	pc+=2;
	continue;
case Cbuild_u22:
	if ((heap_free-=5)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=bsp[0];
	hp[4]=bsp[1];
	asp[-1]=(BC_WORD)hp;
	asp-=1;
	bsp+=2;
	hp+=5;
	pc+=2;
	continue;
case Cbuild_u31:
	if ((heap_free-=5)<0)
		break;
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=asp[-2];
	hp[4]=bsp[0];
	asp[-2]=(BC_WORD)hp;
	asp-=2;
	bsp+=1;
	hp+=5;
	pc+=2;
	continue;
case Cbuild_ua1:
{
	BC_WORD n_a;

	n_a=pc[1];
	if ((heap_free-=n_a+2)<0)
		break;
	hp[0]=*(BC_WORD*)&pc[2];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=asp[-2];
	hp[4]=asp[-3];
	pc+=3;
	do {
		if (n_a< 5) break; hp[ 5]=asp[- 4];
		if (n_a< 6) break; hp[ 6]=asp[- 5];
		if (n_a< 7) break; hp[ 7]=asp[- 6];
		if (n_a< 8) break; hp[ 8]=asp[- 7];
		if (n_a< 9) break; hp[ 9]=asp[- 8];
		if (n_a<10) break; hp[10]=asp[- 9];
		if (n_a<11) break; hp[11]=asp[-10];
		if (n_a<12) break; hp[12]=asp[-11];
		if (n_a<13) break; hp[13]=asp[-12];
		if (n_a<14) break; hp[14]=asp[-13];
		if (n_a<15) break; hp[15]=asp[-14];
		if (n_a<16) break; hp[16]=asp[-15];
		if (n_a<17) break; hp[17]=asp[-16];
		if (n_a<18) break; hp[18]=asp[-17];
		if (n_a<19) break; hp[19]=asp[-18];
		if (n_a<20) break; hp[20]=asp[-19];
		if (n_a<21) break; hp[21]=asp[-20];
		if (n_a<22) break; hp[22]=asp[-21];
		if (n_a<23) break; hp[23]=asp[-22];
		if (n_a<24) break; hp[24]=asp[-23];
		if (n_a<25) break; hp[25]=asp[-24];
		if (n_a<26) break; hp[26]=asp[-25];
		if (n_a<27) break; hp[27]=asp[-26];
		if (n_a<28) break; hp[28]=asp[-27];
		if (n_a<29) break; hp[29]=asp[-28];
		if (n_a<30) break; hp[30]=asp[-29];
		if (n_a<31) break; hp[31]=asp[-30];
	} while (0);
	asp-=n_a;
	*++asp=(BC_WORD)hp;
	hp+=n_a+2;
	hp[-1]=*bsp++;
	continue;
}
case Ccreate:
	if ((heap_free-=3)<0)
		break;
	hp[0]=(BC_WORD)&__cycle__in__spine;
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=1;
	continue;
case Ccreates:
{
	BC_WORD n_a_p_1,i;
	
	n_a_p_1=pc[1];
	if ((heap_free-=n_a_p_1)<0)
		break;
	hp[0]=(BC_WORD)&__cycle__in__spine; /* to do */
	hp[1]=(BC_WORD)hp;
	hp[2]=(BC_WORD)hp;
	pc+=2;
	for (i=3; i<n_a_p_1; ++i)
		hp[i]=(BC_WORD)hp;
	*++asp=(BC_WORD)hp;
	hp+=n_a_p_1;
	continue;
}
case Ccreate_array:
{
	BC_WORD s,n;
	
	s=bsp[0];
	if ((heap_free-=s+3)<0)
		break;
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=0;
	n = asp[0];
	asp[0]=(BC_WORD)hp;
	hp+=3;
	pc+=1;
	while (s!=0){
		hp[0] = n;
		hp+=1;
		--s;
	}
	continue;
}
case Ccreate_arrayBOOL:
{
	BC_WORD s,i,n,sw;
	
	s=bsp[0];
	sw=(s+3)>>2;
	if ((heap_free-=sw+3)<0)
		break;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&BOOL+2;
	hp+=3;
	n=(BC_BOOL)bsp[1];
	n = (n<<24) | (n<<16) | (n<<8) | n;
	bsp+=2;
	*++asp=(BC_WORD)hp;
	for (i=0; i!=sw; ++i)
		hp[i]=n;
	hp+=sw;
	pc+=1;
	continue;			
}
case Ccreate_arrayCHAR:
{
	BC_WORD s,i,n,sw;
	
	s=bsp[0];
	sw=(s+3)>>2;
	if ((heap_free-=sw+2)<0)
		break;
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=s;
	*++asp=(BC_WORD)hp;
	hp+=2;
	n=(BC_BOOL)bsp[1];
	n = (n<<24) | (n<<16) | (n<<8) | n;
	bsp+=2;
	for (i=0; i!=sw; ++i)
		hp[i]=n;
	hp+=sw;
	pc+=1;
	continue;			
}
case Ccreate_arrayINT:
{
	BC_WORD s,i,n;
	
	s=bsp[0];
	if ((heap_free-=s+3)<0)
		break;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&INT+2;
	*++asp=(BC_WORD)hp;
	hp+=3;
	n=bsp[1];
	bsp+=2;
	pc+=1;
	for (i=0; i!=s; ++i)
		hp[i]=n;
	hp+=s;
	continue;			
}
case Ccreate_array_:
{
	BC_WORD s,i;
	
	s=bsp[0];
	if ((heap_free-=s+3)<0)
		break;
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=0;
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=1;
	for (i=0; i!=s; ++i)
		hp[i]=((BC_WORD)&__Nil)-4;
	hp+=s;
	continue;
}
case Ccreate_array_BOOL:
{
	BC_WORD s,sw;
	
	s=bsp[0];
	sw=(s+15)>>2;
	if ((heap_free-=sw)<0)
		break;
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&BOOL+2;
	*++asp=(BC_WORD)hp;
	hp+=sw;
	bsp+=1;
	pc+=1;
	continue;			
}
case Ccreate_array_CHAR:
{
	BC_WORD s,sw;
	
	s=bsp[0];
	sw=(s+11)>>2;
	if ((heap_free-=sw)<0)
		break;
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=s;
	*++asp=(BC_WORD)hp;
	hp+=sw;
	bsp+=1;
	pc+=1;
	continue;			
}
case Ccreate_array_INT:
{
	BC_WORD s;
	
	s=bsp[0];
	if ((heap_free-=s+3)<0)
		break;
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&INT+2;
	*++asp=(BC_WORD)hp;
	hp+=3+s;
	pc+=1;
	continue;
}
case Ccreate_array_r:
{
	BC_WORD s,i,n_ab,n_b,n_a;
	BC_WORD *array;
	
	s=bsp[0];
	n_ab=pc[1];
	if ((heap_free-=s*n_ab+3)<0)
		break;
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=*(BC_WORD*)&pc[3];
	array=hp;
	hp+=3;
	n_b=pc[2];
	n_a=n_ab-n_b;
	asp-=n_a-1;
	pc+=4;
	i=0;
	while (i!=n_a){
		*--bsp = asp[i];
		++i;
	}
	if (n_ab>2){
		while (s!=0){
			hp[0] = bsp[0];
			hp[1] = bsp[1];
			for (i=2; i<n_ab; ++i)
				hp[i] = bsp[i];
			hp+=n_ab;
			--s;
		}
	} else if (n_ab==1){
		while (s!=0){
			hp[0] = bsp[0];
			hp+=1;
			--s;
		}
	} else if (n_ab==2){
		while (s!=0){
			hp[0] = bsp[0];
			hp[1] = bsp[1];
			hp+=2;
			--s;
		}
	}
	bsp+=n_ab;
	asp[0]=(BC_WORD)array;
	continue;
}
case Ccreate_array_r_:
{
	BC_WORD s,i,n_ab,n_a;
	
	s=bsp[0];
	n_ab=pc[1];
	if ((heap_free-=s*n_ab+3)<0)
		break;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=*(BC_WORD*)&pc[3];
	*++asp=(BC_WORD)hp;
	hp+=3;
	++bsp;
	n_a=pc[2];
	pc+=4;
	while (s!=0){
		for (i=0; i!=n_a; ++i)
			hp[i] = ((BC_WORD)&__Nil)-4;
		hp+=n_ab;
		--s;
	}
	continue;
}
case Ccreate_array_r_a:
{
	BC_WORD s,i,n_a,a_n_a;
	
	s=bsp[0];
	n_a=pc[1];
	a_n_a=s*n_a;
	if ((heap_free-=a_n_a+3)<0)
		break;
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=*(BC_WORD*)&pc[2];
	hp+=3;
	*++asp=(BC_WORD)hp;
	pc+=3;
	for (i=0; i!=a_n_a; ++i)
		hp[i] = ((BC_WORD)&__Nil)-4;
	hp+=a_n_a;
	continue;
}
case Ccreate_array_r_b:
{
	BC_WORD s,sw;
	
	s=bsp[0];
	sw=pc[1]*s+3;
	if ((heap_free-=sw)<0)
		break;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=*(BC_WORD*)&pc[2];
	++bsp;
	*++asp=(BC_WORD)hp;
	hp+=sw;
	pc+=3;
	continue;
}
case CdecI:
	bsp[0]=bsp[0] - 1;
	pc+=1;
	continue;
case CdivI:
	bsp[1]=bsp[0] / bsp[1];
	++bsp;
	pc+=1;
	continue;
case Ceq_desc:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
#if 0
	printf ("Ceq_desc %d %d %d\n",(int)n,(int)*n,(int)*n-(int)data);
#endif
	*--bsp = *n==pc[2];
	pc+=3;
	continue;
}
case CeqAC:
{
	BC_WORD *n,*s;
	BC_WORD length;

	n=(BC_WORD*)asp[0];
	s=(BC_WORD*)asp[-1];
	asp-=2;
	pc+=1;
	*--bsp=0;
	length=s[1];
	if (n[1]!=length)
		continue;
	n+=2;
	s+=2;
	for (;;){
		// TODO make 64/32-bit agnostic
		if (length>=4){
			if (*n!=*s)
				break;
			++n;
			++s;
			length-=4;
			continue;
		}
		if (length>=2){
			if (*(uint16_t*)n!=*(uint16_t*)s)
				break;
			if (length>2)
				if (((BC_BOOL*)n)[2]!=((BC_BOOL*)s)[2])
					break;
		} else {
			if (length>0)
				if (((BC_BOOL*)n)[0]!=((BC_BOOL*)s)[0])
					break;
		}
		*bsp=1;
		break;
	}
	continue;
}
case CeqAC_a:
{
	BC_WORD *n,*s;
	BC_WORD length;

	n=(BC_WORD*)asp[0];
	asp-=1;
	s=(BC_WORD*)*(BC_WORD*)&pc[1];
	pc+=2;
	*--bsp=0;
	length=s[0];
	if (n[1]!=length)
		continue;
	n+=2;
	s+=1;
	for (;;){
		// TODO make 64/32-bit agnostic
		if (length>=4){
			if (*n!=*s)
				break;
			++n;
			++s;
			length-=4;
			continue;
		}
		if (length>=2){
			if (*(uint16_t*)n!=*(uint16_t*)s)
				break;
			if (length>2)
				if (((BC_BOOL*)n)[2]!=((BC_BOOL*)s)[2])
					break;
		} else {
			if (length>0)
				if (((BC_BOOL*)n)[0]!=((BC_BOOL*)s)[0])
					break;
		}
		*bsp=1;
		break;
	}
	continue;
}
case CeqB:
case CeqC:
case CeqI:
	bsp[1]=bsp[0] == bsp[1];
	++bsp;
	pc+=1;
	continue;
case CeqB_aFALSE:
{
	BC_WORD_S ao;
	BC_WORD *n;
	
	ao=((BC_WORD_S*)pc)[1];
	n=(BC_WORD*)asp[ao];
	*--bsp = n[1]==0;
	pc+=2;
	continue;
}
case CeqB_aTRUE:
{
	BC_WORD_S ao;
	BC_WORD *n;
	
	ao=((BC_WORD_S*)pc)[1];
	n=(BC_WORD*)asp[ao];
	*--bsp = n[1]!=0;
	pc+=2;
	continue;
}
case CeqB_bFALSE:
{
	int b;
	BC_WORD_S bo;
	
	bo=((BC_WORD_S*)pc)[1];
	b=bsp[bo]==0;
	*--bsp=b;
	pc+=2;
	continue;
}
case CeqB_bTRUE:
{
	int b;
	BC_WORD_S bo;
	
	bo=((BC_WORD_S*)pc)[1];
	b=bsp[bo]!=0;
	*--bsp=b;
	pc+=2;
	continue;
}
case CeqC_a:
case CeqI_a:
{
	BC_WORD_S ao;
	BC_WORD *n;
	
	ao=((BC_WORD_S*)pc)[1];
	n=(BC_WORD*)asp[ao];
	*--bsp = n[1]==*(BC_WORD*)&pc[2];
	pc+=3;
	continue;
}
case CeqC_b:
case CeqI_b:
{
	int b;
	BC_WORD_S bo;
	
	bo=((BC_WORD_S*)pc)[1];
	b=bsp[bo]==(BC_WORD_S)pc[2];
	*--bsp=b;
	pc+=3;
	continue;
}
case CeqD_b:
{
	int b;
	
	b=*bsp==*(BC_WORD_S*)&pc[1];
	*--bsp=b;
	pc+=2;
	continue;
}
case Cfill:
{
	BC_WORD *n,s;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	s=pc[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[0];
	n[2]=asp[-1];
	n[3]=asp[-2];
	n[4]=asp[-3];
	pc+=4;
	do {
		if (s< 5) break; n[ 5]=asp[ -4];
		if (s< 6) break; n[ 6]=asp[ -5];
		if (s< 7) break; n[ 7]=asp[ -6];
		if (s< 8) break; n[ 8]=asp[ -7];
		if (s< 9) break; n[ 9]=asp[ -8];
		if (s<10) break; n[10]=asp[ -9];
		if (s<11) break; n[11]=asp[-10];
		if (s<12) break; n[12]=asp[-11];
		if (s<13) break; n[13]=asp[-12];
		if (s<14) break; n[14]=asp[-13];
		if (s<15) break; n[15]=asp[-14];
		if (s<16) break; n[16]=asp[-15];
		if (s<17) break; n[17]=asp[-16];
		if (s<18) break; n[18]=asp[-17];
		if (s<19) break; n[19]=asp[-18];
		if (s<20) break; n[20]=asp[-19];
		if (s<21) break; n[21]=asp[-20];
		if (s<22) break; n[22]=asp[-21];
		if (s<23) break; n[23]=asp[-22];
		if (s<24) break; n[24]=asp[-23];
		if (s<25) break; n[25]=asp[-24];
		if (s<26) break; n[26]=asp[-25];
		if (s<27) break; n[27]=asp[-26];
		if (s<28) break; n[28]=asp[-27];
		if (s<29) break; n[29]=asp[-28];
		if (s<30) break; n[30]=asp[-29];
		if (s<31) break; n[31]=asp[-30];
		if (s<32) break; n[32]=asp[-31];
	} while (0);
	asp-=s;
	continue;
}
case Cfill3:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=*(BC_WORD_S*)&pc[2];
	n[1]=asp[0];
	n[2]=asp[-1];
	n[3]=asp[-2];
	pc+=3;
	asp-=3;
	continue;
}
case Cfill4:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=*(BC_WORD_S*)&pc[2];
	n[1]=asp[0];
	n[2]=asp[-1];
	n[3]=asp[-2];
	n[4]=asp[-3];
	pc+=3;
	asp-=4;
	continue;
}
case Cfillh:
{
	BC_WORD *n,s_m_1;

	s_m_1=pc[1]; /* >=3 */
	if ((heap_free-=s_m_1)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[0];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[-1];
	hp[1]=asp[-2];
	hp[2]=asp[-3];
	pc+=4;
	do {
		if (s_m_1< 4) break; hp[ 3]=asp[ -4];
		if (s_m_1< 5) break; hp[ 4]=asp[ -5];
		if (s_m_1< 6) break; hp[ 5]=asp[ -6];
		if (s_m_1< 7) break; hp[ 6]=asp[ -7];
		if (s_m_1< 8) break; hp[ 7]=asp[ -8];
		if (s_m_1< 9) break; hp[ 8]=asp[ -9];
		if (s_m_1<10) break; hp[ 9]=asp[-10];
		if (s_m_1<11) break; hp[10]=asp[-11];
		if (s_m_1<12) break; hp[11]=asp[-12];
		if (s_m_1<13) break; hp[12]=asp[-13];
		if (s_m_1<14) break; hp[13]=asp[-14];
		if (s_m_1<15) break; hp[14]=asp[-15];
		if (s_m_1<16) break; hp[15]=asp[-16];
		if (s_m_1<17) break; hp[16]=asp[-17];
		if (s_m_1<18) break; hp[17]=asp[-18];
		if (s_m_1<19) break; hp[18]=asp[-19];
		if (s_m_1<20) break; hp[19]=asp[-20];
		if (s_m_1<21) break; hp[20]=asp[-21];
		if (s_m_1<22) break; hp[21]=asp[-22];
		if (s_m_1<23) break; hp[22]=asp[-23];
		if (s_m_1<24) break; hp[23]=asp[-24];
		if (s_m_1<25) break; hp[24]=asp[-25];
		if (s_m_1<26) break; hp[25]=asp[-26];
		if (s_m_1<27) break; hp[26]=asp[-27];
		if (s_m_1<28) break; hp[27]=asp[-28];
		if (s_m_1<29) break; hp[28]=asp[-29];
		if (s_m_1<30) break; hp[29]=asp[-30];
		if (s_m_1<31) break; hp[30]=asp[-31];
	} while (0);
	hp+=s_m_1;
	asp-=s_m_1+1;
	continue;
}
case Cfillh0:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	pc+=3;
	continue;
}
case Cfill1:
case Cfillh1:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=*(BC_WORD_S*)&pc[2];
	n[1]=asp[0];
	--asp;
	pc+=3;
	continue;
}
case Cfill2:
case Cfillh2:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
#if 0
	printf ("Cfill(h)2 %d %d\n",(int)n,(int)pc[2]);
#endif
	n[0]=pc[2];
	n[1]=asp[0];
	n[2]=asp[-1];
	asp-=2;
	pc+=3;
	continue;
}
case Cfill1010:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[1]=*asp--;
	pc+=2;
	continue;
}
case Cfill1001:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[2]=*asp--;
	pc+=2;
	continue;
}
case Cfill1011:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[1]=asp[0];
	n[2]=asp[-1];
	asp-=2;
	pc+=2;
	continue;
}
case Cfill2a001:
{
	BC_WORD *n,*a;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	a=(BC_WORD*)n[2];
	a[ao]=*asp--;
	pc+=3;
	continue;
}
case Cfill2a011:
{
	BC_WORD *n,*a;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	a=(BC_WORD*)n[2];
	n[1]=asp[0];
	a[ao]=asp[-1];
	asp-=2;
	pc+=3;
	continue;
}
case Cfill2a012:
{
	BC_WORD *n,*a;
	BC_WORD_S ao1,ao2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao1=((BC_WORD_S*)pc)[2];
	ao2=((SS*)pc)[3];
	a=(BC_WORD*)n[2];
	n[1]=asp[0];
	a[ao1]=asp[-1];
	a[ao2]=asp[-2];
	asp-=3;
	pc+=4;
	continue;
}
case Cfill2ab011:
{
	BC_WORD *n,*a;
	BC_WORD_S a_o;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o=((BC_WORD_S*)pc)[2];
	a=(BC_WORD*)n[2];
	n[1]=asp[0];
	a[a_o]=*bsp++;
	--asp;
	pc+=3;
	continue;			
}
case Cfill2ab002:
{
	BC_WORD *n,*a;
	BC_WORD_S a_o1,a_o2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o1=((BC_WORD_S*)pc)[2];
	a_o2=((SS*)pc)[3];
	a=(BC_WORD*)n[2];
	a[a_o1]=asp[0];
	a[a_o2]=bsp[0];
	asp-=1;
	bsp+=1;
	pc+=4;
	continue;
}
case Cfill2ab003:
{
	BC_WORD *n,*a;
	BC_WORD_S a_o1,a_o2,a_o3;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o1=((BC_WORD_S*)pc)[2];
	a_o2=((SS*)pc)[3];
	a_o3=((SS*)pc)[4];
	a=(BC_WORD*)n[2];
	a[a_o1]=asp[0];
	a[a_o2]=asp[-1];
	a[a_o3]=bsp[0];
	asp-=2;
	bsp+=1;
	pc+=5;
	continue;
}
case Cfill2b001:
{
	BC_WORD *n,*a;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	a=(BC_WORD*)n[2];
	a[ao]=*bsp++;
	pc+=3;
	continue;
}
case Cfill2b002:
{
	BC_WORD *n,*a;
	BC_WORD_S ao1,ao2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao1=((BC_WORD_S*)pc)[2];
	ao2=((SS*)pc)[3];
	a=(BC_WORD*)n[2];
	a[ao1]=bsp[0];
	a[ao2]=bsp[1];
	bsp+=2;
	pc+=4;
	continue;
}
case Cfill2b011:
{
	BC_WORD *n,*a;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	a=(BC_WORD*)n[2];
	n[1]=bsp[0];
	a[ao]=bsp[1];
	bsp+=2;
	pc+=3;
	continue;
}
case Cfill2b012:
{
	BC_WORD *n,*a;
	BC_WORD_S ao1,ao2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao1=((BC_WORD_S*)pc)[2];
	ao2=((SS*)pc)[3];
	a=(BC_WORD*)n[2];
	n[1]=bsp[0];
	a[ao1]=bsp[1];
	a[ao2]=bsp[2];
	bsp+=3;
	pc+=4;
	continue;
}
case Cfill2ab013:
{
	BC_WORD *n,*a;
	BC_WORD_S a_o1,a_o2,a_o3;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o1=((BC_WORD_S*)pc)[2];
	a_o2=((SS*)pc)[3];
	a_o3=((SS*)pc)[4];
	a=(BC_WORD*)n[2];
	n[1]=asp[0];
	a[a_o1]=asp[-1];
	a[a_o2]=asp[-2];
	a[a_o3]=bsp[0];
	asp-=3;
	bsp+=1;
	pc+=5;
	continue;
}
case Cfill3a10:
{
	BC_WORD *ns,*nd;
	BC_WORD_S ao;

	ao=((BC_WORD_S*)pc)[1];
	ns=(BC_WORD*)asp[0];
	nd=(BC_WORD*)asp[ao];
	ns[0]=(BC_WORD)&__cycle__in__spine;
	nd[0]=*(BC_WORD_S*)&pc[2];
	pc+=3;
	nd[1]=asp[-1];
	nd[2]=ns[2];
	asp-=2;
	continue;
}
case Cfill3a11:
{
	BC_WORD *ns,*nd,*a;
	BC_WORD_S ao,a_o1;

	ao=((BC_WORD_S*)pc)[1];
	ns=(BC_WORD*)asp[0];
	nd=(BC_WORD*)asp[ao];
	a_o1=((SS*)pc)[3];
	a=(BC_WORD*)ns[2];
	ns[0]=(BC_WORD)&__cycle__in__spine;
	nd[0]=*(BC_WORD_S*)&pc[2];
	pc+=4;
	nd[1]=asp[-1];
	nd[2]=(BC_WORD)a;
	a[a_o1]=asp[-2];
	asp-=3;
	continue;
}
case Cfill3a12:
{
	BC_WORD *ns,*nd,*a;
	BC_WORD_S ao,a_o1,a_o2;

	ao=((BC_WORD_S*)pc)[1];
	ns=(BC_WORD*)asp[0];
	nd=(BC_WORD*)asp[ao];
	a_o1=((SS*)pc)[3];
	a_o2=((SS*)pc)[4];
	a=(BC_WORD*)ns[2];
	ns[0]=(BC_WORD)&__cycle__in__spine;
	nd[0]=*(BC_WORD_S*)&pc[2];
	pc+=5;
	nd[1]=asp[-1];
	nd[2]=(BC_WORD)a;
	a[a_o1]=asp[-2];
	a[a_o2]=asp[-3];
	asp-=4;
	continue;
}
case Cfill3aaab13:
{
	BC_WORD *ns,*nd,*a;
	BC_WORD_S ao,a_o1,a_o2,b_o1;

	ao=((BC_WORD_S*)pc)[1];
	ns=(BC_WORD*)asp[0];
	nd=(BC_WORD*)asp[ao];
	a_o1=((SS*)pc)[3];
	a_o2=((SS*)pc)[4];
	b_o1=((SS*)pc)[5];
	a=(BC_WORD*)ns[2];
	ns[0]=(BC_WORD)&__cycle__in__spine;
	nd[0]=*(BC_WORD_S*)&pc[2];
	pc+=6;
	nd[1]=asp[-1];
	nd[2]=(BC_WORD)a;
	a[a_o1]=asp[-2];
	a[a_o2]=asp[-3];
	a[b_o1]=*bsp++;
	asp-=4;
	continue;
}
case Cfillcaf:
{
	BC_WORD *n,*a;
	BC_WORD n_a,n_b,ui,b_o1;
	n=(BC_WORD*)pc[1];
	n_a=pc[2];
	n_b=pc[3];
	pc+=4;
	b_o1=1+n_a;
	if (n_a!=0){
		n[0]=n_a;
		for (ui=0; ui!=n_a; ++ui)
			n[1+ui]=asp[-ui];					
	} else
		n[0]=1;
	
	for (ui=0; ui!=n_b; ++ui)
		n[b_o1+ui]=bsp[ui];

	if (n_a!=0){
		a=(BC_WORD*)caf_list[1];
		a[-1]=(BC_WORD)n;
		caf_list[1]=n;
	}
	continue;
}
case Cfillh3:
{
	BC_WORD *n;

	if ((heap_free-=2)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=*(BC_WORD_S*)&pc[2];
	n[1]=asp[0];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[-1];
	hp[1]=asp[-2];
	asp-=3;
	hp+=2;
	pc+=3;
	continue;
}
case Cfillh4:
{
	BC_WORD *n;

	if ((heap_free-=3)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=*(BC_WORD_S*)&pc[2];
	n[1]=asp[0];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[-1];
	hp[1]=asp[-2];
	hp[2]=asp[-3];
	asp-=4;
	hp+=3;
	pc+=3;
	continue;
}
case CfillB_b:
{
	BC_WORD *n,i;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	i=(BC_WORD)bsp[((BC_WORD_S*)pc)[2]];
	n[0]=(BC_WORD)&BOOL+2;
	n[1]=i;
	pc+=3;
	continue;
}
case CfillC_b:
{
	BC_WORD *n,i;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	i=(BC_WORD)bsp[((BC_WORD_S*)pc)[2]];
	n[0]=(BC_WORD)&CHAR+2;
	n[1]=i;
	pc+=3;
	continue;
}
case CfillF_b:
{
	BC_WORD *n;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=((BC_WORD_S*)pc)[2];
	n[0]=(BC_WORD)&dFILE+2;
	n[1]=(BC_WORD)bsp[bo];
	n[2]=(BC_WORD)bsp[bo+1];
	pc+=3;
	continue;
}
case CfillI_b:
{
	BC_WORD *n,i;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	i=(BC_WORD_S)bsp[pc[2]];
#if 0
	printf ("fillI_b %d\n",i);
#endif
	n[0]=(BC_WORD)&INT+2;
	n[1]=i;
	pc+=3;
	continue;
}
case Cfill_a:
{
	BC_WORD ao_s,ao_d;
	BC_WORD *n_s,*n_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	n_s=(BC_WORD*)asp[ao_s];
	n_d=(BC_WORD*)asp[ao_d];
#if 0
	printf ("Cfill_a %d %d\n",(int)n_s,(int)n_d);
#endif
	n_d[0]=n_s[0];
	n_d[1]=n_s[1];
	n_d[2]=n_s[2];
	pc+=3;
	continue;
}
case Cfill_a01_pop_rtn:
{
	BC_WORD *n_s,*n_d;

	n_s=(BC_WORD*)asp[0];
	n_d=(BC_WORD*)asp[-1];
#if 0
	printf ("Cfill_a01_pop_rtn %d %d\n",(int)n_s,(int)n_d);
#endif
	asp-=1;
	pc=(BC_WORD*)*csp++;
	n_d[0]=n_s[0];
	n_d[1]=n_s[1];
	n_d[2]=n_s[2];
	continue;
}
case Cfill_r:
{
	BC_WORD_S ao,bo;
	BC_WORD *n,n_a,n_b,n_ab,*ao_p,*bo_p;

	n_ab=pc[1];
	if ((heap_free-=n_ab-1)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	ao=((SS*)pc)[3];
	n[0]=*(BC_WORD*)&pc[4];
	n[1]=asp[ao];
	ao_p=&asp[ao];
	n[2]=(BC_WORD)hp;
	n_a=pc[5];
	n_b=n_ab-n_a;
	hp[0]=ao_p[-1];
	do {
		if (n_a< 3) break; hp[ 1]=ao_p[- 2];
		if (n_a< 4) break; hp[ 2]=ao_p[- 3];
		if (n_a< 5) break; hp[ 3]=ao_p[- 4];
		if (n_a< 6) break; hp[ 4]=ao_p[- 5];
		if (n_a< 7) break; hp[ 5]=ao_p[- 6];
		if (n_a< 8) break; hp[ 6]=ao_p[- 7];
		if (n_a< 9) break; hp[ 7]=ao_p[- 8];
		if (n_a<10) break; hp[ 8]=ao_p[- 9];
		if (n_a<11) break; hp[ 9]=ao_p[-10];
		if (n_a<12) break; hp[10]=ao_p[-11];
		if (n_a<13) break; hp[11]=ao_p[-12];
		if (n_a<14) break; hp[12]=ao_p[-13];
		if (n_a<15) break; hp[13]=ao_p[-14];
		if (n_a<16) break; hp[14]=ao_p[-15];
		if (n_a<17) break; hp[15]=ao_p[-16];
		if (n_a<18) break; hp[16]=ao_p[-17];
		if (n_a<19) break; hp[17]=ao_p[-18];
		if (n_a<20) break; hp[18]=ao_p[-19];
		if (n_a<21) break; hp[19]=ao_p[-20];
		if (n_a<22) break; hp[20]=ao_p[-21];
		if (n_a<23) break; hp[21]=ao_p[-22];
		if (n_a<24) break; hp[22]=ao_p[-23];
		if (n_a<25) break; hp[23]=ao_p[-24];
		if (n_a<26) break; hp[24]=ao_p[-25];
		if (n_a<27) break; hp[25]=ao_p[-26];
		if (n_a<28) break; hp[26]=ao_p[-27];
		if (n_a<29) break; hp[27]=ao_p[-28];
		if (n_a<30) break; hp[28]=ao_p[-29];
	} while (0);
	bo=((SS*)pc)[6];
	pc+=7;
	hp+=n_a-1;
	hp[0]=bsp[bo];
	bo_p=&bsp[bo];
	hp[1]=bo_p[1];
	do {
		if (n_b< 3) break; hp[ 2]=bo_p[ 2];
		if (n_b< 4) break; hp[ 3]=bo_p[ 3];
		if (n_b< 5) break; hp[ 4]=bo_p[ 4];
		if (n_b< 6) break; hp[ 5]=bo_p[ 5];
		if (n_b< 7) break; hp[ 6]=bo_p[ 6];
		if (n_b< 8) break; hp[ 7]=bo_p[ 7];
		if (n_b< 9) break; hp[ 8]=bo_p[ 8];
		if (n_b<10) break; hp[ 9]=bo_p[ 9];
		if (n_b<11) break; hp[10]=bo_p[10];
		if (n_b<12) break; hp[11]=bo_p[11];
		if (n_b<13) break; hp[12]=bo_p[12];
		if (n_b<14) break; hp[13]=bo_p[13];
		if (n_b<15) break; hp[14]=bo_p[14];
		if (n_b<16) break; hp[15]=bo_p[15];
		if (n_b<17) break; hp[16]=bo_p[16];
		if (n_b<18) break; hp[17]=bo_p[17];
		if (n_b<19) break; hp[18]=bo_p[18];
		if (n_b<20) break; hp[19]=bo_p[19];
		if (n_b<21) break; hp[20]=bo_p[20];
		if (n_b<22) break; hp[21]=bo_p[21];
		if (n_b<23) break; hp[22]=bo_p[22];
		if (n_b<24) break; hp[23]=bo_p[23];
		if (n_b<25) break; hp[24]=bo_p[24];
		if (n_b<26) break; hp[25]=bo_p[25];
		if (n_b<27) break; hp[26]=bo_p[26];
		if (n_b<28) break; hp[27]=bo_p[27];
		if (n_b<29) break; hp[28]=bo_p[28];
		if (n_b<30) break; hp[29]=bo_p[29];
	} while (0);				
	hp+=n_b;
	continue;
}
case Cfill_ra0:
{
	BC_WORD_S ao;
	BC_WORD *n,n_a,*ao_p;

	n_a=pc[1];
	if ((heap_free-=n_a-1)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	ao=((SS*)pc)[3];
	n[0]=*(BC_WORD*)&pc[4];
	n[1]=asp[ao];
	ao_p=&asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=ao_p[-1];
	hp[1]=ao_p[-2];
	hp[2]=ao_p[-3];
	hp[3]=ao_p[-4];
	do {
		if (n_a< 6) break; hp[ 4]=ao_p[- 5];
		if (n_a< 7) break; hp[ 5]=ao_p[- 6];
		if (n_a< 8) break; hp[ 6]=ao_p[- 7];
		if (n_a< 9) break; hp[ 7]=ao_p[- 8];
		if (n_a<10) break; hp[ 8]=ao_p[- 9];
		if (n_a<11) break; hp[ 9]=ao_p[-10];
		if (n_a<12) break; hp[10]=ao_p[-11];
		if (n_a<13) break; hp[11]=ao_p[-12];
		if (n_a<14) break; hp[12]=ao_p[-13];
		if (n_a<15) break; hp[13]=ao_p[-14];
		if (n_a<16) break; hp[14]=ao_p[-15];
		if (n_a<17) break; hp[15]=ao_p[-16];
		if (n_a<18) break; hp[16]=ao_p[-17];
		if (n_a<19) break; hp[17]=ao_p[-18];
		if (n_a<20) break; hp[18]=ao_p[-19];
		if (n_a<21) break; hp[19]=ao_p[-20];
		if (n_a<22) break; hp[20]=ao_p[-21];
		if (n_a<23) break; hp[21]=ao_p[-22];
		if (n_a<24) break; hp[22]=ao_p[-23];
		if (n_a<25) break; hp[23]=ao_p[-24];
		if (n_a<26) break; hp[24]=ao_p[-25];
		if (n_a<27) break; hp[25]=ao_p[-26];
		if (n_a<28) break; hp[26]=ao_p[-27];
		if (n_a<29) break; hp[27]=ao_p[-28];
		if (n_a<30) break; hp[28]=ao_p[-29];
		if (n_a<31) break; hp[29]=ao_p[-30];
		if (n_a<32) break; hp[30]=ao_p[-31];
	} while (0);
	pc+=5;
	hp+=n_a-1;
	continue;
}
case Cfill_ra1:
{
	BC_WORD_S ao,bo;
	BC_WORD *n,n_a,*ao_p;

	n_a=pc[1];
	if ((heap_free-=n_a)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	ao=((SS*)pc)[3];
	n[0]=*(BC_WORD*)&pc[4];
	n[1]=asp[ao];
	ao_p=&asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=ao_p[-1];
	hp[1]=ao_p[-2];
	hp[2]=ao_p[-3];
	do {
		if (n_a< 5) break; hp[ 3]=ao_p[- 4];
		if (n_a< 6) break; hp[ 4]=ao_p[- 5];
		if (n_a< 7) break; hp[ 5]=ao_p[- 6];
		if (n_a< 8) break; hp[ 6]=ao_p[- 7];
		if (n_a< 9) break; hp[ 7]=ao_p[- 8];
		if (n_a<10) break; hp[ 8]=ao_p[- 9];
		if (n_a<11) break; hp[ 9]=ao_p[-10];
		if (n_a<12) break; hp[10]=ao_p[-11];
		if (n_a<13) break; hp[11]=ao_p[-12];
		if (n_a<14) break; hp[12]=ao_p[-13];
		if (n_a<15) break; hp[13]=ao_p[-14];
		if (n_a<16) break; hp[14]=ao_p[-15];
		if (n_a<17) break; hp[15]=ao_p[-16];
		if (n_a<18) break; hp[16]=ao_p[-17];
		if (n_a<19) break; hp[17]=ao_p[-18];
		if (n_a<20) break; hp[18]=ao_p[-19];
		if (n_a<21) break; hp[19]=ao_p[-20];
		if (n_a<22) break; hp[20]=ao_p[-21];
		if (n_a<23) break; hp[21]=ao_p[-22];
		if (n_a<24) break; hp[22]=ao_p[-23];
		if (n_a<25) break; hp[23]=ao_p[-24];
		if (n_a<26) break; hp[24]=ao_p[-25];
		if (n_a<27) break; hp[25]=ao_p[-26];
		if (n_a<28) break; hp[26]=ao_p[-27];
		if (n_a<29) break; hp[27]=ao_p[-28];
		if (n_a<30) break; hp[28]=ao_p[-29];
		if (n_a<31) break; hp[29]=ao_p[-30];
	} while (0);
	bo=((SS*)pc)[5];
	pc+=6;
	hp[n_a-1]=bsp[bo];
	hp+=n_a;
	continue;
}
case Cfill_r1b:
{
	BC_WORD_S ao,bo;
	BC_WORD *n,n_b,*bo_p;

	n_b=pc[1];
	if ((heap_free-=n_b)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	ao=((SS*)pc)[3];
	n[0]=*(BC_WORD*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	bo=((SS*)pc)[5];
	pc+=6;
	hp[0]=bsp[bo];
	bo_p=&bsp[bo];
	hp[1]=bo_p[1];
	hp[2]=bo_p[2];
	hp[3]=bo_p[3];
	do {
		if (n_b< 5) break; hp[ 4]=bo_p[ 4];
		if (n_b< 6) break; hp[ 5]=bo_p[ 5];
		if (n_b< 7) break; hp[ 6]=bo_p[ 6];
		if (n_b< 8) break; hp[ 7]=bo_p[ 7];
		if (n_b< 9) break; hp[ 8]=bo_p[ 8];
		if (n_b<10) break; hp[ 9]=bo_p[ 9];
		if (n_b<11) break; hp[10]=bo_p[10];
		if (n_b<12) break; hp[11]=bo_p[11];
		if (n_b<13) break; hp[12]=bo_p[12];
		if (n_b<14) break; hp[13]=bo_p[13];
		if (n_b<15) break; hp[14]=bo_p[14];
		if (n_b<16) break; hp[15]=bo_p[15];
		if (n_b<17) break; hp[16]=bo_p[16];
		if (n_b<18) break; hp[17]=bo_p[17];
		if (n_b<19) break; hp[18]=bo_p[18];
		if (n_b<20) break; hp[19]=bo_p[19];
		if (n_b<21) break; hp[20]=bo_p[20];
		if (n_b<22) break; hp[21]=bo_p[21];
		if (n_b<23) break; hp[22]=bo_p[22];
		if (n_b<24) break; hp[23]=bo_p[23];
		if (n_b<25) break; hp[24]=bo_p[24];
		if (n_b<26) break; hp[25]=bo_p[25];
		if (n_b<27) break; hp[26]=bo_p[26];
		if (n_b<28) break; hp[27]=bo_p[27];
		if (n_b<29) break; hp[28]=bo_p[28];
		if (n_b<30) break; hp[29]=bo_p[29];
		if (n_b<31) break; hp[30]=bo_p[30];
	} while (0);				
	hp+=n_b;
	continue;
}
case Cfill_r01:
{
	BC_WORD *n;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=bsp[bo];
	pc+=4;
	continue;
}
case Cfill_r10:
{
	BC_WORD *n;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[ao];
	pc+=4;
	continue;
}
case Cfill_r02:
{
	BC_WORD *n;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=bsp[bo];
	n[2]=bsp[bo+1];
	pc+=4;
	continue;
}
case Cfill_r03:
{
	BC_WORD *n;
	BC_WORD_S bo;

	if ((heap_free-=2)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=bsp[bo];
	n[2]=(BC_WORD)hp;
	hp[0]=bsp[bo+1];
	hp[1]=bsp[bo+2];
	hp+=2;
	pc+=4;
	continue;
}
case Cfill_r11:
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((SS*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=bsp[bo];
	pc+=5;
	continue;
}
case Cfill_r12:
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	if ((heap_free-=2)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((SS*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=bsp[bo];
	hp[1]=bsp[bo+1];
	hp+=2;
	pc+=5;
	continue;
}
case Cfill_r13:
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	if ((heap_free-=3)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((SS*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=bsp[bo];
	hp[1]=bsp[bo+1];
	hp[2]=bsp[bo+2];
	hp+=3;
	pc+=5;
	continue;
}
case Cfill_r20:
{
	BC_WORD *n;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[ao];
	n[2]=asp[ao-1];
	pc+=4;
	continue;
}
case Cfill_r21:
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	if ((heap_free-=2)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((SS*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=bsp[bo];
	hp+=2;
	pc+=5;
	continue;
}
case Cfill_r22:
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	if ((heap_free-=3)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((SS*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=bsp[bo];
	hp[2]=bsp[bo+1];
	hp+=3;
	pc+=5;
	continue;
}
case Cfill_r30:
{
	BC_WORD *n;
	BC_WORD_S ao;

	if ((heap_free-=2)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=asp[ao-2];
	hp+=2;
	pc+=4;
	continue;
}
case Cfill_r31:
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	if ((heap_free-=3)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((SS*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=asp[ao-2];
	hp[2]=bsp[bo];
	hp+=3;
	pc+=5;
	continue;
}
case Cfill_r40:
{
	BC_WORD *n;
	BC_WORD_S ao;

	if ((heap_free-=3)<0)
		break;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=asp[ao-2];
	hp[2]=asp[ao-3];
	hp+=3;
	pc+=4;
	continue;
}
case Cget_node_arity:
{
	BC_WORD *n;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=((SS*)(n[0]))[-1];
	pc+=2;
	continue;
}
case CgtI:
	bsp[1] = (BC_WORD_S)bsp[0] > (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	continue;
case Chalt:
	printf("\nhalt at %d\n", (int) (pc-code));
#if 0
	printf ("pc = %d __indirection = %d __cycle__in__spine = %d\n",(int)pc,(int)&__indirection,(int)&__cycle__in__spine);
#endif
	printf("%d %d %d\n", (int) (hp-heap), (int) heap_free, (int) (hp-heap+heap_free));
	return 0;
/*				exit (1); */
case CincI:
	bsp[0]=bsp[0] + 1;
	pc+=1;
	continue;
case Cjmp:
	pc=(BC_WORD*)pc[1];
	continue;
case Cjmp_eval:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[0];
	if ((n[0] & 2)!=0){
		pc=(BC_WORD*)*csp++;
		continue;
	}
	pc=(BC_WORD*)n[0];
	continue;
}
case Cjmp_eval_upd:
{
	BC_WORD *n1,*n2,d;
	n1=(BC_WORD*)asp[0];
	d=n1[0];
#if 0
	printf ("jmp_eval_upd %d %d\n",(int)n1,(int)d);
#endif			
	if ((d & 2)!=0){
		n2=(BC_WORD*)asp[-1];
		pc=(BC_WORD*)*csp++;
		n2[0]=d;
		n2[1]=n1[1];
		n2[2]=n1[2];
		--asp;
		continue;
	}
#if 0
	printf ("jmp_eval_upd %d %d\n",(int)d-(int)program,((int)d-(int)program)>>2);
#endif
	pc=(BC_WORD*)(d-20);
	continue;
}
case Cjmp_false:
	if (*bsp++){
		pc+=2;
		continue;
	}
	pc=(BC_WORD*)pc[1];
	continue;
case Cjmp_true:
	if (! *bsp++){
		pc+=2;
		continue;
	}
	pc=(BC_WORD*)pc[1];
	continue;
case Cjsr:
	*--csp=(BC_WORD)&pc[2];
	pc=(BC_WORD*)pc[1];
	continue;
case Cjsr_eval:
{
	BC_WORD *n;
	BC_WORD_S ao;

	ao=pc[1];
	n=(BC_WORD*)asp[(BC_WORD_S)ao];	
	if ((n[0] & 2)!=0){
		pc+=4;
		continue;
	}
	*--csp=(BC_WORD)&pc[2];
	pc=(BC_WORD*)n[0];
	asp[(BC_WORD_S)ao]=asp[0];
	asp[0]=(BC_WORD)n;
	continue;
}
case Cjsr_eval0:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[0];
#if 0
	printf ("Cjsr_eval0 %d\n",n);
	printf ("Cjsr_eval0 %d\n",(int)n[0]);
#endif
	pc+=1;
	if ((n[0] & 2)!=0)
		continue;
#if 0
	printf ("Cjsr_eval0 %d %d\n",n,(int)n[0]);
#endif
	*--csp=(BC_WORD)pc;
	pc=(BC_WORD*)n[0];
	continue;
}
case Cjsr_eval1:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[-1];
	if ((n[0] & 2)!=0){
		pc+=2;
		continue;
	}
	*--csp=(BC_WORD)&pc[1];
	pc=(BC_WORD*)n[0];
	asp[-1]=asp[0];
	asp[0]=(BC_WORD)n;
	continue;
}
case Cjsr_eval2:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[-2];
	if ((n[0] & 2)!=0){
		pc+=2;
		continue;
	}
	*--csp=(BC_WORD)&pc[1];		
	pc=(BC_WORD*)n[0];
	asp[-2]=asp[0];
	asp[0]=(BC_WORD)n;
	continue;
}
case Cjsr_eval3:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[-3];
	if ((n[0] & 2)!=0){
		pc+=2;
		continue;
	}
	*--csp=(BC_WORD)&pc[1];
	pc=(BC_WORD*)n[0];
	asp[-3]=asp[0];
	asp[0]=(BC_WORD)n;
	continue;
}
case CltC:
case CltI:
	bsp[1] = (BC_WORD_S)bsp[0] < (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	continue;
case CmulI:
	bsp[1]=bsp[0] * bsp[1];
	++bsp;
	pc+=1;
	continue;
case CnegI:
	*bsp = - *bsp;
	pc+=1;
	continue;
case CnotB:
	*bsp = *bsp==0;
	pc+=1;
	continue;
case CnotI:
	*bsp = ~ *bsp;
	pc+=1;
	continue;
case CorI:
	bsp[1]=bsp[0] | bsp[1];
	++bsp;
	pc+=1;
	continue;
case Cpop_a:
	asp=(BC_WORD*)(((BC_BOOL*)asp)+((BC_WORD_S*)pc)[1]);
	pc+=2;
	continue;
case Cpop_b:
	bsp=(BC_WORD*)(((BC_BOOL*)bsp)+(2*pc[1])); // TODO the 2* is 64-bit specific (#4)
	pc+=2;
	continue;
case Cprint:
{
	BC_WORD *s;
	char *cs;
	BC_WORD length;
	int i;

	s=(BC_WORD*)pc[1];
	pc+=2;
	length=s[0];
	cs=(char*)&s[1];
	for (i=0; i<length; ++i)
		putchar (cs[i]);
	continue;
}
case Cprint_symbol_sc:
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	d=n[0];
	if (d==(BC_WORD)&INT+2){
		printf ("%d",(int)n[1]);
	} else if (d==(BC_WORD)&CHAR+2){
		printf ("'%c'",(int)n[1]);
	} else {
		BC_WORD *s;
		int l,i;
		char *cs;

		s=(BC_WORD*)((BC_WORD)d+10+*(uint16_t*)d);
		l=s[0];
#if 0
		printf ("? %d %d ",(int)d-(int)data,l);
#endif
		cs=(char*)&s[1];
		for (i=0; i<l; ++i)
			putchar (cs[i]);
	}
	pc+=2;
	continue;
}
case CpushBFALSE:
	*--bsp=0;
	pc+=1;
	continue;
case CpushBTRUE:
	*--bsp=1;
	pc+=1;
	continue;
case CpushC:
case CpushD:
case CpushI:
	*--bsp=pc[1];
	pc+=2;
	continue;
case CpushB_a:
case CpushC_a:
case CpushI_a:
case Cpush_r_args_b0b11:
case Cpush_r_args01:
case Cpush_arraysize_a:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=n[1];
	pc+=2;
	continue;
}
case CpushD_a:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=*n;
	pc+=2;
	continue;
}
case CpushF_a:
case Cpush_r_args02:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bsp[-2]=n[1];
	bsp[-1]=n[2];
	bsp-=2;
	pc+=2;
	continue;
}
case Cpushcaf10:
	*++asp=*(BC_WORD*)pc[1];
	pc+=2;
	continue;
case Cpushcaf31:
{
	BC_WORD *n;
	
	n=(BC_WORD*)pc[1];
	asp[3]=n[0];
	asp[2]=n[1];
	asp[1]=n[2];
	*--bsp=n[3];
	asp+=3;
	pc+=2;
	continue;
}
case Cpush_a:
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	*++asp=v;
	pc+=2;
	continue;
}
case Cpush_arg:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=a[((BC_WORD_S*)pc)[2]];
	pc+=3;
	continue;
}
case Cpush_arg1:
case Cpush_args1:
case Cpush_array:
case Cpush_r_args10:
case Cpush_r_args_a1:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=n[1];
	pc+=2;
	continue;
}
case Cpush_arg2:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=n[2];
	pc+=2;
	continue;
}
case Cpush_arg2l:
case Cpush_r_args_a2l:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=a[0];
	pc+=2;
	continue;
}
case Cpush_arg3:
case Cpush_r_args_a3:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=a[1];
	pc+=2;
	continue;
}
case Cpush_arg4:
case Cpush_r_args_a4:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=a[2];
	pc+=2;
	continue;
}
case Cpush_args:
case Cpush_r_argsa0:
{
	BC_WORD *n,*a,n_a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n_a=pc[2]; /* >= 5 */
	a=(BC_WORD*)n[2];
	asp+=n_a;
	asp[0] = n[1];
	asp[-1]=a[0];
	asp[-2]=a[1];
	asp[-3]=a[2];
	asp[-4]=a[3];
	pc+=3;
	do {
		if (n_a<= 5) break; asp[ -5] = a[ 4];
		if (n_a<= 6) break; asp[ -6] = a[ 5];
		if (n_a<= 7) break; asp[ -7] = a[ 6];
		if (n_a<= 8) break; asp[ -8] = a[ 7];
		if (n_a<= 9) break; asp[ -9] = a[ 8];
		if (n_a<=10) break; asp[-10] = a[ 9];
		if (n_a<=11) break; asp[-11] = a[10];
		if (n_a<=12) break; asp[-12] = a[11];
		if (n_a<=13) break; asp[-13] = a[12];
		if (n_a<=14) break; asp[-14] = a[13];
		if (n_a<=15) break; asp[-15] = a[14];
		if (n_a<=16) break; asp[-16] = a[15];
		if (n_a<=17) break; asp[-17] = a[16];
		if (n_a<=18) break; asp[-18] = a[17];
		if (n_a<=19) break; asp[-19] = a[18];
		if (n_a<=20) break; asp[-20] = a[19];
		if (n_a<=21) break; asp[-21] = a[20];
		if (n_a<=22) break; asp[-22] = a[21];
		if (n_a<=23) break; asp[-23] = a[22];
		if (n_a<=24) break; asp[-24] = a[23];
		if (n_a<=25) break; asp[-25] = a[24];
		if (n_a<=26) break; asp[-26] = a[25];
		if (n_a<=27) break; asp[-27] = a[26];
		if (n_a<=28) break; asp[-28] = a[27];
		if (n_a<=29) break; asp[-29] = a[28];
		if (n_a<=30) break; asp[-30] = a[29];
		if (n_a<=31) break; asp[-31] = a[30];
	} while (0);
	continue;
}
case Cpush_args2:
case Cpush_r_args20:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
#if 0
	printf ("Cpush_args2 %d %d %d\n",(int)n,(int)n[1],(int)n[2]);
#endif
	asp[2]=n[1];
	asp[1]=n[2];
	asp+=2;
	pc+=2;
	continue;
}
case Cpush_args3:
case Cpush_r_args30:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	asp[3]=n[1];
	asp[2]=a[0];
	asp[1]=a[1];
	asp+=3;
	pc+=2;
	continue;
}
case Cpush_args4:
case Cpush_r_args40:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	asp[4]=n[1];
	asp[3]=a[0];
	asp[2]=a[1];
	asp[1]=a[2];
	asp+=4;
	pc+=2;
	continue;			
}
case Cpush_arg_b:
{
	BC_WORD *n,*a;
	BC_WORD ui;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ui=bsp[0];
	pc+=2;
	if (ui<2){
		*++asp=n[1];
		bsp+=2;
	} else {
		a=(BC_WORD*)n[2];
		ui-=2;
		if (ui==0 && bsp[1]==2){
			*++asp=(BC_WORD)a;
			bsp+=2;
		} else {
			*++asp=a[ui];
			bsp+=2;
		}
	}				
	continue;			
}
case Cpush_a_b:
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	*--bsp=v;
	pc+=2;
	continue;
}
case Cpush_b:
{
	BC_WORD v;
	
	v=bsp[pc[1]];
	*--bsp=v;
	pc+=2;
	continue;
}
case Cpush_node:
{
	BC_WORD *n,n_a;
	
	n=(BC_WORD*)*asp;
	n_a=pc[1];
	n[0]=pc[2];

	asp+=n_a;

	asp[0]=n[1];
	asp[-1]=n[2];
	asp[-2]=n[3];
	asp[-3]=n[4];
	asp[-4]=n[5];				
	pc+=3;
	do {
		if (n_a< 6) break; asp[ -5]=n[ 6];
		if (n_a< 7) break; asp[ -6]=n[ 7];
		if (n_a< 8) break; asp[ -7]=n[ 8];
		if (n_a< 9) break; asp[ -8]=n[ 9];
		if (n_a<10) break; asp[ -9]=n[10];
		if (n_a<11) break; asp[-10]=n[11];
		if (n_a<12) break; asp[-11]=n[12];
		if (n_a<13) break; asp[-12]=n[13];
		if (n_a<14) break; asp[-13]=n[14];
		if (n_a<15) break; asp[-14]=n[15];
		if (n_a<16) break; asp[-15]=n[16];
		if (n_a<17) break; asp[-16]=n[17];
		if (n_a<18) break; asp[-17]=n[18];
		if (n_a<19) break; asp[-18]=n[19];
		if (n_a<20) break; asp[-19]=n[20];
		if (n_a<21) break; asp[-20]=n[21];
		if (n_a<22) break; asp[-21]=n[22];
		if (n_a<23) break; asp[-22]=n[23];
		if (n_a<24) break; asp[-23]=n[24];
		if (n_a<25) break; asp[-24]=n[25];
		if (n_a<26) break; asp[-25]=n[26];
		if (n_a<27) break; asp[-26]=n[27];
		if (n_a<28) break; asp[-27]=n[28];
		if (n_a<29) break; asp[-28]=n[29];
		if (n_a<30) break; asp[-29]=n[30];
		if (n_a<31) break; asp[-30]=n[31];
		if (n_a<32) break; asp[-31]=n[32];
	} while (0);
	continue;
}
case Cpush_node0:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	pc+=2;
	continue;
}
case Cpush_node1:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	*++asp=n[1];
	pc+=2;
	continue;
}
case Cpush_node2:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	asp[2]=n[1];
	asp[1]=n[2];
	asp+=2;
	pc+=2;
	continue;
}
case Cpush_node3:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	asp[3]=n[1];
	asp[2]=n[2];
	asp[1]=n[3];
	asp+=3;
	pc+=2;
	continue;
}
case Cpush_node4:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	asp[4]=n[1];
	asp[3]=n[2];
	asp[2]=n[3];
	asp[1]=n[4];
	asp+=4;
	pc+=2;
	continue;
}
case Cpush_node_u:
{
	BC_WORD *n,*b,n_a,n_b;
	
	n=(BC_WORD*)asp[0];
	n_a=pc[1];
	n[0]=pc[2];
	asp+=n_a;
	asp[0] = n[1];
	asp[-1] = n[2];
	do {
		if (n_a< 3) break; asp[ -2] = n[ 3];
		if (n_a< 4) break; asp[ -3] = n[ 4];
		if (n_a< 5) break; asp[ -4] = n[ 5];
		if (n_a< 6) break; asp[ -5] = n[ 6];
		if (n_a< 7) break; asp[ -6] = n[ 7];
		if (n_a< 8) break; asp[ -7] = n[ 8];
		if (n_a< 9) break; asp[ -8] = n[ 9];
		if (n_a<10) break; asp[ -9] = n[10];
		if (n_a<11) break; asp[-10] = n[11];
		if (n_a<12) break; asp[-11] = n[12];
		if (n_a<13) break; asp[-12] = n[13];
		if (n_a<14) break; asp[-13] = n[14];
		if (n_a<15) break; asp[-14] = n[15];
		if (n_a<16) break; asp[-15] = n[16];
		if (n_a<17) break; asp[-16] = n[17];
		if (n_a<18) break; asp[-17] = n[18];
		if (n_a<19) break; asp[-18] = n[19];
		if (n_a<20) break; asp[-19] = n[20];
		if (n_a<21) break; asp[-20] = n[21];
		if (n_a<22) break; asp[-21] = n[22];
		if (n_a<23) break; asp[-22] = n[23];
		if (n_a<24) break; asp[-23] = n[24];
		if (n_a<25) break; asp[-24] = n[25];
		if (n_a<26) break; asp[-25] = n[26];
		if (n_a<27) break; asp[-26] = n[27];
		if (n_a<28) break; asp[-27] = n[28];
		if (n_a<29) break; asp[-28] = n[29];
		if (n_a<30) break; asp[-29] = n[30];
		if (n_a<31) break; asp[-30] = n[31];
		if (n_a<32) break; asp[-31] = n[32];
	} while (0);
	b=n+n_a+1;
	n_b=pc[3];
	bsp-=n_b;
	pc+=4;
	bsp[0]=b[0];
	bsp[1]=b[1];
	do {
		if (n_b< 3) break; bsp[ 2] = b[ 2];
		if (n_b< 4) break; bsp[ 3] = b[ 3];
		if (n_b< 5) break; bsp[ 4] = b[ 4];
		if (n_b< 6) break; bsp[ 5] = b[ 5];
		if (n_b< 7) break; bsp[ 6] = b[ 6];
		if (n_b< 8) break; bsp[ 7] = b[ 7];
		if (n_b< 9) break; bsp[ 8] = b[ 8];
		if (n_b<10) break; bsp[ 9] = b[ 9];
		if (n_b<11) break; bsp[10] = b[10];
		if (n_b<12) break; bsp[11] = b[11];
		if (n_b<13) break; bsp[12] = b[12];
		if (n_b<14) break; bsp[13] = b[13];
		if (n_b<15) break; bsp[14] = b[14];
		if (n_b<16) break; bsp[15] = b[15];
		if (n_b<17) break; bsp[16] = b[16];
		if (n_b<18) break; bsp[17] = b[17];
		if (n_b<19) break; bsp[18] = b[18];
		if (n_b<20) break; bsp[19] = b[19];
		if (n_b<21) break; bsp[20] = b[20];
		if (n_b<22) break; bsp[21] = b[21];
		if (n_b<23) break; bsp[22] = b[22];
		if (n_b<24) break; bsp[23] = b[23];
		if (n_b<25) break; bsp[24] = b[24];
		if (n_b<26) break; bsp[25] = b[25];
		if (n_b<27) break; bsp[26] = b[26];
		if (n_b<28) break; bsp[27] = b[27];
		if (n_b<29) break; bsp[28] = b[28];
		if (n_b<30) break; bsp[29] = b[29];
		if (n_b<31) break; bsp[30] = b[30];
		if (n_b<32) break; bsp[31] = b[31];
	} while (0);
	continue;
}
case Cpush_node_ua1:
{
	BC_WORD *n,n_a;
	
	n=(BC_WORD*)asp[0];
	n_a=pc[1];
	n[0]=*(BC_WORD*)&pc[2];
	asp+=n_a;
	asp[0] = n[1];
	asp[-1] = n[2];
	asp[-2] = n[3];
	pc+=3;
	*--bsp=n[n_a+1];
	do {
		if (n_a< 4) break; asp[ -3] = n[ 4];
		if (n_a< 5) break; asp[ -4] = n[ 5];
		if (n_a< 6) break; asp[ -5] = n[ 6];
		if (n_a< 7) break; asp[ -6] = n[ 7];
		if (n_a< 8) break; asp[ -7] = n[ 8];
		if (n_a< 9) break; asp[ -8] = n[ 9];
		if (n_a<10) break; asp[ -9] = n[10];
		if (n_a<11) break; asp[-10] = n[11];
		if (n_a<12) break; asp[-11] = n[12];
		if (n_a<13) break; asp[-12] = n[13];
		if (n_a<14) break; asp[-13] = n[14];
		if (n_a<15) break; asp[-14] = n[15];
		if (n_a<16) break; asp[-15] = n[16];
		if (n_a<17) break; asp[-16] = n[17];
		if (n_a<18) break; asp[-17] = n[18];
		if (n_a<19) break; asp[-18] = n[19];
		if (n_a<20) break; asp[-19] = n[20];
		if (n_a<21) break; asp[-20] = n[21];
		if (n_a<22) break; asp[-21] = n[22];
		if (n_a<23) break; asp[-22] = n[23];
		if (n_a<24) break; asp[-23] = n[24];
		if (n_a<25) break; asp[-24] = n[25];
		if (n_a<26) break; asp[-25] = n[26];
		if (n_a<27) break; asp[-26] = n[27];
		if (n_a<28) break; asp[-27] = n[28];
		if (n_a<29) break; asp[-28] = n[29];
		if (n_a<30) break; asp[-29] = n[30];
		if (n_a<31) break; asp[-30] = n[31];
		if (n_a<32) break; asp[-31] = n[32];
	} while (0);
	continue;
}
case Cpush_node_u01:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	*--bsp=n[1];
	pc+=2;
	continue;
}
case Cpush_node_u02:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	bsp[-2]=n[1];
	bsp[-1]=n[2];
	bsp-=2;
	pc+=2;
	continue;
}
case Cpush_node_u11:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	*++asp=n[1];
	*--bsp=n[2];
	pc+=2;
	continue;
}
case Cpush_node_u12:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	*++asp=n[1];
	bsp[-2]=n[2];
	bsp[-1]=n[3];
	bsp-=2;
	pc+=2;
	continue;
}
case Cpush_node_u13:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	*++asp=n[1];
	bsp[-3]=n[2];
	bsp[-2]=n[3];
	bsp[-1]=n[4];
	bsp-=3;
	pc+=2;
	continue;
}
case Cpush_node_u22:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	asp[2]=n[1];
	asp[1]=n[2];
	bsp[-2]=n[3];
	bsp[-1]=n[4];
	asp+=2;
	bsp-=2;
	pc+=2;
	continue;
}
case Cpush_node_u21:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	asp[2]=n[1];
	asp[1]=n[2];
	*--bsp=n[3];
	asp+=2;
	pc+=2;
	continue;
}
case Cpush_node_u31:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	asp[3]=n[1];
	asp[2]=n[2];
	asp[1]=n[3];
	*--bsp=n[4];
	asp+=3;
	pc+=2;
	continue;
}
case Cpush_r_args:
{
	BC_WORD *n,*a,*b,n_a,n_b;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n_a=pc[2];
	a=(BC_WORD*)n[2];
	asp+=n_a;
	asp[0] = (BC_WORD)n[1];
	asp[-1] = a[0];
	do {
		if (n_a< 3) break; asp[ -2] = a[ 1];
		if (n_a< 4) break; asp[ -3] = a[ 2];
		if (n_a< 5) break; asp[ -4] = a[ 3];
		if (n_a< 6) break; asp[ -5] = a[ 4];
		if (n_a< 7) break; asp[ -6] = a[ 5];
		if (n_a< 8) break; asp[ -7] = a[ 6];
		if (n_a< 9) break; asp[ -8] = a[ 7];
		if (n_a<10) break; asp[ -9] = a[ 8];
		if (n_a<11) break; asp[-10] = a[ 9];
		if (n_a<12) break; asp[-11] = a[10];
		if (n_a<13) break; asp[-12] = a[11];
		if (n_a<14) break; asp[-13] = a[12];
		if (n_a<15) break; asp[-14] = a[13];
		if (n_a<16) break; asp[-15] = a[14];
		if (n_a<17) break; asp[-16] = a[15];
		if (n_a<18) break; asp[-17] = a[16];
		if (n_a<19) break; asp[-18] = a[17];
		if (n_a<20) break; asp[-19] = a[18];
		if (n_a<21) break; asp[-20] = a[10];
		if (n_a<22) break; asp[-21] = a[20];
		if (n_a<23) break; asp[-22] = a[21];
		if (n_a<24) break; asp[-23] = a[22];
		if (n_a<25) break; asp[-24] = a[23];
		if (n_a<26) break; asp[-25] = a[24];
		if (n_a<27) break; asp[-26] = a[25];
		if (n_a<28) break; asp[-27] = a[26];
		if (n_a<29) break; asp[-28] = a[27];
		if (n_a<30) break; asp[-29] = a[28];
		if (n_a<31) break; asp[-30] = a[20];
		if (n_a<32) break; asp[-31] = a[30];
	} while (0);
	b=a+n_a-1;
	n_b=pc[3];
	bsp-=n_b;
	pc+=4;
	bsp[0]=b[0];
	bsp[1]=b[1];
	do {
		if (n_b< 3) break; bsp[ 2]=b[ 2];
		if (n_b< 4) break; bsp[ 3]=b[ 3];
		if (n_b< 5) break; bsp[ 4]=b[ 4];
		if (n_b< 6) break; bsp[ 5]=b[ 5];
		if (n_b< 7) break; bsp[ 6]=b[ 6];
		if (n_b< 8) break; bsp[ 7]=b[ 7];
		if (n_b< 9) break; bsp[ 8]=b[ 8];
		if (n_b<10) break; bsp[ 9]=b[ 9];
		if (n_b<11) break; bsp[10]=b[10];
		if (n_b<12) break; bsp[11]=b[11];
		if (n_b<13) break; bsp[12]=b[12];
		if (n_b<14) break; bsp[13]=b[13];
		if (n_b<15) break; bsp[14]=b[14];
		if (n_b<16) break; bsp[15]=b[15];
		if (n_b<17) break; bsp[16]=b[16];
		if (n_b<18) break; bsp[17]=b[17];
		if (n_b<19) break; bsp[18]=b[18];
		if (n_b<20) break; bsp[19]=b[19];
		if (n_b<21) break; bsp[20]=b[20];
		if (n_b<22) break; bsp[21]=b[21];
		if (n_b<23) break; bsp[22]=b[22];
		if (n_b<24) break; bsp[23]=b[23];
		if (n_b<25) break; bsp[24]=b[24];
		if (n_b<26) break; bsp[25]=b[25];
		if (n_b<27) break; bsp[26]=b[26];
		if (n_b<28) break; bsp[27]=b[27];
		if (n_b<29) break; bsp[28]=b[28];
		if (n_b<30) break; bsp[29]=b[39];
		if (n_b<31) break; bsp[30]=b[30];
		if (n_b<32) break; bsp[31]=b[31];
	} while (0);
	continue;
}
case Cpush_r_argsa1:
{
	BC_WORD *n,*a,n_a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n_a=pc[2]; /* >= 4 */
	a=(BC_WORD*)n[2];
	asp+=n_a;
	asp[0] = (BC_WORD)n[1];
	asp[-1] = a[0];
	asp[-2] = a[1];
	asp[-3] = a[2];
	*--bsp=a[n_a-1];
	pc+=3;
	do {
		if (n_a< 5) break; asp[ -4] = a[ 3];
		if (n_a< 6) break; asp[ -5] = a[ 4];
		if (n_a< 7) break; asp[ -6] = a[ 5];
		if (n_a< 8) break; asp[ -7] = a[ 6];
		if (n_a< 9) break; asp[ -8] = a[ 7];
		if (n_a<10) break; asp[ -9] = a[ 8];
		if (n_a<11) break; asp[-10] = a[ 9];
		if (n_a<12) break; asp[-11] = a[10];
		if (n_a<13) break; asp[-12] = a[11];
		if (n_a<14) break; asp[-13] = a[12];
		if (n_a<15) break; asp[-14] = a[13];
		if (n_a<16) break; asp[-15] = a[14];
		if (n_a<17) break; asp[-16] = a[15];
		if (n_a<18) break; asp[-17] = a[16];
		if (n_a<19) break; asp[-18] = a[17];
		if (n_a<20) break; asp[-19] = a[18];
		if (n_a<21) break; asp[-20] = a[10];
		if (n_a<22) break; asp[-21] = a[20];
		if (n_a<23) break; asp[-22] = a[21];
		if (n_a<24) break; asp[-23] = a[22];
		if (n_a<25) break; asp[-24] = a[23];
		if (n_a<26) break; asp[-25] = a[24];
		if (n_a<27) break; asp[-26] = a[25];
		if (n_a<28) break; asp[-27] = a[26];
		if (n_a<29) break; asp[-28] = a[27];
		if (n_a<30) break; asp[-29] = a[28];
		if (n_a<31) break; asp[-30] = a[20];
		if (n_a<32) break; asp[-31] = a[30];
	} while (0);
	continue;
}
case Cpush_r_args0b:
{
	BC_WORD *n,*a,n_b;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n_b=pc[2];
	a=(BC_WORD*)n[2];
	bsp-=n_b;
	pc+=3;
	bsp[0]=(BC_WORD)n[1];
	bsp[1]=a[0];
	bsp[2]=a[1];
	bsp[3]=a[2];
	bsp[4]=a[3];
	do {
		if (n_b< 6) break; bsp[ 5]=a[ 4];
		if (n_b< 7) break; bsp[ 6]=a[ 5];
		if (n_b< 8) break; bsp[ 7]=a[ 6];
		if (n_b< 9) break; bsp[ 8]=a[ 7];
		if (n_b<10) break; bsp[ 9]=a[ 8];
		if (n_b<11) break; bsp[10]=a[ 9];
		if (n_b<12) break; bsp[11]=a[10];
		if (n_b<13) break; bsp[12]=a[11];
		if (n_b<14) break; bsp[13]=a[12];
		if (n_b<15) break; bsp[14]=a[13];
		if (n_b<16) break; bsp[15]=a[14];
		if (n_b<17) break; bsp[16]=a[15];
		if (n_b<18) break; bsp[17]=a[16];
		if (n_b<19) break; bsp[18]=a[17];
		if (n_b<20) break; bsp[19]=a[18];
		if (n_b<21) break; bsp[20]=a[19];
		if (n_b<22) break; bsp[21]=a[20];
		if (n_b<23) break; bsp[22]=a[21];
		if (n_b<24) break; bsp[23]=a[22];
		if (n_b<25) break; bsp[24]=a[23];
		if (n_b<26) break; bsp[25]=a[24];
		if (n_b<27) break; bsp[26]=a[25];
		if (n_b<28) break; bsp[27]=a[26];
		if (n_b<29) break; bsp[28]=a[27];
		if (n_b<30) break; bsp[29]=a[28];
		if (n_b<31) break; bsp[30]=a[29];
		if (n_b<32) break; bsp[31]=a[30];
	} while (0);
	continue;
}
case Cpush_r_args1b:
{
	BC_WORD *n,*a,n_b;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n_b=pc[2];
	a=(BC_WORD*)n[2];
	*++asp = (BC_WORD)n[1];
	bsp-=n_b;
	pc+=3;
	bsp[0]=a[0];
	bsp[1]=a[1];
	bsp[2]=a[2];
	bsp[3]=a[3];
	do {
		if (n_b< 5) break; bsp[ 4]=a[ 4];
		if (n_b< 6) break; bsp[ 5]=a[ 5];
		if (n_b< 7) break; bsp[ 6]=a[ 6];
		if (n_b< 8) break; bsp[ 7]=a[ 7];
		if (n_b< 9) break; bsp[ 8]=a[ 8];
		if (n_b<10) break; bsp[ 9]=a[ 9];
		if (n_b<11) break; bsp[10]=a[10];
		if (n_b<12) break; bsp[11]=a[11];
		if (n_b<13) break; bsp[12]=a[12];
		if (n_b<14) break; bsp[13]=a[13];
		if (n_b<15) break; bsp[14]=a[14];
		if (n_b<16) break; bsp[15]=a[15];
		if (n_b<17) break; bsp[16]=a[16];
		if (n_b<18) break; bsp[17]=a[17];
		if (n_b<19) break; bsp[18]=a[18];
		if (n_b<20) break; bsp[19]=a[19];
		if (n_b<21) break; bsp[20]=a[20];
		if (n_b<22) break; bsp[21]=a[21];
		if (n_b<23) break; bsp[22]=a[22];
		if (n_b<24) break; bsp[23]=a[23];
		if (n_b<25) break; bsp[24]=a[24];
		if (n_b<26) break; bsp[25]=a[25];
		if (n_b<27) break; bsp[26]=a[26];
		if (n_b<28) break; bsp[27]=a[27];
		if (n_b<29) break; bsp[28]=a[28];
		if (n_b<30) break; bsp[29]=a[39];
		if (n_b<31) break; bsp[30]=a[30];
	} while (0);
	continue;
}
case Cpush_r_args03:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	bsp[-3]=n[1];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=3;
	pc+=2;
	continue;				
}
case Cpush_r_args04:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	bsp[-4]=n[1];
	bsp[-3]=a[0];
	bsp[-2]=a[1];
	bsp[-1]=a[2];
	bsp-=4;
	pc+=2;
	continue;				
}
case Cpush_r_args11:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=n[1];
	*--bsp=n[2];
	pc+=2;
	continue;				
}
case Cpush_r_args12:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=n[1];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=2;
	pc+=2;
	continue;				
}
case Cpush_r_args13:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	asp[1]=n[1];
	asp+=1;
	bsp[-3]=a[0];
	bsp[-2]=a[1];
	bsp[-1]=a[2];
	bsp-=3;
	pc+=2;
	continue;				
}
case Cpush_r_args21:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	bsp[-1]=a[1];
	asp+=2;
	bsp-=1;
	pc+=2;
	continue;				
}
case Cpush_r_args22:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	bsp[-2]=a[1];
	bsp[-1]=a[2];
	asp+=2;
	bsp-=2;
	pc+=2;
	continue;				
}
case Cpush_r_args31:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	asp[3]=n[1];
	asp[2]=a[0];
	asp[1]=a[1];
	asp+=3;
	bsp[-1]=a[2];
	bsp-=1;
	pc+=2;
	continue;				
}
case Cpush_r_args_aa1:
{
	BC_WORD_S a_o;
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o=pc[2];
	a=(BC_WORD*)n[2];
	*++asp=*(BC_WORD*)((BC_WORD_S)a+a_o);
	pc+=3;
	continue;
}
case Cpush_r_args_b2l1:
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*--bsp=a[0];
	pc+=2;
	continue;
}
case Cpush_r_args_b1l2:
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	bsp[-2]=n[1];
	bsp[-1]=a[0];
	bsp-=2;
	pc+=2;
	continue;
}
case Cpush_r_args_b22:
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=2;
	pc+=2;
	continue;
}
case Cpush_r_args_b31:
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*--bsp=a[1];
	pc+=2;
	continue;
}
case Cpush_r_args_b41:
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*--bsp=a[2];
	pc+=2;
	continue;
}
case CremI:
	bsp[1]=(BC_WORD_S)bsp[0] % (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	continue;
case Creplace:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	asp[0]=array[3+i];
	array[3+i]=asp[-1];
	asp[-1]=(BC_WORD)array;
	pc+=1;
	++bsp;
	continue;
}
case CreplaceBOOL:
case CreplaceCHAR:
{
	BC_WORD *array,i,v;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	v=((BC_BOOL*)array)[8+i];
	((BC_BOOL*)array)[8+i]=bsp[1];
	bsp[1]=v;
	pc+=1;
	++bsp;
	continue;
}
case CreplaceINT:
{
	BC_WORD *array,i,v;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	v=array[3+i];
	array[3+i]=bsp[1];
	bsp[1]=v;
	pc+=1;
	++bsp;
	continue;
}
case Creplace_ra:
{
	BC_WORD *array,array_o,*a;
	BC_WORD_S n_a;
	
	n_a=pc[1];
	array_o = (n_a * (BC_WORD_S)*bsp++) + 3;
	array = (BC_WORD*)asp[0];
	pc+=2;
	a = &array[array_o];
	asp[ 0]=a[0]; a[0]=asp[-1];
	asp[-1]=a[1]; a[1]=asp[-2];
	asp[-2]=a[2]; a[2]=asp[-3];
	asp[-3]=a[3]; a[3]=asp[-4];
	asp[-4]=a[4]; a[4]=asp[-5];
	do {
		if (n_a< 6) break; asp[ -5]=a[ 5]; a[ 5]=asp[ -6];
		if (n_a< 7) break; asp[ -6]=a[ 6]; a[ 6]=asp[ -7];
		if (n_a< 8) break; asp[ -7]=a[ 7]; a[ 7]=asp[ -8];
		if (n_a< 9) break; asp[ -8]=a[ 8]; a[ 8]=asp[ -9];
		if (n_a<10) break; asp[ -9]=a[ 9]; a[ 9]=asp[-10];
		if (n_a<11) break; asp[-10]=a[10]; a[10]=asp[-11];
		if (n_a<12) break; asp[-11]=a[11]; a[11]=asp[-12];
		if (n_a<13) break; asp[-12]=a[12]; a[12]=asp[-13];
		if (n_a<14) break; asp[-13]=a[13]; a[13]=asp[-14];
		if (n_a<15) break; asp[-14]=a[14]; a[14]=asp[-15];
		if (n_a<16) break; asp[-15]=a[15]; a[15]=asp[-16];
		if (n_a<17) break; asp[-16]=a[16]; a[16]=asp[-17];
		if (n_a<18) break; asp[-17]=a[17]; a[17]=asp[-18];
		if (n_a<19) break; asp[-18]=a[18]; a[18]=asp[-19];
		if (n_a<20) break; asp[-19]=a[19]; a[19]=asp[-20];
		if (n_a<21) break; asp[-20]=a[20]; a[20]=asp[-21];
		if (n_a<22) break; asp[-21]=a[21]; a[21]=asp[-22];
		if (n_a<23) break; asp[-22]=a[22]; a[22]=asp[-23];
		if (n_a<24) break; asp[-23]=a[23]; a[23]=asp[-24];
		if (n_a<25) break; asp[-24]=a[24]; a[24]=asp[-25];
		if (n_a<26) break; asp[-25]=a[25]; a[25]=asp[-26];
		if (n_a<27) break; asp[-26]=a[26]; a[26]=asp[-27];
		if (n_a<28) break; asp[-27]=a[27]; a[27]=asp[-28];
		if (n_a<29) break; asp[-28]=a[28]; a[28]=asp[-29];
		if (n_a<30) break; asp[-29]=a[29]; a[29]=asp[-30];
		if (n_a<31) break; asp[-30]=a[30]; a[30]=asp[-31];
		if (n_a<32) break; asp[-31]=a[31]; a[31]=asp[-32];
	} while (0);
	asp[-n_a]=(BC_WORD)array;
	continue;
}
case Crepl_args:
case Crepl_r_argsa0:
{
	BC_WORD *n,*a,n_a_m_1;

	n=(BC_WORD*)asp[0];
	n_a_m_1=pc[1]; /* >=4 */
	a=(BC_WORD*)n[2];
	asp+=n_a_m_1;
	asp[0] = n[1];
	asp[-1]=a[0];
	asp[-2]=a[1];
	asp[-3]=a[2];
	asp[-4]=a[3];
	pc+=2;
	do {
		if (n_a_m_1< 5) break; asp[ -5] = a[ 4];
		if (n_a_m_1< 6) break; asp[ -6] = a[ 5];
		if (n_a_m_1< 7) break; asp[ -7] = a[ 6];
		if (n_a_m_1< 8) break; asp[ -8] = a[ 7];
		if (n_a_m_1< 9) break; asp[ -9] = a[ 8];
		if (n_a_m_1<10) break; asp[-10] = a[ 9];
		if (n_a_m_1<11) break; asp[-11] = a[10];
		if (n_a_m_1<12) break; asp[-12] = a[11];
		if (n_a_m_1<13) break; asp[-13] = a[12];
		if (n_a_m_1<14) break; asp[-14] = a[13];
		if (n_a_m_1<15) break; asp[-15] = a[14];
		if (n_a_m_1<16) break; asp[-16] = a[15];
		if (n_a_m_1<17) break; asp[-17] = a[16];
		if (n_a_m_1<18) break; asp[-18] = a[17];
		if (n_a_m_1<19) break; asp[-19] = a[18];
		if (n_a_m_1<20) break; asp[-20] = a[19];
		if (n_a_m_1<21) break; asp[-21] = a[20];
		if (n_a_m_1<22) break; asp[-22] = a[21];
		if (n_a_m_1<23) break; asp[-23] = a[22];
		if (n_a_m_1<24) break; asp[-24] = a[23];
		if (n_a_m_1<25) break; asp[-25] = a[24];
		if (n_a_m_1<26) break; asp[-26] = a[25];
		if (n_a_m_1<27) break; asp[-27] = a[26];
		if (n_a_m_1<28) break; asp[-28] = a[27];
		if (n_a_m_1<29) break; asp[-29] = a[28];
		if (n_a_m_1<30) break; asp[-30] = a[29];
		if (n_a_m_1<31) break; asp[-31] = a[30];
	} while (0);
	continue;
}
case Cswap_a1:
{
	BC_WORD d;
	
	d=asp[0];
	asp[0]=asp[-1];
	asp[-1]=d;
	pc+=1;
	continue;
}
case Cswap_a2:
{
	BC_WORD d;
	
	d=asp[0];
	asp[0]=asp[-2];
	asp[-2]=d;
	pc+=1;
	continue;
}
case Cswap_a3:
{
	BC_WORD d;
	
	d=asp[0];
	asp[0]=asp[-3];
	asp[-3]=d;
	pc+=1;
	continue;
}
case Cswap_a:
{
	BC_WORD d;
	BC_WORD_S ao;
	
	ao=pc[1];
	d=asp[0];
	asp[0]=asp[ao];
	asp[ao]=d;
	pc+=2;
	continue;
}
case Cswap_b1:
{
	BC_WORD d;
	
	d=bsp[0];
	bsp[0]=bsp[1];
	bsp[1]=d;
	pc+=1;
	continue;
}
case Cpush_r_args_b0221:
case Cpush_r_args_b1111:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=n[2];
	pc+=2;
	continue;
}
case Cpush_r_args_b1:
{
	BC_WORD *n,*a;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=pc[2];
	a=(BC_WORD*)n[2];
	*--bsp=*(BC_WORD*)((BC_WORD_S)a+bo);
	pc+=3;
	continue;
}
case Cpush_r_args_b2:
{
	BC_WORD *n,*a;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=pc[2];
	a=(BC_WORD*)n[2];
	bsp[-2]=*(BC_WORD*)((BC_WORD_S)a+bo);
	bsp[-1]=*(BC_WORD*)((BC_WORD_S)a+bo+sizeof(BC_WORD));
	bsp-=2;
	pc+=3;
	continue;
}
case Crepl_args1:
case Crepl_r_args10:
case Crepl_r_args_aab11:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[0];
	asp[0]=n[1];
	pc+=1;
	continue;
}
case Crepl_args2:
case Crepl_r_args20:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[0];
	asp[1]=n[1];
	asp[0]=n[2];
	asp+=1;
	pc+=1;
	continue;
}
case Crepl_args3:
case Crepl_r_args30:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	asp[0]=a[1];
	asp+=2;
	pc+=1;
	continue;
}
case Crepl_args4:
case Crepl_r_args40:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[3]=n[1];
	asp[2]=a[0];
	asp[1]=a[1];
	asp[0]=a[2];
	asp+=3;
	pc+=1;
	continue;
}
case Crepl_r_args:
{
	BC_WORD *n,*a,*b,n_a_m_1,n_b;
	
	n=(BC_WORD*)asp[0];
	n_a_m_1=pc[1];
	a=(BC_WORD*)n[2];
	asp+=n_a_m_1;
	asp[0]=(BC_WORD)n[1];
	asp[-1]=a[0];
	do {
		if (n_a_m_1< 2) break; asp[ -2] = a[ 1];
		if (n_a_m_1< 3) break; asp[ -3] = a[ 2];
		if (n_a_m_1< 4) break; asp[ -4] = a[ 3];
		if (n_a_m_1< 5) break; asp[ -5] = a[ 4];
		if (n_a_m_1< 6) break; asp[ -6] = a[ 5];
		if (n_a_m_1< 7) break; asp[ -7] = a[ 6];
		if (n_a_m_1< 8) break; asp[ -8] = a[ 7];
		if (n_a_m_1< 9) break; asp[ -9] = a[ 8];
		if (n_a_m_1<10) break; asp[-10] = a[ 9];
		if (n_a_m_1<11) break; asp[-11] = a[10];
		if (n_a_m_1<12) break; asp[-12] = a[11];
		if (n_a_m_1<13) break; asp[-13] = a[12];
		if (n_a_m_1<14) break; asp[-14] = a[13];
		if (n_a_m_1<15) break; asp[-15] = a[14];
		if (n_a_m_1<16) break; asp[-16] = a[15];
		if (n_a_m_1<17) break; asp[-17] = a[16];
		if (n_a_m_1<18) break; asp[-18] = a[17];
		if (n_a_m_1<19) break; asp[-19] = a[18];
		if (n_a_m_1<20) break; asp[-20] = a[19];
		if (n_a_m_1<21) break; asp[-21] = a[20];
		if (n_a_m_1<22) break; asp[-22] = a[21];
		if (n_a_m_1<23) break; asp[-23] = a[22];
		if (n_a_m_1<24) break; asp[-24] = a[23];
		if (n_a_m_1<25) break; asp[-25] = a[24];
		if (n_a_m_1<26) break; asp[-26] = a[25];
		if (n_a_m_1<27) break; asp[-27] = a[26];
		if (n_a_m_1<28) break; asp[-28] = a[27];
		if (n_a_m_1<29) break; asp[-29] = a[28];
	} while (0);
	b=a+n_a_m_1;
	n_b=pc[2];
	pc+=3;
	bsp-=n_b;
	bsp[0]=b[0];
	bsp[1]=b[1];
	do {
		if (n_b<= 2) break; bsp[ 2]=b[ 2];
		if (n_b<= 3) break; bsp[ 3]=b[ 3];
		if (n_b<= 4) break; bsp[ 4]=b[ 4];
		if (n_b<= 5) break; bsp[ 5]=b[ 5];
		if (n_b<= 6) break; bsp[ 6]=b[ 6];
		if (n_b<= 7) break; bsp[ 7]=b[ 7];
		if (n_b<= 8) break; bsp[ 8]=b[ 8];
		if (n_b<= 9) break; bsp[ 9]=b[ 9];
		if (n_b<=10) break; bsp[10]=b[10];
		if (n_b<=11) break; bsp[11]=b[11];
		if (n_b<=12) break; bsp[12]=b[12];
		if (n_b<=13) break; bsp[13]=b[13];
		if (n_b<=14) break; bsp[14]=b[14];
		if (n_b<=15) break; bsp[15]=b[15];
		if (n_b<=16) break; bsp[16]=b[16];
		if (n_b<=17) break; bsp[17]=b[17];
		if (n_b<=18) break; bsp[18]=b[18];
		if (n_b<=19) break; bsp[19]=b[19];
		if (n_b<=20) break; bsp[20]=b[20];
		if (n_b<=21) break; bsp[21]=b[21];
		if (n_b<=22) break; bsp[22]=b[22];
		if (n_b<=23) break; bsp[23]=b[23];
		if (n_b<=24) break; bsp[24]=b[24];
		if (n_b<=25) break; bsp[25]=b[25];
		if (n_b<=26) break; bsp[26]=b[26];
		if (n_b<=27) break; bsp[27]=b[27];
		if (n_b<=28) break; bsp[28]=b[28];
		if (n_b<=29) break; bsp[29]=b[29];
	} while (0);
	continue;
}
case Crepl_r_argsa1:
{
	BC_WORD *n,*a,n_a_m_1;
	
	n=(BC_WORD*)asp[0];
	n_a_m_1=pc[1];
	pc+=2;
	a=(BC_WORD*)n[2];
	asp+=n_a_m_1;
	asp[0]=(BC_WORD)n[1];
	asp[-1]=a[0];
	asp[-2]=a[1];
	*--bsp=a[n_a_m_1];
	do {
		if (n_a_m_1< 3) break; asp[ -3] = a[ 2];
		if (n_a_m_1< 4) break; asp[ -4] = a[ 3];
		if (n_a_m_1< 5) break; asp[ -5] = a[ 4];
		if (n_a_m_1< 6) break; asp[ -6] = a[ 5];
		if (n_a_m_1< 7) break; asp[ -7] = a[ 6];
		if (n_a_m_1< 8) break; asp[ -8] = a[ 7];
		if (n_a_m_1< 9) break; asp[ -9] = a[ 8];
		if (n_a_m_1<10) break; asp[-10] = a[ 9];
		if (n_a_m_1<11) break; asp[-11] = a[10];
		if (n_a_m_1<12) break; asp[-12] = a[11];
		if (n_a_m_1<13) break; asp[-13] = a[12];
		if (n_a_m_1<14) break; asp[-14] = a[13];
		if (n_a_m_1<15) break; asp[-15] = a[14];
		if (n_a_m_1<16) break; asp[-16] = a[15];
		if (n_a_m_1<17) break; asp[-17] = a[16];
		if (n_a_m_1<18) break; asp[-18] = a[17];
		if (n_a_m_1<19) break; asp[-19] = a[18];
		if (n_a_m_1<20) break; asp[-20] = a[10];
		if (n_a_m_1<21) break; asp[-21] = a[20];
		if (n_a_m_1<22) break; asp[-22] = a[21];
		if (n_a_m_1<23) break; asp[-23] = a[22];
		if (n_a_m_1<24) break; asp[-24] = a[23];
		if (n_a_m_1<25) break; asp[-25] = a[24];
		if (n_a_m_1<26) break; asp[-26] = a[25];
		if (n_a_m_1<27) break; asp[-27] = a[26];
		if (n_a_m_1<28) break; asp[-28] = a[27];
		if (n_a_m_1<29) break; asp[-29] = a[28];
		if (n_a_m_1<30) break; asp[-30] = a[29];
		if (n_a_m_1<31) break; asp[-31] = a[30];
	} while (0);
	continue;
}
case Crepl_r_args1b:
{
	BC_WORD *n,*a,n_b;
	
	n=(BC_WORD*)asp[0];
	n_b=pc[1];
	a=(BC_WORD*)n[2];
	asp[0]=(BC_WORD)n[1];
	bsp-=n_b;
	pc+=2;
	bsp[0]=a[0];
	bsp[1]=a[1];
	bsp[2]=a[2];
	bsp[3]=a[3];
	do {
		if (n_b<= 4) break; bsp[ 4]=a[ 4];
		if (n_b<= 5) break; bsp[ 5]=a[ 5];
		if (n_b<= 6) break; bsp[ 6]=a[ 6];
		if (n_b<= 7) break; bsp[ 7]=a[ 7];
		if (n_b<= 8) break; bsp[ 8]=a[ 8];
		if (n_b<= 9) break; bsp[ 9]=a[ 9];
		if (n_b<=10) break; bsp[10]=a[10];
		if (n_b<=11) break; bsp[11]=a[11];
		if (n_b<=12) break; bsp[12]=a[12];
		if (n_b<=13) break; bsp[13]=a[13];
		if (n_b<=14) break; bsp[14]=a[14];
		if (n_b<=15) break; bsp[15]=a[15];
		if (n_b<=16) break; bsp[16]=a[16];
		if (n_b<=17) break; bsp[17]=a[17];
		if (n_b<=18) break; bsp[18]=a[18];
		if (n_b<=19) break; bsp[19]=a[19];
		if (n_b<=20) break; bsp[20]=a[20];
		if (n_b<=21) break; bsp[21]=a[21];
		if (n_b<=22) break; bsp[22]=a[22];
		if (n_b<=23) break; bsp[23]=a[23];
		if (n_b<=24) break; bsp[24]=a[24];
		if (n_b<=25) break; bsp[25]=a[25];
		if (n_b<=26) break; bsp[26]=a[26];
		if (n_b<=27) break; bsp[27]=a[27];
		if (n_b<=28) break; bsp[28]=a[28];
		if (n_b<=29) break; bsp[29]=a[29];
		if (n_b<=30) break; bsp[30]=a[30];
		if (n_b<=31) break; bsp[31]=a[31];
	} while (0);
	continue;
}
case Cpush_arraysize:
case Crepl_r_args01:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp--;
	*--bsp=n[1];
	pc+=1;
	continue;
}
case Crepl_r_args11:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[0];
	asp[0]=n[1];
	*--bsp=n[2];
	pc+=1;
	continue;
}
case Crepl_r_args02:
{
	BC_WORD *n;
	
	n=(BC_WORD*)*asp--;
	bsp[-2]=n[1];
	bsp[-1]=n[2];
	bsp-=2;
	pc+=1;
	continue;
}
case Crepl_r_args03:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)*asp--;
	a=(BC_WORD*)n[2];
	bsp[-3]=n[1];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=3;
	pc+=1;
	continue;
}
case Crepl_r_args04:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)*asp--;
	a=(BC_WORD*)n[2];
	bsp[-4]=n[1];
	bsp[-3]=a[0];
	bsp[-2]=a[1];
	bsp[-1]=a[2];
	bsp-=4;
	pc+=1;
	continue;
}
case Crepl_r_args12:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=n[1];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=2;
	pc+=1;
	continue;
}
case Crepl_r_args13:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=n[1];
	bsp[-3]=a[0];
	bsp[-2]=a[1];
	bsp[-1]=a[2];
	bsp-=3;
	pc+=1;
	continue;
}
case Crepl_r_args21:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[1]=n[1];
	asp[0]=a[0];
	*--bsp=a[1];
	++asp;
	pc+=1;
	continue;
}
case Crepl_r_args22:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[1]=n[1];
	asp[0]=a[0];
	bsp[-2]=a[1];
	bsp[-1]=a[2];
	++asp;
	bsp-=2;
	pc+=1;
	continue;
}
case Crepl_r_args31:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	asp[0]=a[1];
	*--bsp=a[2];
	asp+=2;
	pc+=1;
	continue;
}
case Crepl_r_args_a2021:
{
	BC_WORD *n;
	
	n=(BC_WORD*)asp[0];
	asp[0]=n[2];
	pc+=1;
	continue;
}
case Crepl_r_args_a21:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=a[0];
	pc+=1;
	continue;
}
case Crepl_r_args_a31:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=a[1];
	pc+=1;
	continue;
}
case Crepl_r_args_a41:
{
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=a[2];
	pc+=1;
	continue;
}
case Crepl_r_args_aa1:
{
	BC_WORD_S a_o;
	BC_WORD *n,*a;
	
	n=(BC_WORD*)asp[0];
	a_o=pc[1];
	a=(BC_WORD*)n[2];
	asp[0]=*(BC_WORD*)((BC_WORD_S)a+a_o);
	pc+=2;
	continue;
}
case Cselect:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	asp[0]=array[3+i];
	bsp+=1;
	pc+=1;
	continue;
}
case CselectBOOL:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	bsp[0]=((BC_BOOL*)&array[3])[i];
	--asp;
	pc+=1;
	continue;
}
case CselectCHAR:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	bsp[0]=((BC_BOOL*)&array[2])[i];
	--asp;
	pc+=1;
	continue;
}
case CselectINT:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	bsp[0]=array[3+i];
	--asp;
	pc+=1;
	continue;
}
case Cselect_r:
{
	BC_WORD array_o,*element_p,*a,*b;
	BC_WORD_S n_ab,n_a,n_b;
	
	n_ab=pc[1];
	array_o = (n_ab * (BC_WORD_S)*bsp++) + 3;
	element_p = &((BC_WORD*)(*asp--))[array_o];
	n_a=pc[2];
	b=element_p+n_a;
	asp+=n_a;
	a=element_p;
	asp[ 0]=a[0];
	asp[-1]=a[1];
	do {
		if (n_a< 3) break; asp[ -2]=a[ 2];
		if (n_a< 4) break; asp[ -3]=a[ 3];
		if (n_a< 5) break; asp[ -4]=a[ 4];
		if (n_a< 6) break; asp[ -5]=a[ 5];
		if (n_a< 7) break; asp[ -6]=a[ 6];
		if (n_a< 8) break; asp[ -7]=a[ 7];
		if (n_a< 9) break; asp[ -8]=a[ 8];
		if (n_a<10) break; asp[ -9]=a[ 9];
		if (n_a<11) break; asp[-10]=a[10];
		if (n_a<12) break; asp[-11]=a[11];
		if (n_a<13) break; asp[-12]=a[12];
		if (n_a<14) break; asp[-13]=a[13];
		if (n_a<15) break; asp[-14]=a[14];
		if (n_a<16) break; asp[-15]=a[15];
		if (n_a<17) break; asp[-16]=a[16];
		if (n_a<18) break; asp[-17]=a[17];
		if (n_a<19) break; asp[-18]=a[18];
		if (n_a<20) break; asp[-19]=a[19];
		if (n_a<21) break; asp[-20]=a[20];
		if (n_a<22) break; asp[-21]=a[21];
		if (n_a<23) break; asp[-22]=a[22];
		if (n_a<24) break; asp[-23]=a[23];
		if (n_a<25) break; asp[-24]=a[24];
		if (n_a<26) break; asp[-25]=a[25];
		if (n_a<27) break; asp[-26]=a[26];
		if (n_a<28) break; asp[-27]=a[27];
		if (n_a<29) break; asp[-28]=a[28];
		if (n_a<30) break; asp[-29]=a[29];
	} while (0);
	n_b=pc[3];
	pc+=4;
	bsp-=n_b;
	bsp[0]=b[0];
	bsp[1]=b[1];
	do {
		if (n_b< 3) break; bsp[ 2]=b[ 2];
		if (n_b< 4) break; bsp[ 3]=b[ 3];
		if (n_b< 5) break; bsp[ 4]=b[ 4];
		if (n_b< 6) break; bsp[ 5]=b[ 5];
		if (n_b< 7) break; bsp[ 6]=b[ 6];
		if (n_b< 8) break; bsp[ 7]=b[ 7];
		if (n_b< 9) break; bsp[ 8]=b[ 8];
		if (n_b<10) break; bsp[ 9]=b[ 9];
		if (n_b<11) break; bsp[10]=b[10];
		if (n_b<12) break; bsp[11]=b[11];
		if (n_b<13) break; bsp[12]=b[12];
		if (n_b<14) break; bsp[13]=b[13];
		if (n_b<15) break; bsp[14]=b[14];
		if (n_b<16) break; bsp[15]=b[15];
		if (n_b<17) break; bsp[16]=b[16];
		if (n_b<18) break; bsp[17]=b[17];
		if (n_b<19) break; bsp[18]=b[18];
		if (n_b<20) break; bsp[19]=b[19];
		if (n_b<21) break; bsp[20]=b[20];
		if (n_b<22) break; bsp[21]=b[21];
		if (n_b<23) break; bsp[22]=b[22];
		if (n_b<24) break; bsp[23]=b[23];
		if (n_b<25) break; bsp[24]=b[24];
		if (n_b<26) break; bsp[25]=b[25];
		if (n_b<27) break; bsp[26]=b[26];
		if (n_b<28) break; bsp[27]=b[27];
		if (n_b<29) break; bsp[28]=b[28];
		if (n_b<30) break; bsp[29]=b[29];
	} while (0);
	continue;
}
case Cselect_ra:
{
	BC_WORD array_o,*a;
	BC_WORD_S n_a;
	
	n_a=pc[1];
	array_o = (n_a * (BC_WORD_S)*bsp++) + 3;
	a = &((BC_WORD*)(*asp--))[array_o];
	asp+=n_a;
	pc+=2;
	asp[ 0]=a[0];
	asp[-1]=a[1];
	do {
		if (n_a< 3) break; asp[ -2]=a[ 2];
		if (n_a< 4) break; asp[ -3]=a[ 3];
		if (n_a< 5) break; asp[ -4]=a[ 4];
		if (n_a< 6) break; asp[ -5]=a[ 5];
		if (n_a< 7) break; asp[ -6]=a[ 6];
		if (n_a< 8) break; asp[ -7]=a[ 7];
		if (n_a< 9) break; asp[ -8]=a[ 8];
		if (n_a<10) break; asp[ -9]=a[ 9];
		if (n_a<11) break; asp[-10]=a[10];
		if (n_a<12) break; asp[-11]=a[11];
		if (n_a<13) break; asp[-12]=a[12];
		if (n_a<14) break; asp[-13]=a[13];
		if (n_a<15) break; asp[-14]=a[14];
		if (n_a<16) break; asp[-15]=a[15];
		if (n_a<17) break; asp[-16]=a[16];
		if (n_a<18) break; asp[-17]=a[17];
		if (n_a<19) break; asp[-18]=a[18];
		if (n_a<20) break; asp[-19]=a[19];
		if (n_a<21) break; asp[-20]=a[20];
		if (n_a<22) break; asp[-21]=a[21];
		if (n_a<23) break; asp[-22]=a[22];
		if (n_a<24) break; asp[-23]=a[23];
		if (n_a<25) break; asp[-24]=a[24];
		if (n_a<26) break; asp[-25]=a[25];
		if (n_a<27) break; asp[-26]=a[26];
		if (n_a<28) break; asp[-27]=a[27];
		if (n_a<29) break; asp[-28]=a[28];
		if (n_a<30) break; asp[-29]=a[29];
		if (n_a<31) break; asp[-30]=a[30];
		if (n_a<32) break; asp[-31]=a[31];
	} while (0);
	continue;
}
case Cselect_ra1:
{
	BC_WORD array_o,*a;
	BC_WORD_S n_ab;
	
	n_ab=pc[1];
	array_o = (n_ab * (BC_WORD_S)*bsp++) + 3;
	a = &((BC_WORD*)(*asp--))[array_o];
	asp+=n_ab-1;
	pc+=2;
	*--bsp=a[n_ab-1];
	asp[ 0]=a[0];
	asp[-1]=a[1];
	asp[-2]=a[2];
	asp[-3]=a[3];
	do {
		if (n_ab< 6) break; asp[ -4]=a[ 4];
		if (n_ab< 7) break; asp[ -5]=a[ 5];
		if (n_ab< 8) break; asp[ -6]=a[ 6];
		if (n_ab< 9) break; asp[ -7]=a[ 7];
		if (n_ab<10) break; asp[ -8]=a[ 8];
		if (n_ab<11) break; asp[ -9]=a[ 9];
		if (n_ab<12) break; asp[-10]=a[10];
		if (n_ab<13) break; asp[-11]=a[11];
		if (n_ab<14) break; asp[-12]=a[12];
		if (n_ab<15) break; asp[-13]=a[13];
		if (n_ab<16) break; asp[-14]=a[14];
		if (n_ab<17) break; asp[-15]=a[15];
		if (n_ab<18) break; asp[-16]=a[16];
		if (n_ab<19) break; asp[-17]=a[17];
		if (n_ab<20) break; asp[-18]=a[18];
		if (n_ab<21) break; asp[-19]=a[19];
		if (n_ab<22) break; asp[-20]=a[20];
		if (n_ab<23) break; asp[-21]=a[21];
		if (n_ab<24) break; asp[-22]=a[22];
		if (n_ab<25) break; asp[-23]=a[23];
		if (n_ab<26) break; asp[-24]=a[24];
		if (n_ab<27) break; asp[-25]=a[25];
		if (n_ab<28) break; asp[-26]=a[26];
		if (n_ab<29) break; asp[-27]=a[27];
		if (n_ab<30) break; asp[-28]=a[28];
		if (n_ab<31) break; asp[-29]=a[29];
		if (n_ab<32) break; asp[-30]=a[30];
	} while (0);
	continue;
}
case Cselect_r02:
{
	BC_WORD array_o,*element_p;
	
	array_o = 2 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp--))[array_o];
	bsp[-2] = element_p[3];
	bsp[-1] = element_p[4];
	bsp-=2;
	pc+=1;
	continue;
}
case Cselect_r12:
{
	BC_WORD array_o,*element_p;
	
	array_o = 3 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	*asp = element_p[3];
	bsp[-2] = element_p[4];
	bsp[-1] = element_p[5];
	bsp-=2;
	pc+=1;
	continue;
}
case Cselect_r20:
{
	BC_WORD array_o,*element_p;
	
	array_o = 2 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	asp[1] = element_p[3];
	asp[0] = element_p[4];
	asp+=1;
	pc+=1;
	continue;
}
case Cselect_r21:
{
	BC_WORD array_o,*element_p;
	
	array_o = 3 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	asp[1] = element_p[3];
	asp[0] = element_p[4];
	*--bsp = element_p[5];
	asp+=1;
	pc+=1;
	continue;
}
case Cselect_r40:
{
	BC_WORD array_o,*element_p;
	
	array_o = 4 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	asp[3] = element_p[3];
	asp[2] = element_p[4];
	asp[1] = element_p[5];
	asp[0] = element_p[6];
	asp+=3;
	pc+=1;
	continue;
}
case Crtn:
	pc=(BC_WORD*)*csp++;
	continue;
case CshiftlI:
	bsp[1]=bsp[0] << bsp[1];
	++bsp;
	pc+=2;
	continue;
case CshiftrI:
	bsp[1]=(BC_WORD_S)bsp[0] >> (BC_WORD_S)bsp[1];
	++bsp;
	pc+=2;
	continue;
case CsubI:
	bsp[1]=bsp[0] - bsp[1];
	++bsp;
	pc+=1;
	continue;
case Ctestcaf:
	*--bsp=*(BC_WORD*)pc[1];
	pc+=2;
	continue;
case Cupdate:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	array[3+i]=asp[-1];
	asp[-1]=(BC_WORD)array;
	++bsp;
	--asp;
	pc+=1;
	continue;
}
case CupdateBOOL:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	((BC_BOOL*)&array[3])[i]=bsp[1];
	bsp+=2;
	pc+=1;
	continue;
}
case CupdateCHAR:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	((BC_BOOL*)&array[2])[i]=bsp[1];
	bsp+=2;
	pc+=1;
	continue;
}
case CupdateINT:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	array[3+i]=bsp[1];
	bsp+=2;
	pc+=1;
	continue;
}
case Cupdate_a:	
	asp[((BC_WORD_S*)pc)[2]] = asp[((BC_WORD_S*)pc)[1]];
	pc+=3;
	continue;
case Cupdate_b:
	bsp[pc[2]] = bsp[pc[1]];
	pc+=3;
	continue;
case Cupdate_r:
{
	BC_WORD *array,array_o,*a,*b;
	BC_WORD_S n_ab,n_a,n_b;
	
	n_ab=pc[1];
	array_o = (n_ab * (BC_WORD_S)*bsp++) + 3;
	array = (BC_WORD*)asp[0];
	a = &array[array_o];
	n_a=pc[2];
	b=a+n_a;
	a[0]=asp[-1];
	a[1]=asp[-2];
	do {
		if (n_a< 3) break; a[ 2]=asp[ -3];
		if (n_a< 4) break; a[ 3]=asp[ -4];
		if (n_a< 5) break; a[ 4]=asp[ -5];
		if (n_a< 6) break; a[ 5]=asp[ -6];
		if (n_a< 7) break; a[ 6]=asp[ -7];
		if (n_a< 8) break; a[ 7]=asp[ -8];
		if (n_a< 9) break; a[ 8]=asp[ -9];
		if (n_a<10) break; a[ 9]=asp[-10];
		if (n_a<11) break; a[10]=asp[-11];
		if (n_a<12) break; a[11]=asp[-12];
		if (n_a<13) break; a[12]=asp[-13];
		if (n_a<14) break; a[13]=asp[-14];
		if (n_a<15) break; a[14]=asp[-15];
		if (n_a<16) break; a[15]=asp[-16];
		if (n_a<17) break; a[16]=asp[-17];
		if (n_a<18) break; a[17]=asp[-18];
		if (n_a<19) break; a[18]=asp[-19];
		if (n_a<20) break; a[19]=asp[-20];
		if (n_a<21) break; a[20]=asp[-21];
		if (n_a<22) break; a[21]=asp[-22];
		if (n_a<23) break; a[22]=asp[-23];
		if (n_a<24) break; a[23]=asp[-24];
		if (n_a<25) break; a[24]=asp[-25];
		if (n_a<26) break; a[25]=asp[-26];
		if (n_a<27) break; a[26]=asp[-27];
		if (n_a<28) break; a[27]=asp[-28];
		if (n_a<29) break; a[28]=asp[-29];
		if (n_a<30) break; a[29]=asp[-30];
	} while (0);
	asp-=n_a;
	asp[0]=(BC_WORD)array;
	n_b=pc[3];
	pc+=4;
	b[0]=bsp[0];
	b[1]=bsp[1];
	do {
		if (n_b< 3) break; b[ 2]=bsp[ 2];
		if (n_b< 4) break; b[ 3]=bsp[ 3];
		if (n_b< 5) break; b[ 4]=bsp[ 4];
		if (n_b< 6) break; b[ 5]=bsp[ 5];
		if (n_b< 7) break; b[ 6]=bsp[ 6];
		if (n_b< 8) break; b[ 7]=bsp[ 7];
		if (n_b< 9) break; b[ 8]=bsp[ 8];
		if (n_b<10) break; b[ 9]=bsp[ 9];
		if (n_b<11) break; b[10]=bsp[10];
		if (n_b<12) break; b[11]=bsp[11];
		if (n_b<13) break; b[12]=bsp[12];
		if (n_b<14) break; b[13]=bsp[13];
		if (n_b<15) break; b[14]=bsp[14];
		if (n_b<16) break; b[15]=bsp[15];
		if (n_b<17) break; b[16]=bsp[16];
		if (n_b<18) break; b[17]=bsp[17];
		if (n_b<19) break; b[18]=bsp[18];
		if (n_b<20) break; b[19]=bsp[19];
		if (n_b<21) break; b[20]=bsp[20];
		if (n_b<22) break; b[21]=bsp[21];
		if (n_b<23) break; b[22]=bsp[22];
		if (n_b<24) break; b[23]=bsp[23];
		if (n_b<25) break; b[24]=bsp[24];
		if (n_b<26) break; b[25]=bsp[25];
		if (n_b<27) break; b[26]=bsp[26];
		if (n_b<28) break; b[27]=bsp[27];
		if (n_b<29) break; b[28]=bsp[28];
		if (n_b<30) break; b[29]=bsp[29];
	} while (0);
	bsp+=n_b;
	continue;
}
case Cupdate_ra:
{
	BC_WORD *array,array_o,*a;
	BC_WORD_S n_a;
	
	n_a=pc[1];
	array_o = (n_a * (BC_WORD_S)*bsp++) + 3;
	array = (BC_WORD*)asp[0];
	a = &array[array_o];
	pc+=2;
	a[0]=asp[-1];
	a[1]=asp[-2];
	a[2]=asp[-3];
	a[3]=asp[-4];
	a[4]=asp[-5];
	do {
		if (n_a< 6) break; a[ 5]=asp[ -6];
		if (n_a< 7) break; a[ 6]=asp[ -7];
		if (n_a< 8) break; a[ 7]=asp[ -8];
		if (n_a< 9) break; a[ 8]=asp[ -9];
		if (n_a<10) break; a[ 9]=asp[-10];
		if (n_a<11) break; a[10]=asp[-11];
		if (n_a<12) break; a[11]=asp[-12];
		if (n_a<13) break; a[12]=asp[-13];
		if (n_a<14) break; a[13]=asp[-14];
		if (n_a<15) break; a[14]=asp[-15];
		if (n_a<16) break; a[15]=asp[-16];
		if (n_a<17) break; a[16]=asp[-17];
		if (n_a<18) break; a[17]=asp[-18];
		if (n_a<19) break; a[18]=asp[-19];
		if (n_a<20) break; a[19]=asp[-20];
		if (n_a<21) break; a[20]=asp[-21];
		if (n_a<22) break; a[21]=asp[-22];
		if (n_a<23) break; a[22]=asp[-23];
		if (n_a<24) break; a[23]=asp[-24];
		if (n_a<25) break; a[24]=asp[-25];
		if (n_a<26) break; a[25]=asp[-26];
		if (n_a<27) break; a[26]=asp[-27];
		if (n_a<28) break; a[27]=asp[-28];
		if (n_a<29) break; a[28]=asp[-29];
		if (n_a<30) break; a[29]=asp[-30];
		if (n_a<31) break; a[30]=asp[-31];
		if (n_a<32) break; a[31]=asp[-32];
	} while (0);
	asp-=n_a;
	asp[0]=(BC_WORD)array;
	continue;
}
case Cupdate_ra1:
{
	BC_WORD *array,array_o,*a;
	BC_WORD_S n_ab;

	n_ab=pc[1];
	array_o = (n_ab * (BC_WORD_S)*bsp++) + 3;
	array = (BC_WORD*)asp[0];
	a = &array[array_o];
	pc+=2;
	a[n_ab-1]=*bsp++;
	a[0]=asp[-1];
	a[1]=asp[-2];
	a[2]=asp[-3];
	a[3]=asp[-4];
	do {
		if (n_ab< 6) break; a[ 4]=asp[ -5];
		if (n_ab< 7) break; a[ 5]=asp[ -6];
		if (n_ab< 8) break; a[ 6]=asp[ -7];
		if (n_ab< 9) break; a[ 7]=asp[ -8];
		if (n_ab<10) break; a[ 8]=asp[ -9];
		if (n_ab<11) break; a[ 9]=asp[-10];
		if (n_ab<12) break; a[10]=asp[-11];
		if (n_ab<13) break; a[11]=asp[-12];
		if (n_ab<14) break; a[12]=asp[-13];
		if (n_ab<15) break; a[13]=asp[-14];
		if (n_ab<16) break; a[14]=asp[-15];
		if (n_ab<17) break; a[15]=asp[-16];
		if (n_ab<18) break; a[16]=asp[-17];
		if (n_ab<19) break; a[17]=asp[-18];
		if (n_ab<20) break; a[18]=asp[-19];
		if (n_ab<21) break; a[19]=asp[-20];
		if (n_ab<22) break; a[20]=asp[-21];
		if (n_ab<23) break; a[21]=asp[-22];
		if (n_ab<24) break; a[22]=asp[-23];
		if (n_ab<25) break; a[23]=asp[-24];
		if (n_ab<26) break; a[24]=asp[-25];
		if (n_ab<27) break; a[25]=asp[-26];
		if (n_ab<28) break; a[26]=asp[-27];
		if (n_ab<29) break; a[27]=asp[-28];
		if (n_ab<30) break; a[28]=asp[-29];
		if (n_ab<31) break; a[29]=asp[-30];
		if (n_ab<32) break; a[30]=asp[-31];
	} while (0);
	asp-=n_ab-1;
	asp[0]=(BC_WORD)array;
	continue;
}
case Cupdate_r02:
{
	BC_WORD *array,array_o,*element_p;

	array_o = 2 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=bsp[0];
	element_p[4]=bsp[1];
	bsp+=2;
	continue;
}
case Cupdate_r12:
{
	BC_WORD *array,array_o,*element_p;

	array_o = 3 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	asp-=1;
	asp[0]=(BC_WORD)array;
	element_p[4]=bsp[0];
	element_p[5]=bsp[1];
	bsp+=2;
	continue;
}
case Cupdate_r20:
{
	BC_WORD *array,array_o,*element_p;

	array_o = 2 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	element_p[4]=asp[-2];
	asp-=2;
	asp[0]=(BC_WORD)array;
	continue;
}
case Cupdate_r21:
{
	BC_WORD *array,array_o,*element_p;

	array_o = 3 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	element_p[4]=asp[-2];
	asp-=2;
	asp[0]=(BC_WORD)array;
	element_p[5]=*bsp++;
	continue;
}
case Cupdate_r30:
{
	BC_WORD *array,array_o,*element_p;

	array_o = 3 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[0]=asp[-1];
	element_p[1]=asp[-2];
	element_p[2]=asp[-3];
	asp-=3;
	asp[0]=(BC_WORD)array;
	continue;
}
case Cupdate_r40:
{
	BC_WORD *array,array_o,*element_p;

	array_o = 4 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	element_p[4]=asp[-2];
	element_p[5]=asp[-3];
	element_p[6]=asp[-4];
	asp-=4;
	asp[0]=(BC_WORD)array;
	continue;
}
case Cupdatepop_a:
{
	BC_WORD_S ao;
	
	ao=((BC_WORD_S*)pc)[2];
	asp[ao] = asp[((BC_WORD_S*)pc)[1]];
	asp=&asp[ao];
	pc+=3;
	continue;
}
case Cupdatepop_b:
{
	BC_WORD_S bo;
	
	bo=((BC_WORD_S*)pc)[2];
	bsp[bo] = bsp[pc[1]];
	bsp=&bsp[bo];
	pc+=3;
	continue;
}
case CxorI:
	bsp[1]=bsp[0] ^ bsp[1];
	++bsp;
	pc+=1;
	continue;
case CCtoAC:
{
	BC_WORD c;
	
	if ((heap_free-=3)<0)
		break;
	c=(BC_BOOL)bsp[0];
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=1;
	hp[2]=c;
	*++asp=(BC_WORD)hp;
	++bsp;
	hp+=3;
	pc+=1;
	continue;			
}
case CItoC:
	bsp[0] = (BC_BOOL)bsp[0];
	pc+=1;
	continue;

case CaddIi:
	bsp[0]+=*(BC_WORD*)&pc[1];
	pc+=2;
	continue;
case CandIi:
	bsp[0]&=*(BC_WORD*)&pc[1];
	pc+=2;
	continue;
case CandIio:
{
	BC_WORD v;
	
	v=bsp[((BC_WORD_S*)pc)[1]] & *(BC_WORD*)&pc[2];
	*--bsp=v;
	pc+=3;
	continue;
}
case Cbuildh0_dup_a:
{
	BC_WORD v;
	BC_WORD_S ao;
	
	v=pc[1];
	ao=((BC_WORD_S*)pc)[2];
	asp[1] = v;
	asp[ao] = v;
	++asp;
	pc+=3;
	continue;
}
case Cbuildh0_dup2_a:
{
	BC_WORD v;
	BC_WORD_S ao;
	
	v=pc[1];
	ao=((BC_WORD_S*)pc)[2];
	asp[1] = v;
	asp[ao] = v;
	asp[ao-1] = v;
	++asp;
	pc+=3;
	continue;
}
case Cbuildh0_dup3_a:
{
	BC_WORD v;
	BC_WORD_S ao;
	
	v=pc[1];
	ao=((BC_WORD_S*)pc)[2];
	asp[1] = v;
	asp[ao] = v;
	asp[ao-1] = v;
	asp[ao-2] = v;
	++asp;
	pc+=3;
	continue;
}
case Cbuildh0_put_a:
	asp[((BC_WORD_S*)pc)[2]] = pc[1];
	pc+=3;
	continue;
case Cbuildh0_put_a_jsr:
	asp[((BC_WORD_S*)pc)[2]] = pc[1];
	*--csp=(BC_WORD)&pc[4];
	pc=(BC_WORD*)pc[3];
	continue;
case Cbuildo1:
{
	BC_WORD_S ao;

	if ((heap_free-=3)<0)
		break;
	ao=pc[1];
	hp[0]=pc[2];
	hp[1]=asp[ao];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=3;
	continue;
}
case Cbuildho2:
case Cbuildo2:
{
	BC_WORD_S ao1,ao2;

	if ((heap_free-=3)<0)
		break;
	ao1=((BC_WORD_S*)pc)[1];
	ao2=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao1];
	hp[2]=asp[ao2];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=4;
	continue;
}
case Cdup_a:
	asp[((BC_WORD_S*)pc)[1]] = asp[0];
	pc+=2;
	continue;
case Cdup2_a:
{
	BC_WORD_S ao_s;
	BC_WORD v;
	
	v=asp[0];
	ao_s=((BC_WORD_S*)pc)[1];
	asp[ao_s-1]=v;
	asp[ao_s]=v;
	pc+=2;
	continue;
}
case Cdup3_a:
{
	BC_WORD_S ao_s;
	BC_WORD v;
	
	v=asp[0];
	ao_s=((BC_WORD_S*)pc)[1];
	asp[ao_s-2]=v;
	asp[ao_s-1]=v;
	asp[ao_s]=v;
	pc+=2;
	continue;
}
case Cexchange_a:
{
	BC_WORD_S ao_1,ao_2;
	BC_WORD v;

	ao_1=((BC_WORD_S*)pc)[1];
	ao_2=((BC_WORD_S*)pc)[2];
	v = asp[ao_1];
	asp[ao_1] = asp[ao_2];
	asp[ao_2] = v;
	pc+=3;
	continue;
}
case CgeC:
	bsp[1] = (BC_WORD_S)bsp[0] >= (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	continue;
case Cjmp_b_false:
	if (bsp[((BC_WORD_S*)pc)[1]]){
		pc+=3;
		continue;
	}
	pc=(BC_WORD*)pc[2];
	continue;
case Cjmp_eqACio:
{
	BC_WORD_S ao;
	BC_WORD *n,*s;
	BC_WORD length;

	ao=pc[1];
	s=(BC_WORD*)*(BC_WORD*)&pc[2];
	n=(BC_WORD*)asp[ao];
	length=s[0];
	pc+=4;
	if (n[1]!=length)
		continue;
	n+=2;
	s+=1;
	for (;;){
		// TODO make 64/32 bit agnostic
		if (length>=4){
			if (*n!=*s)
				break;
			++n;
			++s;
			length-=4;
			continue;
		}
		if (length>=2){
			if (*(uint16_t*)n!=*(uint16_t*)s)
				break;
			if (length>2)
				if (((uint8_t*)n)[2]!=((uint8_t*)s)[2])
					break;
		} else {
			if (length>0)
				if (((uint8_t*)n)[0]!=((uint8_t*)s)[0])
					break;
		}
		pc=*(BC_WORD**)&pc[-1];
		break;
	}
	continue;
}
case Cjmp_eqC_b:
case Cjmp_eqI_b:
{
	BC_WORD_S bo;
	
	bo=((BC_WORD_S*)pc)[1];
	if (bsp[bo]!=(BC_WORD_S)pc[2]){
		pc+=4;
		continue;
	}
	pc=(BC_WORD*)pc[3];
	continue;
}
case Cjmp_eqC_b2:
case Cjmp_eqI_b2:
{
	BC_WORD v;
	
	v=bsp[((BC_WORD_S*)pc)[1]];
	if (v==(BC_WORD_S)pc[2]){
		pc=(BC_WORD*)pc[3];
		continue;
	}
	if (v==(BC_WORD_S)pc[4]){
		pc=(BC_WORD*)pc[5];
		continue;
	}
	pc+=6;
	continue;
}
case Cjmp_eqD_b:
	if (bsp[0]!=*(BC_WORD_S*)&pc[1]){
		pc+=3;
		continue;
	}
	pc=(BC_WORD*)pc[2];
	continue;
case Cjmp_eqD_b2:
{
	BC_WORD v;
	
	v=bsp[0];
	if (v==*(BC_WORD_S*)&pc[1]){
		pc=(BC_WORD*)pc[2];
		continue;
	}
	if (v==*(BC_WORD_S*)&pc[3]){
		pc=(BC_WORD*)pc[4];
		continue;
	}
	pc+=5;
	continue;
}
case Cjmp_eqI:
	if (bsp[0] == bsp[1]){
		bsp+=2;
		pc=(BC_WORD*)pc[1];
		continue;
	}
	bsp+=2;
	pc+=2;
	continue;			
case Cjmp_eq_desc:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	if (*n!=*(BC_WORD_S*)&pc[2]){
		pc+=4;
		continue;
	}
	pc=(BC_WORD*)pc[3];
	continue;
}
case Cjmp_geI:
	if ((int)bsp[0] >= (int)bsp[1]){
		bsp+=2;
		pc=(BC_WORD*)pc[1];
		continue;
	}
	bsp+=2;
	pc+=2;
	continue;			
case Cjmp_ltI:
	if ((int)bsp[0] < (int)bsp[1]){
		bsp+=2;
		pc=(BC_WORD*)pc[1];
		continue;
	}
	bsp+=2;
	pc+=2;
	continue;			
case Cjmp_neI:
	if (bsp[0] != bsp[1]){
		bsp+=2;
		pc=(BC_WORD*)pc[1];
		continue;
	}
	bsp+=2;
	pc+=2;
	continue;			
case Cjmp_neC_b:
case Cjmp_neI_b:
{
	BC_WORD_S bo;
	
	bo=((BC_WORD_S*)pc)[1];
	if (bsp[bo]!=(BC_WORD_S)pc[2]){
		pc+=4;
		continue;
	}
	pc=(BC_WORD*)pc[3];
	continue;
}
case Cjmp_ne_desc:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	if (*n==*(BC_WORD_S*)&pc[2]){
		pc+=4;
		continue;
	}
	pc=(BC_WORD*)pc[3];
	continue;
}
case Cjmp_o_geI:
	if ((int)bsp[((BC_WORD_S*)pc)[1]] >= (int)bsp[0]){
		bsp+=1;
		pc=(BC_WORD*)pc[2];
		continue;
	}
	bsp+=1;
	pc+=3;
	continue;
case Cjmp_o_geI_arraysize_a:
{

	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	if ((int)bsp[((BC_WORD_S*)pc)[1]] >= (int)n[1]){
		pc=(BC_WORD*)pc[3];
		continue;
	}
	pc+=4;
	continue;
}
case CltIi:
	bsp[0] = (BC_WORD_S)bsp[0] < *(BC_WORD_S*)&pc[1];
	pc+=2;
	continue;
case CneI:
	bsp[1]=bsp[0] != bsp[1];
	++bsp;
	pc+=1;
	continue;
case Cpop_a_jmp:
	asp=(BC_WORD*)(((BC_BOOL*)asp)+((BC_WORD_S*)pc)[1]);
	pc=*(BC_WORD**)&pc[2];
	continue;
case Cpop_a_jsr:
	asp=(BC_WORD*)(((BC_BOOL*)asp)+((BC_WORD_S*)pc)[1]);
	*--csp=(BC_WORD)&pc[3];
	pc=*(BC_WORD**)&pc[2];
	continue;
case Cpop_a_rtn:
	asp=(BC_WORD*)(((BC_BOOL*)asp)+((BC_WORD_S*)pc)[1]);
	pc=(BC_WORD*)*csp++;
	continue;
case Cpop_ab_rtn:
	asp=(BC_WORD*)(((BC_BOOL*)asp)+((BC_WORD_S*)pc)[1]);
	bsp=(BC_WORD*)(((BC_BOOL*)bsp)+pc[2]);
	pc=(BC_WORD*)*csp++;
	continue;
case Cpop_b_jmp:
	bsp=(BC_WORD*)(((BC_BOOL*)bsp)+pc[1]);
	pc=*(BC_WORD**)&pc[2];
	continue;
case Cpop_b_jsr:
	bsp=(BC_WORD*)(((BC_BOOL*)bsp)+pc[1]);
	*--csp=(BC_WORD)&pc[3];
	pc=*(BC_WORD**)&pc[2];
	continue;
case Cpop_b_pushBFALSE:
	bsp=(BC_WORD*)(((BC_BOOL*)bsp)+pc[1]);
	*bsp=0;
	pc+=2;
	continue;				
case Cpop_b_pushBTRUE:
	bsp=(BC_WORD*)(((BC_BOOL*)bsp)+pc[1]);
	*bsp=1;
	pc+=2;
	continue;
case Cpop_b_rtn:
	bsp=(BC_WORD*)(((BC_BOOL*)bsp)+(2*pc[1])); // TODO the 2* is 64-bit specific (see #4)
	pc=(BC_WORD*)*csp++;
	continue;
case CpushD_a_jmp_eqD_b2:
{
	BC_WORD *n,v;
	
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	v=*n;
	*--bsp=v;
	if (v==*(BC_WORD_S*)&pc[2]){
		pc=(BC_WORD*)pc[3];
		continue;
	}
	if (v==*(BC_WORD_S*)&pc[4]){
		pc=(BC_WORD*)pc[5];
		continue;
	}
	pc+=6;
	continue;
}
case Cpush_a_jsr:
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	*--csp=(BC_WORD)&pc[3];
	pc=(BC_WORD*)pc[2];
	*++asp=v;
	continue;
}
case Cpush_a2:
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	asp[1]=v;
	v=asp[((BC_WORD_S*)pc)[2]];
	asp[2]=v;
	asp+=2;
	pc+=3;
	continue;
}
case Cpush_ab:
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	*++asp=v;
	v=bsp[((BC_WORD_S*)pc)[2]];
	*--bsp=v;
	pc+=3;
	continue;
}
case Cpush_b_incI:
{
	BC_WORD v;
	v=bsp[((BC_WORD_S*)pc)[1]];
	*--bsp=v+1;
	pc+=2;
	continue;
}
case Cpush_b_jsr:
{
	BC_WORD v;

	v=bsp[((BC_WORD_S*)pc)[1]];
	*--csp=(BC_WORD)&pc[3];
	pc=(BC_WORD*)pc[2];
	*--bsp=v;
	continue;
}
case Cpush_b2:
{
	BC_WORD v;

	v=bsp[((BC_WORD_S*)pc)[1]];
	bsp[-1]=v;
	v=bsp[((BC_WORD_S*)pc)[2]];
	bsp[-2]=v;
	bsp-=2;
	pc+=3;
	continue;
}
case Cpush2_a:
{
	BC_WORD_S ao_s;

	ao_s=((BC_WORD_S*)pc)[1];
	asp[1]=asp[ao_s];
	asp[2]=asp[ao_s+1];
	asp+=2;
	pc+=2;
	continue;
}
case Cpush2_b:
{
	BC_WORD_S bo_s;

	bo_s=((BC_WORD_S*)pc)[1];
	bsp[-1]=asp[bo_s];
	bsp[-2]=asp[bo_s-1];
	bsp-=2;
	pc+=2;
	continue;
}
case Cpush3_a:
{
	BC_WORD_S ao_s;

	ao_s=((BC_WORD_S*)pc)[1];
	asp[1]=asp[ao_s];
	asp[2]=asp[ao_s+1];
	asp[3]=asp[ao_s+2];
	asp+=3;
	pc+=2;
	continue;
}
case Cpush3_b:
{
	BC_WORD_S bo_s;

	bo_s=((BC_WORD_S*)pc)[1];
	bsp[-1]=asp[bo_s];
	bsp[-2]=asp[bo_s-1];
	bsp[-3]=asp[bo_s-2];
	bsp-=3;
	pc+=2;
	continue;
}
case Cpush_jsr_eval:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=(BC_WORD)n;
	pc+=2;
	if ((n[0] & 2)!=0)
		continue;
	*--csp=(BC_WORD)pc;
	pc=(BC_WORD*)n[0];
	continue;
}
case Cpush_update_a:
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[1];
	ao_2=((BC_WORD_S*)pc)[2];
	asp[1] = asp[ao_1];
	asp[ao_1] = asp[ao_2];
	asp+=1;
	pc+=3;
	continue;
}
case Cput_a:
	asp[((BC_WORD_S*)pc)[1]] = asp[0];
	--asp;
	pc+=2;
	continue;
case Cput_b:
	bsp[((BC_WORD_S*)pc)[1]] = bsp[0];
	++bsp;
	pc+=2;
	continue;
case CselectCHARoo:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[(BC_WORD_S)pc[1]];
	i=bsp[(BC_WORD_S)pc[2]];
	*--bsp=((BC_BOOL*)&array[2])[i];
	pc+=3;
	continue;
}
case Cselectoo:
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[(BC_WORD_S)pc[1]];
	i=bsp[(BC_WORD_S)pc[2]];
	*++asp=array[3+i];
	pc+=3;
	continue;
}
case Cupdate2_a:
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	pc+=3;
	continue;
}
case Cupdate2_b:
{
	BC_WORD_S bo_s,bo_d;

	bo_s=((BC_WORD_S*)pc)[1];
	bo_d=((BC_WORD_S*)pc)[2];
	bsp[bo_d+1] = bsp[bo_s+1];
	bsp[bo_d] = bsp[bo_s];
	pc+=3;
	continue;
}
case Cupdate2pop_a:
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	asp=&asp[ao_d];
	pc+=3;
	continue;
}
case Cupdate2pop_b:
{
	BC_WORD_S bo_s,bo_d;

	bo_s=((BC_WORD_S*)pc)[1];
	bo_d=((BC_WORD_S*)pc)[2];
	bsp[bo_d+1] = bsp[bo_s+1];
	bsp[bo_d] = bsp[bo_s];
	bsp=&bsp[bo_d];
	pc+=3;
	continue;
}
case Cupdate3_a:
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-2] = asp[ao_s-2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	pc+=3;
	continue;
}
case Cupdate3_b:
{
	BC_WORD_S bo_s,bo_d;

	bo_s=((BC_WORD_S*)pc)[1];
	bo_d=((BC_WORD_S*)pc)[2];
	bsp[bo_d+2] = bsp[bo_s+2];
	bsp[bo_d+1] = bsp[bo_s+1];
	bsp[bo_d] = bsp[bo_s];
	pc+=3;
	continue;
}
case Cupdate3pop_a:
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-2] = asp[ao_s-2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	asp=&asp[ao_d];
	pc+=3;
	continue;
}
case Cupdate4_a:
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-3] = asp[ao_s-3];
	asp[ao_d-2] = asp[ao_s-2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	pc+=3;
	continue;
}
case Cupdate3pop_b:
{
	BC_WORD_S bo_s,bo_d;

	bo_s=((BC_WORD_S*)pc)[1];
	bo_d=((BC_WORD_S*)pc)[2];
	bsp[bo_d+2] = bsp[bo_s+2];
	bsp[bo_d+1] = bsp[bo_s+1];
	bsp[bo_d] = bsp[bo_s];
	bsp=&bsp[bo_d];
	pc+=3;
	continue;
}
case Cupdates2_a:
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[2];
	ao_2=((BC_WORD_S*)pc)[3];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[1];
	asp[ao_1] = asp[ao_2];
	pc+=4;
	continue;
}
case Cupdates2_a_pop_a:
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[2];
	ao_2=((BC_WORD_S*)pc)[3];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[1];
	asp[ao_1] = asp[ao_2];
	asp=(BC_WORD*)(((BC_BOOL*)asp)+((BC_WORD_S*)pc)[4]);
	pc+=5;
	continue;
}
case Cupdates2_b:
{
	BC_WORD_S bo_1,bo_2;

	bo_1=((BC_WORD_S*)pc)[2];
	bo_2=((BC_WORD_S*)pc)[3];
	bsp[bo_2] = bsp[bo_1];
	bo_2=((BC_WORD_S*)pc)[1];
	bsp[bo_1] = bsp[bo_2];
	pc+=4;
	continue;
}
case Cupdates2pop_a:
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[2];
	ao_2=((BC_WORD_S*)pc)[3];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[1];
	asp[ao_1] = asp[ao_2];
	asp=&asp[ao_1];
	pc+=4;
	continue;
}
case Cupdates2pop_b:
{
	BC_WORD_S bo_1,bo_2;

	bo_1=((BC_WORD_S*)pc)[2];
	bo_2=((BC_WORD_S*)pc)[3];
	bsp[bo_2] = bsp[bo_1];
	bo_2=((BC_WORD_S*)pc)[1];
	bsp[bo_1] = bsp[bo_2];
	bsp=&bsp[bo_1];
	pc+=4;
	continue;
}
case Cupdates3_a:
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[3];
	ao_2=((BC_WORD_S*)pc)[4];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[2];
	asp[ao_1] = asp[ao_2];
	ao_1=((BC_WORD_S*)pc)[1];
	asp[ao_2] = asp[ao_1];
	pc+=5;
	continue;
}
case Cupdates3_b:
{
	BC_WORD_S bo_1,bo_2;

	bo_1=((BC_WORD_S*)pc)[3];
	bo_2=((BC_WORD_S*)pc)[4];
	bsp[bo_2] = bsp[bo_1];
	bo_2=((BC_WORD_S*)pc)[2];
	bsp[bo_1] = bsp[bo_2];
	bo_1=((BC_WORD_S*)pc)[1];
	bsp[bo_2] = bsp[bo_1];
	pc+=5;
	continue;
}
case Cupdates3pop_a:
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[3];
	ao_2=((BC_WORD_S*)pc)[4];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[2];
	asp[ao_1] = asp[ao_2];
	ao_1=((BC_WORD_S*)pc)[1];
	asp[ao_2] = asp[ao_1];
	asp=&asp[ao_2];
	pc+=5;
	continue;
}
case Cupdates3pop_b:
{
	BC_WORD_S bo_1,bo_2;

	bo_1=((BC_WORD_S*)pc)[3];
	bo_2=((BC_WORD_S*)pc)[4];
	bsp[bo_2] = bsp[bo_1];
	bo_2=((BC_WORD_S*)pc)[2];
	bsp[bo_1] = bsp[bo_2];
	bo_1=((BC_WORD_S*)pc)[1];
	bsp[bo_2] = bsp[bo_1];
	bsp=&bsp[bo_2];
	pc+=5;
	continue;
}
case Cupdates4_a:
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[4];
	ao_2=((BC_WORD_S*)pc)[5];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[3];
	asp[ao_1] = asp[ao_2];
	ao_1=((BC_WORD_S*)pc)[2];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[1];
	asp[ao_1] = asp[ao_2];
	pc+=6;
	continue;
}

case Cjsr_ap4:
	*--csp=(BC_WORD)&pc[1];
case Cjmp_ap4:
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
	if (((uint16_t*)d)[0]==32){
		BC_WORD arity;
		
		arity=((uint16_t*)d)[-1];
		pc = (BC_WORD*) ((*(BC_WORD*)(d+32-6)) - 12);
		if (arity<=1){
			if (arity<1){
				--asp;
			} else {
				asp[0]=n[1];
			}
		} else {
			BC_WORD *args,a1;
			
			args=(BC_WORD*)n[2];
			a1=n[1];
			if (arity==2){
				*++asp=(BC_WORD)args;
			} else {
				asp[0]=args[arity-2];
				arity-=3;
				do {
					*++asp = args[arity];
					--arity;
				} while (arity!=0);
			}
			*++asp = a1;
		}
		continue;
	} else {
		*--csp=(BC_WORD)&Fjmp_ap3;
		pc = *(BC_WORD**)(d+2);
		continue;
	}
}
case Cjsr_ap3:
	*--csp=(BC_WORD)&pc[1];
case Cjmp_ap3:
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
	if (((uint16_t*)d)[0]==24){
		BC_WORD arity;
		
		arity=((uint16_t*)d)[-1];
		pc = (BC_WORD*) ((*(BC_WORD*)(d+24-6)) - 12);
		if (arity<=1){
			if (arity<1){
				--asp;
			} else {
				asp[0]=n[1];
			}
		} else {
			BC_WORD *args,a1;
			
			args=(BC_WORD*)n[2];
			a1=n[1];
			if (arity==2){
				*++asp=(BC_WORD)args;
			} else {
				asp[0]=args[arity-2];
				arity-=3;
				do {
					*++asp = args[arity];
					--arity;
				} while (arity!=0);
			}
			*++asp = a1;
		}
		continue;
	} else {
		*--csp=(BC_WORD)&Fjmp_ap2;
		pc = *(BC_WORD**)(d+2);
		continue;
	}
}
case Cjsr_ap2:
	*--csp=(BC_WORD)&pc[1];
case Cjmp_ap2:
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
	if (((uint16_t*)d)[0]==16){
		BC_WORD arity;
		
		arity=((uint16_t*)d)[-1];
		pc = (BC_WORD*) ((*(BC_WORD*)(d+16-6)) - 12);
		if (arity<=1){
			if (arity<1){
				--asp;
			} else {
				asp[0]=n[1];
			}
		} else {
			BC_WORD *args,a1;
			
			args=(BC_WORD*)n[2];
			a1=n[1];
			if (arity==2){
				*++asp=(BC_WORD)args;
			} else {
				asp[0]=args[arity-2];
				arity-=3;
				do {
					*++asp = args[arity];
					--arity;
				} while (arity!=0);
			}
			*++asp = a1;
		}
		continue;
	} else {
		*--csp=(BC_WORD)&Fjmp_ap1;
		pc = *(BC_WORD**)(d+2);
		continue;
	}
}
case Cjsr_ap1:
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	*--csp=(BC_WORD)&pc[1];
	d=n[0];
#if 0
	printf ("Cjsr_ap1 %d %d %d %d\n",(int)(pc-program),n,d,(int)d-(int)data);
#endif
	pc = *(BC_WORD**)(d+2);
	continue;
}
case Cjmp_ap1:
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
	pc = *(BC_WORD**)(d+2);
	continue;
}
case Cadd_arg0:
{
	BC_WORD *n;
#if 0
	printf ("Cadd_arg0 %d %d\n",(int)(pc-program),(int)n);
#endif
	if ((heap_free-=2)<0)
		break;
	n=(BC_WORD*)asp[0];
	pc=(BC_WORD*)*csp++;
	hp[1]=asp[-1];
	asp[-1]=(BC_WORD)hp;
	hp[0]=n[0]+8;
	--asp;
	hp+=2;
	continue;
}
case Cadd_arg1:
{
	BC_WORD *n;
	
	if ((heap_free-=3)<0)
		break;
	n=(BC_WORD*)asp[0];
	hp[2]=asp[-1];
	pc=(BC_WORD*)*csp++;
	hp[1]=n[1];
	asp[-1]=(BC_WORD)hp;
	hp[0]=n[0]+8;
	--asp;
	hp+=3;
	continue;
}
case Cadd_arg2:
{
	BC_WORD *n;
	
	if ((heap_free-=5)<0)
		break;
	n=(BC_WORD*)asp[0];
	hp[4]=asp[-1];
	pc=(BC_WORD*)*csp++;
	hp[1]=n[1];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=n[2];
	asp[-1]=(BC_WORD)hp;
	hp[0]=n[0]+8;
	--asp;
	hp+=5;
	continue;
}
case Cadd_arg3:
{
	BC_WORD *n,*a;
	
	if ((heap_free-=6)<0)
		break;
	n=(BC_WORD*)asp[0];
	hp[5]=asp[-1];
	pc=(BC_WORD*)*csp++;
	a=(BC_WORD*)n[2];
	hp[1]=n[1];
	hp[2]=(BC_WORD)&hp[3];
	hp[0]=n[0]+8;
	hp[3]=a[0];
	hp[4]=a[1];
	asp[-1]=(BC_WORD)hp;
	--asp;
	hp+=6;
	continue;
}
case Ceval_upd0:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__indirection[5];
	n[1]=asp[-1];
	--asp;
	pc=*(BC_WORD**)&pc[1];
	continue;
}
case Ceval_upd1:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__indirection[5];
	asp[0]=n[1];
	n[1]=asp[-1];
	pc=*(BC_WORD**)&pc[1];
	continue;
}
case Ceval_upd2:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__indirection[5];
	asp[1]=n[1];
	n[1]=asp[-1];
#if 0
	printf ("Ceval_upd2 %d %d\n",(int)n,(int)asp[-1]);
#endif
	asp[0]=n[2];
	pc=*(BC_WORD**)&pc[1];
	asp+=1;
	continue;
}
case Ceval_upd3:
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__indirection[5];
	asp[2]=n[1];
	n[1]=asp[-1];
	asp[1]=n[2];
	asp[0]=n[3];
	pc=*(BC_WORD**)&pc[1];
	asp+=2;
	continue;
}
case Cjsr_stack_check:
	csp-=3;
	csp[0]=(BC_WORD)&pc[4];
	csp[1]=(BC_WORD)&asp[(BC_WORD)pc[2]];
	csp[2]=(BC_WORD)&bsp[-(BC_WORD)pc[3]];
	pc=(BC_WORD*)pc[1];
	continue;
case Cstack_check:
	if (csp[0]!=(BC_WORD)asp){
		printf("Cstack_check asp incorrect %lu %p %lu %p\n",csp[0],(void*)asp,csp[1],(void*)bsp);
		printf("%lu %d %d %d\n",*pc,(int)(pc-code),(int)(asp-stack),(int)(&stack[stack_size]-bsp));
		exit (1);
	}
	if (csp[1]!=(BC_WORD)bsp){
		printf("Cstack_check bsp incorrect %lu %p %lu %p\n",csp[0],(void*)asp,csp[1],(void*)bsp);
		printf("%lu %d %d %d\n",*pc,(int)(pc-code),(int)(asp-stack),(int)(&stack[stack_size]-bsp));
		exit (1);
	}
	csp+=2;
	++pc;
	continue;
case Cjesr:
#if 0
	printf ("%d\n",pc[1]);
#endif
	switch (pc[1]){
		
		case 1:
			pc+=2;
			g_asp=asp;
			g_bsp=bsp;
			g_heap_free=heap_free;
			g_hp=hp;
			clean_catAC();
			asp=g_asp;
			bsp=g_bsp;
			heap_free=g_heap_free;
			hp=g_hp;
			continue;
		case 2:
			pc+=2;
			g_asp=asp;
			g_bsp=bsp;
			g_heap_free=heap_free;
			g_hp=hp;
			clean_sliceAC();
			asp=g_asp;
			bsp=g_bsp;
			heap_free=g_heap_free;
			hp=g_hp;
			continue;
		case 3:
			pc+=2;
			g_asp=asp;
			g_bsp=bsp;
			g_heap_free=heap_free;
			g_hp=hp;
			clean_ItoAC();
			asp=g_asp;
			bsp=g_bsp;
			heap_free=g_heap_free;
			hp=g_hp;
			continue;
		case 5:
			pc+=2;
			g_asp=asp;
			clean_print_string_();
			asp=g_asp;
			continue;
		case 6:
			pc+=2;
			g_asp=asp;
			g_bsp=bsp;
			clean_openF();
			asp=g_asp;
			bsp=g_bsp;
			continue;
		case 8:
			pc+=2;
			g_bsp=bsp;
			clean_closeF();
			bsp=g_bsp;
			continue;
		case 9:
			pc+=2;
			g_asp=asp;
			g_bsp=bsp;
			g_heap_free=heap_free;
			g_hp=hp;
			clean_readLineF();
			asp=g_asp;
			bsp=g_bsp;
			heap_free=g_heap_free;
			hp=g_hp;
			continue;
		case 10:
			pc+=2;
			g_bsp=bsp;
			clean_endF();
			bsp=g_bsp;
			continue;
		case 11:
			pc+=2;
			g_asp=asp;
			g_bsp=bsp;
			clean_cmpAC();
			asp=g_asp;
			bsp=g_bsp;
			continue;
		case 12:
			pc+=2;
			g_bsp=bsp;
			clean_writeFI();
			bsp=g_bsp;
			continue;
		case 13:
			pc+=2;
			g_asp=asp;
			g_bsp=bsp;
			clean_writeFS();
			asp=g_asp;
			bsp=g_bsp;
			continue;
		case 14:
			pc+=2;
			g_bsp=bsp;
			clean_writeFC();
			bsp=g_bsp;
			continue;
	}
default:
	fprintf(stderr, "Unimplemented instruction at %d\n", (int) (pc-code));
	return 1;
