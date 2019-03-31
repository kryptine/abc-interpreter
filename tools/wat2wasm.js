if (scriptArgs.length!=3 || scriptArgs[1]!='-o')
	crash('Usage: js wat2wasm.js INFILE -o OUTFILE');

var infile=scriptArgs[0];
var outfile=scriptArgs[2];

var wat=read(infile);
var wasm=wasmTextToBinary(wat);
os.file.writeTypedArrayToFile(outfile, wasm);
