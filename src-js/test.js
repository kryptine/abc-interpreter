var memory = new WebAssembly.Memory({initial: 80});
var membuffer = new Uint32Array(memory.buffer);

var progbytes = read('nfib.ubc', 'binary');
var prog = new Uint32Array(progbytes.buffer);

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
WebAssembly.instantiate(wasm,
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
).then(function (wasm) {
	console.log('interpreting...');
	var r=wasm.instance.exports.interpret(start);
	if (r==0)
		console.log('done!');
	else
		console.log('failed with return code', r);
});
