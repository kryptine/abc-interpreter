#ifdef COMPUTED_GOTOS
# define INSTRUCTION_BLOCK(instr) instr_ ## instr
# define GARBAGE_COLLECT goto garbage_collect
# define END_INSTRUCTION_BLOCK goto **(void**)pc
# define UNIMPLEMENTED_INSTRUCTION_BLOCK instr_unimplemented
#else
# define INSTRUCTION_BLOCK(instr) case C ## instr
# define GARBAGE_COLLECT break
# define END_INSTRUCTION_BLOCK continue
# define UNIMPLEMENTED_INSTRUCTION_BLOCK default
#endif

#define NEED_HEAP(words) {if ((heap_free-=words)<0){ heap_free+=words; GARBAGE_COLLECT;}}

INSTRUCTION_BLOCK(absR):
{
	BC_REAL d=fabs(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(acosR):
{
	BC_REAL d=acos(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(addI):
	bsp[1]=(BC_WORD_S)bsp[0]+(BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(addIo):
	bsp[1]=(BC_WORD_S)bsp[0]+(BC_WORD_S)bsp[1];
	bsp[0]=(BC_WORD_S)bsp[1]<(BC_WORD_S)bsp[0];
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(addLU):
	bsp[2]=(BC_WORD_S)bsp[1] + (BC_WORD_S)bsp[2];
	bsp[1]=bsp[0]+(bsp[2]<bsp[1] ? 1 : 0);
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(addR):
{
	BC_REAL d=*(BC_REAL*)&bsp[0] + *(BC_REAL*)&bsp[1];
	bsp[1]=*(BC_WORD*)&d;
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(add_empty_node2):
	NEED_HEAP(3);
	pc=*(BC_WORD**)&pc[1];
	hp[0]=(BC_WORD)&__cycle__in__spine;
	asp[1]=asp[0];
	asp[0]=asp[-1];
	asp[-1]=(BC_WORD)hp;
	asp+=1;
	hp+=3;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(add_empty_node3):
	NEED_HEAP(3);
	pc=*(BC_WORD**)&pc[1];
	hp[0]=(BC_WORD)&__cycle__in__spine;
	asp[1]=asp[0];
	asp[0]=asp[-1];
	asp[-1]=asp[-2];
	asp[-2]=(BC_WORD)hp;
	asp+=1;
	hp+=3;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(add_empty_node32): instr_arg=32; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node31): instr_arg=31; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node30): instr_arg=30; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node29): instr_arg=29; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node28): instr_arg=28; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node27): instr_arg=27; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node26): instr_arg=26; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node25): instr_arg=25; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node24): instr_arg=24; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node23): instr_arg=23; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node22): instr_arg=22; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node21): instr_arg=21; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node20): instr_arg=20; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node19): instr_arg=19; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node18): instr_arg=18; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node17): instr_arg=17; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node16): instr_arg=16; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node15): instr_arg=15; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node14): instr_arg=14; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node13): instr_arg=13; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node12): instr_arg=12; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node11): instr_arg=11; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node10): instr_arg=10; goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node9):  instr_arg=9;  goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node8):  instr_arg=8;  goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node7):  instr_arg=7;  goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node6):  instr_arg=6;  goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node5):  instr_arg=5;  goto add_empty_node_n;
INSTRUCTION_BLOCK(add_empty_node4):  instr_arg=4;
add_empty_node_n:
{
	NEED_HEAP(3);
	pc=*(BC_WORD**)&pc[1];
	hp[0]=(BC_WORD)&__cycle__in__spine;
	for (int i=0; i>-instr_arg; i--)
		asp[i+1]=asp[i];
	asp[1-instr_arg]=(BC_WORD)hp;
	asp+=1;
	hp+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(andI):
	bsp[1]=bsp[0] & bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(asinR):
{
	BC_REAL d=asin(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(atanR):
{
	BC_REAL d=atan(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build):
{
	BC_WORD s;

	s=pc[1];
	NEED_HEAP(s+1);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build0):
	NEED_HEAP(3);
	hp[0]=pc[1];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build1):
	NEED_HEAP(3);
	hp[0]=pc[1];
	hp[1]=asp[0];
	asp[0]=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build2):
INSTRUCTION_BLOCK(buildh2):
INSTRUCTION_BLOCK(buildhr20):
	NEED_HEAP(3);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	*--asp=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build3):
	NEED_HEAP(4);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=asp[-2];
	asp[-2]=(BC_WORD)hp;
	asp-=2;
	hp+=4;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build4):
	NEED_HEAP(5);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=asp[-2];
	hp[4]=asp[-3];
	asp[-3]=(BC_WORD)hp;
	asp-=3;
	hp+=5;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildh0):
INSTRUCTION_BLOCK(buildAC):
	*++asp=pc[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildh):
INSTRUCTION_BLOCK(buildhra0):
{
	BC_WORD s_p_2;

	s_p_2=pc[1]; /* >=5+2 */
	NEED_HEAP(s_p_2);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildh1):
INSTRUCTION_BLOCK(buildhr10):
	NEED_HEAP(2);
	hp[0]=pc[1];
	hp[1]=*asp;
	*asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildh3):
INSTRUCTION_BLOCK(buildhr30):
	NEED_HEAP(5);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[-1];
	hp[4]=asp[-2];
	asp[-2]=(BC_WORD)hp;
	asp-=2;
	hp+=5;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildh4):
