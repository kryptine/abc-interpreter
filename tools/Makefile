BIN:=interpretergenc

interpretergenc: .FORCE
	clm -nr -nt -I interpretergen-c interpretergen -o $@

clean:
	$(RM) $(BIN)
	find . \
		-name '*.abc' -delete -or -name '*.o' -delete \
		-or -name 'Clean System Files' -empty -delete

.FORCE:
