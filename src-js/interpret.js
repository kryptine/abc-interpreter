if (typeof scriptArgs == 'undefined')
	scriptArgs = arguments;

var DEBUG=false;

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

var gc = os.file.readRelativeToScript('gc.wasm', 'binary');
gc = new Uint8Array(gc);

var intp = os.file.readRelativeToScript('abc_interpreter.wasm', 'binary');
intp = new Uint8Array(intp);

(async function(gc, intp){
	gc = await WebAssembly.instantiate(gc,
		{
			clean: {
				memory: memory,

				debug: function(where,n1,n2,n3) {
					if (!DEBUG)
						return;
					where=[
						'garbage collection!',
						'copy-a',
						'update-a',
						'copy-thunk',
						'copy-hnf',
						'update-thunk',
						'update-hnf',
					][where];
					console.log(where,n1,n2,n3);
				},
			}
		}
	);
	gc.instance.exports.setup(hp, heap_size);

	intp = await WebAssembly.instantiate(intp,
		{
			clean: {
				memory: memory,

				debug_instr: function (addr, instr) {
					if (!DEBUG)
						return;
					console.log((addr/8-133)+'\t'+abc_instructions[instr]);
				},
				illegal_instr: function (addr, instr) {
					crash('illegal instruction '+instr+' ('+abc_instructions[instr]+') at address '+(addr/8-1));
				},
				out_of_memory: function () {
					crash('out of memory');
				},
				gc: gc.instance.exports.gc,
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
		var obj = wasmExtractCode(intp.module, 'best');

		var exports = {};
		for (var f in intp.instance.exports)
			exports[intp.instance.exports[f].name] = f;

		obj.segments.filter(seg => seg.kind == 0).map(function (seg) {
			var name = seg.funcIndex in exports ? exports[seg.funcIndex] : ('_'+seg.funcIndex);
			var filename = 'disas-'+name+'.bin';
			var code = obj.code.subarray(seg.funcBodyBegin, seg.funcBodyEnd);

			console.log('extracting '+filename+'...');
			os.file.writeTypedArrayToFile(filename, code);
		});
	}

	var r=intp.instance.exports.interpret(start, asp, bsp, csp, hp, heap_size/8);
	if (r!=0)
		console.log('failed with return code', r);
})(gc, intp);
