implementation module specialized

import target

instr_halt :: !Target -> Target
instr_halt t = foldl (flip append) t
	[ "#ifdef DEBUG_CURSES"
	, "\tdebugger_graceful_end();"
	, "#endif"
	, "#ifdef LINK_CLEAN_RUNTIME"
	, "\t{"
	, "\t\tEPRINTF(\"Stack trace:\\n\");"
	, "\t\tBC_WORD *start_csp = &stack[stack_size >> 1];"
	, "\t\tchar _tmp[256];"
	, "\t\tfor (; csp>start_csp; csp--) {"
	, "\t\t\tprint_label(_tmp, 256, 1, (BC_WORD*)*csp, program, ie->heap, ie->heap_size);"
	, "\t\t\tEPRINTF(\"%s\\n\",_tmp);"
	, "\t\t}"
	, "\t}"
	, "\tinterpret_error=&e__ABC_PInterpreter__dDV__Halt;"
	, "\tEXIT(ie,1);"
	, "\tgoto eval_to_hnf_return_failure;"
	, "#else"
	, "\treturn 0;"
	, "#endif"
	]

instr_divLU :: !Target -> Target
instr_divLU t = foldl (flip append) t
	[ "{"
	, "#if defined(WINDOWS) && WORD_WIDTH==64"
	, "\tEPRINTF(\"divLU is not supported on 64-bit Windows\\n\");"
	, "#else"
	, "# if WORD_WIDTH==64"
	, "\t__int128_t num=((__int128_t)bsp[0] << 64) + bsp[1];"
	, "# else"
	, "\tint64_t num=((int64_t)bsp[0] << 32) + bsp[1];"
	, "# endif"
	, "\tbsp[1]=num%bsp[2];"
	, "\tbsp[2]=num/bsp[2];"
	, "\tbsp+=1;"
	, "\tpc+=1;"
	, "#endif"
	, "}"
	]

instr_mulUUL :: !Target -> Target
instr_mulUUL t = foldl (flip append) t
	[ "{"
	, "#if defined(WINDOWS) && WORD_WIDTH==64"
	, "\tEPRINTF(\"mulUUL is not supported on 64-bit Windows\\n\");"
	, "#else"
	, "# if WORD_WIDTH==64"
	, "\t__uint128_t res=(__uint128_t)((__uint128_t)bsp[0] * (__uint128_t)bsp[1]);"
	, "# else"
	, "\tuint64_t res=(uint64_t)bsp[0] * (uint64_t)bsp[1];"
	, "# endif"
	, "\tbsp[0]=res>>WORD_WIDTH;"
	, "\tbsp[1]=(BC_WORD)res;"
	, "\tpc+=1;"
	, "#endif"
	, "}"
	]

instr_RtoAC :: !Target -> Target
instr_RtoAC t = foldl (flip append) t
	[ "{"
	, "char r[22];"
	, "int n=sprintf(r,BC_REAL_FMT,*((BC_REAL*)bsp)+0.0);"
	, "NEED_HEAP(2+((n+IF_INT_64_OR_32(7,3))>>IF_INT_64_OR_32(3,2)));"
	, "hp[0]=(BC_WORD)&__STRING__+2;"
	, "hp[1]=n;"
	, "memcpy(&hp[2],r,n);"
	, "pc+=1;"
	, "bsp+=1;"
	, "asp[1]=(BC_WORD)hp;"
	, "asp+=1;"
	, "hp+=2+((n+IF_INT_64_OR_32(7,3))>>IF_INT_64_OR_32(3,2));"
	, "}"
	]

instr_load_i :: !Target -> Target
instr_load_i t = foldl (flip append) t
	[ "bsp[0]=*(BC_WORD*)(bsp[0]+pc[1]);"
	, "pc+=2;"
	]

instr_load_si16 :: !Target -> Target
instr_load_si16 t = foldl (flip append) t
	[ "bsp[0]=*(short*)(bsp[0]+pc[1]);"
	, "pc+=2;"
	]

instr_load_si32 :: !Target -> Target
instr_load_si32 t = foldl (flip append) t
	[ "bsp[0]=*(int*)(bsp[0]+pc[1]);"
	, "pc+=2;"
	]

instr_load_ui8 :: !Target -> Target
instr_load_ui8 t = foldl (flip append) t
	[ "bsp[0]=*(unsigned char*)(bsp[0]+pc[1]);"
	, "pc+=2;"
	]

instr_closeF :: !Target -> Target
instr_closeF t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "bsp++;"
	, "if (f==&clean_stdinout) {"
	, "\tif (!stdio_open)"
	, "\t\tIO_error(\"fclose: file not open (stdio)\");"
	, "\tstdio_open=0;"
	, "\tbsp[0]=(ferror(stdin) || ferror(stdout)) ? 0 : 1;"
	, "} else if (f==&clean_stderr) {"
	, "\tbsp[0]=1;"
	, "} else {"
	, "\tbsp[0]=fclose(f->file_handle) ? 0 : 1;"
	, "\tfree(f);"
	, "}"
	, "}"
	]

