CPM:=cpm

TESTS:=CodeSharing GraphTest
RUNTESTS:=$(addprefix test-,$(TESTS))

all: $(TESTS)

$(TESTS): %: %.prj library .FORCE
	$(CPM) $<

%.prj: %.prj.default
	cp $< $@

test: $(RUNTESTS)

$(RUNTESTS): test-%: % .FORCE
	./$< | tee $<.result; echo
	git diff --no-index --word-diff $<.expected $<.result && echo "\033[0;32m$< passed\033[0m"

library: .FORCE
	$(MAKE) -C ../src $@

.FORCE:
.PHONY: all test $(RUNTESTS) library
