"use strict";

var ABC_DEBUG=false;
var ABC_TRACE_LENGTH=50;

class ABCError extends Error {
	constructor(desc,arg) {
		super(desc);

		switch (desc) {
			case 'illegal instruction':
				this.message+=' '+arg+' ('+ABCInterpreter.instructions[arg]+')';
				break;
			case 'unknown instruction':
				this.message+=' '+arg;
				break;
		}
	}
}

class SharedCleanValue {
	constructor (index) {
		this.shared_clean_value_index=index;
	}
}

class ABCInterpreter {
	// Just to setup properties. New instances should be created with the static
	// method instantiate() below.
	constructor () {
		this.memory=null;
		this.memory_array=null;

		this.start=null;
		this.code_offset=null;

		this.stack_size=null;
		this.heap_size=null;

		this.util=null;
		this.interpreter=null;

		this.log_buffer='';

		this.js=[]; // javascript objects accessible from Clean
		this.empty_js_values=[]; // empty indexes in the above array
		this.addresses={
			JSInt:       0,
			JSBool:      0,
			JSString:    0,
			JSReal:      0,
			JSNull:      0,
			JSUndefined: 0,
			JSArray:     0,
			JSRef:       0,
			JSCleanRef:  0,
		};

		this.shared_clean_values=[]; // pointers to the Clean heap
		this.empty_shared_clean_values=[]; // empty indexes in the above array
	}

	log (s) {
		s=String(s);
		this.log_buffer+=s;
		if (s.indexOf('\n')>=0) {
			var lines=this.log_buffer.split('\n');
			for (var i=0; i<lines.length-1; i++)
				console.log(lines[i]);
			this.log_buffer=lines[lines.length-1];
		}
	}
	empty_log_buffer () {
		if (this.log_buffer.length>0)
			console.log(this.log_buffer);
	}

	require_hp (needed_words) {
		var free_words = this.interpreter.instance.exports.get_hp_free();

		// Each gc iteration may cause frees on the JS side , which may in turn
		// free more nodes in Clean. Therefore we run gc as long as the number of
		// free words decreases or until there is enough space. It will be possible
		// to do this much neater in the future when JS has weak references /
		// finalizers and/or when WebAssembly has GC access.
		while (free_words < needed_words) {
			console.warn('gc from js');
			this.util.instance.exports.gc(this.interpreter.instance.exports.get_asp());

			var new_free_words=this.interpreter.instance.exports.get_hp_free();
			if (new_free_words<=free_words)
				throw new ABCError('out of memory');
			free_words=new_free_words;
		}
	}

	_deserialize (addr, size) {
		const old_hp=this.interpreter.instance.exports.get_hp();
		const new_hp=this.util.instance.exports.copy_from_string(
			addr,
			size,
			this.interpreter.instance.exports.get_asp()+8,
			this.interpreter.instance.exports.get_bsp()-8,
			old_hp,
			this.code_offset*8);
		this.interpreter.instance.exports.set_hp(new_hp);

		const new_hp_free=this.interpreter.instance.exports.get_hp_free()-(new_hp-old_hp)/8;
		if (new_hp_free<0)
			throw 'hp_free was '+new_hp_free+' after deserialize: '+string;

		this.interpreter.instance.exports.set_hp_free(new_hp_free);

		return this.memory_array[addr/4];
	}
	deserialize_from_unique_string (str_ptr) {
		const size=this.memory_array[str_ptr/4+2];
		this.require_hp(size/8*4); // rough upper bound

		return this._deserialize(str_ptr+16, size/8);
	}
	deserialize (string) {
		const max_words_needed=string.length/8*4; // rough upper bound
		this.require_hp(max_words_needed);

		const array=new Uint8Array(string.length);
		for (var i=0; i<string.length; i++)
			array[i]=string.charCodeAt(i);
		const graph=new Uint32Array(array.buffer);

		const unused_semispace=this.util.instance.exports.get_unused_semispace();
		for (var i=0; i<graph.length; i++)
			this.memory_array[unused_semispace/4+i]=graph[i];

		return this._deserialize(unused_semispace, graph.length/2);
	}

