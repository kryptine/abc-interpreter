var memory = new WebAssembly.Memory({initial: 80});
var membuffer = new Uint32Array(memory.buffer);

if (typeof scriptArgs == 'undefined')
	scriptArgs = arguments;

var progbytes = read(scriptArgs[scriptArgs.length-1], 'binary');
var prog = 'buffer' in progbytes
	? new Uint32Array(progbytes.buffer) // spidermonkey
	: new Uint32Array(progbytes); // d8

function find_start_address(prog) {
	while (prog.length > 0) {
		if (prog[0]==2)
			return prog[2];
		prog = prog.slice(2+2*prog[1]);
	}
	return undefined;
}
var start = find_start_address(prog);
if (start == undefined) {
	console.log('program has no start address');
	quit();
}

for (var i in prog) {
	membuffer[i] = prog[i];
}

load('abc_instructions.js');
var wasm = read('abc_interpreter.wasm', 'binary');
wasm = new Uint8Array(wasm);

(async function(wasm){
	wasm = await WebAssembly.instantiate(wasm,
		{
			clean: {
				memory: memory,

				debug_instr: function (addr, instr) {
					console.log((addr/8-1)+'\t'+abc_instructions[instr]);
				},

				memcpy: function (dest,src,n) {
					// TODO
					console.log('memcpy',dest,src,n);
				},
				strncmp: function (s1,s2,n) {
					// TODO
					return 0;
				},
				putchar: function (v) {
					putstr(String.fromCharCode(v));
				},
				print_int: function (v) {
					// TODO
					putstr(String(v));
				},
				print_char: function (v) {
					// TODO
					console.log('print_char',v);
				},
				print_real: function (v) {
					// TODO
					console.log('print_real',v);
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

	var r=wasm.instance.exports.interpret(start);
	if (r!=0)
		console.log('failed with return code', r);
})(wasm);
