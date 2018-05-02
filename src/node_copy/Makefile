NCLUDES:=-I $$CLEAN_HOME/lib/Platform -I $$CLEAN_HOME/lib/Generics -I $$CLEAN_HOME/lib/StdLib -I $$CLEAN_HOME/lib/Dynamics
FLAGS:=-no-opt-link -l -static -l -no-pie
DIR:="Clean System Files"
CLIBS:=-l $(DIR)/copy_node_c.o -l $(DIR)/copy_node_asm.o
EXE:=copy_node

all: asm c *.icl
	clm $(FLAGS) $(INCLUDES) $(CLIBS) $(EXE) -o $(EXE).out

c: copy_node.c
	gcc -c copy_node.c -o $(DIR)/copy_node_c.o

asm: copy_node.s
	as copy_node.s -o $(DIR)/copy_node_asm.o

clean:
	$(RM) -r "Clean System Files" $(EXE)
	mkdir $(DIR)