	share_js_value (obj) {
		if (this.empty_js_values.length > 0) {
			const i=this.empty_js_values.pop();
			if (this.js[i]!=undefined)
				throw 'internal error in ABCInterpreter.share_js_value';
			this.js[i]=obj;
			return i;
		} else {
			this.js.push(obj);
			return this.js.length-1;
		}
	}

	ap (index) { // create a JavaScript closure to call the interpreter
		const me=this;
		var f=function () {
			var args=[];
			for (var i=0; i<arguments.length; i++)
				args[i]=arguments[i];
			me.interpret(new SharedCleanValue(index), args);
		};
		f.shared_clean_value_index=index;
		return f;
	}

	_copy_js_to_clean (values, store_ptrs, hp, hp_free) {
		for (var i=0; i<values.length; i++) {
			if (values[i]===null) {
				this.memory_array[store_ptrs/4]=this.addresses.JSNull-10;
			} else if (typeof values[i]=='undefined') {
				this.memory_array[store_ptrs/4]=this.addresses.JSUndefined-10;
			} else if (typeof values[i]=='number') {
				this.memory_array[store_ptrs/4]=hp;
				if (Number.isInteger(values[i])) {
					this.memory_array[hp/4]=this.addresses.JSInt;
					this.memory_array[hp/4+1]=0;
					if (values[i]>2**31 || values[i]<0-2**31) {
						if (typeof BigInt64Array!='undefined') {
							const bigint_array=new BigInt64Array(this.memory_array.buffer, hp+8);
							bigint_array[0]=BigInt(values[i]);
						} else {
							this.memory_array[hp/4+2]=values[i];
							this.memory_array[hp/4+3]=Math.floor(values[i] / 2**32); // NB: >> is 32-bit in JS, can't use it here
						}
					} else {
						this.memory_array[hp/4+2]=values[i];
						this.memory_array[hp/4+3]=0;
					}
				} else {
					this.memory_array[hp/4]=this.addresses.JSReal;
					this.memory_array[hp/4+1]=0;
					const float_array=new Float64Array(this.memory_array.buffer, hp+8);
					float_array[0]=values[i];
				}
				hp+=16;
				hp_free-=2;
			} else if (typeof values[i]=='boolean') {
				this.memory_array[store_ptrs/4]=hp;
				this.memory_array[hp/4]=this.addresses.JSBool;
				this.memory_array[hp/4+1]=0;
				this.memory_array[hp/4+2]=values[i] ? 1 : 0;
				this.memory_array[hp/4+3]=0;
				hp+=16;
				hp_free-=2;
			} else if (typeof values[i]=='string') {
				this.memory_array[store_ptrs/4]=hp;
				this.memory_array[hp/4]=this.addresses.JSString;
				this.memory_array[hp/4+1]=0;
				this.memory_array[hp/4+2]=hp+16;
				this.memory_array[hp/4+3]=0;
				hp+=16;
				hp_free-=2;
				this.memory_array[hp/4]=6*8+2; // _STRING_
				this.memory_array[hp/4+1]=0;
				this.memory_array[hp/4+2]=values[i].length;
				this.memory_array[hp/4+3]=0;
				var array=new Int8Array(((values[i].length+3)>>2)<<2);
				for (var j=0; j<values[i].length; j++)
					array[j]=values[i].charCodeAt(j);
				array=new Uint32Array(array.buffer);
				for (var j=0; j<((values[i].length+3)>>2); j++)
					this.memory_array[hp/4+4+j]=array[j];
				hp+=16+(((values[i].length+7)>>3)<<3);
				hp_free-=2+((values[i].length+7)>>3);
			} else if (Array.isArray(values[i])) {
				this.memory_array[store_ptrs/4]=hp;
				// On the first run, we don't have the JSArray address yet, so we use
				// the dummy 2 to ensure that jsr_eval won't try to evaluate it. The
				// array elements are unwrapped immediately, so the constructor does
				// not matter (apart from the fact that the HNF bit is set).
				this.memory_array[hp/4]=this.initialized ? this.addresses.JSArray : 2;
				this.memory_array[hp/4+1]=0;
				this.memory_array[hp/4+2]=hp+16;
				this.memory_array[hp/4+3]=0;
				hp+=16;
				hp_free-=2;
				this.memory_array[hp/4]=1*8+2; // _ARRAY_
				this.memory_array[hp/4+1]=0;
				this.memory_array[hp/4+2]=values[i].length;
				this.memory_array[hp/4+3]=0;
				this.memory_array[hp/4+4]=0;
				this.memory_array[hp/4+5]=0;
				hp+=24;
				hp_free-=3+values[i].length;;
				var copied=this._copy_js_to_clean(values[i], hp, hp+8*values[i].length, hp_free);
				hp=copied.hp;
				hp_free=copied.hp_free;
			} else if ('shared_clean_value_index' in values[i]) {
				this.memory_array[store_ptrs/4]=hp;
				this.memory_array[hp/4]=this.addresses.JSCleanRef;
				this.memory_array[hp/4+1]=0;
				this.memory_array[hp/4+2]=values[i].shared_clean_value_index;
				this.memory_array[hp/4+3]=0;
				hp+=16;
				hp_free-=2;
			} else if (typeof values[i]=='object' || typeof values[i]=='function') {
				this.memory_array[store_ptrs/4]=hp;
				this.memory_array[hp/4]=this.addresses.JSRef;
				this.memory_array[hp/4+1]=0;
				this.memory_array[hp/4+2]=this.share_js_value(values[i]);
				this.memory_array[hp/4+3]=0;
				hp+=16;
				hp_free-=2;
			} else { // should be handled by copied_node_size
				throw new ABCError('internal in copy_js_to_clean');
			}

			store_ptrs+=8;
		}

		return {
			hp: hp,
			hp_free: hp_free,
		};
	}
	copied_node_size (value) {
		if (value===null)
			return 0;
		else if (typeof value=='undefined')
			return 0;
		else if (typeof value=='number')
			return 2;
		else if (typeof value=='boolean')
			return 2;
		else if (typeof value=='string')
			return 2+2+((value.length+7)>>3);
		else if (Array.isArray(value)) {
			var size=2+3+value.length;
			for (var i=0; i<value.length; i++)
				size+=this.copied_node_size(value[i]);
			return size;
		} else if ('shared_clean_value_index' in value)
			return 2;
		else if (typeof value=='object' || typeof value=='function')
			return 2;
		else {
			console.error('Cannot pass this JavaScript value to Clean:',value);
			throw new ABCError('missing case in copy_js_to_clean');
		}
	}
	copy_js_to_clean (value, store_ptrs) {
		const node_size=this.copied_node_size(value);
		this.require_hp(node_size);
		const hp=this.interpreter.instance.exports.get_hp();
		const hp_free=this.interpreter.instance.exports.get_hp_free();

		const result=this._copy_js_to_clean([value], store_ptrs, hp, hp_free);

		if (hp_free-result.hp_free!=node_size)
			console.warn('copied_node_size: expected',node_size,'; got',hp_free-result.hp_free,'for',value);

		return result;
	}