INSTRUCTION_BLOCK(buildhr40):
	NEED_HEAP(6);
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
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildhr0b):
{
	BC_WORD n_b;

	n_b=pc[1];
	NEED_HEAP(n_b+2);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildhr1b):
{
	BC_WORD n_b;

	n_b=pc[1];
	NEED_HEAP(n_b+3);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildBFALSE):
	NEED_HEAP(2);
	hp[0]=(BC_WORD)&BOOL+2;
	hp[1]=0;
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildBTRUE):
	NEED_HEAP(2);
	hp[0]=(BC_WORD)&BOOL+2;
	hp[1]=1;
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildB_b):
{
	BC_WORD_S bo;

	NEED_HEAP(2);
	bo = pc[1];
	hp[0]=(BC_WORD)&BOOL+2;
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildC):
	NEED_HEAP(2);
	hp[0]=(BC_WORD)&CHAR+2;
	hp[1]=pc[1];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildC_b):
{
	BC_WORD_S bo;

	NEED_HEAP(2);
	bo = pc[1];
	hp[0]=(BC_WORD)&CHAR+2;
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildF_b):
{
	BC_WORD_S bo;

	NEED_HEAP(3);
	bo = pc[1];
	hp[0]=(BC_WORD)&dFILE+2;
	hp[1]=bsp[bo];
	hp[2]=bsp[bo+1];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildI):
	NEED_HEAP(2);
	hp[0]=(BC_WORD)&INT+2;
	hp[1]=pc[1];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildI_b):
{
	BC_WORD_S bo;

	NEED_HEAP(2);
	bo = pc[1];
	hp[0]=(BC_WORD)&INT+2;
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildR):
	NEED_HEAP(2);
	hp[0]=(BC_WORD)&REAL+2;
	hp[1]=pc[1];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildR_b):
{
	BC_WORD_S bo;
	NEED_HEAP(2);
	bo=pc[1];
	hp[0]=(BC_WORD)&REAL+2;
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildhr):
{
	BC_WORD n_a,n_b,n_ab;

	n_ab=pc[1];
	NEED_HEAP(n_ab+2);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildhra1):
{
	BC_WORD n_a;

	n_a=pc[1];
	NEED_HEAP(n_a+3);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildhr01):
{
	NEED_HEAP(2);
	hp[0]=pc[1];
	hp[1]=*bsp++;
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildhr02):
{
	NEED_HEAP(3);
	hp[0]=pc[1];
	hp[1]=bsp[0];
	hp[2]=bsp[1];
	*++asp=(BC_WORD)hp;
	bsp+=2;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildhr03):
{
	NEED_HEAP(5);
	hp[0]=pc[1];
	hp[1]=bsp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[1];
	hp[4]=bsp[2];
	*++asp=(BC_WORD)hp;
	bsp+=3;
	hp+=5;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildhr11):
	NEED_HEAP(3);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=bsp[0];
	*asp=(BC_WORD)hp;
	++bsp;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildhr12):
	NEED_HEAP(5);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[0];
	hp[4]=bsp[1];
	*asp=(BC_WORD)hp;
	bsp+=2;
	hp+=5;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildhr13):
	NEED_HEAP(6);
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
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildhr21):
	NEED_HEAP(5);
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
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildhr22):
	NEED_HEAP(6);
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
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildhr31):
	NEED_HEAP(6);
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
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_r):
{
	BC_WORD_S ao,bo;
	BC_WORD n_a,n_b,n_ab,*ao_p,*bo_p;

	n_ab=pc[1];
	NEED_HEAP(n_ab+2);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_ra1):
{
	BC_WORD_S ao,bo;
	BC_WORD n_a,*ao_p;

	n_a=pc[1];
	NEED_HEAP(n_a+3);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_ra0):
{
	BC_WORD_S ao;
	BC_WORD n_a,*ao_p;

	n_a=pc[1];
	NEED_HEAP(n_a+2);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r01):
{
	BC_WORD_S bo;

	NEED_HEAP(2);
	bo=pc[1];
	hp[0]=pc[2];
	hp[1]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r02):
{
	BC_WORD_S bo;

	NEED_HEAP(3);
	bo=pc[1];
	hp[0]=pc[2];
	hp[1]=bsp[bo];
	hp[2]=bsp[bo+1];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r03):
{
	BC_WORD_S bo;

	NEED_HEAP(5);
	bo=pc[1];
	hp[0]=pc[2];
	hp[1]=bsp[bo];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[bo+1];
	hp[4]=bsp[bo+2];
	*++asp=(BC_WORD)hp;
	hp+=5;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r04):
{
	BC_WORD_S bo;

	NEED_HEAP(6);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r0b):
{
	BC_WORD_S bo;
	BC_WORD n_b,*bo_p;

	n_b=pc[1];
	NEED_HEAP(n_b+2);
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
	*++asp=(BC_WORD)hp;
	hp+=n_b+2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r10):
{
	BC_WORD_S ao;

	NEED_HEAP(2);
	ao=pc[1];
	hp[0]=pc[2];
	hp[1]=asp[ao];
	*++asp=(BC_WORD)hp;
	hp+=2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r11):
{
	BC_WORD_S ao,bo;

	NEED_HEAP(3);
	ao=((BC_WORD_S*)pc)[1];
	bo=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao];
	hp[2]=bsp[bo];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r12):
{
	BC_WORD_S ao,bo;

	NEED_HEAP(5);
	ao=((BC_WORD_S*)pc)[1];
	bo=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[bo];
	hp[4]=bsp[bo+1];
	*++asp=(BC_WORD)hp;
	hp+=5;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r13):
{
	BC_WORD_S ao,bo;

	NEED_HEAP(6);
	ao=((BC_WORD_S*)pc)[1];
	bo=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=bsp[bo];
	hp[4]=bsp[bo+1];
	hp[5]=bsp[bo+2];
	*++asp=(BC_WORD)hp;
	hp+=6;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r1b):
{
	BC_WORD_S ao,bo;
	BC_WORD n_b,*bo_p;

	n_b=pc[1];
	NEED_HEAP(n_b+3);
	ao=pc[2];
	hp[0]=pc[3];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	bo=pc[4];
	*++asp=(BC_WORD)hp;
	pc+=5;
	bo_p=&bsp[bo];
	hp[3]=bo_p[0];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r20):
{
	BC_WORD_S ao;

	NEED_HEAP(3);
	ao=((BC_WORD_S*)pc)[1];
	hp[0]=pc[2];
	hp[1]=asp[ao];
	hp[2]=asp[ao-1];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r21):
{
	BC_WORD_S ao,bo;

	NEED_HEAP(5);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r30):
{
	BC_WORD_S ao;

	NEED_HEAP(5);
	ao=((BC_WORD_S*)pc)[1];
	hp[0]=*(BC_WORD*)&pc[2];
	hp[1]=asp[ao];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=asp[ao-1];
	hp[4]=asp[ao-2];
	*++asp=(BC_WORD)hp;
	hp+=5;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r31):
{
	BC_WORD_S ao,bo;

	NEED_HEAP(6);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_r40):
{
	BC_WORD_S ao;

	NEED_HEAP(6);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_u):
{
	BC_WORD n_a,n_b,n_ab;

	n_ab=pc[1];
	NEED_HEAP(n_ab+1);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_u01):
	NEED_HEAP(3);
	hp[0]=pc[1];
	hp[1]=*bsp++;
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_u02):
	NEED_HEAP(3);
	hp[0]=pc[1];
	hp[1]=bsp[0];
	hp[2]=bsp[1];
	*++asp=(BC_WORD)hp;
	bsp+=2;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_u03):
	NEED_HEAP(4);
	hp[0]=pc[1];
	hp[1]=bsp[0];
	hp[2]=bsp[1];
	hp[3]=bsp[2];
	*++asp=(BC_WORD)hp;
	bsp+=3;
	hp+=4;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_u0b):
{
	int n_b=pc[1];
	NEED_HEAP(n_b+1);
	hp[0]=pc[2];
	hp[1]=bsp[0];
	hp[2]=bsp[1];
	hp[3]=bsp[2];
	hp[4]=bsp[3];
	do {
		if (n_b< 5) break; hp[ 5]=bsp[ 4];
		if (n_b< 6) break; hp[ 6]=bsp[ 5];
		if (n_b< 7) break; hp[ 7]=bsp[ 6];
		if (n_b< 8) break; hp[ 8]=bsp[ 7];
		if (n_b< 9) break; hp[ 9]=bsp[ 8];
		if (n_b<10) break; hp[10]=bsp[ 9];
		if (n_b<11) break; hp[11]=bsp[10];
		if (n_b<12) break; hp[12]=bsp[11];
		if (n_b<13) break; hp[13]=bsp[12];
		if (n_b<14) break; hp[14]=bsp[13];
		if (n_b<15) break; hp[15]=bsp[14];
		if (n_b<16) break; hp[16]=bsp[15];
		if (n_b<17) break; hp[17]=bsp[16];
		if (n_b<18) break; hp[18]=bsp[17];
		if (n_b<19) break; hp[19]=bsp[18];
		if (n_b<20) break; hp[20]=bsp[19];
		if (n_b<21) break; hp[21]=bsp[20];
		if (n_b<22) break; hp[22]=bsp[21];
		if (n_b<23) break; hp[23]=bsp[22];
		if (n_b<24) break; hp[24]=bsp[23];
		if (n_b<25) break; hp[25]=bsp[24];
		if (n_b<26) break; hp[26]=bsp[25];
		if (n_b<27) break; hp[27]=bsp[26];
		if (n_b<28) break; hp[28]=bsp[27];
		if (n_b<29) break; hp[29]=bsp[28];
		if (n_b<30) break; hp[30]=bsp[29];
		if (n_b<31) break; hp[31]=bsp[30];
		if (n_b<32) break; hp[32]=bsp[31];
	} while (0);
	*++asp=(BC_WORD)hp;
	bsp+=n_b;
	hp+=n_b+1;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_u11):
	NEED_HEAP(3);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=*bsp++;
	asp[0]=(BC_WORD)hp;
	hp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_u12):
	NEED_HEAP(4);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=bsp[0];
	hp[3]=bsp[1];
	asp[0]=(BC_WORD)hp;
	bsp+=2;
	hp+=4;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_u13):
	NEED_HEAP(5);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=bsp[0];
	hp[3]=bsp[1];
	hp[4]=bsp[2];
	asp[0]=(BC_WORD)hp;
	bsp+=3;
	hp+=5;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_u1b):
{
	int n_b=pc[1];
	NEED_HEAP(n_b+2);
	hp[0]=pc[2];
	hp[1]=asp[0];
	hp[2]=bsp[0];
	hp[3]=bsp[1];
	hp[4]=bsp[2];
	hp[5]=bsp[3];
	do {
		if (n_b< 5) break; hp[ 6]=bsp[ 4];
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
	} while (0);
	asp[0]=(BC_WORD)hp;
	bsp+=n_b;
	hp+=n_b+2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_u21):
	NEED_HEAP(4);
	hp[0]=pc[1];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=bsp[0];
	asp[-1]=(BC_WORD)hp;
	asp-=1;
	bsp+=1;
	hp+=4;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_u22):
	NEED_HEAP(5);
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
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_u2b):
{
	int n_b=pc[1];
	NEED_HEAP(n_b+3);
	hp[0]=pc[2];
	hp[1]=asp[0];
	hp[2]=asp[-1];
	hp[3]=bsp[0];
	hp[4]=bsp[1];
	hp[5]=bsp[2];
	do {
		if (n_b< 4) break; hp[ 6]=bsp[ 3];
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
	} while (0);
	asp[-1]=(BC_WORD)hp;
	asp--;
	bsp+=n_b;
	hp+=n_b+3;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(build_u31):
	NEED_HEAP(5);
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
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(build_ua1):
{
	BC_WORD n_a;

	n_a=pc[1];
	NEED_HEAP(n_a+2);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(cosR):
{
	BC_REAL d=cos(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create):
	NEED_HEAP(3);
	hp[0]=(BC_WORD)&__cycle__in__spine;
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(creates):
{
	BC_WORD n_a_p_1;

	n_a_p_1=pc[1];
	NEED_HEAP(n_a_p_1);
	hp[0]=(BC_WORD)&__cycle__in__spine; /* TODO */
	hp[1]=(BC_WORD)hp;
	hp[2]=(BC_WORD)hp;
	pc+=2;
	do {
		if (n_a_p_1< 3) break; hp[ 3]=(BC_WORD)hp;
		if (n_a_p_1< 4) break; hp[ 4]=(BC_WORD)hp;
		if (n_a_p_1< 5) break; hp[ 5]=(BC_WORD)hp;
		if (n_a_p_1< 6) break; hp[ 6]=(BC_WORD)hp;
		if (n_a_p_1< 7) break; hp[ 7]=(BC_WORD)hp;
		if (n_a_p_1< 8) break; hp[ 8]=(BC_WORD)hp;
		if (n_a_p_1< 9) break; hp[ 9]=(BC_WORD)hp;
		if (n_a_p_1<10) break; hp[10]=(BC_WORD)hp;
		if (n_a_p_1<11) break; hp[11]=(BC_WORD)hp;
		if (n_a_p_1<12) break; hp[12]=(BC_WORD)hp;
		if (n_a_p_1<13) break; hp[13]=(BC_WORD)hp;
		if (n_a_p_1<14) break; hp[14]=(BC_WORD)hp;
		if (n_a_p_1<15) break; hp[15]=(BC_WORD)hp;
		if (n_a_p_1<16) break; hp[16]=(BC_WORD)hp;
		if (n_a_p_1<17) break; hp[17]=(BC_WORD)hp;
		if (n_a_p_1<18) break; hp[18]=(BC_WORD)hp;
		if (n_a_p_1<19) break; hp[19]=(BC_WORD)hp;
		if (n_a_p_1<20) break; hp[20]=(BC_WORD)hp;
		if (n_a_p_1<21) break; hp[21]=(BC_WORD)hp;
		if (n_a_p_1<22) break; hp[22]=(BC_WORD)hp;
		if (n_a_p_1<23) break; hp[23]=(BC_WORD)hp;
		if (n_a_p_1<24) break; hp[24]=(BC_WORD)hp;
		if (n_a_p_1<25) break; hp[25]=(BC_WORD)hp;
		if (n_a_p_1<26) break; hp[26]=(BC_WORD)hp;
		if (n_a_p_1<27) break; hp[27]=(BC_WORD)hp;
		if (n_a_p_1<28) break; hp[28]=(BC_WORD)hp;
		if (n_a_p_1<29) break; hp[29]=(BC_WORD)hp;
		if (n_a_p_1<30) break; hp[30]=(BC_WORD)hp;
		if (n_a_p_1<31) break; hp[31]=(BC_WORD)hp;
		if (n_a_p_1<32) break; hp[32]=(BC_WORD)hp;
	} while (0);
	*++asp=(BC_WORD)hp;
	hp+=n_a_p_1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array):
{
	BC_WORD s,n;

	s=bsp[0];
	NEED_HEAP(s+3);
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=0;
	n = asp[0];
	asp[0]=(BC_WORD)hp;
	hp+=3;
	pc+=1;
	while (s>3){
		hp[0]=n;
		hp[1]=n;
		hp[2]=n;
		hp[3]=n;
		hp+=4;
		s-=4;
	}
	hp+=s;
	while (s)
		hp[-(s--)]=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_arrayBOOL):
{
	BC_WORD s,n,sw;

	s=bsp[0];
#if (WORD_WIDTH == 64)
	sw=(s+7)>>3;
#else
	sw=(s+3)>>2;
#endif
	NEED_HEAP(sw+3);
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&BOOL+2;
	hp+=3;
	n=(BC_BOOL)bsp[1];
#if (WORD_WIDTH == 64)
	n = (n<<56) | (n<<48) | (n<<40) | (n<<32) | (n<<24) | (n<<16) | (n<<8) | n;
#else
	n = (n<<24) | (n<<16) | (n<<8) | n;
#endif
	bsp+=2;
	*++asp=(BC_WORD)hp;
	while (sw>3) {
		hp[0]=n;
		hp[1]=n;
		hp[2]=n;
		hp[3]=n;
		hp+=4;
		sw-=4;
	}
	hp+=sw;
	while (sw)
		hp[-(sw--)]=n;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_arrayCHAR):
{
	BC_WORD s,n,sw;

	s=bsp[0];
#if (WORD_WIDTH == 64)
	sw=(s+7)>>3;
#else
	sw=(s+3)>>2;
#endif
	NEED_HEAP(sw+2);
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=s;
	*++asp=(BC_WORD)hp;
	hp+=2;
	n=(BC_BOOL)bsp[1];
#if (WORD_WIDTH == 64)
	n = (n<<56) | (n<<48) | (n<<40) | (n<<32) | (n<<24) | (n<<16) | (n<<8) | n;
#else
	n = (n<<24) | (n<<16) | (n<<8) | n;
#endif
	bsp+=2;
	while (sw>3) {
		hp[0]=n;
		hp[1]=n;
		hp[2]=n;
		hp[3]=n;
		hp+=4;
		sw-=4;
	}
	hp+=sw;
	while (sw)
		hp[-(sw--)]=n;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_arrayINT):
{
	BC_WORD s,n;

	s=bsp[0];
	NEED_HEAP(s+3);
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&INT+2;
	*++asp=(BC_WORD)hp;
	hp+=3;
	n=bsp[1];
	bsp+=2;
	pc+=1;
	while (s>3) {
		hp[0]=n;
		hp[1]=n;
		hp[2]=n;
		hp[3]=n;
		hp+=4;
		s-=4;
	}
	hp+=s;
	while (s)
		hp[-(s--)]=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_arrayREAL):
{
	BC_WORD s,n;

	s=bsp[0];
	NEED_HEAP(s+3);
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&REAL+2;
	*++asp=(BC_WORD)hp;
	hp+=3;
	n=bsp[1];
	bsp+=2;
	pc+=1;
	while (s>3) {
		hp[0]=n;
		hp[1]=n;
		hp[2]=n;
		hp[3]=n;
		hp+=4;
		s-=4;
	}
	hp+=s;
	while (s)
		hp[-(s--)]=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array_):
{
	BC_WORD s;

	s=bsp[0];
	NEED_HEAP(s+3);
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=0;
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=1;
	BC_WORD n=((BC_WORD)&d___Nil[1])-IF_INT_64_OR_32(8,4);
	while (s>3) {
		hp[0]=n;
		hp[1]=n;
		hp[2]=n;
		hp[3]=n;
		hp+=4;
		s-=4;
	}
	hp+=s;
	while (s)
		hp[-(s--)]=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array_BOOL):
{
	BC_WORD s,sw;

	s=bsp[0];
	sw=(s+15)>>2;
	NEED_HEAP(sw);
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&BOOL+2;
	*++asp=(BC_WORD)hp;
	hp+=sw;
	bsp+=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array_CHAR):
{
	BC_WORD s,sw;

	s=bsp[0];
	sw=(s+11)>>2;
	NEED_HEAP(sw);
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=s;
	*++asp=(BC_WORD)hp;
	hp+=sw;
	bsp+=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array_INT):
{
	BC_WORD s;

	s=bsp[0];
	NEED_HEAP(s+3);
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=(BC_WORD)&INT+2;
	*++asp=(BC_WORD)hp;
	hp+=3+s;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array_r):
{
	BC_WORD s,i,n_ab,n_b,n_a;
	BC_WORD *array;

	s=bsp[0];
	n_ab=pc[1];
	NEED_HEAP(s*n_ab+3);
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
	BC_WORD *ao=asp;
	while (i<n_a-3){
		bsp[-1]=ao[0];
		bsp[-2]=ao[1];
		bsp[-3]=ao[2];
		bsp[-4]=ao[3];
		i+=4;
		ao+=4;
	}
	for (; i!=n_a; i++)
		*--bsp=asp[i];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array_r_):
{
	BC_WORD s,i,n_ab,n_a;

	s=bsp[0];
	n_ab=pc[1];
	NEED_HEAP(s*n_ab+3);
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
			hp[i] = ((BC_WORD)&d___Nil[1])-IF_INT_64_OR_32(8,4);
		hp+=n_ab;
		--s;
	}
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array_r_a):
{
	BC_WORD s,n_a,a_n_a;

	s=bsp[0];
	n_a=pc[1];
	a_n_a=s*n_a;
	NEED_HEAP(a_n_a+3);
	++bsp;
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=*(BC_WORD*)&pc[2];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=3;
	BC_WORD n=((BC_WORD)&d___Nil[1])-IF_INT_64_OR_32(8,4);
	while (a_n_a>3) {
		hp[0]=n;
		hp[1]=n;
		hp[2]=n;
		hp[3]=n;
		hp+=4;
		a_n_a-=4;
	}
	hp+=a_n_a;
	while (a_n_a)
		hp[-(a_n_a--)]=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(create_array_r_b):
{
	BC_WORD s,sw;

	s=bsp[0];
	sw=pc[1]*s+3;
	NEED_HEAP(sw);
	hp[0]=(BC_WORD)&__ARRAY__+2;
	hp[1]=s;
	hp[2]=*(BC_WORD*)&pc[2];
	++bsp;
	*++asp=(BC_WORD)hp;
	hp+=sw;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(decI):
	bsp[0]=bsp[0] - 1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(divI):
	bsp[1]=bsp[0] / bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(divLU):
{
#if defined(WINDOWS) && WORD_WIDTH==64
	EPRINTF("divLU is not supported on 64-bit Windows\n");
#else
# if WORD_WIDTH==64
	__int128_t num=((__int128_t)bsp[0] << 64) + bsp[1];
# else
	int64_t num=((int64_t)bsp[0] << 32) + bsp[1];
# endif
	bsp[1]=num%bsp[2];
	bsp[2]=num/bsp[2];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
#endif
}
INSTRUCTION_BLOCK(divR):
{
	BC_REAL d=*(BC_REAL*)&bsp[0] / *(BC_REAL*)&bsp[1];
	bsp[1]=*(BC_WORD*)&d;
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(entierR):
	bsp[0]=floor(*(BC_REAL*)&bsp[0]);
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(eq_desc):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp = *n==pc[2];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eq_desc_b0):
{
	bsp[0] = bsp[0]==pc[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eq_nulldesc):
{
	BC_WORD *n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	int16_t arity=((int16_t*)(n[0]))[-1];
	*--bsp = *n==pc[2]+arity*IF_INT_64_OR_32(16,8);
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqAC):
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
		END_INSTRUCTION_BLOCK;
	n+=2;
	s+=2;
	for (;;){
		if (length>=IF_INT_64_OR_32(8,4)){
			if (*n!=*s)
				break;
			++n;
			++s;
			length-=IF_INT_64_OR_32(8,4);
			continue;
		}
#if WORD_WIDTH==64
		if (length>=4){
			if (*(uint32_t*)n!=*(uint32_t*)s)
				break;
			length-=4;
			n=(BC_WORD*)((BC_WORD)n+4);
			s=(BC_WORD*)((BC_WORD)s+4);
		}
#endif
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqAC_a):
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
		END_INSTRUCTION_BLOCK;
	n+=2;
	s+=1;
	for (;;){
		if (length>=IF_INT_64_OR_32(8,4)){
			if (*n!=*s)
				break;
			++n;
			++s;
			length-=IF_INT_64_OR_32(8,4);
			continue;
		}
#if WORD_WIDTH==64
		if (length>=4){
			if (*(uint32_t*)n!=*(uint32_t*)s)
				break;
			length-=4;
			n=(BC_WORD*)((BC_WORD)n+4);
			s=(BC_WORD*)((BC_WORD)s+4);
		}
#endif
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqB):
INSTRUCTION_BLOCK(eqC):
INSTRUCTION_BLOCK(eqI):
	bsp[1]=bsp[0] == bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(eqR):
	bsp[1]=*(BC_REAL*)&bsp[0] == *(BC_REAL*)&bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(eqB_aFALSE):
{
	BC_WORD_S ao;
	BC_WORD *n;

	ao=((BC_WORD_S*)pc)[1];
	n=(BC_WORD*)asp[ao];
	*--bsp = n[1]==0;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqB_aTRUE):
{
	BC_WORD_S ao;
	BC_WORD *n;

	ao=((BC_WORD_S*)pc)[1];
	n=(BC_WORD*)asp[ao];
	*--bsp = n[1]!=0;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqB_bFALSE):
{
	int b;
	BC_WORD_S bo;

	bo=((BC_WORD_S*)pc)[1];
	b=bsp[bo]==0;
	*--bsp=b;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqB_bTRUE):
{
	int b;
	BC_WORD_S bo;

	bo=((BC_WORD_S*)pc)[1];
	b=bsp[bo]!=0;
	*--bsp=b;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqC_a):
INSTRUCTION_BLOCK(eqI_a):
{
	BC_WORD_S ao;
	BC_WORD *n;

	ao=((BC_WORD_S*)pc)[1];
	n=(BC_WORD*)asp[ao];
	*--bsp = n[1]==*(BC_WORD*)&pc[2];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqC_b):
INSTRUCTION_BLOCK(eqI_b):
{
	int b;
	BC_WORD_S bo;

	bo=((BC_WORD_S*)pc)[1];
	b=bsp[bo]==(BC_WORD_S)pc[2];
	*--bsp=b;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqR_b):
{
	int b;
	BC_WORD_S bo;

	bo=((BC_WORD_S*)pc)[1];
	b=*(BC_REAL*)&bsp[bo]==*(BC_REAL*)&pc[2];
	*--bsp=b;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eqD_b):
{
	int b;

	b=*bsp==*(BC_WORD_S*)&pc[1];
	*--bsp=b;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(expR):
{
	BC_REAL d=exp(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill3):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=*(BC_WORD_S*)&pc[2];
	n[1]=asp[0];
	n[2]=asp[-1];
	n[3]=asp[-2];
	pc+=3;
	asp-=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill4):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillh):
{
	BC_WORD *n,s_m_1;

	s_m_1=pc[1]; /* >=3 */
	NEED_HEAP(s_m_1);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill0):
INSTRUCTION_BLOCK(fillh0):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1):
INSTRUCTION_BLOCK(fillh1):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[1]=asp[0];
	--asp;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2):
INSTRUCTION_BLOCK(fillh2):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[1]=asp[0];
	n[2]=asp[-1];
	asp-=2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r0101):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[1]=*bsp++;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r0111):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[1]=*bsp++;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r02001):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[2]=*bsp++;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r02010):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[1]=*bsp++;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r02011):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[1]=bsp[0];
	n[2]=bsp[1];
	bsp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r02101):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[2]=*bsp++;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r02110):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[1]=*bsp++;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r02111):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[1]=bsp[0];
	n[2]=bsp[1];
	bsp+=2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r11001):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[2]=*bsp++;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r11011):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[1]=*asp--;
	n[2]=*bsp++;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r11101):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[2]=*bsp++;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r11111):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[1]=*asp--;
	n[2]=*bsp++;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1_r20111):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[1]=asp[0];
	n[2]=asp[-1];
	asp-=2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1010):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[1]=*asp--;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1001):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[2]=*asp--;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1011):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[1]=asp[0];
	n[2]=asp[-1];
	asp-=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill1101):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	n[2]=asp[0];
	--asp;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2a001):
{
	BC_WORD *n,*a;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	a=(BC_WORD*)n[2];
	a[ao]=*asp--;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2a011):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2a002):
{
	BC_WORD *n,*a;
	BC_WORD_S ao1,ao2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao1=((BC_WORD_S*)pc)[2];
	ao2=pc[3];
	a=(BC_WORD*)n[2];
	a[ao1]=asp[0];
	a[ao2]=asp[-1];
	asp-=2;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2a012):
{
	BC_WORD *n,*a;
	BC_WORD_S ao1,ao2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao1=((BC_WORD_S*)pc)[2];
	ao2=pc[3];
	a=(BC_WORD*)n[2];
	n[1]=asp[0];
	a[ao1]=asp[-1];
	a[ao2]=asp[-2];
	asp-=3;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2ab011):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2ab002):
{
	BC_WORD *n,*a;
	BC_WORD_S a_o1,a_o2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o1=((BC_WORD_S*)pc)[2];
	a_o2=pc[3];
	a=(BC_WORD*)n[2];
	a[a_o1]=asp[0];
	a[a_o2]=bsp[0];
	asp-=1;
	bsp+=1;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2ab003):
{
	BC_WORD *n,*a;
	BC_WORD_S a_o1,a_o2,a_o3;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o1=((BC_WORD_S*)pc)[2];
	a_o2=pc[3];
	a_o3=pc[4];
	a=(BC_WORD*)n[2];
	a[a_o1]=asp[0];
	a[a_o2]=asp[-1];
	a[a_o3]=bsp[0];
	asp-=2;
	bsp+=1;
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2b001):
{
	BC_WORD *n,*a;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	a=(BC_WORD*)n[2];
	a[ao]=*bsp++;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2b002):
{
	BC_WORD *n,*a;
	BC_WORD_S ao1,ao2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao1=((BC_WORD_S*)pc)[2];
	ao2=pc[3];
	a=(BC_WORD*)n[2];
	a[ao1]=bsp[0];
	a[ao2]=bsp[1];
	bsp+=2;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2b011):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2b012):
{
	BC_WORD *n,*a;
	BC_WORD_S ao1,ao2;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao1=((BC_WORD_S*)pc)[2];
	ao2=pc[3];
	a=(BC_WORD*)n[2];
	n[1]=bsp[0];
	a[ao1]=bsp[1];
	a[ao2]=bsp[2];
	bsp+=3;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2ab013):
{
	BC_WORD *n,*a;
	BC_WORD_S a_o1,a_o2,a_o3;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o1=((BC_WORD_S*)pc)[2];
	a_o2=pc[3];
	a_o3=pc[4];
	a=(BC_WORD*)n[2];
	n[1]=asp[0];
	a[a_o1]=asp[-1];
	a[a_o2]=asp[-2];
	a[a_o3]=bsp[0];
	asp-=3;
	bsp+=1;
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2_r00):
{
	BC_WORD *n,*args;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	args=(BC_WORD*)n[2];
	int a_size_minus_one=pc[2];
	BC_WORD bits=pc[3];
	int i=0;
	for (; bits>0; bits>>=1) {
		if (bits&1) {
			if (i>=a_size_minus_one)
				args[i]=*bsp++;
			else
				args[i]=*asp--;
		}
		i++;
	}
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2_r01):
{
	BC_WORD *n,*args;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	args=(BC_WORD*)n[2];
	int a_size=pc[2];
	BC_WORD bits=pc[3];
	if (a_size==0)
		n[1]=*bsp++;
	else
		n[1]=*asp--;
	a_size--;
	int i=0;
	for (; bits>0; bits>>=1) {
		if (bits&1) {
			if (i>=a_size)
				args[i]=*bsp++;
			else
				args[i]=*asp--;
		}
		i++;
	}
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2_r10):
{
	BC_WORD *n,*args;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	args=(BC_WORD*)n[2];
	int a_size_minus_one=pc[3];
	BC_WORD bits=pc[4];
	int i=0;
	for (; bits>0; bits>>=1) {
		if (bits&1) {
			if (i>=a_size_minus_one)
				args[i]=*bsp++;
			else
				args[i]=*asp--;
		}
		i++;
	}
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill2_r11):
{
	BC_WORD *n,*args;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=pc[2];
	args=(BC_WORD*)n[2];
	int a_size=pc[3];
	BC_WORD bits=pc[4];
	if (a_size==0)
		n[1]=*bsp++;
	else
		n[1]=*asp--;
	a_size--;
	int i=0;
	for (; bits>0; bits>>=1) {
		if (bits&1) {
			if (i>=a_size)
				args[i]=*bsp++;
			else
				args[i]=*asp--;
		}
		i++;
	}
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill3a10):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill3a11):
{
	BC_WORD *ns,*nd,*a;
	BC_WORD_S ao,a_o1;

	ao=((BC_WORD_S*)pc)[1];
	ns=(BC_WORD*)asp[0];
	nd=(BC_WORD*)asp[ao];
	a_o1=pc[3];
	a=(BC_WORD*)ns[2];
	ns[0]=(BC_WORD)&__cycle__in__spine;
	nd[0]=*(BC_WORD_S*)&pc[2];
	pc+=4;
	nd[1]=asp[-1];
	nd[2]=(BC_WORD)a;
	a[a_o1]=asp[-2];
	asp-=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill3a12):
{
	BC_WORD *ns,*nd,*a;
	BC_WORD_S ao,a_o1,a_o2;

	ao=((BC_WORD_S*)pc)[1];
	ns=(BC_WORD*)asp[0];
	nd=(BC_WORD*)asp[ao];
	a_o1=pc[3];
	a_o2=pc[4];
	a=(BC_WORD*)ns[2];
	ns[0]=(BC_WORD)&__cycle__in__spine;
	nd[0]=*(BC_WORD_S*)&pc[2];
	pc+=5;
	nd[1]=asp[-1];
	nd[2]=(BC_WORD)a;
	a[a_o1]=asp[-2];
	a[a_o2]=asp[-3];
	asp-=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill3aaab13):
{
	BC_WORD *ns,*nd,*a;
	BC_WORD_S ao,a_o1,a_o2,b_o1;

	ao=((BC_WORD_S*)pc)[1];
	ns=(BC_WORD*)asp[0];
	nd=(BC_WORD*)asp[ao];
	a_o1=pc[3];
	a_o2=pc[4];
	b_o1=pc[5];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillcaf):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillh3):
{
	BC_WORD *n;

	NEED_HEAP(2);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	n[0]=*(BC_WORD_S*)&pc[2];
	n[1]=asp[0];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[-1];
	hp[1]=asp[-2];
	asp-=3;
	hp+=2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillh4):
{
	BC_WORD *n;

	NEED_HEAP(3);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillB_b):
{
	BC_WORD *n,i;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	i=(BC_WORD)bsp[((BC_WORD_S*)pc)[2]];
	n[0]=(BC_WORD)&BOOL+2;
	n[1]=i;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillC_b):
{
	BC_WORD *n,i;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	i=(BC_WORD)bsp[((BC_WORD_S*)pc)[2]];
	n[0]=(BC_WORD)&CHAR+2;
	n[1]=i;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillF_b):
{
	BC_WORD *n;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=((BC_WORD_S*)pc)[2];
	n[0]=(BC_WORD)&dFILE+2;
	n[1]=(BC_WORD)bsp[bo];
	n[2]=(BC_WORD)bsp[bo+1];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillI):
{
	BC_WORD *n,i;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	i=(BC_WORD_S)pc[2];
	n[0]=(BC_WORD)&INT+2;
	n[1]=i;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillI_b):
{
	BC_WORD *n,i;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	i=(BC_WORD_S)bsp[pc[2]];
	n[0]=(BC_WORD)&INT+2;
	n[1]=i;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fillR_b):
{
	BC_WORD *n,i;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	i=(BC_WORD_S)bsp[pc[2]];
	n[0]=(BC_WORD)&REAL+2;
	n[1]=i;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_a):
{
	BC_WORD ao_s,ao_d;
	BC_WORD *n_s,*n_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	n_s=(BC_WORD*)asp[ao_s];
	n_d=(BC_WORD*)asp[ao_d];
	n_d[0]=n_s[0];
	n_d[1]=n_s[1];
	n_d[2]=n_s[2];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_a01_pop_rtn):
{
	BC_WORD *n_s,*n_d;

	n_s=(BC_WORD*)asp[0];
	n_d=(BC_WORD*)asp[-1];
	asp-=1;
	pc=(BC_WORD*)*csp++;
	n_d[0]=n_s[0];
	n_d[1]=n_s[1];
	n_d[2]=n_s[2];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r):
{
	BC_WORD_S ao,bo;
	BC_WORD *n,n_a,n_b,n_ab,*ao_p,*bo_p;

	n_ab=pc[1];
	NEED_HEAP(n_ab-1);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	ao=pc[3];
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
	bo=pc[6];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_ra0):
{
	BC_WORD_S ao;
	BC_WORD *n,n_a,*ao_p;

	n_a=pc[1];
	NEED_HEAP(n_a-1);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	ao=pc[3];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_ra1):
{
	BC_WORD_S ao,bo;
	BC_WORD *n,n_a,*ao_p;

	n_a=pc[1];
	NEED_HEAP(n_a);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	ao=pc[3];
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
	bo=pc[5];
	pc+=6;
	hp[n_a-1]=bsp[bo];
	hp+=n_a;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r0b):
{
	BC_WORD_S bo;
	BC_WORD *n,n_b,*bo_p;

	n_b=pc[1];
	NEED_HEAP(n_b-1);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	bo=pc[4];
	bo_p=&bsp[bo];
	n[0]=*(BC_WORD*)&pc[3];
	n[1]=bo_p[0];
	n[2]=(BC_WORD)hp;
	pc+=5;
	hp[0]=bo_p[1];
	hp[1]=bo_p[2];
	hp[2]=bo_p[3];
	hp[3]=bo_p[4];
	do {
		if (n_b< 6) break; hp[ 4]=bo_p[ 5];
		if (n_b< 7) break; hp[ 5]=bo_p[ 6];
		if (n_b< 8) break; hp[ 6]=bo_p[ 7];
		if (n_b< 9) break; hp[ 7]=bo_p[ 8];
		if (n_b<10) break; hp[ 8]=bo_p[ 9];
		if (n_b<11) break; hp[ 9]=bo_p[10];
		if (n_b<12) break; hp[10]=bo_p[11];
		if (n_b<13) break; hp[11]=bo_p[12];
		if (n_b<14) break; hp[12]=bo_p[13];
		if (n_b<15) break; hp[13]=bo_p[14];
		if (n_b<16) break; hp[14]=bo_p[15];
		if (n_b<17) break; hp[15]=bo_p[16];
		if (n_b<18) break; hp[16]=bo_p[17];
		if (n_b<19) break; hp[17]=bo_p[18];
		if (n_b<20) break; hp[18]=bo_p[19];
		if (n_b<21) break; hp[19]=bo_p[20];
		if (n_b<22) break; hp[20]=bo_p[21];
		if (n_b<23) break; hp[21]=bo_p[22];
		if (n_b<24) break; hp[22]=bo_p[23];
		if (n_b<25) break; hp[23]=bo_p[24];
		if (n_b<26) break; hp[24]=bo_p[25];
		if (n_b<27) break; hp[25]=bo_p[26];
		if (n_b<28) break; hp[26]=bo_p[27];
		if (n_b<29) break; hp[27]=bo_p[28];
		if (n_b<30) break; hp[28]=bo_p[29];
		if (n_b<31) break; hp[29]=bo_p[30];
	} while (0);
	hp+=n_b-1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r1b):
{
	BC_WORD_S ao,bo;
	BC_WORD *n,n_b,*bo_p;

	n_b=pc[1];
	NEED_HEAP(n_b);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	ao=pc[3];
	n[0]=*(BC_WORD*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	bo=pc[5];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r01):
{
	BC_WORD *n;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=bsp[bo];
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r10):
{
	BC_WORD *n;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[ao];
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r02):
{
	BC_WORD *n;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=bsp[bo];
	n[2]=bsp[bo+1];
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r03):
{
	BC_WORD *n;
	BC_WORD_S bo;

	NEED_HEAP(2);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=bsp[bo];
	n[2]=(BC_WORD)hp;
	hp[0]=bsp[bo+1];
	hp[1]=bsp[bo+2];
	hp+=2;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r11):
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((BC_WORD_S*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=bsp[bo];
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r12):
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	NEED_HEAP(2);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((BC_WORD_S*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=bsp[bo];
	hp[1]=bsp[bo+1];
	hp+=2;
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r13):
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	NEED_HEAP(3);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((BC_WORD_S*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=bsp[bo];
	hp[1]=bsp[bo+1];
	hp[2]=bsp[bo+2];
	hp+=3;
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r20):
{
	BC_WORD *n;
	BC_WORD_S ao;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[ao];
	n[2]=asp[ao-1];
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r21):
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	NEED_HEAP(2);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((BC_WORD_S*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=bsp[bo];
	hp+=2;
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r22):
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	NEED_HEAP(3);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((BC_WORD_S*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=bsp[bo];
	hp[2]=bsp[bo+1];
	hp+=3;
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r30):
{
	BC_WORD *n;
	BC_WORD_S ao;

	NEED_HEAP(2);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	n[0]=*(BC_WORD_S*)&pc[3];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=asp[ao-2];
	hp+=2;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r31):
{
	BC_WORD *n;
	BC_WORD_S ao,bo;

	NEED_HEAP(3);
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	ao=((BC_WORD_S*)pc)[2];
	bo=((BC_WORD_S*)pc)[3];
	n[0]=*(BC_WORD_S*)&pc[4];
	n[1]=asp[ao];
	n[2]=(BC_WORD)hp;
	hp[0]=asp[ao-1];
	hp[1]=asp[ao-2];
	hp[2]=bsp[bo];
	hp+=3;
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(fill_r40):
{
	BC_WORD *n;
	BC_WORD_S ao;

	NEED_HEAP(3);
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(get_node_arity):
{
	BC_WORD *n;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=((int16_t*)(n[0]))[-1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(gtI):
	bsp[1] = (BC_WORD_S)bsp[0] > (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(halt):
	PRINTF("halt at %d\n", (int) (pc-program->code));
	PRINTF("%d %d %d\n", (int) (heap_size-heap_free), (int) heap_free, (int) heap_size);
#ifdef DEBUG_CURSES
	debugger_graceful_end();
#endif
	return 0;
INSTRUCTION_BLOCK(incI):
	bsp[0]=bsp[0] + 1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(is_record):
{
	BC_WORD *n;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=((int16_t*)(n[0]))[-1] > 127;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp):
	pc=(BC_WORD*)pc[1];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_eval):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	if ((n[0] & 2)!=0){
		pc=(BC_WORD*)*csp++;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)n[0];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_eval_upd):
{
	BC_WORD *n1,*n2,d;
	n1=(BC_WORD*)asp[0];
	d=n1[0];
	if ((d & 2)!=0){
		n2=(BC_WORD*)asp[-1];
		pc=(BC_WORD*)*csp++;
		n2[0]=d;
		n2[1]=n1[1];
		n2[2]=n1[2];
		--asp;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)(d-IF_INT_64_OR_32(40,20));
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_false):
	if (*bsp++){
		pc+=2;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)pc[1];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_true):
	if (! *bsp++){
		pc+=2;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)pc[1];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jsr):
	*--csp=(BC_WORD)&pc[2];
	pc=(BC_WORD*)pc[1];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jsr_eval):
{
	BC_WORD *n;
	BC_WORD_S ao;

	ao=pc[1];
	n=(BC_WORD*)asp[(BC_WORD_S)ao];
	if ((n[0] & 2)!=0){
		pc+=4;
		END_INSTRUCTION_BLOCK;
	}
	*--csp=(BC_WORD)&pc[2];
	pc=(BC_WORD*)n[0];
	asp[(BC_WORD_S)ao]=asp[0];
	asp[0]=(BC_WORD)n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jsr_eval0):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	pc+=1;
	if ((n[0] & 2)!=0)
		END_INSTRUCTION_BLOCK;
	*--csp=(BC_WORD)pc;
	pc=(BC_WORD*)n[0];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jsr_eval1):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[-1];
	if ((n[0] & 2)!=0){
		pc+=2;
		END_INSTRUCTION_BLOCK;
	}
	*--csp=(BC_WORD)&pc[1];
	pc=(BC_WORD*)n[0];
	asp[-1]=asp[0];
	asp[0]=(BC_WORD)n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jsr_eval2):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[-2];
	if ((n[0] & 2)!=0){
		pc+=2;
		END_INSTRUCTION_BLOCK;
	}
	*--csp=(BC_WORD)&pc[1];
	pc=(BC_WORD*)n[0];
	asp[-2]=asp[0];
	asp[0]=(BC_WORD)n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jsr_eval3):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[-3];
	if ((n[0] & 2)!=0){
		pc+=2;
		END_INSTRUCTION_BLOCK;
	}
	*--csp=(BC_WORD)&pc[1];
	pc=(BC_WORD*)n[0];
	asp[-3]=asp[0];
	asp[0]=(BC_WORD)n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(lnR):
{
	BC_REAL d=log(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(log10R):
{
	BC_REAL d=log10(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(ltC):
INSTRUCTION_BLOCK(ltI):
	bsp[1] = (BC_WORD_S)bsp[0] < (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(ltR):
	bsp[1] = *(BC_REAL*)&bsp[0] < *(BC_REAL*)&bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(ltU):
	bsp[1]=bsp[0] < bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(mulI):
	bsp[1]=(BC_WORD_S)bsp[0] * (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(mulIo):
{
	BC_WORD_S x=bsp[0], y=bsp[1];
	bsp[1]=x*y;
	bsp[0]=x != 0 && bsp[1]/x != y;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(mulR):
{
	BC_REAL d=*(BC_REAL*)&bsp[0] * *(BC_REAL*)&bsp[1];
	bsp[1]=*(BC_WORD*)&d;
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(mulUUL):
{
#if defined(WINDOWS) && WORD_WIDTH==64
	EPRINTF("mulUUL is not supported on 64-bit Windows\n");
#else
# if WORD_WIDTH==64
	__uint128_t res=(__uint128_t)((__uint128_t)bsp[0] * (__uint128_t)bsp[1]);
# else
	uint64_t res=(uint64_t)bsp[0] * (uint64_t)bsp[1];
# endif
	bsp[0]=res>>WORD_WIDTH;
	bsp[1]=(BC_WORD)res;
	pc+=1;
	END_INSTRUCTION_BLOCK;
#endif
}
INSTRUCTION_BLOCK(negI):
	*bsp = - *bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(negR):
{
	BC_REAL d=-(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(notB):
	*bsp = *bsp==0;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(notI):
	*bsp = ~ *bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(orI):
	bsp[1]=bsp[0] | bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_a):
	asp = (BC_WORD*) (((uint8_t*)asp) + pc[1]);
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_b):
	bsp = (BC_WORD*) (((uint8_t*)bsp) + pc[1]);
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(powR):
{
	BC_REAL d=pow(*(BC_REAL*)&bsp[0], *(BC_REAL*)&bsp[1]);
	bsp[1]=*(BC_WORD*)&d;
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(print):
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
		PUTCHAR(*cs++);
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(printD):
{
	BC_WORD *s;
	BC_WORD l,i;
	char *cs;

	s=(BC_WORD*)*bsp++;
	l=s[0];
	if (l>256) { /* record; skip arity and type string */
		s+=2+s[1]/IF_INT_64_OR_32(8,4)+(s[1]%IF_INT_64_OR_32(8,4) ? 1 : 0);
		l=s[0];
	}
	cs=(char*)&s[1];
	for (i=0; i<l; ++i)
		PUTCHAR(*cs++);
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(print_symbol_sc):
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	d=n[0];
	if (d==(BC_WORD)&INT+2)
		PRINTF(BC_WORD_S_FMT,(BC_WORD_S)n[1]);
	else if (d==(BC_WORD)&BOOL+2)
	  PRINTF("%s",n[1] ? "True" : "False");
	else if (d==(BC_WORD)&CHAR+2)
	  PRINTF("'%c'",(int)n[1]);
	else if (d==(BC_WORD)&REAL+2)
	  PRINTF("%.15g", (*(BC_REAL*)&n[1]) + 0.0);
	else {
		uint32_t *s;
		int l,i;
		char *cs;

		int16_t arity = ((int16_t*)d)[-1];

		if (arity > 256) { /* record */
			int ts_len = ((BC_WORD*)(d-2))[1];
			int child_descriptors=0;
			char *type_string=(char*)(d+IF_INT_64_OR_32(14,6));
			for (int i=0; i<ts_len; i++)
				if (*type_string++=='{')
					child_descriptors++;
			ts_len = (ts_len+IF_INT_64_OR_32(7,3)) / IF_INT_64_OR_32(8,4);
			ts_len += child_descriptors;
			ts_len *= IF_INT_64_OR_32(8,4);
			s=(uint32_t*)(d+ts_len+IF_INT_64_OR_32(14,6));
		} else {
#if (WORD_WIDTH == 64)
			s=(uint32_t*)((BC_WORD)d+22+2*(*(uint16_t*)d));
#else
			s=(uint32_t*)((BC_WORD)d+10+*(uint16_t*)d);
#endif
		}
		l=s[0];
		cs=(char*)&s[IF_INT_64_OR_32(2,1)];
		for (i=0; i<l; ++i)
			PUTCHAR(*cs++);
	}
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(print_char):
	PRINTF("%c",(char)*bsp++);
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(print_int):
	PRINTF(BC_WORD_S_FMT,*bsp++);
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(print_real):
	PRINTF("%.15g",(*(BC_REAL*)bsp++)+0.0);
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pushA_a):
{
	BC_WORD *n;
	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=n[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(pushBFALSE):
	*--bsp=0;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pushBTRUE):
	*--bsp=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pushC):
INSTRUCTION_BLOCK(pushD):
INSTRUCTION_BLOCK(pushI):
INSTRUCTION_BLOCK(pushR):
	*--bsp=pc[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pushB_a):
INSTRUCTION_BLOCK(pushC_a):
INSTRUCTION_BLOCK(pushI_a):
INSTRUCTION_BLOCK(pushR_a):
INSTRUCTION_BLOCK(push_r_args_b0b11):
INSTRUCTION_BLOCK(push_r_args01):
INSTRUCTION_BLOCK(push_arraysize_a):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=n[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(pushB0_pop_a1):
INSTRUCTION_BLOCK(pushC0_pop_a1):
INSTRUCTION_BLOCK(pushI0_pop_a1):
{
	BC_WORD *n=(BC_WORD*)asp[0];
	asp--;
	*--bsp=n[1];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(pushD_a):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=*n;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(pushF_a):
INSTRUCTION_BLOCK(push_r_args02):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bsp[-2]=n[1];
	bsp[-1]=n[2];
	bsp-=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(pushcaf):
{
	BC_WORD *n;
	int na,ntotal,i;
	na=pc[1]+1;
	ntotal=pc[2];

	n=(BC_WORD*)pc[3];
	for (i=0; i<na; i++)
		asp[na-i]=*n++;
	asp+=na;
	for (; i<ntotal; i++)
		bsp[na-ntotal+i]=*n++;
	bsp-=ntotal-na;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(pushcaf10):
	*++asp=*((BC_WORD*)pc[1]+1);
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pushcaf11):
{
	BC_WORD *n;

	n=(BC_WORD*)pc[1]+1;
	*++asp=n[0];
	*--bsp=n[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(pushcaf20):
{
	BC_WORD *n;

	n=(BC_WORD*)pc[1]+1;
	asp[2]=n[0];
	asp[1]=n[1];
	asp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(pushcaf31):
{
	BC_WORD *n;

	n=(BC_WORD*)pc[1]+1;
	asp[3]=n[0];
	asp[2]=n[1];
	asp[1]=n[2];
	*--bsp=n[3];
	asp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_a):
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	*++asp=v;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_arg):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=a[((BC_WORD_S*)pc)[2]];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_arg1):
INSTRUCTION_BLOCK(push_args1):
INSTRUCTION_BLOCK(push_array):
INSTRUCTION_BLOCK(push_r_args10):
INSTRUCTION_BLOCK(push_r_args_a1):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=n[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_arg2):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=n[2];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_arg2l):
INSTRUCTION_BLOCK(push_r_args_a2l):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=a[0];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_arg3):
INSTRUCTION_BLOCK(push_r_args_a3):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=a[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_arg4):
INSTRUCTION_BLOCK(push_r_args_a4):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=a[2];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_args):
INSTRUCTION_BLOCK(push_args_u): /* no instruction reordering in the interpreter */
INSTRUCTION_BLOCK(push_r_argsa0):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_args2):
INSTRUCTION_BLOCK(push_r_args20):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	asp[2]=n[1];
	asp[1]=n[2];
	asp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_args3):
INSTRUCTION_BLOCK(push_r_args30):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	asp[3]=n[1];
	asp[2]=a[0];
	asp[1]=a[1];
	asp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_args4):
INSTRUCTION_BLOCK(push_r_args40):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_arg_b):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_a_b):
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	*--bsp=v;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_b):
{
	BC_WORD v;

	v=bsp[pc[1]];
	*--bsp=v;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_):
{
	BC_WORD *n,n_a;

	n=(BC_WORD*)*asp;
	n_a=pc[1];

	asp+=n_a;

	asp[0]=n[1];
	pc+=2;
	do {
		if (n_a< 2) break; asp[ -1]=n[ 2];
		if (n_a< 3) break; asp[ -2]=n[ 3];
		if (n_a< 4) break; asp[ -3]=n[ 4];
		if (n_a< 5) break; asp[ -4]=n[ 5];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node0):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node1):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	*++asp=n[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node2):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	asp[2]=n[1];
	asp[1]=n[2];
	asp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node3):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	asp[3]=n[1];
	asp[2]=n[2];
	asp[1]=n[3];
	asp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node4):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_ua1):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u01):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	*--bsp=n[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u02):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	bsp[-2]=n[1];
	bsp[-1]=n[2];
	bsp-=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u03):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=pc[1];
	bsp[-3]=n[1];
	bsp[-2]=n[2];
	bsp[-1]=n[3];
	bsp-=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u0b):
{
	BC_WORD *n;
	int n_b=pc[1];

	n=(BC_WORD*)*asp;
	n[0]=pc[2];
	bsp-=n_b;
	bsp[0]=n[1];
	bsp[1]=n[2];
	bsp[2]=n[3];
	bsp[3]=n[4];
	do {
		if (n_b< 5) break; bsp[ 4]=n[ 5];
		if (n_b< 6) break; bsp[ 5]=n[ 6];
		if (n_b< 7) break; bsp[ 6]=n[ 7];
		if (n_b< 8) break; bsp[ 7]=n[ 8];
		if (n_b< 9) break; bsp[ 8]=n[ 9];
		if (n_b<10) break; bsp[ 9]=n[10];
		if (n_b<11) break; bsp[10]=n[11];
		if (n_b<12) break; bsp[11]=n[12];
		if (n_b<13) break; bsp[12]=n[13];
		if (n_b<14) break; bsp[13]=n[14];
		if (n_b<15) break; bsp[14]=n[15];
		if (n_b<16) break; bsp[15]=n[16];
		if (n_b<17) break; bsp[16]=n[17];
		if (n_b<18) break; bsp[17]=n[18];
		if (n_b<19) break; bsp[18]=n[19];
		if (n_b<20) break; bsp[19]=n[20];
		if (n_b<21) break; bsp[20]=n[21];
		if (n_b<22) break; bsp[21]=n[22];
		if (n_b<23) break; bsp[22]=n[23];
		if (n_b<24) break; bsp[23]=n[24];
		if (n_b<25) break; bsp[24]=n[25];
		if (n_b<26) break; bsp[25]=n[26];
		if (n_b<27) break; bsp[26]=n[27];
		if (n_b<28) break; bsp[27]=n[28];
		if (n_b<29) break; bsp[28]=n[29];
		if (n_b<30) break; bsp[29]=n[30];
		if (n_b<31) break; bsp[30]=n[31];
		if (n_b<32) break; bsp[31]=n[32];
	} while (0);
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u11):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	*++asp=n[1];
	*--bsp=n[2];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u12):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	*++asp=n[1];
	bsp[-2]=n[2];
	bsp[-1]=n[3];
	bsp-=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u13):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u1b):
{
	BC_WORD *n;
	int n_b=pc[1];

	n=(BC_WORD*)*asp;
	n[0]=pc[2];
	*++asp=n[1];
	bsp-=n_b;
	bsp[0]=n[2];
	bsp[1]=n[3];
	bsp[2]=n[4];
	bsp[3]=n[5];
	do {
		if (n_b< 5) break; bsp[ 4]=n[ 6];
		if (n_b< 6) break; bsp[ 5]=n[ 7];
		if (n_b< 7) break; bsp[ 6]=n[ 8];
		if (n_b< 8) break; bsp[ 7]=n[ 9];
		if (n_b< 9) break; bsp[ 8]=n[10];
		if (n_b<10) break; bsp[ 9]=n[11];
		if (n_b<11) break; bsp[10]=n[12];
		if (n_b<12) break; bsp[11]=n[13];
		if (n_b<13) break; bsp[12]=n[14];
		if (n_b<14) break; bsp[13]=n[15];
		if (n_b<15) break; bsp[14]=n[16];
		if (n_b<16) break; bsp[15]=n[17];
		if (n_b<17) break; bsp[16]=n[18];
		if (n_b<18) break; bsp[17]=n[19];
		if (n_b<19) break; bsp[18]=n[20];
		if (n_b<20) break; bsp[19]=n[21];
		if (n_b<21) break; bsp[20]=n[22];
		if (n_b<22) break; bsp[21]=n[23];
		if (n_b<23) break; bsp[22]=n[24];
		if (n_b<24) break; bsp[23]=n[25];
		if (n_b<25) break; bsp[24]=n[26];
		if (n_b<26) break; bsp[25]=n[27];
		if (n_b<27) break; bsp[26]=n[28];
		if (n_b<28) break; bsp[27]=n[29];
		if (n_b<29) break; bsp[28]=n[30];
		if (n_b<30) break; bsp[29]=n[31];
		if (n_b<31) break; bsp[30]=n[32];
		if (n_b<32) break; bsp[31]=n[33];
	} while (0);
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u22):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u21):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp;
	n[0]=*(BC_WORD*)&pc[1];
	asp[2]=n[1];
	asp[1]=n[2];
	*--bsp=n[3];
	asp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_node_u31):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_argsa1):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args0b):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args1b):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args03):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	bsp[-3]=n[1];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args04):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args11):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=n[1];
	*--bsp=n[2];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args12):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*++asp=n[1];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args13):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args21):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args22):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args31):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_aa1):
{
	BC_WORD_S a_o;
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a_o=pc[2];
	a=(BC_WORD*)n[2];
	*++asp=*(BC_WORD*)((BC_WORD_S)a+a_o);
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_a):
{
	BC_WORD *n;
	int size,arg_no,nr_args;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	size=pc[2];
	arg_no=pc[3];
	nr_args=pc[4];

	if (size < 3) { /* must be two A elements */
		asp[1]=n[2];
		asp[2]=n[1];
	} else {
		if (arg_no==1) {
			asp[nr_args]=n[1];
			asp++;
			nr_args--;
			arg_no--;
		} else {
			arg_no-=2;
		}
		n=(BC_WORD*)n[2];
		for (int i=arg_no; i<arg_no+nr_args; i++)
			asp[nr_args-arg_no+i+1]=n[i];
	}
	asp+=nr_args;
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b):
{
	BC_WORD *n,*a;
	BC_WORD bo=pc[2];
	int n_args=pc[3];

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	BC_WORD *ao=&a[bo-3];
	bsp-=n_args;
	bsp[0]=ao[0];
	do {
		if (n_args< 2) break; bsp[ 1]=ao[ 1];
		if (n_args< 3) break; bsp[ 2]=ao[ 2];
		if (n_args< 4) break; bsp[ 3]=ao[ 3];
		if (n_args< 5) break; bsp[ 4]=ao[ 4];
		if (n_args< 6) break; bsp[ 5]=ao[ 5];
		if (n_args< 7) break; bsp[ 6]=ao[ 6];
		if (n_args< 8) break; bsp[ 7]=ao[ 7];
		if (n_args< 9) break; bsp[ 8]=ao[ 8];
		if (n_args<10) break; bsp[ 9]=ao[ 9];
		if (n_args<11) break; bsp[10]=ao[10];
		if (n_args<12) break; bsp[11]=ao[11];
		if (n_args<13) break; bsp[12]=ao[12];
		if (n_args<14) break; bsp[13]=ao[13];
		if (n_args<15) break; bsp[14]=ao[14];
		if (n_args<16) break; bsp[15]=ao[15];
		if (n_args<17) break; bsp[16]=ao[16];
		if (n_args<18) break; bsp[17]=ao[17];
		if (n_args<19) break; bsp[18]=ao[18];
		if (n_args<20) break; bsp[19]=ao[19];
		if (n_args<21) break; bsp[20]=ao[20];
		if (n_args<22) break; bsp[21]=ao[21];
		if (n_args<23) break; bsp[22]=ao[22];
		if (n_args<24) break; bsp[23]=ao[23];
		if (n_args<25) break; bsp[24]=ao[24];
		if (n_args<26) break; bsp[25]=ao[25];
		if (n_args<27) break; bsp[26]=ao[26];
		if (n_args<28) break; bsp[27]=ao[27];
		if (n_args<29) break; bsp[28]=ao[28];
		if (n_args<30) break; bsp[29]=ao[29];
		if (n_args<31) break; bsp[30]=ao[30];
		if (n_args<32) break; bsp[31]=ao[31];
		if (n_args<33) break; bsp[32]=ao[32];
	} while (0);
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b2l1):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*--bsp=a[0];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b1l2):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	bsp[-2]=n[1];
	bsp[-1]=a[0];
	bsp-=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b22):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b31):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*--bsp=a[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b41):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	a=(BC_WORD*)n[2];
	*--bsp=a[2];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(remI):
	bsp[1]=(BC_WORD_S)bsp[0] % (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(replace):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	asp[0]=array[3+i];
	array[3+i]=asp[-1];
	asp[-1]=(BC_WORD)array;
	pc+=1;
	++bsp;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(replaceBOOL):
INSTRUCTION_BLOCK(replaceCHAR):
{
	BC_WORD *array,i,v;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	v=((BC_BOOL*)array)[8+i];
	((BC_BOOL*)array)[8+i]=bsp[1];
	bsp[1]=v;
	pc+=1;
	++bsp;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(replaceINT):
INSTRUCTION_BLOCK(replaceREAL):
{
	BC_WORD *array,i,v;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	v=array[3+i];
	array[3+i]=bsp[1];
	bsp[1]=v;
	pc+=1;
	++bsp;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(replace_r):
{
	BC_WORD *array,array_o,*a,*b;
	BC_WORD_S n_a,n_b;

	n_a=pc[1];
	n_b=pc[2];
	array_o = ((n_a+n_b) * (BC_WORD_S)*bsp++) + 3;
	array = (BC_WORD*)asp[0];
	pc+=3;
	a=&array[array_o];
	b=a+n_a;

	do {
		if (n_a< 1) break; asp[  0]=a[ 0]; a[ 0]=asp[-1];
		if (n_a< 2) break; asp[ -1]=a[ 1]; a[ 1]=asp[ 0];
		if (n_a< 3) break; asp[ -2]=a[ 2]; a[ 2]=asp[ 1];
		if (n_a< 4) break; asp[ -3]=a[ 3]; a[ 3]=asp[ 2];
		if (n_a< 5) break; asp[ -4]=a[ 4]; a[ 4]=asp[ 3];
		if (n_a< 6) break; asp[ -5]=a[ 5]; a[ 5]=asp[ 4];
		if (n_a< 7) break; asp[ -6]=a[ 6]; a[ 6]=asp[ 5];
		if (n_a< 8) break; asp[ -7]=a[ 7]; a[ 7]=asp[ 6];
		if (n_a< 9) break; asp[ -8]=a[ 8]; a[ 8]=asp[ 7];
		if (n_a<10) break; asp[ -9]=a[ 9]; a[ 9]=asp[ 8];
		if (n_a<11) break; asp[-10]=a[10]; a[10]=asp[ 9];
		if (n_a<12) break; asp[-11]=a[11]; a[11]=asp[10];
		if (n_a<13) break; asp[-12]=a[12]; a[12]=asp[11];
		if (n_a<14) break; asp[-13]=a[13]; a[13]=asp[12];
		if (n_a<15) break; asp[-14]=a[14]; a[14]=asp[13];
		if (n_a<16) break; asp[-15]=a[15]; a[15]=asp[14];
		if (n_a<17) break; asp[-16]=a[16]; a[16]=asp[15];
		if (n_a<18) break; asp[-17]=a[17]; a[17]=asp[16];
		if (n_a<19) break; asp[-18]=a[18]; a[18]=asp[17];
		if (n_a<20) break; asp[-19]=a[19]; a[19]=asp[18];
		if (n_a<21) break; asp[-20]=a[20]; a[20]=asp[19];
		if (n_a<22) break; asp[-21]=a[21]; a[21]=asp[20];
		if (n_a<23) break; asp[-22]=a[22]; a[22]=asp[21];
		if (n_a<24) break; asp[-23]=a[23]; a[23]=asp[22];
		if (n_a<25) break; asp[-24]=a[24]; a[24]=asp[23];
		if (n_a<26) break; asp[-25]=a[25]; a[25]=asp[24];
		if (n_a<27) break; asp[-26]=a[26]; a[26]=asp[25];
		if (n_a<28) break; asp[-27]=a[27]; a[27]=asp[26];
		if (n_a<29) break; asp[-28]=a[28]; a[28]=asp[27];
		if (n_a<30) break; asp[-29]=a[29]; a[29]=asp[28];
		if (n_a<31) break; asp[-30]=a[30]; a[30]=asp[29];
		if (n_a<32) break; asp[-31]=a[31]; a[31]=asp[30];
	} while (0);
	do {
		BC_WORD temp;
		if (n_b< 1) break; temp=b[ 0]; b[ 0]=bsp[ 0]; bsp[ 0]=temp;
		if (n_b< 2) break; temp=b[ 1]; b[ 1]=bsp[ 1]; bsp[ 1]=temp;
		if (n_b< 3) break; temp=b[ 2]; b[ 2]=bsp[ 2]; bsp[ 2]=temp;
		if (n_b< 4) break; temp=b[ 3]; b[ 3]=bsp[ 3]; bsp[ 3]=temp;
		if (n_b< 5) break; temp=b[ 4]; b[ 4]=bsp[ 4]; bsp[ 4]=temp;
		if (n_b< 6) break; temp=b[ 5]; b[ 5]=bsp[ 5]; bsp[ 5]=temp;
		if (n_b< 7) break; temp=b[ 6]; b[ 6]=bsp[ 6]; bsp[ 6]=temp;
		if (n_b< 8) break; temp=b[ 7]; b[ 7]=bsp[ 7]; bsp[ 7]=temp;
		if (n_b< 9) break; temp=b[ 8]; b[ 8]=bsp[ 8]; bsp[ 8]=temp;
		if (n_b<10) break; temp=b[ 9]; b[ 9]=bsp[ 9]; bsp[ 9]=temp;
		if (n_b<11) break; temp=b[10]; b[10]=bsp[10]; bsp[10]=temp;
		if (n_b<12) break; temp=b[11]; b[11]=bsp[11]; bsp[11]=temp;
		if (n_b<13) break; temp=b[12]; b[12]=bsp[12]; bsp[12]=temp;
		if (n_b<14) break; temp=b[13]; b[13]=bsp[13]; bsp[13]=temp;
		if (n_b<15) break; temp=b[14]; b[14]=bsp[14]; bsp[14]=temp;
		if (n_b<16) break; temp=b[15]; b[15]=bsp[15]; bsp[15]=temp;
		if (n_b<17) break; temp=b[16]; b[16]=bsp[16]; bsp[16]=temp;
		if (n_b<18) break; temp=b[17]; b[17]=bsp[17]; bsp[17]=temp;
		if (n_b<19) break; temp=b[18]; b[18]=bsp[18]; bsp[18]=temp;
		if (n_b<20) break; temp=b[19]; b[19]=bsp[19]; bsp[19]=temp;
		if (n_b<21) break; temp=b[20]; b[20]=bsp[20]; bsp[20]=temp;
		if (n_b<22) break; temp=b[21]; b[21]=bsp[21]; bsp[21]=temp;
		if (n_b<23) break; temp=b[22]; b[22]=bsp[22]; bsp[22]=temp;
		if (n_b<24) break; temp=b[23]; b[23]=bsp[23]; bsp[23]=temp;
		if (n_b<25) break; temp=b[24]; b[24]=bsp[24]; bsp[24]=temp;
		if (n_b<26) break; temp=b[25]; b[25]=bsp[25]; bsp[25]=temp;
		if (n_b<27) break; temp=b[26]; b[26]=bsp[26]; bsp[26]=temp;
		if (n_b<28) break; temp=b[27]; b[27]=bsp[27]; bsp[27]=temp;
		if (n_b<29) break; temp=b[28]; b[28]=bsp[28]; bsp[28]=temp;
		if (n_b<30) break; temp=b[29]; b[29]=bsp[29]; bsp[29]=temp;
		if (n_b<31) break; temp=b[30]; b[30]=bsp[30]; bsp[30]=temp;
		if (n_b<32) break; temp=b[31]; b[31]=bsp[31]; bsp[31]=temp;
	} while (0);
	asp[-n_a]=(BC_WORD)array;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_args):
INSTRUCTION_BLOCK(repl_r_argsa0):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_args_b):
{
	BC_WORD *n,*a,n_a_m_1;

	n=(BC_WORD*)asp[0];
	n_a_m_1=bsp[0];
	bsp+=2;
	asp+=n_a_m_1-1;
	do {
		asp[0]=n[1];
		if (n_a_m_1 < 2) break;
		if (n_a_m_1 == 2) {
			asp[-1]=n[2];
		} else {
			a=(BC_WORD*)n[2];
			asp[-1]=a[0];
			asp[-2]=a[1];
			do {
				if (n_a_m_1< 4) break; asp[ -3] = a[ 2];
				if (n_a_m_1< 5) break; asp[ -4] = a[ 3];
				if (n_a_m_1< 6) break; asp[ -5] = a[ 4];
				if (n_a_m_1< 7) break; asp[ -6] = a[ 5];
				if (n_a_m_1< 8) break; asp[ -7] = a[ 6];
				if (n_a_m_1< 9) break; asp[ -8] = a[ 7];
				if (n_a_m_1<10) break; asp[ -9] = a[ 8];
				if (n_a_m_1<11) break; asp[-10] = a[ 9];
				if (n_a_m_1<12) break; asp[-11] = a[10];
				if (n_a_m_1<13) break; asp[-12] = a[11];
				if (n_a_m_1<14) break; asp[-13] = a[12];
				if (n_a_m_1<15) break; asp[-14] = a[13];
				if (n_a_m_1<16) break; asp[-15] = a[14];
				if (n_a_m_1<17) break; asp[-16] = a[15];
				if (n_a_m_1<18) break; asp[-17] = a[16];
				if (n_a_m_1<19) break; asp[-18] = a[17];
				if (n_a_m_1<20) break; asp[-19] = a[18];
				if (n_a_m_1<21) break; asp[-20] = a[19];
				if (n_a_m_1<22) break; asp[-21] = a[20];
				if (n_a_m_1<23) break; asp[-22] = a[21];
				if (n_a_m_1<24) break; asp[-23] = a[22];
				if (n_a_m_1<25) break; asp[-24] = a[23];
				if (n_a_m_1<26) break; asp[-25] = a[24];
				if (n_a_m_1<27) break; asp[-26] = a[25];
				if (n_a_m_1<28) break; asp[-27] = a[26];
				if (n_a_m_1<29) break; asp[-28] = a[27];
				if (n_a_m_1<30) break; asp[-29] = a[28];
				if (n_a_m_1<31) break; asp[-30] = a[29];
				if (n_a_m_1<32) break; asp[-31] = a[30];
			} while (0);
		}
	} while (0);
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(swap_a1):
{
	BC_WORD d;

	d=asp[0];
	asp[0]=asp[-1];
	asp[-1]=d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(swap_a2):
{
	BC_WORD d;

	d=asp[0];
	asp[0]=asp[-2];
	asp[-2]=d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(swap_a3):
{
	BC_WORD d;

	d=asp[0];
	asp[0]=asp[-3];
	asp[-3]=d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(swap_a):
{
	BC_WORD d;
	BC_WORD_S ao;

	ao=pc[1];
	d=asp[0];
	asp[0]=asp[ao];
	asp[ao]=d;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(swap_b1):
{
	BC_WORD d;

	d=bsp[0];
	bsp[0]=bsp[1];
	bsp[1]=d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b0221):
INSTRUCTION_BLOCK(push_r_args_b1111):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*--bsp=n[2];
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b1):
{
	BC_WORD *n,*a;
	BC_WORD_S bo;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	bo=pc[2];
	a=(BC_WORD*)n[2];
	*--bsp=*(BC_WORD*)((BC_WORD_S)a+bo);
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_args_b2):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_args1):
INSTRUCTION_BLOCK(repl_r_args10):
INSTRUCTION_BLOCK(repl_r_args_aab11):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	asp[0]=n[1];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_args2):
INSTRUCTION_BLOCK(repl_r_args20):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	asp[1]=n[1];
	asp[0]=n[2];
	asp+=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_args3):
INSTRUCTION_BLOCK(repl_r_args30):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	asp[0]=a[1];
	asp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_args4):
INSTRUCTION_BLOCK(repl_r_args40):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_argsa1):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args0b):
{
	BC_WORD *n,*a,n_b;

	n=(BC_WORD*)*asp--;
	n_b=pc[1];
	a=(BC_WORD*)n[2];
	bsp-=n_b;
	pc+=2;
	bsp[0]=n[1];
	bsp[1]=a[0];
	bsp[2]=a[1];
	bsp[3]=a[2];
	do {
		if (n_b<= 4) break; bsp[ 4]=a[ 3];
		if (n_b<= 5) break; bsp[ 5]=a[ 4];
		if (n_b<= 6) break; bsp[ 6]=a[ 5];
		if (n_b<= 7) break; bsp[ 7]=a[ 6];
		if (n_b<= 8) break; bsp[ 8]=a[ 7];
		if (n_b<= 9) break; bsp[ 9]=a[ 8];
		if (n_b<=10) break; bsp[10]=a[ 9];
		if (n_b<=11) break; bsp[11]=a[10];
		if (n_b<=12) break; bsp[12]=a[11];
		if (n_b<=13) break; bsp[13]=a[12];
		if (n_b<=14) break; bsp[14]=a[13];
		if (n_b<=15) break; bsp[15]=a[14];
		if (n_b<=16) break; bsp[16]=a[15];
		if (n_b<=17) break; bsp[17]=a[16];
		if (n_b<=18) break; bsp[18]=a[17];
		if (n_b<=19) break; bsp[19]=a[18];
		if (n_b<=20) break; bsp[20]=a[19];
		if (n_b<=21) break; bsp[21]=a[20];
		if (n_b<=22) break; bsp[22]=a[21];
		if (n_b<=23) break; bsp[23]=a[22];
		if (n_b<=24) break; bsp[24]=a[23];
		if (n_b<=25) break; bsp[25]=a[24];
		if (n_b<=26) break; bsp[26]=a[25];
		if (n_b<=27) break; bsp[27]=a[26];
		if (n_b<=28) break; bsp[28]=a[27];
		if (n_b<=29) break; bsp[29]=a[28];
		if (n_b<=30) break; bsp[30]=a[29];
		if (n_b<=31) break; bsp[31]=a[30];
	} while (0);
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_arraysize):
INSTRUCTION_BLOCK(repl_r_args01):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp--;
	*--bsp=n[1];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args02):
{
	BC_WORD *n;

	n=(BC_WORD*)*asp--;
	bsp[-2]=n[1];
	bsp[-1]=n[2];
	bsp-=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args03):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)*asp--;
	a=(BC_WORD*)n[2];
	bsp[-3]=n[1];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=3;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args04):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args11):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	asp[0]=n[1];
	*--bsp=n[2];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args12):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=n[1];
	bsp[-2]=a[0];
	bsp[-1]=a[1];
	bsp-=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args13):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args14):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=n[1];
	bsp[-4]=a[0];
	bsp[-3]=a[1];
	bsp[-2]=a[2];
	bsp[-1]=a[3];
	bsp-=4;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args1b):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args21):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[1]=n[1];
	asp[0]=a[0];
	*--bsp=a[1];
	++asp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args22):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args23):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[1]=n[1];
	asp[0]=a[0];
	bsp[-3]=a[1];
	bsp[-2]=a[2];
	bsp[-1]=a[3];
	++asp;
	bsp-=3;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args24):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[1]=n[1];
	asp[0]=a[0];
	bsp[-4]=a[1];
	bsp[-3]=a[2];
	bsp[-2]=a[3];
	bsp[-1]=a[4];
	++asp;
	bsp-=4;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args2b):
{
	BC_WORD *n,*a,n_b;

	n=(BC_WORD*)asp[0];
	n_b=pc[1];
	a=(BC_WORD*)n[2];
	asp[1]=n[1];
	asp[0]=a[0];
	bsp-=n_b;
	pc+=2;
	bsp[0]=a[1];
	bsp[1]=a[2];
	bsp[2]=a[3];
	bsp[3]=a[4];
	do {
		if (n_b<= 4) break; bsp[ 4]=a[ 5];
		if (n_b<= 5) break; bsp[ 5]=a[ 6];
		if (n_b<= 6) break; bsp[ 6]=a[ 7];
		if (n_b<= 7) break; bsp[ 7]=a[ 8];
		if (n_b<= 8) break; bsp[ 8]=a[ 9];
		if (n_b<= 9) break; bsp[ 9]=a[10];
		if (n_b<=10) break; bsp[10]=a[11];
		if (n_b<=11) break; bsp[11]=a[12];
		if (n_b<=12) break; bsp[12]=a[13];
		if (n_b<=13) break; bsp[13]=a[14];
		if (n_b<=14) break; bsp[14]=a[15];
		if (n_b<=15) break; bsp[15]=a[16];
		if (n_b<=16) break; bsp[16]=a[17];
		if (n_b<=17) break; bsp[17]=a[18];
		if (n_b<=18) break; bsp[18]=a[19];
		if (n_b<=19) break; bsp[19]=a[20];
		if (n_b<=20) break; bsp[20]=a[21];
		if (n_b<=21) break; bsp[21]=a[22];
		if (n_b<=22) break; bsp[22]=a[23];
		if (n_b<=23) break; bsp[23]=a[24];
		if (n_b<=24) break; bsp[24]=a[25];
		if (n_b<=25) break; bsp[25]=a[26];
		if (n_b<=26) break; bsp[26]=a[27];
		if (n_b<=27) break; bsp[27]=a[28];
		if (n_b<=28) break; bsp[28]=a[29];
		if (n_b<=29) break; bsp[29]=a[30];
		if (n_b<=30) break; bsp[30]=a[31];
	} while (0);
	++asp;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args31):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args32):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	asp[0]=a[1];
	bsp[-2]=a[2];
	bsp[-1]=a[3];
	bsp-=2;
	asp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args33):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	asp[0]=a[1];
	bsp[-3]=a[2];
	bsp[-2]=a[3];
	bsp[-1]=a[4];
	bsp-=3;
	asp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args34):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	asp[0]=a[1];
	bsp[-4]=a[2];
	bsp[-3]=a[3];
	bsp[-2]=a[4];
	bsp[-1]=a[5];
	bsp-=4;
	asp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args3b):
{
	BC_WORD *n,*a,n_b;

	n=(BC_WORD*)asp[0];
	n_b=pc[1];
	a=(BC_WORD*)n[2];
	asp[2]=n[1];
	asp[1]=a[0];
	asp[0]=a[1];
	bsp-=n_b;
	pc+=2;
	bsp[0]=a[2];
	bsp[1]=a[3];
	bsp[2]=a[4];
	bsp[3]=a[5];
	do {
		if (n_b<= 4) break; bsp[ 4]=a[ 6];
		if (n_b<= 5) break; bsp[ 5]=a[ 7];
		if (n_b<= 6) break; bsp[ 6]=a[ 8];
		if (n_b<= 7) break; bsp[ 7]=a[ 9];
		if (n_b<= 8) break; bsp[ 8]=a[10];
		if (n_b<= 9) break; bsp[ 9]=a[11];
		if (n_b<=10) break; bsp[10]=a[12];
		if (n_b<=11) break; bsp[11]=a[13];
		if (n_b<=12) break; bsp[12]=a[14];
		if (n_b<=13) break; bsp[13]=a[15];
		if (n_b<=14) break; bsp[14]=a[16];
		if (n_b<=15) break; bsp[15]=a[17];
		if (n_b<=16) break; bsp[16]=a[18];
		if (n_b<=17) break; bsp[17]=a[19];
		if (n_b<=18) break; bsp[18]=a[20];
		if (n_b<=19) break; bsp[19]=a[21];
		if (n_b<=20) break; bsp[20]=a[22];
		if (n_b<=21) break; bsp[21]=a[23];
		if (n_b<=22) break; bsp[22]=a[24];
		if (n_b<=23) break; bsp[23]=a[25];
		if (n_b<=24) break; bsp[24]=a[26];
		if (n_b<=25) break; bsp[25]=a[27];
		if (n_b<=26) break; bsp[26]=a[28];
		if (n_b<=27) break; bsp[27]=a[29];
		if (n_b<=28) break; bsp[28]=a[30];
		if (n_b<=29) break; bsp[29]=a[31];
	} while (0);
	asp+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args_a2021):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	asp[0]=n[2];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args_a21):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=a[0];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args_a31):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=a[1];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args_a41):
{
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a=(BC_WORD*)n[2];
	asp[0]=a[2];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args_aa1):
{
	BC_WORD_S a_o;
	BC_WORD *n,*a;

	n=(BC_WORD*)asp[0];
	a_o=pc[1];
	a=(BC_WORD*)n[2];
	asp[0]=*(BC_WORD*)((BC_WORD_S)a+a_o);
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(repl_r_args_a):
{
	BC_WORD *n;
	int size,arg_no,nr_args;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	size=pc[2];
	arg_no=pc[3];
	nr_args=pc[4];

	if (size < 3) { /* must be two A elements */
		asp[0]=n[2];
		asp[1]=n[1];
	} else {
		if (arg_no==1) {
			nr_args--;
			asp[nr_args]=n[1];
			asp++;
			arg_no--;
		} else {
			arg_no-=2;
		}
		n=(BC_WORD*)n[2];
		for (int i=arg_no; i<arg_no+nr_args; i++)
			asp[nr_args-arg_no+i]=n[i];
	}
	asp+=nr_args-1;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	asp[0]=array[3+i];
	bsp+=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(selectBOOL):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	bsp[0]=((BC_BOOL*)&array[3])[i];
	--asp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(selectCHAR):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	bsp[0]=((BC_BOOL*)&array[2])[i];
	--asp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(selectINT):
INSTRUCTION_BLOCK(selectREAL):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	bsp[0]=array[3+i];
	--asp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r):
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
	do {
		if (n_b< 1) break; bsp[ 0]=b[ 0];
		if (n_b< 2) break; bsp[ 1]=b[ 1];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r01):
{
	BC_WORD array_o,*element_p;

	array_o = (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp--))[array_o];
	bsp[-1] = element_p[3];
	bsp-=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r02):
{
	BC_WORD array_o,*element_p;

	array_o = 2 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp--))[array_o];
	bsp[-2] = element_p[3];
	bsp[-1] = element_p[4];
	bsp-=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r03):
{
	BC_WORD array_o,*element_p;

	array_o = 3 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp--))[array_o];
	bsp[-3] = element_p[3];
	bsp[-2] = element_p[4];
	bsp[-1] = element_p[5];
	bsp-=3;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r04):
{
	BC_WORD array_o,*element_p;

	array_o = 4 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp--))[array_o];
	bsp[-4] = element_p[3];
	bsp[-3] = element_p[4];
	bsp[-2] = element_p[5];
	bsp[-1] = element_p[6];
	bsp-=4;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r0b):
{
	BC_WORD array_o,*b;
	BC_WORD_S n_b;

	n_b=pc[1];
	array_o = (n_b * (BC_WORD_S)*bsp++) + 3;
	b = &((BC_WORD*)(*asp--))[array_o];
	pc+=2;
	bsp-=n_b;
	bsp[0]=b[0];
	bsp[1]=b[1];
	bsp[2]=b[2];
	bsp[3]=b[3];
	bsp[4]=b[4];
	do {
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
		if (n_b<31) break; bsp[30]=b[30];
		if (n_b<32) break; bsp[31]=b[31];
		if (n_b<33) break; bsp[32]=b[32];
	} while (0);
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r10):
{
	BC_WORD array_o,*element_p;

	array_o = (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	*asp = element_p[3];
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r11):
{
	BC_WORD array_o,*element_p;

	array_o = 2 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	*asp = element_p[3];
	bsp[-1] = element_p[4];
	bsp-=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r12):
{
	BC_WORD array_o,*element_p;

	array_o = 3 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	*asp = element_p[3];
	bsp[-2] = element_p[4];
	bsp[-1] = element_p[5];
	bsp-=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r13):
{
	BC_WORD array_o,*element_p;

	array_o = 4 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	*asp = element_p[3];
	bsp[-3] = element_p[4];
	bsp[-2] = element_p[5];
	bsp[-1] = element_p[6];
	bsp-=3;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r14):
{
	BC_WORD array_o,*element_p;

	array_o = 5 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	*asp = element_p[3];
	bsp[-4] = element_p[4];
	bsp[-3] = element_p[5];
	bsp[-2] = element_p[6];
	bsp[-1] = element_p[7];
	bsp-=4;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r1b):
{
	BC_WORD array_o,*a,*b;
	BC_WORD_S n_b;

	n_b=pc[1];
	array_o = ((n_b+1) * (BC_WORD_S)*bsp++) + 3;
	a=&((BC_WORD*)(*asp--))[array_o];
	*++asp=a[0];
	b=a+1;
	pc+=2;
	bsp-=n_b;
	bsp[0]=b[0];
	bsp[1]=b[1];
	bsp[2]=b[2];
	bsp[3]=b[3];
	bsp[4]=b[4];
	do {
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
		if (n_b<31) break; bsp[30]=b[30];
		if (n_b<32) break; bsp[31]=b[31];
	} while (0);
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r20):
{
	BC_WORD array_o,*element_p;

	array_o = 2 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	asp[1] = element_p[3];
	asp[0] = element_p[4];
	asp+=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r21):
{
	BC_WORD array_o,*element_p;

	array_o = 3 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	asp[1] = element_p[3];
	asp[0] = element_p[4];
	*--bsp = element_p[5];
	asp+=1;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r22):
{
	BC_WORD array_o,*element_p;

	array_o = 4 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	asp[1] = element_p[3];
	asp[0] = element_p[4];
	asp+=1;
	bsp[-2] = element_p[5];
	bsp[-1] = element_p[6];
	bsp-=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r23):
{
	BC_WORD array_o,*element_p;

	array_o = 5 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	asp[1] = element_p[3];
	asp[0] = element_p[4];
	asp+=1;
	bsp[-3] = element_p[5];
	bsp[-2] = element_p[6];
	bsp[-1] = element_p[7];
	bsp-=3;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r24):
{
	BC_WORD array_o,*element_p;

	array_o = 6 * (BC_WORD_S)*bsp++;
	element_p = &((BC_WORD*)(*asp))[array_o];
	asp[1] = element_p[3];
	asp[0] = element_p[4];
	asp+=1;
	bsp[-4] = element_p[5];
	bsp[-3] = element_p[6];
	bsp[-2] = element_p[7];
	bsp[-1] = element_p[8];
	bsp-=4;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(select_r2b):
{
	BC_WORD array_o,*a,*b;
	BC_WORD_S n_b;

	n_b=pc[1];
	array_o = ((n_b+2) * (BC_WORD_S)*bsp++) + 3;
	a=&((BC_WORD*)asp[0])[array_o];
	asp[1]=a[0];
	asp[0]=a[1];
	asp+=1;
	b=a+2;
	pc+=2;
	bsp-=n_b;
	bsp[0]=b[0];
	bsp[1]=b[1];
	bsp[2]=b[2];
	bsp[3]=b[3];
	bsp[4]=b[4];
	do {
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
		if (n_b<31) break; bsp[30]=b[30];
	} while (0);
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(rtn):
	pc=(BC_WORD*)*csp++;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(shiftlI):
	bsp[1]=bsp[0] << bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(shiftrI):
	bsp[1]=(BC_WORD_S)bsp[0] >> (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(shiftrU):
	bsp[1]=(BC_WORD)bsp[0] >> (BC_WORD)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(sinR):
{
	BC_REAL d=sin(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(subI):
	bsp[1]=(BC_WORD_S)bsp[0]-(BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(subIo):
	bsp[1]=(BC_WORD_S)bsp[0]-(BC_WORD_S)bsp[1];
	bsp[0]=(BC_WORD_S)bsp[1]>(BC_WORD_S)bsp[0];
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(subLU):
	bsp[2]=(BC_WORD_S)bsp[1] - (BC_WORD_S)bsp[2];
	bsp[1]=bsp[0]+(bsp[2]>bsp[1] ? 1 : 0);
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(subR):
{
	BC_REAL d=*(BC_REAL*)&bsp[0] - *(BC_REAL*)&bsp[1];
	bsp[1]=*(BC_WORD*)&d;
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(sqrtR):
{
	BC_REAL d=sqrt(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(tanR):
{
	BC_REAL d=tan(*(BC_REAL*)&bsp[0]);
	bsp[0]=*(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(testcaf):
	*--bsp=*(BC_WORD*)pc[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(update):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	array[3+i]=asp[-1];
	asp[-1]=(BC_WORD)array;
	++bsp;
	--asp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updateBOOL):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	((BC_BOOL*)&array[3])[i]=bsp[1];
	bsp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updateCHAR):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	((BC_BOOL*)&array[2])[i]=bsp[1];
	bsp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updateINT):
INSTRUCTION_BLOCK(updateREAL):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[0];
	i=bsp[0];
	array[3+i]=bsp[1];
	bsp+=2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_a):
	asp[((BC_WORD_S*)pc)[2]] = asp[((BC_WORD_S*)pc)[1]];
	pc+=3;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(update_b):
	bsp[pc[2]] = bsp[pc[1]];
	pc+=3;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(update_r):
{
	BC_WORD *array,array_o,*a,*b;
	BC_WORD_S n_a,n_b;

	n_a=pc[1];
	n_b=pc[2];
	pc+=3;
	array_o = ((n_a+n_b) * (BC_WORD_S)*bsp++) + 3;
	array = (BC_WORD*)asp[0];
	a = &array[array_o];
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r01):
{
	BC_WORD *array,array_o,*element_p;

	array_o = (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=bsp[0];
	bsp+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r02):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 2 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=bsp[0];
	element_p[4]=bsp[1];
	bsp+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r03):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 3 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=bsp[0];
	element_p[4]=bsp[1];
	element_p[5]=bsp[2];
	bsp+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r04):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 4 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=bsp[0];
	element_p[4]=bsp[1];
	element_p[5]=bsp[2];
	element_p[6]=bsp[3];
	bsp+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r0b):
{
	BC_WORD *array,array_o,*element_p,n;

	n=pc[1];
	array_o = n * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=2;
	element_p=&array[array_o+3];
	element_p[0]=bsp[0];
	element_p[1]=bsp[1];
	element_p[2]=bsp[2];
	element_p[3]=bsp[3];
	element_p[4]=bsp[4];
	do {
		if (n< 6) break; element_p[ 5]=bsp[ 5];
		if (n< 7) break; element_p[ 6]=bsp[ 6];
		if (n< 8) break; element_p[ 7]=bsp[ 7];
		if (n< 9) break; element_p[ 8]=bsp[ 8];
		if (n<10) break; element_p[ 9]=bsp[ 9];
		if (n<11) break; element_p[10]=bsp[10];
		if (n<12) break; element_p[11]=bsp[11];
		if (n<13) break; element_p[12]=bsp[12];
		if (n<14) break; element_p[13]=bsp[13];
		if (n<15) break; element_p[14]=bsp[14];
		if (n<16) break; element_p[15]=bsp[15];
		if (n<17) break; element_p[16]=bsp[16];
		if (n<18) break; element_p[17]=bsp[17];
		if (n<19) break; element_p[18]=bsp[18];
		if (n<20) break; element_p[19]=bsp[19];
		if (n<21) break; element_p[20]=bsp[20];
		if (n<22) break; element_p[21]=bsp[21];
		if (n<23) break; element_p[22]=bsp[22];
		if (n<24) break; element_p[23]=bsp[23];
		if (n<25) break; element_p[24]=bsp[24];
		if (n<26) break; element_p[25]=bsp[25];
		if (n<27) break; element_p[26]=bsp[26];
		if (n<28) break; element_p[27]=bsp[27];
		if (n<29) break; element_p[28]=bsp[28];
		if (n<30) break; element_p[29]=bsp[29];
		if (n<31) break; element_p[30]=bsp[30];
		if (n<32) break; element_p[31]=bsp[31];
		if (n<33) break; element_p[32]=bsp[32];
	} while (0);
	bsp+=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r10):
{
	BC_WORD *array,array_o,*element_p;

	array_o = (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	asp-=1;
	asp[0]=(BC_WORD)array;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r11):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 2 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	asp-=1;
	asp[0]=(BC_WORD)array;
	element_p[4]=bsp[0];
	bsp+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r12):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r13):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 4 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	asp-=1;
	asp[0]=(BC_WORD)array;
	element_p[4]=bsp[0];
	element_p[5]=bsp[1];
	element_p[6]=bsp[2];
	bsp+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r14):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 5 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	asp-=1;
	asp[0]=(BC_WORD)array;
	element_p[4]=bsp[0];
	element_p[5]=bsp[1];
	element_p[6]=bsp[2];
	element_p[7]=bsp[3];
	bsp+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r1b):
{
	BC_WORD *array,array_o,*element_p,n;

	n=pc[1];
	array_o = (n+1) * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=2;
	element_p = &array[array_o+3];
	element_p[0]=asp[-1];
	asp-=1;
	asp[0]=(BC_WORD)array;
	element_p[1]=bsp[0];
	element_p[2]=bsp[1];
	element_p[3]=bsp[2];
	element_p[4]=bsp[3];
	element_p[5]=bsp[4];
	do {
		if (n< 6) break; element_p[ 6]=bsp[ 5];
		if (n< 7) break; element_p[ 7]=bsp[ 6];
		if (n< 8) break; element_p[ 8]=bsp[ 7];
		if (n< 9) break; element_p[ 9]=bsp[ 8];
		if (n<10) break; element_p[10]=bsp[ 9];
		if (n<11) break; element_p[11]=bsp[10];
		if (n<12) break; element_p[12]=bsp[11];
		if (n<13) break; element_p[13]=bsp[12];
		if (n<14) break; element_p[14]=bsp[13];
		if (n<15) break; element_p[15]=bsp[14];
		if (n<16) break; element_p[16]=bsp[15];
		if (n<17) break; element_p[17]=bsp[16];
		if (n<18) break; element_p[18]=bsp[17];
		if (n<19) break; element_p[19]=bsp[18];
		if (n<20) break; element_p[20]=bsp[19];
		if (n<21) break; element_p[21]=bsp[20];
		if (n<22) break; element_p[22]=bsp[21];
		if (n<23) break; element_p[23]=bsp[22];
		if (n<24) break; element_p[24]=bsp[23];
		if (n<25) break; element_p[25]=bsp[24];
		if (n<26) break; element_p[26]=bsp[25];
		if (n<27) break; element_p[27]=bsp[26];
		if (n<28) break; element_p[28]=bsp[27];
		if (n<29) break; element_p[29]=bsp[28];
		if (n<30) break; element_p[30]=bsp[29];
		if (n<31) break; element_p[31]=bsp[30];
		if (n<32) break; element_p[32]=bsp[31];
	} while (0);
	bsp+=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r20):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r21):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r22):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 4 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	element_p[4]=asp[-2];
	asp-=2;
	asp[0]=(BC_WORD)array;
	element_p[5]=bsp[0];
	element_p[6]=bsp[1];
	bsp+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r23):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 5 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	element_p[4]=asp[-2];
	asp-=2;
	asp[0]=(BC_WORD)array;
	element_p[5]=bsp[0];
	element_p[6]=bsp[1];
	element_p[7]=bsp[2];
	bsp+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r24):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 6 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o];
	element_p[3]=asp[-1];
	element_p[4]=asp[-2];
	asp-=2;
	asp[0]=(BC_WORD)array;
	element_p[5]=bsp[0];
	element_p[6]=bsp[1];
	element_p[7]=bsp[2];
	element_p[8]=bsp[3];
	bsp+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r2b):
{
	BC_WORD *array,array_o,*element_p,n;

	n=pc[1];
	array_o = (n+2) * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=2;
	element_p = &array[array_o+3];
	element_p[0]=asp[-1];
	element_p[1]=asp[-2];
	asp-=2;
	asp[0]=(BC_WORD)array;
	element_p[2]=bsp[0];
	element_p[3]=bsp[1];
	element_p[4]=bsp[2];
	element_p[5]=bsp[3];
	element_p[6]=bsp[4];
	do {
		if (n< 6) break; element_p[ 7]=bsp[ 5];
		if (n< 7) break; element_p[ 8]=bsp[ 6];
		if (n< 8) break; element_p[ 9]=bsp[ 7];
		if (n< 9) break; element_p[10]=bsp[ 8];
		if (n<10) break; element_p[11]=bsp[ 9];
		if (n<11) break; element_p[12]=bsp[10];
		if (n<12) break; element_p[13]=bsp[11];
		if (n<13) break; element_p[14]=bsp[12];
		if (n<14) break; element_p[15]=bsp[13];
		if (n<15) break; element_p[16]=bsp[14];
		if (n<16) break; element_p[17]=bsp[15];
		if (n<17) break; element_p[18]=bsp[16];
		if (n<18) break; element_p[19]=bsp[17];
		if (n<19) break; element_p[20]=bsp[18];
		if (n<20) break; element_p[21]=bsp[19];
		if (n<21) break; element_p[22]=bsp[20];
		if (n<22) break; element_p[23]=bsp[21];
		if (n<23) break; element_p[24]=bsp[22];
		if (n<24) break; element_p[25]=bsp[23];
		if (n<25) break; element_p[26]=bsp[24];
		if (n<26) break; element_p[27]=bsp[25];
		if (n<27) break; element_p[28]=bsp[26];
		if (n<28) break; element_p[29]=bsp[27];
		if (n<29) break; element_p[30]=bsp[28];
		if (n<30) break; element_p[31]=bsp[29];
		if (n<31) break; element_p[32]=bsp[30];
	} while (0);
	bsp+=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r30):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 3 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o+3];
	element_p[0]=asp[-1];
	element_p[1]=asp[-2];
	element_p[2]=asp[-3];
	asp-=3;
	asp[0]=(BC_WORD)array;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r31):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 4 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o+3];
	element_p[0]=asp[-1];
	element_p[1]=asp[-2];
	element_p[2]=asp[-3];
	element_p[3]=*bsp++;
	asp-=3;
	asp[0]=(BC_WORD)array;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r32):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 5 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o+3];
	element_p[0]=asp[-1];
	element_p[1]=asp[-2];
	element_p[2]=asp[-3];
	element_p[3]=bsp[0];
	element_p[4]=bsp[1];
	asp-=3;
	bsp+=2;
	asp[0]=(BC_WORD)array;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r33):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 6 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o+3];
	element_p[0]=asp[-1];
	element_p[1]=asp[-2];
	element_p[2]=asp[-3];
	element_p[3]=bsp[0];
	element_p[4]=bsp[1];
	element_p[5]=bsp[2];
	asp-=3;
	bsp+=3;
	asp[0]=(BC_WORD)array;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r34):
{
	BC_WORD *array,array_o,*element_p;

	array_o = 7 * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=1;
	element_p = &array[array_o+3];
	element_p[0]=asp[-1];
	element_p[1]=asp[-2];
	element_p[2]=asp[-3];
	element_p[3]=bsp[0];
	element_p[4]=bsp[1];
	element_p[5]=bsp[2];
	element_p[6]=bsp[3];
	asp-=3;
	bsp+=4;
	asp[0]=(BC_WORD)array;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update_r3b):
{
	BC_WORD *array,array_o,*element_p,n;

	n=pc[1];
	array_o = (n+3) * (BC_WORD_S)*bsp++;
	array = (BC_WORD*)asp[0];
	pc+=2;
	element_p = &array[array_o+3];
	element_p[0]=asp[-1];
	element_p[1]=asp[-2];
	element_p[2]=asp[-3];
	asp-=3;
	asp[0]=(BC_WORD)array;
	element_p[3]=bsp[0];
	element_p[4]=bsp[1];
	element_p[5]=bsp[2];
	element_p[6]=bsp[3];
	element_p[7]=bsp[4];
	do {
		if (n< 6) break; element_p[ 8]=bsp[ 5];
		if (n< 7) break; element_p[ 9]=bsp[ 6];
		if (n< 8) break; element_p[10]=bsp[ 7];
		if (n< 9) break; element_p[11]=bsp[ 8];
		if (n<10) break; element_p[12]=bsp[ 9];
		if (n<11) break; element_p[13]=bsp[10];
		if (n<12) break; element_p[14]=bsp[11];
		if (n<13) break; element_p[15]=bsp[12];
		if (n<14) break; element_p[16]=bsp[13];
		if (n<15) break; element_p[17]=bsp[14];
		if (n<16) break; element_p[18]=bsp[15];
		if (n<17) break; element_p[19]=bsp[16];
		if (n<18) break; element_p[20]=bsp[17];
		if (n<19) break; element_p[21]=bsp[18];
		if (n<20) break; element_p[22]=bsp[19];
		if (n<21) break; element_p[23]=bsp[20];
		if (n<22) break; element_p[24]=bsp[21];
		if (n<23) break; element_p[25]=bsp[22];
		if (n<24) break; element_p[26]=bsp[23];
		if (n<25) break; element_p[27]=bsp[24];
		if (n<26) break; element_p[28]=bsp[25];
		if (n<27) break; element_p[29]=bsp[26];
		if (n<28) break; element_p[30]=bsp[27];
		if (n<29) break; element_p[31]=bsp[28];
		if (n<30) break; element_p[32]=bsp[29];
	} while (0);
	bsp+=n;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updatepop_a):
{
	BC_WORD_S ao;

	ao=((BC_WORD_S*)pc)[2];
	asp[ao] = asp[((BC_WORD_S*)pc)[1]];
	asp=&asp[ao];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updatepop_b):
{
	BC_WORD_S bo;

	bo=((BC_WORD_S*)pc)[2];
	bsp[bo] = bsp[pc[1]];
	bsp=&bsp[bo];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(xorI):
	bsp[1]=bsp[0] ^ bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(CtoAC):
{
	BC_WORD c;

	NEED_HEAP(3);
	c=(BC_BOOL)bsp[0];
	hp[0]=(BC_WORD)&__STRING__+2;
	hp[1]=1;
	hp[2]=c;
	*++asp=(BC_WORD)hp;
	++bsp;
	hp+=3;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(ItoC):
	bsp[0] = (BC_BOOL)bsp[0];
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(ItoR):
{
	BC_REAL d = (BC_REAL) ((BC_WORD_S) bsp[0]);
	bsp[0] = *(BC_WORD*)&d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(RtoI):
{
	BC_REAL d = *(BC_REAL*)&bsp[0];
	bsp[0] = (BC_WORD_S) d;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}

INSTRUCTION_BLOCK(addIi):
	bsp[0]+=*(BC_WORD*)&pc[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(andIi):
	bsp[0]&=*(BC_WORD*)&pc[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(andIio):
{
	BC_WORD v;

	v=bsp[((BC_WORD_S*)pc)[1]] & *(BC_WORD*)&pc[2];
	*--bsp=v;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildh0_dup_a):
{
	BC_WORD v;
	BC_WORD_S ao;

	v=pc[1];
	ao=((BC_WORD_S*)pc)[2];
	asp[1] = v;
	asp[ao] = v;
	++asp;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildh0_dup2_a):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildh0_dup3_a):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildh0_put_a):
	asp[((BC_WORD_S*)pc)[2]] = pc[1];
	pc+=3;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildh0_put_a_jsr):
	asp[((BC_WORD_S*)pc)[2]] = pc[1];
	*--csp=(BC_WORD)&pc[4];
	pc=(BC_WORD*)pc[3];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(buildo1):
{
	BC_WORD_S ao;

	NEED_HEAP(3);
	ao=pc[1];
	hp[0]=pc[2];
	hp[1]=asp[ao];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(buildho2):
INSTRUCTION_BLOCK(buildo2):
{
	BC_WORD_S ao1,ao2;

	NEED_HEAP(3);
	ao1=((BC_WORD_S*)pc)[1];
	ao2=((BC_WORD_S*)pc)[2];
	hp[0]=*(BC_WORD*)&pc[3];
	hp[1]=asp[ao1];
	hp[2]=asp[ao2];
	*++asp=(BC_WORD)hp;
	hp+=3;
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(dup_a):
	asp[((BC_WORD_S*)pc)[1]] = asp[0];
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(dup2_a):
{
	BC_WORD_S ao_s;
	BC_WORD v;

	v=asp[0];
	ao_s=((BC_WORD_S*)pc)[1];
	asp[ao_s-1]=v;
	asp[ao_s]=v;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(dup3_a):
{
	BC_WORD_S ao_s;
	BC_WORD v;

	v=asp[0];
	ao_s=((BC_WORD_S*)pc)[1];
	asp[ao_s-2]=v;
	asp[ao_s-1]=v;
	asp[ao_s]=v;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(exchange_a):
{
	BC_WORD_S ao_1,ao_2;
	BC_WORD v;

	ao_1=((BC_WORD_S*)pc)[1];
	ao_2=((BC_WORD_S*)pc)[2];
	v = asp[ao_1];
	asp[ao_1] = asp[ao_2];
	asp[ao_2] = v;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(geC):
	bsp[1] = (BC_WORD_S)bsp[0] >= (BC_WORD_S)bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_b_false):
	if (bsp[((BC_WORD_S*)pc)[1]]){
		pc+=3;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)pc[2];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_eqACio):
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
		END_INSTRUCTION_BLOCK;
	n+=2;
	s+=1;
	for (;;){
		if (length>=IF_INT_64_OR_32(8,4)){
			if (*n!=*s)
				break;
			++n;
			++s;
			length-=IF_INT_64_OR_32(8,4);
			continue;
		}
#if WORD_WIDTH==64
		if (length>=4){
			if (*(uint32_t*)n!=*(uint32_t*)s)
				break;
			length-=4;
			n=(BC_WORD*)((BC_WORD)n+4);
			s=(BC_WORD*)((BC_WORD)s+4);
		}
#endif
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_eqC_b):
INSTRUCTION_BLOCK(jmp_eqI_b):
{
	BC_WORD_S bo;

	bo=((BC_WORD_S*)pc)[1];
	if (bsp[bo]!=(BC_WORD_S)pc[2]){
		pc+=4;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)pc[3];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_eqC_b2):
INSTRUCTION_BLOCK(jmp_eqI_b2):
{
	BC_WORD v;

	v=bsp[((BC_WORD_S*)pc)[1]];
	if (v==(BC_WORD_S)pc[2]){
		pc=(BC_WORD*)pc[3];
		END_INSTRUCTION_BLOCK;
	}
	if (v==(BC_WORD_S)pc[4]){
		pc=(BC_WORD*)pc[5];
		END_INSTRUCTION_BLOCK;
	}
	pc+=6;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_eqD_b):
	if (bsp[0]!=*(BC_WORD_S*)&pc[1]){
		pc+=3;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)pc[2];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_eqD_b2):
{
	BC_WORD v;

	v=bsp[0];
	if (v==*(BC_WORD_S*)&pc[1]){
		pc=(BC_WORD*)pc[2];
		END_INSTRUCTION_BLOCK;
	}
	if (v==*(BC_WORD_S*)&pc[3]){
		pc=(BC_WORD*)pc[4];
		END_INSTRUCTION_BLOCK;
	}
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_eqI):
	if (bsp[0] == bsp[1]){
		bsp+=2;
		pc=(BC_WORD*)pc[1];
		END_INSTRUCTION_BLOCK;
	}
	bsp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_eq_desc):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	if (*n!=*(BC_WORD_S*)&pc[2]){
		pc+=4;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)pc[3];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_geI):
	if ((BC_WORD_S)bsp[0] >= (BC_WORD_S)bsp[1]){
		bsp+=2;
		pc=(BC_WORD*)pc[1];
		END_INSTRUCTION_BLOCK;
	}
	bsp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_ltI):
	if ((BC_WORD_S)bsp[0] < (BC_WORD_S)bsp[1]){
		bsp+=2;
		pc=(BC_WORD*)pc[1];
		END_INSTRUCTION_BLOCK;
	}
	bsp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_neI):
	if (bsp[0] != bsp[1]){
		bsp+=2;
		pc=(BC_WORD*)pc[1];
		END_INSTRUCTION_BLOCK;
	}
	bsp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_neC_b):
INSTRUCTION_BLOCK(jmp_neI_b):
{
	BC_WORD_S bo;

	bo=((BC_WORD_S*)pc)[1];
	if (bsp[bo]==(BC_WORD_S)pc[2]){
		pc+=4;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)pc[3];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_ne_desc):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	if (*n==*(BC_WORD_S*)&pc[2]){
		pc+=4;
		END_INSTRUCTION_BLOCK;
	}
	pc=(BC_WORD*)pc[3];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_o_geI):
	if ((BC_WORD_S)bsp[((BC_WORD_S*)pc)[1]] >= (BC_WORD_S)bsp[0]){
		bsp+=1;
		pc=(BC_WORD*)pc[2];
		END_INSTRUCTION_BLOCK;
	}
	bsp+=1;
	pc+=3;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(jmp_o_geI_arraysize_a):
{

	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[2]];
	if ((BC_WORD_S)bsp[((BC_WORD_S*)pc)[1]] >= (BC_WORD_S)n[1]){
		pc=(BC_WORD*)pc[3];
		END_INSTRUCTION_BLOCK;
	}
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(ltIi):
	bsp[0] = (BC_WORD_S)bsp[0] < *(BC_WORD_S*)&pc[1];
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(neI):
	bsp[1]=bsp[0] != bsp[1];
	++bsp;
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_a_jmp):
	asp = (BC_WORD*) (((uint8_t*)asp) + pc[1]);
	pc=*(BC_WORD**)&pc[2];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_a_jsr):
	asp = (BC_WORD*) (((uint8_t*)asp) + pc[1]);
	*--csp=(BC_WORD)&pc[3];
	pc=*(BC_WORD**)&pc[2];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_a_rtn):
	asp = (BC_WORD*) (((uint8_t*)asp) + pc[1]);
	pc=(BC_WORD*)*csp++;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_ab_rtn):
	asp = (BC_WORD*) (((uint8_t*)asp) + pc[1]);
	bsp = (BC_WORD*) (((uint8_t*)bsp) + pc[2]);
	pc=(BC_WORD*)*csp++;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_b_jmp):
	bsp = (BC_WORD*) (((uint8_t*)bsp) + pc[1]);
	pc=*(BC_WORD**)&pc[2];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_b_jsr):
	bsp = (BC_WORD*) (((uint8_t*)bsp) + pc[1]);
	*--csp=(BC_WORD)&pc[3];
	pc=*(BC_WORD**)&pc[2];
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_b_pushBFALSE):
	bsp = (BC_WORD*) (((uint8_t*)bsp) + pc[1]);
	*bsp=0;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_b_pushBTRUE):
	bsp = (BC_WORD*) (((uint8_t*)bsp) + pc[1]);
	*bsp=1;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pop_b_rtn):
	bsp = (BC_WORD*) (((uint8_t*)bsp) + pc[1]);
	pc=(BC_WORD*)*csp++;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(pushD_a_jmp_eqD_b2):
{
	BC_WORD *n,v;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	v=*n;
	*--bsp=v;
	if (v==*(BC_WORD_S*)&pc[2]){
		pc=(BC_WORD*)pc[3];
		END_INSTRUCTION_BLOCK;
	}
	if (v==*(BC_WORD_S*)&pc[4]){
		pc=(BC_WORD*)pc[5];
		END_INSTRUCTION_BLOCK;
	}
	pc+=6;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_a_jsr):
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	*--csp=(BC_WORD)&pc[3];
	pc=(BC_WORD*)pc[2];
	*++asp=v;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_a2):
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	asp[1]=v;
	v=asp[((BC_WORD_S*)pc)[2]];
	asp[2]=v;
	asp+=2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_ab):
{
	BC_WORD v;

	v=asp[((BC_WORD_S*)pc)[1]];
	*++asp=v;
	v=bsp[((BC_WORD_S*)pc)[2]];
	*--bsp=v;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_b_incI):
{
	BC_WORD v;
	v=bsp[((BC_WORD_S*)pc)[1]];
	*--bsp=v+1;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_b_jsr):
{
	BC_WORD v;

	v=bsp[((BC_WORD_S*)pc)[1]];
	*--csp=(BC_WORD)&pc[3];
	pc=(BC_WORD*)pc[2];
	*--bsp=v;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_b2):
{
	BC_WORD v;

	v=bsp[((BC_WORD_S*)pc)[1]];
	bsp[-1]=v;
	v=bsp[((BC_WORD_S*)pc)[2]];
	bsp[-2]=v;
	bsp-=2;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push2_a):
{
	BC_WORD_S ao_s;

	ao_s=((BC_WORD_S*)pc)[1];
	asp[1]=asp[ao_s];
	asp[2]=asp[ao_s+1];
	asp+=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push2_b):
{
	BC_WORD bo_s;

	bo_s=((BC_WORD*)pc)[1];
	bsp[-1]=bsp[bo_s];
	bsp[-2]=bsp[bo_s-1];
	bsp-=2;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push3_a):
{
	BC_WORD_S ao_s;

	ao_s=((BC_WORD_S*)pc)[1];
	asp[1]=asp[ao_s];
	asp[2]=asp[ao_s+1];
	asp[3]=asp[ao_s+2];
	asp+=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push3_b):
{
	BC_WORD bo_s;

	bo_s=((BC_WORD*)pc)[1];
	bsp[-1]=bsp[bo_s];
	bsp[-2]=bsp[bo_s-1];
	bsp[-3]=bsp[bo_s-2];
	bsp-=3;
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_jsr_eval):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[((BC_WORD_S*)pc)[1]];
	*++asp=(BC_WORD)n;
	pc+=2;
	if ((n[0] & 2)!=0)
		END_INSTRUCTION_BLOCK;
	*--csp=(BC_WORD)pc;
	pc=(BC_WORD*)n[0];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_update_a):
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[1];
	ao_2=((BC_WORD_S*)pc)[2];
	asp[1] = asp[ao_1];
	asp[ao_1] = asp[ao_2];
	asp+=1;
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(put_a):
	asp[((BC_WORD_S*)pc)[1]] = asp[0];
	--asp;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(put_b):
	bsp[((BC_WORD_S*)pc)[1]] = bsp[0];
	++bsp;
	pc+=2;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(selectCHARoo):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[(BC_WORD_S)pc[1]];
	i=bsp[(BC_WORD_S)pc[2]];
	*--bsp=((BC_BOOL*)&array[2])[i];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(selectoo):
{
	BC_WORD *array,i;

	array=(BC_WORD*)asp[(BC_WORD_S)pc[1]];
	i=bsp[(BC_WORD_S)pc[2]];
	*++asp=array[3+i];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update2_a):
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update2_b):
{
	BC_WORD_S bo_s,bo_d;

	bo_s=((BC_WORD_S*)pc)[1];
	bo_d=((BC_WORD_S*)pc)[2];
	bsp[bo_d+1] = bsp[bo_s+1];
	bsp[bo_d] = bsp[bo_s];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update2pop_a):
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	asp=&asp[ao_d];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update2pop_b):
{
	BC_WORD_S bo_s,bo_d;

	bo_s=((BC_WORD_S*)pc)[1];
	bo_d=((BC_WORD_S*)pc)[2];
	bsp[bo_d+1] = bsp[bo_s+1];
	bsp[bo_d] = bsp[bo_s];
	bsp=&bsp[bo_d];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update3_a):
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-2] = asp[ao_s-2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update3_b):
{
	BC_WORD_S bo_s,bo_d;

	bo_s=((BC_WORD_S*)pc)[1];
	bo_d=((BC_WORD_S*)pc)[2];
	bsp[bo_d+2] = bsp[bo_s+2];
	bsp[bo_d+1] = bsp[bo_s+1];
	bsp[bo_d] = bsp[bo_s];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update3pop_a):
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-2] = asp[ao_s-2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	asp=&asp[ao_d];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update4_a):
{
	BC_WORD_S ao_s,ao_d;

	ao_s=((BC_WORD_S*)pc)[1];
	ao_d=((BC_WORD_S*)pc)[2];
	asp[ao_d-3] = asp[ao_s-3];
	asp[ao_d-2] = asp[ao_s-2];
	asp[ao_d-1] = asp[ao_s-1];
	asp[ao_d] = asp[ao_s];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(update3pop_b):
{
	BC_WORD_S bo_s,bo_d;

	bo_s=((BC_WORD_S*)pc)[1];
	bo_d=((BC_WORD_S*)pc)[2];
	bsp[bo_d+2] = bsp[bo_s+2];
	bsp[bo_d+1] = bsp[bo_s+1];
	bsp[bo_d] = bsp[bo_s];
	bsp=&bsp[bo_d];
	pc+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates2_a):
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[2];
	ao_2=((BC_WORD_S*)pc)[3];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[1];
	asp[ao_1] = asp[ao_2];
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates2_a_pop_a):
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[2];
	ao_2=((BC_WORD_S*)pc)[3];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[1];
	asp[ao_1] = asp[ao_2];
	asp=(BC_WORD*) (((uint8_t*)asp)-pc[4]);
	pc+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates2_b):
{
	BC_WORD_S bo_1,bo_2;

	bo_1=((BC_WORD_S*)pc)[2];
	bo_2=((BC_WORD_S*)pc)[3];
	bsp[bo_2] = bsp[bo_1];
	bo_2=((BC_WORD_S*)pc)[1];
	bsp[bo_1] = bsp[bo_2];
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates2pop_a):
{
	BC_WORD_S ao_1,ao_2;

	ao_1=((BC_WORD_S*)pc)[2];
	ao_2=((BC_WORD_S*)pc)[3];
	asp[ao_2] = asp[ao_1];
	ao_2=((BC_WORD_S*)pc)[1];
	asp[ao_1] = asp[ao_2];
	asp=&asp[ao_1];
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates2pop_b):
{
	BC_WORD_S bo_1,bo_2;

	bo_1=((BC_WORD_S*)pc)[2];
	bo_2=((BC_WORD_S*)pc)[3];
	bsp[bo_2] = bsp[bo_1];
	bo_2=((BC_WORD_S*)pc)[1];
	bsp[bo_1] = bsp[bo_2];
	bsp=&bsp[bo_1];
	pc+=4;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates3_a):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates3_b):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates3pop_a):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates3pop_b):
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
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(updates4_a):
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
	END_INSTRUCTION_BLOCK;
}

INSTRUCTION_BLOCK(jsr_ap):
	*--csp=(BC_WORD)&pc[2];
INSTRUCTION_BLOCK(jmp_ap):
{
	BC_WORD *n,d;
	int n_apply_args=pc[1];

	n=(BC_WORD*)asp[0];
	d=n[0];
	if (((uint16_t*)d)[0]==n_apply_args*8){
		BC_WORD arity;

		arity=((uint16_t*)d)[-1];
#if (WORD_WIDTH == 64)
		pc = (BC_WORD*) ((*(BC_WORD*)(d+(16*(n_apply_args-1))+6)) - 24);
#else
		pc = (BC_WORD*) ((*(BC_WORD*)(d+(8*(n_apply_args-1))+2)) - 12);
#endif
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
				asp[0]=(BC_WORD)args;
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
		END_INSTRUCTION_BLOCK;
	} else {
		*--csp=(BC_WORD)&Fjmp_ap[(n_apply_args-2)*2];
		pc = *(BC_WORD**)(d+IF_INT_64_OR_32(6,2));
		END_INSTRUCTION_BLOCK;
	}
}
INSTRUCTION_BLOCK(jsr_ap5):
	*--csp=(BC_WORD)&pc[1];
INSTRUCTION_BLOCK(jmp_ap5):
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
#ifdef DEBUG_ALL_INSTRUCTIONS
	EPRINTF("\t" BC_WORD_FMT ": %d/%d -> " BC_WORD_FMT "\n",
			d-(BC_WORD)program->data,
			((uint16_t*)d)[0],
			((uint16_t*)d)[-1],
			(*(BC_WORD*)(d+40-6) - (BC_WORD) program->code) / 8);
#endif
	if (((uint16_t*)d)[0]==40){
		BC_WORD arity;

		arity=((uint16_t*)d)[-1];
#if (WORD_WIDTH == 64)
		pc = (BC_WORD*) ((*(BC_WORD*)(d+64+6)) - 24);
#else
		pc = (BC_WORD*) ((*(BC_WORD*)(d+32+2)) - 12);
#endif
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
				asp[0]=(BC_WORD)args;
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
		END_INSTRUCTION_BLOCK;
	} else {
		*--csp=(BC_WORD)&Fjmp_ap[6];
		pc = *(BC_WORD**)(d+IF_INT_64_OR_32(6,2));
		END_INSTRUCTION_BLOCK;
	}
}
INSTRUCTION_BLOCK(jsr_ap4):
	*--csp=(BC_WORD)&pc[1];
INSTRUCTION_BLOCK(jmp_ap4):
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
	if (((uint16_t*)d)[0]==32){
		BC_WORD arity;

		arity=((uint16_t*)d)[-1];
#if (WORD_WIDTH == 64)
		pc = (BC_WORD*) ((*(BC_WORD*)(d+48+6)) - 24);
#else
		pc = (BC_WORD*) ((*(BC_WORD*)(d+24+2)) - 12);
#endif
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
				asp[0]=(BC_WORD)args;
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
		END_INSTRUCTION_BLOCK;
	} else {
		*--csp=(BC_WORD)&Fjmp_ap[4];
		pc = *(BC_WORD**)(d+2);
		END_INSTRUCTION_BLOCK;
	}
}
INSTRUCTION_BLOCK(jsr_ap3):
	*--csp=(BC_WORD)&pc[1];