instr_endF :: !Target -> Target
instr_endF t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "FILE *h;"
	, "if (f==&clean_stdinout)"
	, "\th=stdin;"
	, "else if (f==&clean_stderr)"
	, "\tIO_error(\"endF stderr not implemented\");"
	, "else"
	, "\th=f->file_handle;"
	, "char c=getc(h);"
	, "if (c==EOF) {"
	, "\t*--bsp=1;"
	, "} else {"
	, "\tungetc(c,h);"
	, "\t*--bsp=0;"
	, "}"
	, "}"
	]

instr_errorF :: !Target -> Target
instr_errorF t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "if (f==&clean_stdinout)"
	, "\t*--bsp=(ferror(stdin) || ferror(stdout)) ? 1 : 0;"
	, "else if (f==&clean_stderr)"
	, "\t*--bsp=ferror(stderr) ? 1 : 0;"
	, "else"
	, "\t*--bsp=ferror(f->file_handle) ? 1 : 0;"
	, "}"
	]

instr_flushF :: !Target -> Target
instr_flushF t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "if (f==&clean_stdinout)"
	, "\t*--bsp=fflush(stdout) ? 0 : 1;"
	, "else if (f==&clean_stderr)"
	, "\t*--bsp=fflush(stdin) ? 0 : 1;"
	, "else"
	, "\t*--bsp=fflush(f->file_handle) ? 0 : 1;"
	, "}"
	]

instr_openF :: !Target -> Target
instr_openF t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=safe_malloc(sizeof(struct file));"
	, "BC_WORD *clean_file_name=(BC_WORD*)asp[0];"
	, "asp--;"
	, "char *file_name=safe_malloc(clean_file_name[1]+1);"
	, "memcpy(file_name,&clean_file_name[2],clean_file_name[1]);"
	, "file_name[clean_file_name[1]]='\\0';"
	, "if (bsp[0]>=6)"
	, "\tIO_error(\"openF: illegal mode\");"
	, "char *mode=file_modes[bsp[0]];"
	, "if (mode==NULL)"
	, "\tIO_error(\"openF: unimplemented mode\");"
	, "f->file_handle=fopen(file_name,mode);"
	, "f->file_mode=bsp[0];"
	, "bsp-=2;"
	, "bsp[1]=0;"
	, "if (f->file_handle==NULL) {"
	, "\tbsp[0]=0;" // not ok
	, "\tfree(f);"
	, "} else {"
	, "\tbsp[0]=1;" // ok
	, "\tbsp[2]=(BC_WORD)f;"
	, "}"
	, "free(file_name);"
	, "}"
	]

instr_positionF :: !Target -> Target
instr_positionF t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "if (f==&clean_stdinout || f==&clean_stderr)"
	, "\tIO_error(\"FPosition: not allowed for StdIO and StdErr\");"
	, "else"
	, "\t*--bsp=ftell(f->file_handle);"
	, "}"
	]

instr_readFC :: !Target -> Target
instr_readFC t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "bsp-=2;"
	, "if (f==&clean_stdinout) {"
	, "\tbsp[1]=getchar();"
	, "\tbsp[0]=ferror(stdin) ? 0 : 1;"
	, "} else if (f==&clean_stderr) {"
	, "\tIO_error(\"FReadC: can't read from StdErr\");"
	, "} else {"
	, "\tbsp[1]=getc(f->file_handle);"
	, "\tbsp[0]=ferror(f->file_handle) ? 0 : 1;"
	, "}"
	, "}"
	]

instr_readFI :: !Target -> Target
instr_readFI t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "bsp-=2;"
	, "if (f==&clean_stdinout) {"
	, "\tbsp[0]=fscanf(stdin,BC_WORD_S_FMT,(BC_WORD_S*)&bsp[1])==1;"
	, "} else if (f==&clean_stderr) {"
	, "\tIO_error(\"FReadI: can't read from StdErr\");"
	, "} else if (F_IS_TEXT_MODE(f->file_mode)) {"
	, "\tbsp[0]=fscanf(f->file_handle,BC_WORD_S_FMT,(BC_WORD_S*)&bsp[1])==1;"
	, "} else {"
	, "\tint i;"
	, "\tbsp[0]=1;"
	, "\tbsp[1]=0;"
	, "\tfor (int n=0; n<4; n++) {"
	, "\t\tif ((i=fgetc(f->file_handle))==EOF) break;"
	, "\t\t((char*)&bsp[1])[n]=i;"
	, "\t}"
	, "#if WORD_WIDTH == 64"
	, "\tbsp[1]=*(int*)&bsp[1];"
	, "#endif"
	, "}"
	, "}"
	]

instr_readFR :: !Target -> Target
instr_readFR t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "bsp-=2;"
	, "if (f==&clean_stdinout) {"
	, "\tbsp[0]=fscanf(stdin,BC_REAL_SCAN_FMT,(BC_REAL*)&bsp[1])==1;"
	, "} else if (f==&clean_stderr) {"
	, "\tIO_error(\"FReadI: can't read from StdErr\");"
	, "} else if (F_IS_TEXT_MODE(f->file_mode)) {"
	, "\tbsp[0]=fscanf(f->file_handle,BC_REAL_SCAN_FMT,(BC_REAL*)&bsp[1])==1;"
	, "} else {"
	, "\tint i;"
	, "\tbsp[0]=1;"
	, "\t*(BC_REAL*)&bsp[1]=0.0;"
	, "\tfor (int n=0; n<8; n++) {"
	, "\t\tif ((i=fgetc(f->file_handle))==EOF) break;"
	, "\t\t((char*)&bsp[1])[n]=i;"
	, "\t}"
	, "}"
	, "}"
	]

instr_readFS :: !Target -> Target
instr_readFS t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "BC_WORD len=bsp[2];"
	, "NEED_HEAP(len);"
	, "bsp[2]=bsp[1];"
	, "bsp[1]=bsp[0];"
	, "bsp++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "pc++;"
	, "*++asp=(BC_WORD)hp;"
	, "hp[0]=(BC_WORD)&__STRING__+2;"
	, "if (f==&clean_stdinout) {"
	, "\thp[1]=fread((char*)&hp[2],1,len,stdin);"
	, "} else if (f==&clean_stderr) {"
	, "\tIO_error(\"FReadS: can't read from StdErr\");"
	, "} else {"
	, "\thp[1]=fread((char*)&hp[2],1,len,f->file_handle);"
	, "}"
	, "hp+=2+((hp[1]+7)>>3);"
	, "}"
	]

instr_readLineF :: !Target -> Target
instr_readLineF t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "struct file *f=(struct file*)bsp[1];"
	, "if (f==&clean_stdinout) {"
	, "\thp[0]=(BC_WORD)&__STRING__+2;"
	, "\tBC_WORD max_bytes=(heap_free-2)<<IF_INT_64_OR_32(3,2);"
	, "\tchar *dest=(char*)&hp[2];"
	, "\tif (last_readLineF_failed) {"
	, "\t\tBC_WORD *old_string=(BC_WORD*)asp[0];"
	, "\t\tmemcpy(dest,&old_string[2],old_string[1]);"
	, "\t\tdest+=old_string[1];"
	, "\t\tasp[0]=(BC_WORD)hp;"
	, "\t} else {"
	, "\t\tasp[1]=(BC_WORD)hp;"
	, "\t\tasp++;"
	, "\t}"
	, "\thp[1]=clean_get_line((char*)dest,max_bytes);"
	, "\tif (hp[1]==-1) {"
	, "\t\thp[1]=max_bytes;"
	, "\t\theap_free=0;"
	, "\t\tlast_readLineF_failed=1;"
	, "\t\tGARBAGE_COLLECT;"
	, "\t} else {"
	, "\t\tBC_WORD words_used=2+((hp[1]+IF_INT_64_OR_32(7,3))>>IF_INT_64_OR_32(3,2));"
	, "\t\thp+=words_used;"
	, "\t\theap_free-=words_used;"
	, "\t}"
	, "\tpc++;"
	, "\tlast_readLineF_failed=0;"
	, "} else if (f==&clean_stderr) {"
	, "\tIO_error(\"freadline: can't read from stderr\");"
	, "} else {"
	, "\tIO_error(\"readLineF fallthrough\");" // TODO
	, "}"
	, "}"
	]

instr_seekF :: !Target -> Target
instr_seekF t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "unsigned int seek_mode=bsp[3];"
	, "int position=bsp[2];"
	, "if (seek_mode>2)"
	, "\tIO_error(\"FSeek: invalid mode\");"
	, "struct file *f=(struct file*)bsp[1];"
	, "bsp[3]=bsp[1];"
	, "bsp[2]=bsp[0];"
	, "bsp++;"
	, "if (f==&clean_stdinout || f==&clean_stderr)"
	, "\tIO_error(\"FSeek: can't seek on StdIO and StdErr\");"
	, "else"
	, "\tbsp[0]=fseek(f->file_handle,position,seek_mode) ? 0 : 1;"
	, "}"
	]