	share_clean_value (ref, component) {
		if (typeof component.shared_clean_values=='undefined')
			throw 'could not attach shared Clean value to an iTasks component';
		if (component.shared_clean_values==null)
			component.shared_clean_values=new Set();

		const record={ref: ref, component: component};
		var i=null;

		if (this.empty_shared_clean_values.length > 0) {
			i=this.empty_shared_clean_values.pop();
			if (this.shared_clean_values[i]!=null)
				throw 'internal error in share_clean_value';
			this.shared_clean_values[i]=record;
		} else {
			i=this.shared_clean_values.length;
			this.shared_clean_values.push(record);
		}

		component.shared_clean_values.add(i);

		return i;
	}
	clear_shared_clean_value (ref, update_component=true) {
		const component=this.shared_clean_values[ref].component;
		if (update_component && typeof component.shared_clean_values!='undefined')
			component.shared_clean_values.delete(ref);

		this.shared_clean_values[ref]=null;
		this.empty_shared_clean_values.push(ref);
	}

	get_clean_string (hp_ptr, string_may_be_discarded=false) {
		const size=this.memory_array[hp_ptr/4+2];

		if (string_may_be_discarded) {
			// Try to clean up the Clean heap by discarding the string sent to JS.
			const hp=this.interpreter.instance.exports.get_hp();
			const string_bytes=16+(((size+7)>>3)<<3);
			if (hp_ptr+string_bytes==hp) {
				// The string is at the end of the heap. Simply move the heap pointer back.
				this.interpreter.instance.exports.set_hp(hp_ptr);
				this.interpreter.instance.exports.set_hp_free(this.interpreter.instance.exports.get_hp_free()+string_bytes/8);
			} else {
				const asp=this.interpreter.instance.exports.get_asp();
				if (hp_ptr+string_bytes+24==hp && this.memory_array[asp/4-2]==hp-24) {
					this.memory_array[asp/4-2]=hp_ptr;
					this.interpreter.instance.exports.set_hp(hp_ptr+24);
					this.interpreter.instance.exports.set_hp_free(this.interpreter.instance.exports.get_hp_free()+string_bytes/8);
				} else if (ABC_DEBUG) {
					console.warn('get_clean_string: could not clean up heap:',hp_ptr,hp,string_bytes);
				}
			}
		}

		const string_buffer=new Uint8Array(this.memory.buffer, hp_ptr+16, size);
		if (typeof TextDecoder!='undefined') {
			return new TextDecoder('x-user-defined').decode(string_buffer);
		} else {
			var string='';
			for (var i=0; i<size; i++)
				string+=String.fromCharCode(string_buffer[i]);
			return string;
		}
	}

