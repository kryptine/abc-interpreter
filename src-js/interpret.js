if (typeof scriptArgs == 'undefined')
	scriptArgs = arguments;

function fetch(path) {
	if (path.indexOf('/js/')==0) {
		return Promise.resolve({
			ok: true,
			arrayBuffer: () => os.file.readRelativeToScript(path.slice(4), 'binary').buffer,
		});
	}

	var prog=read(path, 'binary');
	if ('buffer' in prog) // spidermonkey
		prog=prog.buffer;

	return Promise.resolve({
		ok: true,
		arrayBuffer: () => prog,
	});
}

loadRelativeToScript('abc-interpreter.js');

var stack_size=512<<10;
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

const heapi=scriptArgs.lastIndexOf('-h');
if (heapi>=0)
	heap_size=string_to_size(scriptArgs[heapi+1]);

const stacki=scriptArgs.lastIndexOf('-s');
if (stacki>=0)
	stack_size=string_to_size(scriptArgs[stacki+1]);
stack_size*=2;

ABC=null;
ABCInterpreter.instantiate({
	bytecode_path: scriptArgs[scriptArgs.length-1],
	heap_size: heap_size,
	stack_size: stack_size,

	interpreter_imports: {
		debug_instr: function (addr, instr) {
			printErr((addr/8-ABC.code_offset)+'\t'+ABCInterpreter.instructions[instr]);
		},
		illegal_instr: function (addr, instr) {
			crash('illegal instruction '+instr+' ('+ABCInterpreter.instructions[instr]+') at address '+(addr/8-ABC.code_offset-ABC.code_offset-ABC.code_offset-ABC.code_offset-ABC.code_offset-ABC.code_offset-ABC.code_offset-ABC.code_offset-ABC.code_offset));
		},
		out_of_memory: function () {
			crash('out of memory');
		},
		halt: function (pc, hp_free, hp_size) {
		},

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
	}
}).then(function(abc){
	ABC=abc;

	if (scriptArgs.indexOf('--extract-code')>=0) {
		const obj=wasmExtractCode(abc.interpreter.module, 'best');

		const extractable=['interpret'];

		var exports={};
		for (var f in abc.interpreter.instance.exports)
			exports[abc.interpreter.instance.exports[f].name]=f;

		obj.segments
			.filter(seg => seg.kind==0 && extractable.indexOf(exports[seg.funcIndex])>=0)
			.map(function (seg) {
				const name=seg.funcIndex in exports ? exports[seg.funcIndex] : ('_'+seg.funcIndex);
				const filename='disas-'+name+'.bin';
				const code=obj.code.subarray(seg.funcBodyBegin, seg.funcBodyEnd);

				printErr('extracting '+filename+'...');
				os.file.writeTypedArrayToFile(filename, code);
			});
	}

	if (!abc.start)
		crash('program has no start address');
	abc.interpreter.instance.exports.set_pc(abc.start);

	const time_start=new Date().getTime();

	const r=abc.interpreter.instance.exports.interpret();
	if (r!=0)
		printErr('failed with return code', r);

	if (scriptArgs.indexOf('--time') >= 0) {
		const time_end=new Date().getTime();
		const time=(time_end-time_start)/1000;
		printErr('user '+time.toLocaleString(['en-US'], {maximumFractionDigits: 2}));
	}
});
