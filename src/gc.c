#include <stdlib.h>
#include <string.h>

#include "gc.h"

#if DEBUG_GARBAGE_COLLECTOR > 0
# include "util.h"
#endif

#ifdef LINK_CLEAN_RUNTIME
# define UPDATE_REF update_ref
#else
# define UPDATE_REF(old,heap_size,ref,hp,snoh) update_ref(old,heap_size,ref,hp)
#endif

static inline BC_WORD *update_ref(BC_WORD *old, size_t heap_size,
		BC_WORD **ref, BC_WORD *hp
#ifdef LINK_CLEAN_RUNTIME
		, BC_WORD **shared_nodes_of_host
#endif
		) {
	BC_WORD *n,d;
	int32_t arity;

	n=*ref;

	if (!(old<=n && n<old+heap_size))
		return hp;

	d=n[0];
	if (d & 1) {
		*ref=(BC_WORD*)(d-1);
		return hp;
	}

	*ref=hp;

#if DEBUG_GARBAGE_COLLECTOR > 1
	EPRINTF ("%p <- %p (d %p)\n",hp,n,(void*)d);
#endif

	if (d & 2) {
		if (d == (BC_WORD)&INT+2) {
			if (n[1]<33) {
				BC_WORD *shared_node=&small_integers[n[1]*2];
				*ref=shared_node;
				*n=(BC_WORD)shared_node+1;
				return hp;
			} else {
				hp[0]=d;
				hp[1]=n[1];
				*n=(BC_WORD)hp+1;
				return &hp[2];
			}
		} else if (d == (BC_WORD)&BOOL+2) {
			BC_WORD *shared_node=&static_booleans[n[1] ? 2 : 0];
			*ref=shared_node;
			*n=(BC_WORD)shared_node+1;
			return hp;
		} else if (d == (BC_WORD)&CHAR+2) {
			BC_WORD *shared_node=&static_characters[(n[1]&0xff)<<1];
			*ref=shared_node;
			*n=(BC_WORD)shared_node+1;
			return hp;
		} else if (d == (BC_WORD)&REAL+2) {
			hp[0]=d;
			hp[1]=n[1];
			*n=(BC_WORD)hp+1;
			return &hp[2];
		} else if (d == (BC_WORD)&__STRING__+2) {
			unsigned int size=n[1];
			size=(size+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
			memcpy (hp,n,(size+2)*sizeof(BC_WORD));
			*n=(BC_WORD)hp+1;
			return &hp[2+size];
		} else if (d == (BC_WORD)&__ARRAY__+2) {
			unsigned int size=n[1];
			d=n[2];

			if (d == (BC_WORD)&INT+2
				|| d == (BC_WORD)&REAL+2
				|| d == 0) {
				/* size is correct */
			} else if (d == (BC_WORD)&BOOL+2) {
				size=(size+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
			} else {
				size*=(((int16_t*)d)[-1]-256);
			}

			memcpy (hp,n,(size+3)*sizeof(BC_WORD));
			*n=(BC_WORD)hp+1;
			return &hp[3+size];
		}

#ifdef LINK_CLEAN_RUNTIME
		if ((BC_WORD)HOST_NODE_DESCRIPTORS <= d &&
				d <= (BC_WORD)HOST_NODE_DESCRIPTORS+sizeof(HOST_NODE_DESCRIPTORS)) {
			int host_nodeid=((BC_WORD*)n[1])[1];
			shared_nodes_of_host[host_nodeid]=(BC_WORD*)((BC_WORD)shared_nodes_of_host[host_nodeid]|1);
		}
#endif

		/* Not a built-in type */
		arity=((int16_t*)d)[-1];
		if (arity>256) /* records */
			arity-=256;
#if DEBUG_GARBAGE_COLLECTOR > 2
		EPRINTF ("\thnf %d\n",arity);
#endif

		if (arity==0) {
			BC_WORD *shared_node=(BC_WORD*)(d-2)-1;
			*ref=shared_node;
			*n=(BC_WORD)shared_node+1;
			return hp;
		}

		*n=(BC_WORD)hp+1;
		hp[0]=d;
		hp[1]=n[1];
		if (arity>2) { /* hnf spread over two blocks */
			hp[2]=(BC_WORD)&hp[3];
			hp+=3;
			n=(BC_WORD*)n[2];
			arity--;
			for (int i=0; i<arity; i++)
				hp[i]=n[i];
			return &hp[arity];
		} else {
			hp[2]=n[2];
			return &hp[arity+1];
		}
	} else {
		arity=((int32_t*)d)[-1];
#if DEBUG_GARBAGE_COLLECTOR > 2
		EPRINTF ("\tthunk %d\n",arity);
#endif
		if (arity<0) /* negative for selectors etc. */
			arity=1;
		arity&=0xff;

#ifdef LINK_CLEAN_RUNTIME
		if (d == (BC_WORD)&HOST_NODE_INSTRUCTIONS[1]) {
			int host_nodeid=n[1];
			shared_nodes_of_host[host_nodeid]=(BC_WORD*)((BC_WORD)shared_nodes_of_host[host_nodeid]|1);
		}
#endif

		for (int i=0; i<=arity; i++)
			hp[i]=n[i];
		*n=(BC_WORD)hp+1;

		return &hp[arity>2 ? (arity+1) : 3];
	}
}

BC_WORD *garbage_collect(BC_WORD *stack, BC_WORD *asp,
		BC_WORD *heap, size_t heap_size, BC_WORD_S *heap_free,
		void **cafs, struct interpretation_options *options
#ifdef LINK_CLEAN_RUNTIME
		, BC_WORD **shared_nodes_of_host
#endif
#ifdef DEBUG_GARBAGE_COLLECTOR
		, BC_WORD *code, BC_WORD *data
#endif
		) {
#if DEBUG_GARBAGE_COLLECTOR > 0
	EPRINTF ("Collecting trash... stack @ %p; heap @ %p; code @ %p; data @ %p\n",
			(void*) stack, (void*) heap, (void*) code, (void*) data);
#endif

	BC_WORD *old=options->in_first_semispace ? heap : heap+heap_size;
	BC_WORD *new=options->in_first_semispace ? heap+heap_size : heap;

	BC_WORD *n,d;
	int32_t arity,a_arity;

#if DEBUG_GARBAGE_COLLECTOR > 1
	EPRINTF ("Copying A stack roots...\n");
#endif
	for (;
#ifdef LINK_CLEAN_RUNTIME
			asp>=stack;
#else
			asp!=stack;
#endif
			asp--)
		new=UPDATE_REF (old,heap_size,(BC_WORD**)asp,new,shared_nodes_of_host);

#if DEBUG_GARBAGE_COLLECTOR > 1
	EPRINTF ("Copying CAF roots...\n");
#endif
	BC_WORD **cafptr=(BC_WORD**)&cafs[1];
	while (cafptr[-1]!=0) {
		cafptr=(BC_WORD**)cafptr[-1];
		for (a_arity=(int16_t)(BC_WORD)cafptr[0]; a_arity>0; a_arity--)
			new=UPDATE_REF (old,heap_size,(BC_WORD**)&cafptr[a_arity],new,shared_nodes_of_host);
	}

#ifdef LINK_CLEAN_RUNTIME
#if DEBUG_GARBAGE_COLLECTOR > 1
	EPRINTF ("Copying shared node roots (%p)...\n",interpreter_finalizer);
#endif
	struct finalizers *finalizers=NULL;
	while ((finalizers=next_interpreter_finalizer(finalizers)) != NULL) {
		BC_WORD ref=finalizers->cur->arg;
		if (ref & 2) { /* hyperstrict reference */
			ref-=2;
			new=UPDATE_REF (old,heap_size,(BC_WORD**)&ref,new,shared_nodes_of_host);
			finalizers->cur->arg=ref+2;
		} else {
			new=UPDATE_REF (old,heap_size,(BC_WORD**)&finalizers->cur->arg,new,shared_nodes_of_host);
		}
	}
#endif

#if DEBUG_GARBAGE_COLLECTOR > 1
	EPRINTF ("Breadth-first search of child nodes...\n");
#endif
	n=options->in_first_semispace ? heap+heap_size : heap;
	while (n<new) {
		d=n[0];
#if DEBUG_GARBAGE_COLLECTOR > 1
		EPRINTF ("%p Update %p (d %p)\n",new,n,(void*)d);
#endif

		if (d & 2) {
			if (d == (BC_WORD)&INT+2 || d == (BC_WORD)&REAL+2) {
				/* CHAR and BOOL are always unboxed */
				n+=2;
				continue;
			} else if (d == (BC_WORD)&__STRING__+2) {
				unsigned int size=n[1];
				size=(size+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
				n=&n[2+size];
				continue;
			} else if (d == (BC_WORD)&__ARRAY__+2) {
				unsigned int size;
				size=n[1];
				d=n[2];

				if (d == 0) {
					arity=a_arity=1;
				} else if (d == (BC_WORD)&INT+2
					|| d == (BC_WORD)&REAL+2) {
					n=&n[3+size];
					continue;
				} else if (d == (BC_WORD)&BOOL+2) {
					size=(size+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
					n=&n[3+size];
					continue;
				} else {
					arity=((int16_t*)d)[-1]-256;
					a_arity=((int16_t*)d)[0];
				}

				if (a_arity==0) {
					n=&n[3+size*arity];
					continue;
				}

				n=&n[3];
				for (; size>0; size--) {
					for (d=0; d<a_arity; d++)
						new=UPDATE_REF (old,heap_size,(BC_WORD**)&n[d],new,shared_nodes_of_host);
					n=&n[arity];
				}

				continue;
			}

			/* Not a built-in type */
			a_arity=arity=((int16_t*)d)[-1];
			if (arity>256) { /* records */
				arity-=256;
				a_arity=((int16_t*)d)[0];
			}
			if (arity>2) { /* hnf spread over two blocks */
				if (a_arity>0) {
					new=UPDATE_REF (old,heap_size,(BC_WORD**)&n[1],new,shared_nodes_of_host);
					while (--a_arity)
						new=UPDATE_REF (old,heap_size,(BC_WORD**)&n[2+a_arity],new,shared_nodes_of_host);
				}
				n+=2+arity;
				continue;
			} else {
				if (a_arity>0) {
					new=UPDATE_REF (old,heap_size,(BC_WORD**)&n[1],new,shared_nodes_of_host);
					if (a_arity==2)
						new=UPDATE_REF (old,heap_size,(BC_WORD**)&n[2],new,shared_nodes_of_host);
				}
				n+=1+arity;
				continue;
			}
		} else {
			arity=((int32_t*)d)[-1];
			if (arity<0) { /* negative for selectors etc. */
				arity=1;
				a_arity=1;
			} else {
				a_arity=(arity & 0xff)-((arity>>8)&0xff);
				arity&=0xff;
			}

			for (int i=1; i<=a_arity; i++)
				new=UPDATE_REF (old,heap_size,(BC_WORD**)&n[i],new,shared_nodes_of_host);

			n=&n[arity>2 ? (arity+1) : 3];
			continue;
		}
	}

#ifdef LINK_CLEAN_RUNTIME
# if DEBUG_GARBAGE_COLLECTOR > 1
	EPRINTF ("Clean up shared nodes array...\n");
# endif
	extern BC_WORD __Nil;
	for (int host_nodeid=(BC_WORD)shared_nodes_of_host[-2]-1; host_nodeid>=0; host_nodeid--) {
		if ((BC_WORD)shared_nodes_of_host[host_nodeid]&1)
			shared_nodes_of_host[host_nodeid]=(BC_WORD*)((BC_WORD)shared_nodes_of_host[host_nodeid]-1);
		else
			shared_nodes_of_host[host_nodeid]=&__Nil-1;
	}
#endif

	*heap_free=heap+(options->in_first_semispace ? 2 : 1)*heap_size-new;
	options->in_first_semispace=1-options->in_first_semispace;

	return new;
}