	get_trace () {
		var trace=['  {0}',this.interpreter.instance.exports.get_pc()/8-this.code_offset,'\n'];
		var csp=this.interpreter.instance.exports.get_csp();
		for (var i=1; i<=ABC_TRACE_LENGTH; i++) {
			var addr=this.memory_array[csp/4];
			if (addr==0)
				break;
			trace.push('  {'+i+'}',addr/8-this.code_offset,'\n');
			csp-=8;
		}
		return trace;
	}

	interpret (f,args) {
		throw new ABCError('the interpreter has not been initialized yet');
	}

	static instantiate (args) {
		const opts={
			bytecode_path: null,
			util_path: '/js/abc-interpreter-util.wasm',
			interpreter_path: '/js/abc-interpreter.wasm',

			stack_size: 512<<10,
			heap_size: 2<<20,

			util_imports: {},
			interpreter_imports: {},
		};
		Object.assign(opts,args);

		const me=new ABCInterpreter();

		me.stack_size=opts.stack_size*2;
		me.heap_size=opts.heap_size;

		return fetch(opts.bytecode_path).then(function(resp){
			if (!resp.ok)
				throw new ABCError('failed to fetch bytecode');
			return resp.arrayBuffer();
		}).then(function(bytecode){
			const parse_prelinked_bytecode=function (prog, to_array=null) {
				var prog_offset=0;
				var words_needed=0;

				while (prog.length>0) {
					switch (prog[0]) {
						case 1: /* ST_Code */
							me.code_offset=words_needed;
						case 0: /* ST_Preamble */
						case 2: /* ST_Data */
							const words_in_section=prog[1]*2;
							if (to_array!=null)
								for (var k=0; k<words_in_section; k++)
									to_array[prog_offset+k]=prog[k+2];
							prog_offset+=words_in_section;
							words_needed+=prog[2];
							break;
						case 3: /* ST_Start */
							me.start=prog[2];
							break;
						default:
							throw new ABCError ('could not parse bytecode');
					}

					prog=prog.slice(2+2*prog[1]);
				}

				return words_needed;
			};

			bytecode=new Uint32Array(bytecode);

			me.words_needed_for_program=parse_prelinked_bytecode(bytecode);
			var data_size=me.stack_size+me.heap_size*2;
			if (data_size<bytecode.length/4)
				data_size=bytecode.length/4;
			const blocks_needed=Math.floor((me.words_needed_for_program*8 + data_size + 65535) / 65536);

			me.memory=new WebAssembly.Memory({initial: blocks_needed});
			me.memory_array=new Uint32Array(me.memory.buffer);

			parse_prelinked_bytecode(bytecode, new Uint32Array(me.memory.buffer,me.words_needed_for_program*8));

			const util_imports={
				clean: {
					memory: me.memory,

					has_host_reference: index => 0,
					update_host_reference: function (index, new_location) {
						throw new ABCError('update_host_reference should not be called')
					},

					gc_start: function() {
						me.active_js=[];
					},
					js_ref_found: function(ref) {
						me.active_js[ref]=true;
					},
					gc_end: function() {
						if (ABC_DEBUG)
							console.log(me.interpreter.instance.exports.get_hp_free(),'free words after gc');
						me.empty_js_values=[];
						// NB: we cannot reorder me.js, because garbage collection may be
						// triggered while computing a string to send to JavaScript which
						// can then contain illegal references.
						for (var i=0; i<me.js.length; i++) {
							if (typeof me.active_js[i]=='undefined') {
								delete me.js[i];
								me.empty_js_values.push(i);
							}
						}
						delete me.active_js;
					},

					set_hp: hp => me.interpreter.instance.exports.set_hp(hp),
					set_hp_free: free => me.interpreter.instance.exports.set_hp_free(free),

					debug: function(what,a,b,c) {
						if (!ABC_DEBUG)
							return;
						switch (what) {
							case 0: console.log('loop',a,'/',b,'; hp at',c); break;
							case 1: console.log('desc',a); break;
							case 2: console.log('hnf, arity',a); break;
							case 3: console.log('thunk, arities',a,b,c); break;
						}
					}
				}
			};
			Object.assign(util_imports.clean, opts.util_imports);

			return fetch(opts.util_path)
				.then(response => response.arrayBuffer())
				.then(buffer => WebAssembly.instantiate(buffer, util_imports));
		}).then(function(util){
			me.util=util;

			me.util.instance.exports.decode_prelinked_bytecode(me.words_needed_for_program*8);

			const interpreter_imports={
				clean: {
					memory: me.memory,

					debug_instr: function (addr, instr) {
						if (ABC_DEBUG)
							console.log(addr/8-me.code_offset,ABCInterpreter.instructions[instr]);
					},
					handle_illegal_instr: (pc, instr, asp, bsp, csp, hp, hp_free) => 0,
					illegal_instr: function (addr, instr) {
						me.empty_log_buffer();
						throw new ABCError('illegal instruction',instr);
					},
					out_of_memory: function () {
						me.empty_log_buffer();
						throw new ABCError('out of memory');
					},
					gc: util.instance.exports.gc,
					halt: function (pc, hp_free, heap_size) {
						me.empty_log_buffer();
						throw new ABCError('halt');
					},

					memcpy: util.instance.exports.memcpy,
					strncmp: util.instance.exports.strncmp,

					putchar: function (v) {
						me.log(String.fromCharCode(v));
					},
					print_int: function (high,low) {
						if ((high==0 && low>=0) || (high==-1 && low<0)) {
							me.log(low);
						} else if (typeof BigInt!='undefined') {
							var n=BigInt(high)*BigInt(2)**BigInt(32);
							if (low<0) {
								n+=BigInt(2)**BigInt(31);
								low+=2**31;
							}
							n+=BigInt(low);
							me.log(n);
						} else {
							console.warn('print_int: truncating 64-bit integer because this browser has no BigInt');
							me.log(low);
						}
					},
					print_bool: function (v) {
						me.log(v==0 ? 'False' : 'True');
					},
					print_char: function (v) {
						me.log("'"+String.fromCharCode(v)+"'");
					},
					print_real: function (v) {
						me.log(Number(0+v).toPrecision(15));
					},

					powR: Math.pow,
					acosR: Math.acos,
					asinR: Math.asin,
					atanR: Math.atan,
					cosR: Math.cos,
					sinR: Math.sin,
					tanR: Math.tan,
					expR: Math.exp,
					lnR: Math.log,
					log10R: Math.log10,
					RtoAC_words_needed: function(v) {
						v=Number(0+v).toPrecision(15);
						return 2+((v.length+7)>>3);
					},
					RtoAC: function (dest, v) {
						v=Number(0+v).toPrecision(15);
						me.memory_array[dest/4]=6*8+2; // __STRING__
						me.memory_array[dest/4+1]=0;
						me.memory_array[dest/4+2]=v.length;
						me.memory_array[dest/4+3]=0;
						const arr=new Uint8Array(me.memory_array.buffer, dest+16);
						for (var i=0; i<v.length; i++)
							arr[i]=v.charCodeAt(i);
						return dest+16+(((v.length+7)>>3)<<3);
					},
				}
			};
			Object.assign(interpreter_imports.clean, opts.interpreter_imports);

			return fetch(opts.interpreter_path)
				.then(response => response.arrayBuffer())
				.then(bytes => WebAssembly.instantiate(bytes, interpreter_imports));
		}).then(function(intp){
			me.interpreter=intp;

			const asp=Math.floor((me.words_needed_for_program*8+7)/8)*8;
			delete me.words_needed_for_program;
			const bsp=asp+me.stack_size;
			const csp=asp+me.stack_size/2;
			const hp=bsp+8;

			me.util.instance.exports.setup_gc(hp, me.heap_size, asp, 97*8);

			me.interpreter.instance.exports.set_asp(asp);
			me.interpreter.instance.exports.set_bsp(bsp);
			me.interpreter.instance.exports.set_csp(csp);
			me.interpreter.instance.exports.set_hp(hp);
			me.interpreter.instance.exports.set_hp_free(me.heap_size/8);
			me.interpreter.instance.exports.set_hp_size(me.heap_size/8);

			me.interpret=function (f, args) {
				const asp=me.interpreter.instance.exports.get_asp();
				const old_asp=asp;
				var hp=me.interpreter.instance.exports.get_hp();
				var hp_free=me.interpreter.instance.exports.get_hp_free();

				/* NB: the order here matters: copy_js_to_clean may trigger garbage
				 * collection, so do that first, then set the rest of the arguments and
				 * update asp. */
				const copied=me.copy_js_to_clean(args, asp+8);
				me.memory_array[asp/4]=(31+17*2)*8; // JSWorld: INT 17
				me.memory_array[asp/4+4]=me.shared_clean_values[f.shared_clean_value_index].ref;
				me.interpreter.instance.exports.set_asp(asp+16);

				hp=copied.hp;
				hp_free=copied.hp_free;

				var csp=me.interpreter.instance.exports.get_csp();
				me.memory_array[csp/4]=659*8; // instruction 0; to return
				csp+=8;

				const old_pc=me.interpreter.instance.exports.get_pc();
				me.interpreter.instance.exports.set_pc(100*8); // jmp_ap2
				me.interpreter.instance.exports.set_csp(csp);
				me.interpreter.instance.exports.set_hp(hp);
				me.interpreter.instance.exports.set_hp_free(hp_free);

				try {
					me.interpreter.instance.exports.interpret();
				} catch (e) {
					if (e.constructor.name!='ABCError' &&
							(e.fileName!='abc-interpreter.js' || e.lineNumber>700))
						throw e;

					var trace=[e.message, '\n'].concat(me.get_trace());
					console.error.apply(null,trace);

					throw e.toString();
				}

				me.interpreter.instance.exports.set_pc(old_pc);
				me.interpreter.instance.exports.set_asp(old_asp);
			};

			return me;
		});
	}
}
