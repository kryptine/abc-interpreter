#ifdef LINK_CLEAN_RUNTIME
INSTRUCTION_BLOCK(jsr_eval_host_node):
{
	BC_WORD *n=(BC_WORD*)asp[0];
	int host_nodeid=n[1];
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
		host_node=__interpret__evaluate__host(ie, host_node);
		ie->host->clean_ie=(struct InterpretationEnvironment*)*--ie->host->host_a_ptr;
		asp=ie->asp;
		bsp=ie->bsp;
		csp=ie->csp;
		hp=ie->hp;
#if DEBUG_CLEAN_LINKS > 1
		EPRINTF("\tnew node after evaluation: %p -> %p\n",host_node,(void*)*host_node);
#endif
	} else {
		/* needs to be up to date for copying */
		ie->asp=asp;
		ie->hp=hp;
	}

	BC_WORD *new_n;
	int words_used=copy_to_interpreter_or_garbage_collect(ie, &new_n, host_node);
	if (words_used<0) {
		EPRINTF("Interpreter is out of memory\n");
		interpret_error=&e__ABC_PInterpreter__dDV__HeapFull;
		goto eval_to_hnf_return_failure;
	}
	n=(BC_WORD*)asp[0]; /* may have been updated due to garbage collection */
	n[0]=new_n[0];
	n[1]=new_n[1];
	n[2]=new_n[2];
	hp=ie->hp+words_used;
	heap_free = heap + (ie->options.in_first_semispace ? 1 : 2) * heap_size - hp;

	pc=(BC_WORD*)*csp--;
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
		EXIT(NULL,-1);
	}

#if DEBUG_CLEAN_LINKS > 1
	EPRINTF("\thost node (%d: %p -> %p) arity is %d; %d needed\n",
			host_nodeid,host_node,(void*)*host_node,node_arity,args_needed);
	for (int i = 1; i <= instr_arg; i++)
		EPRINTF("\targ %d: %p -> %p\n",i,(void*)asp[-i],*(void**)asp[-i]);
#endif

	*ie->host->host_a_ptr++=(BC_WORD)ie->host->clean_ie;

	for (int i=instr_arg; i>=1; i--) {
		int added_words=copy_to_host_or_garbage_collect(ie,
				(BC_WORD**)ie->host->host_a_ptr++, (BC_WORD*)asp[-i], 0);
		if (added_words<0) {
			EPRINTF("copying to host failed\n");
			EXIT(ie,1);
			goto eval_to_hnf_return_failure;
		}
		ie->host->host_hp_ptr+=added_words;
		ie->host->host_hp_free-=added_words;
	}
	BC_WORD *arg1, *arg2 = NULL;
	arg1 = (BC_WORD*) *--ie->host->host_a_ptr;
	if (instr_arg >= 2)
		arg2 = (BC_WORD*) *--ie->host->host_a_ptr;

	asp-=instr_arg;
	*asp=((BC_WORD)&d___Nil[1])-IF_INT_64_OR_32(8,4);
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

	ie->host->clean_ie=(struct InterpretationEnvironment*)*--ie->host->host_a_ptr;

	asp=ie->asp;
	bsp=ie->bsp;
	csp=ie->csp;
	hp=ie->hp;
	int words_used=copy_to_interpreter_or_garbage_collect(ie, (BC_WORD**)asp, host_node);
	if (words_used<0) {
		EPRINTF("Interpreter is out of memory\n");
		interpret_error=&e__ABC_PInterpreter__dDV__HeapFull;
		goto eval_to_hnf_return_failure;
	}
	hp=ie->hp+words_used;
	heap_free = heap + (ie->options.in_first_semispace ? 1 : 2) * heap_size - hp;

	pc=(BC_WORD*)*csp--;
	END_INSTRUCTION_BLOCK;
}
#endif