INSTRUCTION_BLOCK(jmp_ap3):
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
#ifdef DEBUG_ALL_INSTRUCTIONS
	EPRINTF("\t" BC_WORD_FMT ": %d/%d -> " BC_WORD_FMT "\n",
			d-(BC_WORD)program->data,
			((uint16_t*)d)[0],
			((uint16_t*)d)[-1],
			(*(BC_WORD*)(d+24-6) - (BC_WORD) program->code) / 8);
#endif
	if (((uint16_t*)d)[0]==24){
		BC_WORD arity;

		arity=((uint16_t*)d)[-1];
#if (WORD_WIDTH == 64)
		pc = (BC_WORD*) ((*(BC_WORD*)(d+32+6)) - 24);
#else
		pc = (BC_WORD*) ((*(BC_WORD*)(d+16+2)) - 12);
#endif
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
				asp[0]=(BC_WORD)args;
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
		END_INSTRUCTION_BLOCK;
	} else {
		*--csp=(BC_WORD)&Fjmp_ap[2];
		pc = *(BC_WORD**)(d+IF_INT_64_OR_32(6,2));
		END_INSTRUCTION_BLOCK;
	}
}
INSTRUCTION_BLOCK(jsr_ap2):
	*--csp=(BC_WORD)&pc[1];
