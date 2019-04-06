if (typeof scriptArgs == 'undefined')
	scriptArgs = arguments;

var progbytes = read(scriptArgs[scriptArgs.length-1], 'binary');
var prog = 'buffer' in progbytes
	? new Uint32Array(progbytes.buffer) // spidermonkey
	: new Uint32Array(progbytes); // d8

var stack_size=(512<<10)*2;
var heap_size=2<<20;

function string_to_size(s) {
	var res=0;
	var i=0;

	while ('0'<=s[i] && s[i]<='9') {
		res*=10;
		res+=s.charCodeAt(i)-48;
		i++;
	}

	if (i>=s.length)
		return res;

	switch (s[i]) {
		case 'm':
		case 'M':
			res<<=10;
		case 'k':
		case 'K':
			res<<=10;
			i++;
			break;
		default:
			return -1;
	}

	if (i==s.length)
		return res;
	return -1;
}

var heapi=scriptArgs.lastIndexOf('-h');
if (heapi>=0)
	heap_size=string_to_size(scriptArgs[heapi+1]);
heap_size=Math.floor(heap_size/8)*8;

var stacki=scriptArgs.lastIndexOf('-s');
if (stacki>=0)
	stack_size=string_to_size(scriptArgs[stacki+1]);
stack_size*=8;

var asp=4*prog.length;
var bsp=asp+stack_size;
var csp=asp+stack_size/2;
var hp=bsp+8;

var blocks_needed = Math.floor((prog.length*4 + stack_size + heap_size*2 + 65535) / 65536);

var memory = new WebAssembly.Memory({initial: blocks_needed});
var membuffer = new Uint32Array(memory.buffer);

var start=undefined;
var code_offset=undefined;
function find_offsets(prog) {
	var i=0;
	while (prog.length > 0) {
		if (prog[0]==1) /* ST_Code */
			code_offset=i+1;
		if (prog[0]==3) /* ST_Start */
			start=prog[2];
		i+=1+prog[1];
		prog=prog.slice(2+2*prog[1]);
	}
}
find_offsets(prog);
if (start == undefined) {
	crash('program has no start address');
}

for (var i in prog) {
	membuffer[i] = prog[i];
}

loadRelativeToScript('abc-instructions.js');

var util = os.file.readRelativeToScript('abc-interpreter-util.wasm', 'binary');
util = new Uint8Array(util);

var intp = os.file.readRelativeToScript('abc-interpreter.wasm', 'binary');
intp = new Uint8Array(intp);

(async function(util, intp){
	util = await WebAssembly.instantiate(util,
		{
			clean: {
				memory: memory,

				has_host_reference: function(index) {
					return 0;
				},
				update_host_reference: function(index,new_location) {
				},

				debug: function(what,a,b,c) {
					switch (what) {
						case 0:
							console.log('loop',a,'/',b,'; hp at',c);
							break;
						case 1:
							console.log('desc',a);
							break;
						case 2:
							console.log('hnf, arity',a);
							break;
						case 3:
							console.log('thunk, arity',a);
							break;
					}
				}
			}
		}
	);
	util.instance.exports.setup_gc(hp, heap_size, asp, 98*8);

	intp = await WebAssembly.instantiate(intp,
		{
			clean: {
				memory: memory,

				debug_instr: function (addr, instr) {
					printErr((addr/8-code_offset)+'\t'+abc_instructions[instr]);
				},
				handle_illegal_instr: function (pc, instr, asp, bsp, csp, hp) {
					return 0;
				},
				illegal_instr: function (addr, instr) {
					crash('illegal instruction '+instr+' ('+abc_instructions[instr]+') at address '+(addr/8-1));
				},
				out_of_memory: function () {
					crash('out of memory');
				},
				gc: util.instance.exports.gc,
				halt: function (pc, hp_free, hp_size) {
					print('halt at', (pc/8)-code_offset);
					print(hp_size-hp_free, hp_free, hp_size);
				},

				memcpy: util.instance.exports.memcpy,
				strncmp: util.instance.exports.strncmp,

				putchar: function (v) {
					putstr(String.fromCharCode(v));
				},
				print_int: function (high,low) {
					if (high==0 && low>=0) {
						putstr(String(low));
					} else {
						var n=BigInt(high)*2n**32n;
						if (low<0) {
							n+=2n**31n;
							low+=2**31;
						}
						n+=BigInt(low);
						putstr(String(n));
					}
				},
				print_bool: function (v) {
					putstr(v==0 ? 'False' : 'True');
				},
				print_char: function (v) {
					putstr("'"+String.fromCharCode(v)+"'");
				},
				print_real: function (v) {
					putstr(Number(0+v).toLocaleString(
						['en-US'],
						{
							useGrouping: false,
							maximumSignificantDigits: 15,
						}
					));
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
			}
		}
	);

	if (scriptArgs.indexOf('--extract-code') >= 0) {
		var obj = wasmExtractCode(intp.module, 'best');

		var exports = {};
		for (var f in intp.instance.exports)
			exports[intp.instance.exports[f].name] = f;

		obj.segments.filter(seg => seg.kind == 0).map(function (seg) {
			var name = seg.funcIndex in exports ? exports[seg.funcIndex] : ('_'+seg.funcIndex);
			var filename = 'disas-'+name+'.bin';
			var code = obj.code.subarray(seg.funcBodyBegin, seg.funcBodyEnd);

			printErr('extracting '+filename+'...');
			os.file.writeTypedArrayToFile(filename, code);
		});
	}

	var i=scriptArgs.indexOf('--graph');
	if (i >= 0) {
		var graph = os.file.readFile(scriptArgs[i+1], 'binary');
		graph = new Uint32Array(graph.buffer);
		var unused_semispace = util.instance.exports.get_unused_semispace();
		for (var i=0; i<graph.length; i++)
			membuffer[unused_semispace/4+i] = graph[i];
		var node = hp;
		hp = util.instance.exports.copy_from_string(unused_semispace,graph.length/2,asp,bsp,hp,code_offset*8);
		asp+=8;
		membuffer[asp/4] = node;
		start+=32; /* skip bootstrap to build start node; jump to _print_graph */
	}

	var time_start=new Date().getTime();

	intp.instance.exports.set_pc(start);
	intp.instance.exports.set_asp(asp);
	intp.instance.exports.set_bsp(bsp);
	intp.instance.exports.set_csp(csp);
	intp.instance.exports.set_hp(hp);
	intp.instance.exports.set_hp_free(heap_size/8);
	intp.instance.exports.set_hp_size(heap_size/8);

	var r=intp.instance.exports.interpret();
	if (r!=0)
		printErr('failed with return code', r);

	if (scriptArgs.indexOf('--time') >= 0) {
		var time_end=new Date().getTime();
		var time=(time_end-time_start)/1000;
		printErr('user '+time.toLocaleString(['en-US'], {maximumFractionDigits: 2}));
	}
})(util, intp);
