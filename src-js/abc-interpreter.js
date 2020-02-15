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

var ABC=null; /* global reference to currently running interpreter, to be able to refer to it from Clean */
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

		this.encoding=null;

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
		this.initialized=false;

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

		var array=new Uint8Array(string.length);
		if (typeof string=='string') {
			for (var i=0; i<string.length; i++)
				array[i]=string.charCodeAt(i);
		} else {
			for (var i=0; i<string.length; i++)
				array[i]=string[i];
		}
		const graph=new Uint32Array(array.buffer);

		const unused_semispace=this.util.instance.exports.get_unused_semispace();
		for (var i=0; i<graph.length; i++)
			this.memory_array[unused_semispace/4+i]=graph[i];

		return this._deserialize(unused_semispace, graph.length/2);
	}

	copy_to_string (node) {
		const start=this.util.instance.exports.copy_to_string(node, this.code_offset*8);
		this.util.instance.exports.remove_forwarding_pointers_from_graph(node, this.code_offset*8);
		/* -8 because get_clean_string expects a _STRING_ constructor;
		 * false because we are in the unused semispace so we don't need to clean up */
		const string=this.get_clean_string(start-8, false);
		/* base64 encode */
		if (typeof btoa!='undefined') /* browsers */
			return btoa(string);
		else if (typeof Buffer!='undefined') /* node.js */
			return Buffer.from(string).toString('base64');
		else {
			var chars='ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789+/=';
			var block;
			var enc='';
			for (var idx=0;
					string.charAt(idx|0) || (chars='=',idx%1);
					enc+=chars.charAt(63 & block>>8 - idx%1 * 8)
			) {
				var char_code=string.charCodeAt(idx+=3 / 4);
				block=block<<8 | char_code;
			}
			return enc;
		}
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

			var result=undefined;
			const new_asp=me.interpreter.instance.exports.get_asp();
			const hp_ptr=me.memory_array[new_asp/4];
			if (me.memory_array[hp_ptr/4]!=25*8+2) { // INT, i.e. JSWorld
				// Assume we have received a tuple with the first element as the result
				// This is the case with jsWrapFunWithResult
				const str_ptr=me.memory_array[hp_ptr/4+2];
				const string=me.get_clean_string(me.memory_array[str_ptr/4+2], false);
				if (ABC_DEBUG)
					console.log('result:',string);
				let old_ABC=ABC;
				ABC=me;
				result=eval('('+string+')');
				ABC=old_ABC;
			}

			if (typeof result!='undefined')
				return result;
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
				this.memory_array[hp/4]=5*8+2; // _STRING_
				this.memory_array[hp/4+1]=0;
				var array;
				var length=values[i].length;
				switch (this.encoding) {
					case 'utf-8':
						if (typeof TextEncoder!='undefined') {
							var encoded=new TextEncoder().encode(values[i]);
							length=encoded.length;
							if (length%4) { // length must be divisible by 4 to cast to Uint32Array below
								array=new Uint8Array(((length+3)>>2)<<2);
								for (var j=0; j<length; j++)
									array[j]=encoded[j];
							} else {
								array=encoded;
							}
						}
						break;
					default:
						console.warn('copy_js_to_clean: this browser cannot encode text in '+this.encoding);
					case 'x-user-defined':
						array=new Uint8Array(((values[i].length+3)>>2)<<2);
						for (var j=0; j<values[i].length; j++)
							array[j]=values[i].charCodeAt(j);
						break;
				}
				this.memory_array[hp/4+2]=length;
				this.memory_array[hp/4+3]=0;
				array=new Uint32Array(array.buffer);
				for (var j=0; j<((length+3)>>2); j++)
					this.memory_array[hp/4+4+j]=array[j];
				hp+=16+(((length+7)>>3)<<3);
				hp_free-=2+((length+7)>>3);
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
				this.memory_array[hp/4]=0*8+2; // _ARRAY_
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
		else if (typeof value=='string') {
			var length=value.length;
			if (this.encoding=='utf-8' && typeof TextEncoder!='undefined')
				length=new TextEncoder().encode(value).length;
			return 2+2+((length+7)>>3);
		} else if (Array.isArray(value)) {
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
			throw 'could not attach shared Clean value to a JavaScript component';
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
	clear_shared_clean_value (ref, update_component=true /* Clean library assumes true! */) {
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
			return new TextDecoder(this.encoding).decode(string_buffer);
		} else {
			if (this.encoding!='x-user-defined')
				console.warn('get_clean_string: this browser does not have TextDecoder; string could not be decoded using '+this.encoding);
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

			encoding: 'x-user-defined',
			fetch: path => fetch(path), // to be able to override
		};
		Object.assign(opts,args);

		const me=new ABCInterpreter();

		if ('with_js_ffi' in args && args.with_js_ffi){
			Object.assign(opts.util_imports,ABCInterpreter.util_imports.js_references(me));
			Object.assign(opts.interpreter_imports,ABCInterpreter.interpreter_imports.js_ffi(me));
		}

		me.stack_size=opts.stack_size*2;
		me.heap_size=opts.heap_size;

		me.encoding=opts.encoding;

		return opts.fetch(opts.bytecode_path).then(function(resp){
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

			return opts.fetch(opts.util_path)
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
						me.log(Number(0+v).toPrecision(15).replace(/\.?0*$/,''));
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
					RtoAC: util.instance.exports.convert_real_to_string,
				}
			};
			Object.assign(interpreter_imports.clean, opts.interpreter_imports);

			return opts.fetch(opts.interpreter_path)
				.then(response => response.arrayBuffer())
				.then(bytes => WebAssembly.instantiate(bytes, interpreter_imports));
		}).then(function(intp){
			me.interpreter=intp;

			const asp=Math.floor((me.words_needed_for_program*8+7)/8)*8;
			delete me.words_needed_for_program;
			const bsp=asp+me.stack_size;
			const csp=asp+me.stack_size/2;
			const hp=bsp+8;

			me.util.instance.exports.setup_gc(hp, me.heap_size, asp, 96*8);

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
				me.memory_array[asp/4]=(30+17*2)*8; // JSWorld: INT 17
				me.memory_array[asp/4+4]=me.shared_clean_values[f.shared_clean_value_index].ref;
				me.interpreter.instance.exports.set_asp(asp+16);

				hp=copied.hp;
				hp_free=copied.hp_free;

				var csp=me.interpreter.instance.exports.get_csp();
				me.memory_array[csp/4]=658*8; // instruction 0; to return
				csp+=8;

				const old_pc=me.interpreter.instance.exports.get_pc();
				me.interpreter.instance.exports.set_pc(99*8); // jmp_ap2
				me.interpreter.instance.exports.set_csp(csp);
				me.interpreter.instance.exports.set_hp(hp);
				me.interpreter.instance.exports.set_hp_free(hp_free);

				const old_ABC=ABC;
				try {
					ABC=me;
					me.interpreter.instance.exports.interpret();
					ABC=old_ABC;
				} catch (e) {
					ABC=old_ABC;
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

ABCInterpreter.util_imports={
	js_references: me => ({
		has_host_reference: function (index) {
			if (index>=me.shared_clean_values.length)
				return 0;
			if (me.shared_clean_values[index]==null)
				return -1;
			return me.shared_clean_values[index].ref;
		},
		update_host_reference: function (index, new_location) {
			me.shared_clean_values[index].ref=new_location;
		},
	}),
};
ABCInterpreter.interpreter_imports={
	js_ffi: me => ({
		handle_illegal_instr: function (pc, instr, asp, bsp, csp, hp, hp_free) {
			if (ABCInterpreter.instructions[instr]=='instruction') {
				const arg=me.memory_array[(pc+8)/4];
				switch (arg) {
					case 0: /* evaluation finished */
						return 0;
					case 1: /* ABC.Interpreter.JavaScript: set_js */
						var v=me.get_clean_string(me.memory_array[asp/4], true);
						var x=me.get_clean_string(me.memory_array[asp/4-2], true);
						if (ABC_DEBUG)
							console.log(v,'.=',x);
						try {
							var ref=eval(v+'.shared_clean_value_index');
							if (typeof ref != 'undefined') {
								if (ABC_DEBUG)
									console.log('removing old reference to Clean',ref);
								me.clear_shared_clean_value(ref);
							}
						} catch (e) {}
						eval(v+'='+x);
						break;
					case 2: /* ABC.Interpreter.JavaScript: eval_js */
						var string=me.get_clean_string(me.memory_array[asp/4], true);
						if (ABC_DEBUG)
							console.log('eval',string);
						eval(string);
						break;
					case 3: /* ABC.Interpreter.JavaScript: eval_js_with_return_value */
						var string=me.get_clean_string(me.memory_array[asp/4], true);
						if (ABC_DEBUG)
							console.log('eval',string);
						var result=eval('('+string+')'); // the parentheses are needed for {}, for instance
						var copied=me.copy_js_to_clean(result, asp);
						me.interpreter.instance.exports.set_hp(copied.hp);
						me.interpreter.instance.exports.set_hp_free(copied.hp_free);
						break;
					case 4: /* ABC.Interpreter.JavaScript: share */
						var attach_to=me.memory_array[bsp/4];
						var index=me.share_clean_value(me.memory_array[asp/4],me.js[attach_to]);
						me.memory_array[bsp/4]=index;
						break;
					case 5: /* ABC.Interpreter.JavaScript: fetch */
						var index=me.memory_array[bsp/4];
						me.memory_array[asp/4]=me.shared_clean_values[index].ref;
						break;
					case 6: /* ABC.Interpreter.JavaScript: deserialize */
						var hp_ptr=me.memory_array[asp/4];
						me.memory_array[asp/4]=me.deserialize_from_unique_string(hp_ptr);
						break;
					case 7: /* ABC.Interpreter.Javascript: jsSerializeOnClient */
						me.require_hp(2);
						var string=me.copy_to_string(me.memory_array[asp/4]);
						me.memory_array[hp/4]=me.addresses.JSRef;
						me.memory_array[hp/4+1]=0;
						me.memory_array[hp/4+2]=me.share_js_value(string);
						me.memory_array[hp/4+3]=0;
						me.memory_array[asp/4]=hp;
						me.interpreter.instance.exports.set_hp(hp+16);
						me.interpreter.instance.exports.set_hp_free(hp_free-2);
						break;
					case 8: /* ABC.Interpreter.JavaScript: initialize_client in wrapInitUIFunction */
						var array=me.memory_array[asp/4]+24;
						me.addresses.JSInt=      me.memory_array[me.memory_array[array/4]/4];
						me.addresses.JSBool=     me.memory_array[me.memory_array[array/4+2]/4];
						me.addresses.JSString=   me.memory_array[me.memory_array[array/4+4]/4];
						me.addresses.JSReal=     me.memory_array[me.memory_array[array/4+6]/4];
						me.addresses.JSNull=     me.memory_array[me.memory_array[array/4+8]/4];
						me.addresses.JSUndefined=me.memory_array[me.memory_array[array/4+10]/4];
						me.addresses.JSArray=    me.memory_array[me.memory_array[array/4+12]/4];
						me.addresses.JSRef=      me.memory_array[me.memory_array[array/4+14]/4];
						me.addresses.JSCleanRef= me.memory_array[me.memory_array[array/4+16]/4];
						me.util.instance.exports.set_js_ref_constructor(me.addresses.JSRef);
						me.initialized=true;
						break;
					case 10: /* ABC.Interpreter.JavaScript: add CSS */
						var url=me.get_clean_string(me.memory_array[asp/4], false);
						var css=document.createElement('link');
						css.rel='stylesheet';
						css.type='text/css';
						css.async=true;
						css.href=url;
						document.head.appendChild(css);
						break;
					case 11: /* ABC.Interpreter.JavaScript: add JS */
						var url=me.get_clean_string(me.memory_array[asp/4], false);
						var callback=me.get_clean_string(me.memory_array[asp/4-2], true);
						var js=document.createElement('script');
						js.type='text/javascript';
						js.async=false;
						let current_ABC=ABC;
						if (callback.length>0)
							js.onload=() => { Function('ABC',callback+'();')(current_ABC); };
						document.head.appendChild(js);
						js.src=url;
						break;
					default:
						throw new ABCError('unknown instruction',arg);
				}
				return pc+16;
			}
			return 0;
		},
		illegal_instr: function (addr, instr) {
			me.empty_log_buffer();
			if (ABCInterpreter.instructions[instr]=='instruction')
				/* `instruction 0` ends the interpretation, so this is no error */
				return;
			throw new ABCError('illegal instruction',instr);
		},
	}),
};

if (typeof module!='undefined') module.exports={
	ABC_DEBUG: ABC_DEBUG,
	ABCError: ABCError,
	SharedCleanValue: SharedCleanValue,
	ABCInterpreter: ABCInterpreter,
	global: global
};