instr_stderrF :: !Target -> Target
instr_stderrF t = foldl (flip append) t
	[ "CHECK_FILE_IO;"
	, "pc+=1;"
	, "bsp[-1]=(BC_WORD)&clean_stderr;"
	, "bsp[-2]=-1;"
	, "bsp-=2;"
	]

instr_stdioF :: !Target -> Target
instr_stdioF t = foldl (flip append) t
	[ "CHECK_FILE_IO;"
	, "if (stdio_open)"
	, "\tIO_error(\"stdio: already open\");"
	, "pc+=1;"
	, "stdio_open=1;"
	, "bsp[-1]=(BC_WORD)&clean_stdinout;"
	, "bsp[-2]=-1;"
	, "bsp-=2;"
	]

instr_writeFC :: !Target -> Target
instr_writeFC t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "char c=*bsp++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "if (f==&clean_stdinout)"
	, "\tPUTCHAR(c);"
	, "else if (f==&clean_stderr)"
	, "\tEPUTCHAR(c);"
	, "else"
	, "\tputc(c,f->file_handle);"
	, "}"
	]

instr_writeFI :: !Target -> Target
instr_writeFI t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "BC_WORD_S i=*bsp++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "if (f==&clean_stdinout)"
	, "\tPRINTF(BC_WORD_S_FMT,i);"
	, "else if (f==&clean_stderr)"
	, "\tEPRINTF(BC_WORD_S_FMT,i);"
	, "else if (F_IS_TEXT_MODE(f->file_mode))"
	, "\tfprintf(f->file_handle,BC_WORD_S_FMT,i);"
	, "else {"
	, "\tputc(((char*)&i)[0],f->file_handle);"
	, "\tputc(((char*)&i)[1],f->file_handle);"
	, "\tputc(((char*)&i)[2],f->file_handle);"
	, "\tputc(((char*)&i)[3],f->file_handle);"
	, "}"
	, "}"
	]

instr_writeFR :: !Target -> Target
instr_writeFR t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "pc++;"
	, "BC_REAL r=0.0 + *(BC_REAL*)bsp;"
	, "bsp++;"
	, "struct file *f=(struct file*)bsp[1];"
	, "if (f==&clean_stdinout)"
	, "\tPRINTF(BC_REAL_FMT,r);"
	, "else if (f==&clean_stderr)"
	, "\tEPRINTF(BC_REAL_FMT,r);"
	, "else if (F_IS_TEXT_MODE(f->file_mode))"
	, "\tfprintf(f->file_handle,BC_REAL_FMT,r);"
	, "else {"
	, "\tBC_WORD i=bsp[-1];"
	, "\tputc(((char*)&i)[0],f->file_handle);"
	, "\tputc(((char*)&i)[1],f->file_handle);"
	, "\tputc(((char*)&i)[2],f->file_handle);"
	, "\tputc(((char*)&i)[3],f->file_handle);"
	, "\tputc(((char*)&i)[4],f->file_handle);"
	, "\tputc(((char*)&i)[5],f->file_handle);"
	, "\tputc(((char*)&i)[6],f->file_handle);"
	, "\tputc(((char*)&i)[7],f->file_handle);"
	, "}"
	, "}"
	]

instr_writeFS :: !Target -> Target
instr_writeFS t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "struct file *f=(struct file*)bsp[1];"
	, "BC_WORD *n=(BC_WORD*)asp[0];"
	, "int len=n[1];"
	, "char *s=(char*)&n[2];"
	, "pc++;"
	, "asp--;"
	, "if (f==&clean_stdinout)"
	, "\tfor (;len;len--) PUTCHAR(*s++);"
	, "else if (f==&clean_stderr)"
	, "\tfor (;len;len--) EPUTCHAR(*s++);"
	, "else"
	, "\tfwrite(s,1,len,f->file_handle);"
	, "}"
	]

instr_writeFString :: !Target -> Target
instr_writeFString t = foldl (flip append) t
	[ "{"
	, "CHECK_FILE_IO;"
	, "struct file *f=(struct file*)bsp[3];"
	, "BC_WORD *n=(BC_WORD*)asp[0];"
	, "BC_WORD start=bsp[0];"
	, "BC_WORD len=bsp[1];"
	, "if (start+len>n[1])"
	, "\tIO_error(\"Error in fwritesubstring parameters.\");"
	, "bsp+=2;"
	, "pc++;"
	, "asp--;"
	, "char *s=(char*)&n[2]+start;"
	, "if (f==&clean_stdinout)"
	, "\tfor (;len;len--) PUTCHAR(*s++);"
	, "else if (f==&clean_stderr)"
	, "\tfor (;len;len--) EPUTCHAR(*s++);"
	, "else"
	, "\tfwrite(s,1,len,f->file_handle);" // TODO
	, "}"
	]