INSTRUCTION_BLOCK(jmp_ap2):
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
#ifdef DEBUG_ALL_INSTRUCTIONS
	EPRINTF("\t" BC_WORD_FMT ": %u/%d -> " BC_WORD_FMT "\n",
			d-(BC_WORD)program->data,
			((uint16_t*)d)[0],
			((int16_t*)d)[-1],
			(*(BC_WORD*)(d+16+6) - (BC_WORD) program->code) / 8);
#endif
	if (((uint16_t*)d)[0]==16){
		int16_t arity;

		arity=((int16_t*)d)[-1];
#if (WORD_WIDTH == 64)
		pc = (BC_WORD*) ((*(BC_WORD*)(d+16+6)) - 24);
#else
		pc = (BC_WORD*) ((*(BC_WORD*)(d+8+2)) - 12);
#endif
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
				asp[0]=(BC_WORD)args;
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
		END_INSTRUCTION_BLOCK;
	} else {
		*--csp=(BC_WORD)&Fjmp_ap[0];
		pc = *(BC_WORD**)(d+IF_INT_64_OR_32(6,2));
		END_INSTRUCTION_BLOCK;
	}
}
INSTRUCTION_BLOCK(jsr_ap1):
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	*--csp=(BC_WORD)&pc[1];
	d=n[0];
#ifdef DEBUG_ALL_INSTRUCTIONS
	EPRINTF("\t%p: " BC_WORD_FMT "; " BC_WORD_FMT "\n", (void*) d, *(BC_WORD*)(d+IF_INT_64_OR_32(6,2)) - (BC_WORD)program->code, d-(BC_WORD)program->data);
#endif
	pc = *(BC_WORD**)(d+IF_INT_64_OR_32(6,2));
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jmp_ap1):
{
	BC_WORD *n,d;

	n=(BC_WORD*)asp[0];
	d=n[0];
#ifdef DEBUG_ALL_INSTRUCTIONS
	EPRINTF("\t%p: " BC_WORD_FMT "; " BC_WORD_FMT "\n", (void*) d, *(BC_WORD*)(d+IF_INT_64_OR_32(6,2)) - (BC_WORD)program->code, d-(BC_WORD)program->data);
#endif
	pc = *(BC_WORD**)(d+IF_INT_64_OR_32(6,2));
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(add_arg0):
{
	BC_WORD *n;
	NEED_HEAP(2);
	n=(BC_WORD*)asp[0];
	pc=(BC_WORD*)*csp++;
	hp[1]=asp[-1];
	asp[-1]=(BC_WORD)hp;
	hp[0]=n[0]+IF_INT_64_OR_32(16,8);
	--asp;
	hp+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(add_arg1):
{
	BC_WORD *n;

	NEED_HEAP(3);
	n=(BC_WORD*)asp[0];
	hp[2]=asp[-1];
	pc=(BC_WORD*)*csp++;
	hp[1]=n[1];
	asp[-1]=(BC_WORD)hp;
	hp[0]=n[0]+IF_INT_64_OR_32(16,8);
	--asp;
	hp+=3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(add_arg2):
{
	BC_WORD *n;

	NEED_HEAP(5);
	n=(BC_WORD*)asp[0];
	hp[4]=asp[-1];
	pc=(BC_WORD*)*csp++;
	hp[1]=n[1];
	hp[2]=(BC_WORD)&hp[3];
	hp[3]=n[2];
	asp[-1]=(BC_WORD)hp;
	hp[0]=n[0]+IF_INT_64_OR_32(16,8);
	--asp;
	hp+=5;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(add_arg3):
{
	BC_WORD *n,*a;

	NEED_HEAP(6);
	n=(BC_WORD*)asp[0];
	hp[5]=asp[-1];
	pc=(BC_WORD*)*csp++;
	a=(BC_WORD*)n[2];
	hp[1]=n[1];
	hp[2]=(BC_WORD)&hp[3];
	hp[0]=n[0]+IF_INT_64_OR_32(16,8);
	hp[3]=a[0];
	hp[4]=a[1];
	asp[-1]=(BC_WORD)hp;
	--asp;
	hp+=6;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(add_arg32): instr_arg=32; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg31): instr_arg=31; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg30): instr_arg=30; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg29): instr_arg=29; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg28): instr_arg=28; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg27): instr_arg=27; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg26): instr_arg=26; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg25): instr_arg=25; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg24): instr_arg=24; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg23): instr_arg=23; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg22): instr_arg=22; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg21): instr_arg=21; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg20): instr_arg=20; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg19): instr_arg=19; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg18): instr_arg=18; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg17): instr_arg=17; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg16): instr_arg=16; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg15): instr_arg=15; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg14): instr_arg=14; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg13): instr_arg=13; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg12): instr_arg=12; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg11): instr_arg=11; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg10): instr_arg=10; goto add_arg_n;
