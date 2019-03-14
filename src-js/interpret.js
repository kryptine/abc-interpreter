if (typeof scriptArgs == 'undefined')
	scriptArgs = arguments;

var progbytes = read(scriptArgs[scriptArgs.length-1], 'binary');
var prog = 'buffer' in progbytes
	? new Uint32Array(progbytes.buffer) // spidermonkey
	: new Uint32Array(progbytes); // d8

var stack_size=(512<<10)*2*8;
var heap_size=2<<20;
var asp=4*prog.length;
var bsp=asp+stack_size;
var csp=asp+stack_size/2;
var hp=bsp+8;

var blocks_needed = Math.floor((prog.length*4 + stack_size + heap_size*2*8 + 65535) / 65536);

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
	console.log('program has no start address');
	quit();
}

for (var i in prog) {
	membuffer[i] = prog[i];
}

loadRelativeToScript('abc_instructions.js');
var wasm = os.file.readRelativeToScript('abc_interpreter.wasm', 'binary');
wasm = new Uint8Array(wasm);

(async function(wasm){
	wasm = await WebAssembly.instantiate(wasm,
		{
			clean: {
				memory: memory,

				debug_instr: function (addr, instr) {
					/*console.log(
						wasm.instance.exports.get_asp(),
						wasm.instance.exports.get_bsp(),
						wasm.instance.exports.get_csp(),
						wasm.instance.exports.get_hp());*/
					//console.log(addr+'\t'+(addr/8-133)+'\t'+abc_instructions[instr]);
				},
				illegal_instr: function (addr, instr) {
					crash('illegal instruction '+instr+' ('+abc_instructions[instr]+') at address '+(addr/8-1));
				},
				gc_needed: function (addr, instr) {
					crash('gc needed at '+(addr/8-1)+' ('+abc_instructions[membuffer[addr/4]]+')');
				},
				halt: function (pc, hp_free, hp_size) {
					console.log('halt at', (pc/8)-code_offset);
					console.log(hp_size-hp_free, hp_free, hp_size);
				},

				memcpy: function (dest,src,n) {
					// TODO: optimise; move to wasm
					var mem=new Uint8Array(membuffer.buffer, membuffer.byteOffset);
					for (var i=0; i<n; i++) {
						mem[dest+i]=mem[src+i];
					}
				},
				strncmp: function (s1,s2,n) {
					// TODO
					console.log('strncmp',s1,s2,n);
					return 0;
				},
				putchar: function (v) {
					putstr(String.fromCharCode(v));
				},
				print_int: function (v) {
					putstr(String(v));
				},
				print_bool: function (v) {
					putstr(v==0 ? 'False' : 'True');
				},
				print_char: function (v) {
					putstr("'"+String.fromCharCode(v)+"'");
				},
				print_real: function (v) {
					putstr(Number(v).toLocaleString(
						['en-US'],
						{
							useGrouping: false,
							maximumFractionDigits: 15,
						}
					));
				}
			}
		}
	);

	if (scriptArgs.indexOf('--extract-code') >= 0) {
		var obj = wasmExtractCode(wasm.module, 'best');

		var exports = {};
		for (var f in wasm.instance.exports)
			exports[wasm.instance.exports[f].name] = f;

		obj.segments.filter(seg => seg.kind == 0).map(function (seg) {
			var name = seg.funcIndex in exports ? exports[seg.funcIndex] : ('_'+seg.funcIndex);
			var filename = 'disas-'+name+'.bin';
			var code = obj.code.subarray(seg.funcBodyBegin, seg.funcBodyEnd);

			console.log('extracting '+filename+'...');
			os.file.writeTypedArrayToFile(filename, code);
		});
	}

	var r=wasm.instance.exports.interpret(start, asp, bsp, csp, hp, heap_size/8);
	if (r!=0)
		console.log('failed with return code', r);
})(wasm);
