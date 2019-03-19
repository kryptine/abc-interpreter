#include <stdlib.h>
#include <string.h>

#include "gc.h"

#if DEBUG_GARBAGE_COLLECTOR > 0
# include "util.h"
#endif

static inline BC_WORD *update_ref(BC_WORD *old, size_t heap_size,
		BC_WORD **ref, BC_WORD *hp) {
	BC_WORD *n,d;
	int16_t ab_arity;

	n=*ref;

	if (!(old<=n && n<old+heap_size))
		return hp;

	d=n[0];
	if (d & 1) {
		*ref=(BC_WORD*)(d-1);
		return hp;
	}

	*ref=hp;

	hp[0]=d;
	*n=(BC_WORD)hp+1;

#if DEBUG_GARBAGE_COLLECTOR > 1
	EPRINTF ("%p <- %p (d %p)\n",hp,n,(void*)d);
#endif

	if (d & 2) {
		if (d == (BC_WORD)&INT+2
				|| d == (BC_WORD)&BOOL+2
				|| d == (BC_WORD)&CHAR+2
				|| d == (BC_WORD)&REAL+2) {
			hp[1]=n[1];
			return &hp[2];
		} else if (d == (BC_WORD)&__STRING__+2) {
			unsigned int size=n[1];
			size=(size+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
			memcpy (&hp[1],&n[1],(size+1)*sizeof(BC_WORD));
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

			memcpy (&hp[1],&n[1],(size+2)*sizeof(BC_WORD));
			return &hp[3+size];
		}

		/* Not a built-in type */
		ab_arity=((int16_t*)d)[-1];
		if (ab_arity>256) /* records */
			ab_arity-=256;
#if DEBUG_GARBAGE_COLLECTOR > 2
		EPRINTF ("\thnf %d\n",ab_arity);
#endif
		hp[1]=n[1];
		if (ab_arity>2) { /* hnf spread over two blocks */
			hp[2]=(BC_WORD)&hp[3];
			hp+=3;
			n=(BC_WORD*)n[2];
			ab_arity--;
			for (int i=0; i<ab_arity; i++)
				hp[i]=n[i];
			return &hp[ab_arity];
		} else {
			hp[2]=n[2];
			return &hp[ab_arity+1];
		}
	} else {
		ab_arity=((int16_t*)d)[-1];
#if DEBUG_GARBAGE_COLLECTOR > 2
		EPRINTF ("\tthunk %d\n",ab_arity);
#endif
		if (ab_arity<0) /* negative for selectors etc. */
			ab_arity=1;
		ab_arity&=0xff;

		for (int i=1; i<=ab_arity; i++)
			hp[i]=n[i];

		return &hp[ab_arity>2 ? (ab_arity+1) : 3];
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
	int16_t ab_arity,a_arity,b_arity;

	for (; asp!=stack; asp--)
		new=update_ref (old,heap_size,(BC_WORD**)asp,new);

	BC_WORD **cafptr=(BC_WORD**)&cafs[1];
	while (cafptr[-1]!=0) {
		cafptr=(BC_WORD**)cafptr[-1];
		for (a_arity=(int16_t)(BC_WORD)cafptr[0]; a_arity>0; a_arity--)
			new=update_ref (old,heap_size,(BC_WORD**)&cafptr[a_arity],new);
	}

	n=options->in_first_semispace ? heap+heap_size : heap;
	while (n<new) {
		d=n[0];
#if DEBUG_GARBAGE_COLLECTOR > 1
		EPRINTF ("%p Update %p (d %p)\n",new,n,(void*)d);
#endif

		if (d & 2) {
			if (d == (BC_WORD)&INT+2
					|| d == (BC_WORD)&BOOL+2
					|| d == (BC_WORD)&CHAR+2
					|| d == (BC_WORD)&REAL+2) {
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
					ab_arity=a_arity=1;
				} else if (d == (BC_WORD)&INT+2
					|| d == (BC_WORD)&REAL+2) {
					n=&n[3+size];
					continue;
				} else if (d == (BC_WORD)&BOOL+2) {
					size=(size+IF_INT_64_OR_32(7,3))/IF_INT_64_OR_32(8,4);
					n=&n[3+size];
					continue;
				} else {
					ab_arity=((int16_t*)d)[-1]-256;
					a_arity=((int16_t*)d)[0];
				}

				if (a_arity==0) {
					n=&n[3+size*ab_arity];
					continue;
				}

				n=&n[3];
				for (; size>0; size--) {
					for (d=0; d<a_arity; d++)
						new=update_ref (old,heap_size,(BC_WORD**)&n[d],new);
					n=&n[ab_arity];
				}

				continue;
			}

			/* Not a built-in type */
			a_arity=ab_arity=((int16_t*)d)[-1];
			if (ab_arity>256) { /* records */
				ab_arity-=256;
				a_arity=((int16_t*)d)[0];
				b_arity=ab_arity-a_arity;
			}
			if (ab_arity>2) { /* hnf spread over two blocks */
				if (a_arity>0) {
					new=update_ref (old,heap_size,(BC_WORD**)&n[1],new);
					while (--a_arity)
						new=update_ref (old,heap_size,(BC_WORD**)&n[2+a_arity],new);
				}
				n+=2+ab_arity;
				continue;
			} else {
				if (a_arity>0) {
					new=update_ref (old,heap_size,(BC_WORD**)&n[1],new);
					if (a_arity==2)
						new=update_ref (old,heap_size,(BC_WORD**)&n[2],new);
				}
				n+=1+ab_arity;
				continue;
			}
		} else {
			ab_arity=((int16_t*)d)[-1];
			if (ab_arity<0) { /* negative for selectors etc. */
				ab_arity=1;
				a_arity=1;
			} else {
				b_arity=(ab_arity>>8) & 0xff;
				a_arity=(ab_arity & 0xff)-b_arity;
				ab_arity&=0xff;
			}

			for (int i=1; i<=a_arity; i++)
				new=update_ref (old,heap_size,(BC_WORD**)&n[i],new);

			n=&n[ab_arity>2 ? (ab_arity+1) : 3];
			continue;
		}
	}

	*heap_free=heap+(options->in_first_semispace ? 2 : 1)*heap_size-new;
	options->in_first_semispace=1-options->in_first_semispace;

	return new;
}