INSTRUCTION_BLOCK(add_arg9):  instr_arg=9;  goto add_arg_n;
INSTRUCTION_BLOCK(add_arg8):  instr_arg=8;  goto add_arg_n;
INSTRUCTION_BLOCK(add_arg7):  instr_arg=7;  goto add_arg_n;
INSTRUCTION_BLOCK(add_arg6):  instr_arg=6;  goto add_arg_n;
INSTRUCTION_BLOCK(add_arg5):  instr_arg=5;  goto add_arg_n;
INSTRUCTION_BLOCK(add_arg4):  instr_arg=4;
add_arg_n:
{
	BC_WORD *n,*a;

	NEED_HEAP(instr_arg+3);
	n=(BC_WORD*)asp[0];
	hp[5]=asp[-1];
	pc=(BC_WORD*)*csp++;
	a=(BC_WORD*)n[2];
	hp[1]=n[1];
	hp[2]=(BC_WORD)&hp[3];
	hp[0]=n[0]+IF_INT_64_OR_32(16,8);
	hp[3]=a[0];
	hp[4]=a[1];
	hp[5]=a[2];
	do {
		if (instr_arg< 5) break; hp[ 6]=a[ 3];
		if (instr_arg< 6) break; hp[ 7]=a[ 4];
		if (instr_arg< 7) break; hp[ 8]=a[ 5];
		if (instr_arg< 8) break; hp[ 9]=a[ 6];
		if (instr_arg< 9) break; hp[10]=a[ 7];
		if (instr_arg<10) break; hp[11]=a[ 8];
		if (instr_arg<11) break; hp[12]=a[ 9];
		if (instr_arg<12) break; hp[13]=a[10];
		if (instr_arg<13) break; hp[14]=a[11];
		if (instr_arg<14) break; hp[15]=a[12];
		if (instr_arg<15) break; hp[16]=a[13];
		if (instr_arg<16) break; hp[17]=a[14];
		if (instr_arg<17) break; hp[18]=a[15];
		if (instr_arg<18) break; hp[19]=a[16];
		if (instr_arg<19) break; hp[20]=a[17];
		if (instr_arg<20) break; hp[21]=a[18];
		if (instr_arg<21) break; hp[22]=a[19];
		if (instr_arg<22) break; hp[23]=a[20];
		if (instr_arg<23) break; hp[24]=a[21];
		if (instr_arg<24) break; hp[25]=a[22];
		if (instr_arg<25) break; hp[26]=a[23];
		if (instr_arg<26) break; hp[27]=a[24];
		if (instr_arg<27) break; hp[28]=a[25];
		if (instr_arg<28) break; hp[29]=a[26];
		if (instr_arg<29) break; hp[30]=a[27];
		if (instr_arg<30) break; hp[31]=a[28];
		if (instr_arg<31) break; hp[32]=a[29];
		if (instr_arg<32) break; hp[33]=a[30];
	} while (0);
	asp[-1]=(BC_WORD)hp;
	--asp;
	hp+=instr_arg+3;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eval_upd0):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__interpreter_indirection[5];
	n[1]=asp[-1];
	--asp;
	pc=*(BC_WORD**)&pc[1];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eval_upd1):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__interpreter_indirection[5];
	asp[0]=n[1];
	n[1]=asp[-1];
	pc=*(BC_WORD**)&pc[1];
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eval_upd2):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__interpreter_indirection[5];
	asp[1]=n[1];
	n[1]=asp[-1];
	asp[0]=n[2];
	pc=*(BC_WORD**)&pc[1];
	asp+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eval_upd3):
{
	BC_WORD *n;

	n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__interpreter_indirection[5];
	asp[2]=n[1];
	n[1]=asp[-1];
	asp[1]=n[2];
	asp[0]=n[3];
	pc=*(BC_WORD**)&pc[1];
	asp+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(eval_upd32): instr_arg=32; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd31): instr_arg=31; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd30): instr_arg=30; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd29): instr_arg=29; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd28): instr_arg=28; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd27): instr_arg=27; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd26): instr_arg=26; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd25): instr_arg=25; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd24): instr_arg=24; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd23): instr_arg=23; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd22): instr_arg=22; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd21): instr_arg=21; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd20): instr_arg=20; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd19): instr_arg=19; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd18): instr_arg=18; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd17): instr_arg=17; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd16): instr_arg=16; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd15): instr_arg=15; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd14): instr_arg=14; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd13): instr_arg=13; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd12): instr_arg=12; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd11): instr_arg=11; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd10): instr_arg=10; goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd9):  instr_arg=9;  goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd8):  instr_arg=8;  goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd7):  instr_arg=7;  goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd6):  instr_arg=6;  goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd5):  instr_arg=5;  goto eval_upd_n;
INSTRUCTION_BLOCK(eval_upd4):  instr_arg=4;
eval_upd_n:
{
	BC_WORD *n=(BC_WORD*)asp[0];
	n[0]=(BC_WORD)&__interpreter_indirection[5];
	asp[instr_arg-1]=n[1];
	n[1]=asp[-1];
	for (int i=instr_arg-2; i>=0; i--)
		asp[i]=n[instr_arg-i];
	pc=*(BC_WORD**)&pc[1];
	asp+=instr_arg-1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jesr):
	g_asp=asp;
	g_bsp=bsp;
	g_heap_free=heap_free;
	g_hp=hp;
	switch (pc[1]){
		case 1:
			clean_catAC();
			break;
		case 2:
			clean_sliceAC();
			break;
		case 3:
			clean_ItoAC();
			break;
		case 5:
			clean_print_string_();
			break;
		case 6:
			clean_openF();
			break;
		case 8:
			clean_closeF();
			break;
		case 9:
			clean_readLineF();
			break;
		case 10:
			clean_endF();
			break;
		case 11:
			clean_cmpAC();
			break;
		case 12:
			clean_writeFI();
			break;
		case 13:
			clean_writeFS();
			break;
		case 14:
			clean_writeFC();
			break;
	}
	asp=g_asp;
	bsp=g_bsp;
	heap_free=g_heap_free;
	hp=g_hp;
	if (trap_needs_gc) {
		trap_needs_gc = 0;
		GARBAGE_COLLECT;
	}
	pc+=2;
	END_INSTRUCTION_BLOCK;

#ifndef COMPUTED_GOTOS
case EVAL_TO_HNF_LABEL:
	goto eval_to_hnf_return;
	break;
#endif

#ifdef LINK_CLEAN_RUNTIME
INSTRUCTION_BLOCK(jsr_eval_host_node):
{
	BC_WORD *n=(BC_WORD*)asp[0];
	int host_nodeid = ((BC_WORD*)n[1])[1];
	BC_WORD *host_node = ie->host->clean_ie->__ie_2->__ie_shared_nodes[3+host_nodeid];
#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\t%p -> [%d; %p -> %p]\n",n,host_nodeid,host_node,(void*)*host_node);
#endif
	if (!(host_node[0] & 2)) {
		ie->asp = asp;
		ie->bsp = bsp;
		ie->csp = csp;
		ie->hp = hp;
		*ie->host->host_a_ptr++=(BC_WORD)ie->host->clean_ie;
		host_node = __interpret__evaluate__host(ie, host_node);
		ie->host->clean_ie=(struct InterpretationEnvironment*)*--ie->host->host_a_ptr;
		hp = ie->hp;
#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("\tnew node after evaluation: %p -> %p\n",host_node,(void*)*host_node);
#endif
	}

	ie->asp = asp;
	ie->bsp = bsp;
	ie->csp = csp;
	ie->hp = hp;
	int words_used=copy_to_interpreter_or_garbage_collect(ie, host_node);
	if (words_used<0) {
		EPRINTF("Interpreter is out of memory\n");
		return -1;
	}
	BC_WORD *old_hp=ie->hp;
	hp=ie->hp+words_used;
	heap_free = heap + (ie->in_first_semispace ? 1 : 2) * heap_size - hp;
	n=(BC_WORD*)asp[0];
	n[0]=old_hp[0];
	n[1]=old_hp[1];
	if (((int16_t*)(n[0]))[-1] >= 2)
		n[2]=old_hp[2];

	pc=(BC_WORD*)*csp++;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(jsr_eval_host_node_31):
	instr_arg=31;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_30):
	instr_arg=30;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_29):
	instr_arg=29;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_28):
	instr_arg=28;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_27):
	instr_arg=27;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_26):
	instr_arg=26;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_25):
	instr_arg=25;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_24):
	instr_arg=24;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_23):
	instr_arg=23;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_22):
	instr_arg=22;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_21):
	instr_arg=21;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_20):
	instr_arg=20;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_19):
	instr_arg=19;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_18):
	instr_arg=18;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_17):
	instr_arg=17;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_16):
	instr_arg=16;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_15):
	instr_arg=15;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_14):
	instr_arg=14;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_13):
	instr_arg=13;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_12):
	instr_arg=12;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_11):
	instr_arg=11;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_10):
	instr_arg=10;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_9):
	instr_arg=9;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_8):
	instr_arg=8;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_7):
	instr_arg=7;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_6):
	instr_arg=6;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_5):
	instr_arg=5;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_4):
	instr_arg=4;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_3):
	instr_arg=3;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_2):
	instr_arg=2;
	goto jsr_eval_host_node_with_args;
INSTRUCTION_BLOCK(jsr_eval_host_node_1):
	instr_arg=1;
jsr_eval_host_node_with_args:
{
	BC_WORD *n=(BC_WORD*)asp[0];
	int host_nodeid=n[1];
	BC_WORD *host_node = ie->host->clean_ie->__ie_2->__ie_shared_nodes[3+host_nodeid];
	int args_needed=((int16_t*)(host_node[0]))[0]>>IF_MACH_O_ELSE(4,3);
	int node_arity=((int16_t*)(host_node[0]))[-1];

	if (args_needed!=instr_arg) {
		EPRINTF("Error in jsr_eval_host_node: wanted nr. of args (%d) is not the given nr (%d)\n",args_needed,instr_arg);
		exit(-1);
	}

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\thost node (%d: %p -> %p) arity is %d; %d needed\n",
			host_nodeid,host_node,(void*)*host_node,node_arity,args_needed);
	for (int i = 1; i <= instr_arg; i++)
		EPRINTF("\targ %d: %p -> %p\n",i,(void*)asp[-i],*(void**)asp[-i]);
#endif

	*ie->host->host_a_ptr++=(BC_WORD)ie->host->clean_ie;

	for (int i=instr_arg; i>=1; i--) {
		int added_words=copy_to_host_or_garbage_collect(
				ie->host->clean_ie, ie->host->host_hp_ptr,
				(BC_WORD**)ie->host->host_a_ptr++, (BC_WORD*)asp[-i]);
		if (added_words<0) { /* TODO: -2 is garbage collection */
			EPRINTF("copying to host failed\n");
			exit(1);
		}
		ie->host->host_hp_ptr+=added_words;
	}
	BC_WORD *arg1, *arg2 = NULL;
	arg1 = (BC_WORD*) *--ie->host->host_a_ptr;
	if (instr_arg >= 2)
		arg2 = (BC_WORD*) *--ie->host->host_a_ptr;

	ie->asp = asp;
	ie->bsp = bsp;
	ie->csp = csp;
	ie->hp = hp;
	if (instr_arg >= 2) {
#ifdef WINDOWS
		host_node = __interpret__evaluate__host_with_args(arg2, arg1, host_node, ap_addresses[instr_arg-2], ie);
#else
		host_node = __interpret__evaluate__host_with_args(ie, 0, arg1, arg2, host_node, ap_addresses[instr_arg-2]);
#endif
	} else if (node_arity > 0) {
#ifdef MACH_O64
		BC_WORD *lazy_entry = (BC_WORD*) (((BC_WORD*)(host_node[0]+6))[0]);
#else
		BC_WORD *lazy_entry = (BC_WORD*) (((BC_WORD*)(host_node[0]+2))[0] & 0xffffffff);
#endif
		*ie->host->host_a_ptr++ = (BC_WORD) host_node;
#ifdef WINDOWS
		host_node = __interpret__evaluate__host_with_args(arg1,host_node,arg1, lazy_entry, ie);
#else
		host_node = __interpret__evaluate__host_with_args(ie, 0, host_node,arg1,arg1, lazy_entry);
#endif
		ie->host->host_a_ptr--;
	} else {
#ifdef MACH_O64
		BC_WORD *lazy_entry = (BC_WORD*) (((BC_WORD*)(host_node[0]+6))[0]);
#else
		BC_WORD *lazy_entry = (BC_WORD*) (((BC_WORD*)(host_node[0]+2))[0] & 0xffffffff);
#endif
#ifdef WINDOWS
		host_node = __interpret__evaluate__host_with_args(arg1,arg1,arg1, lazy_entry, ie);
#else
		host_node = __interpret__evaluate__host_with_args(ie, 0, arg1,arg1,arg1, lazy_entry);
#endif
	}
	hp = ie->hp;

	ie->host->clean_ie=(struct InterpretationEnvironment*)*--ie->host->host_a_ptr;

	asp-=instr_arg+1;
	ie->asp = asp;
	ie->bsp = bsp;
	ie->csp = csp;
	ie->hp = hp;
	int words_used=copy_to_interpreter_or_garbage_collect(ie, host_node);
	if (words_used<0) {
		EPRINTF("Interpreter is out of memory\n");
		return -1;
	}
	hp=ie->hp;
	heap_free = heap + (ie->in_first_semispace ? 1 : 2) * heap_size - hp;
	*++asp=(BC_WORD)hp;
	hp+=words_used;

	pc=(BC_WORD*)*csp++;
	END_INSTRUCTION_BLOCK;
}
#endif

INSTRUCTION_BLOCK(push_r_arg_t):
	bsp[0]=*(char*)bsp[0];
	pc+=1;
	END_INSTRUCTION_BLOCK;
INSTRUCTION_BLOCK(push_t_r_a):
{
	BC_WORD *n=(BC_WORD*)asp[pc[1]];
	*--bsp=n[0]+IF_INT_64_OR_32(14,6);
	pc+=2;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_t_r_args):
{
	BC_WORD *n=(BC_WORD*)*asp--;
	BC_WORD *d=(BC_WORD*)n[0];
	int16_t a_arity=((int16_t*)d)[-1];
	a_arity = ((int16_t*)d)[0];
	int16_t b_arity = ((int16_t*)d)[-1] - 256 - a_arity;
	if (a_arity + b_arity < 3) {
		if (a_arity==2) {
			*++asp=n[2];
			*++asp=n[1];
		} else if (a_arity==1) {
			*++asp=n[1];
			if (b_arity==1)
				*--bsp=n[2];
		} else if (b_arity==2) {
			*--bsp=n[2];
			*--bsp=n[1];
		} else if (b_arity==1) {
			*--bsp=n[1];
		}
	} else {
		BC_WORD *rest=(BC_WORD*)n[2];
		int i=a_arity+b_arity-2;
		while (b_arity-- && i >= 0)
			*--bsp=rest[i--];
		while (a_arity-- && i >= 0)
			*++asp=rest[i--];
		if (a_arity >= 0)
			*++asp=n[1];
		else
			*--bsp=n[1];
	}
	*--bsp=n[0]+IF_INT_64_OR_32(14,6);
	pc+=1;
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_a_r_args):
	/* Unboxed array of records in asp[0], index in bsp[0].
	 * Copy elements to the stacks and push record descriptor to B-stack. */
{
	BC_WORD *array,*desc,n,array_o,*a,*b;
	int16_t a_arity,b_arity,ab_arity;

	pc+=1;
	n=*bsp++;
	array=(BC_WORD*)asp[0];
	desc=(BC_WORD*)array[2];
	ab_arity=((int16_t*)desc)[0]-256;
	a_arity=((int16_t*)desc)[1];
	b_arity=ab_arity-a_arity;
	array_o=(ab_arity*n)+3;
	a=&array[array_o];
	b=a+a_arity;
	asp+=a_arity-1;
	for (int i=0;i<a_arity;i++)
		asp[-i]=a[i];
	bsp-=b_arity;
	for (int i=0;i<b_arity;i++)
		bsp[i]=b[i];
	*--bsp=(BC_WORD)(desc+2);
	END_INSTRUCTION_BLOCK;
}
INSTRUCTION_BLOCK(push_r_arg_D):
	/* Pop a pointer to the end of the type string of a record from bsp;
	 * set bsp[0] to the bsp[0]'th 'child descriptor' in that record. */
{
	BC_WORD desc=*bsp++;
	desc=(desc+IF_INT_64_OR_32(7,3)) & IF_INT_64_OR_32(-8,-4);
	desc+=bsp[0]<<IF_INT_64_OR_32(3,2);
	bsp[0]=*(BC_WORD*)desc-2;
	pc+=1;
	END_INSTRUCTION_BLOCK;
}

INSTRUCTION_BLOCK(ccall):
INSTRUCTION_BLOCK(centry):
INSTRUCTION_BLOCK(fill3_r):
INSTRUCTION_BLOCK(fill3_r01a):
INSTRUCTION_BLOCK(fill3_r01b):
INSTRUCTION_BLOCK(fill_u):
INSTRUCTION_BLOCK(instruction):
INSTRUCTION_BLOCK(load_i):
INSTRUCTION_BLOCK(load_si16):
INSTRUCTION_BLOCK(load_si32):
INSTRUCTION_BLOCK(load_ui8):
INSTRUCTION_BLOCK(push_finalizers):
INSTRUCTION_BLOCK(pushL):
INSTRUCTION_BLOCK(pushLc):
INSTRUCTION_BLOCK(set_finalizers):
INSTRUCTION_BLOCK(A_data_IIIla):
INSTRUCTION_BLOCK(A_data_IIl):
INSTRUCTION_BLOCK(A_data_IlI):
INSTRUCTION_BLOCK(A_data_IlIla):
INSTRUCTION_BLOCK(A_data_a):
INSTRUCTION_BLOCK(A_data_la):
#ifdef COMPUTED_GOTOS
	goto instr_unimplemented; /* Just to stop gcc complaining about an unused label */
#endif
UNIMPLEMENTED_INSTRUCTION_BLOCK:
	EPRINTF("Unimplemented instruction " BC_WORD_FMT " (%s) at %d\n", *pc, instruction_name(*pc), (int) (pc-program->code));
	if (asp + 10 > csp)
		EPRINTF("A and C stack pointers are dangerously close; perhaps try with a larger stack.\n");
	return 1;
