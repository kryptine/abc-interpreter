INCLUDES:=-I $$CLEAN_HOME/lib/Platform -I $$CLEAN_HOME/lib/Generics -I $$CLEAN_HOME/lib/StdLib -I $$CLEAN_HOME/lib/Dynamics
FLAGS:=
DIR:="Clean System Files"
EXE:=copy_node

all: asm c *.icl
	clm $(FLAGS) $(INCLUDES) $(EXE) -o $(EXE).out

c: copy_node.c
	gcc -c copy_node.c -o $(DIR)/copy_node_c.o

asm: copy_node.s
	as copy_node.s -o $(DIR)/copy_node_asm.o

clean:
	$(RM) -r "Clean System Files" $(EXE).out
	mkdir $(DIR)

run: all
	./copy_node.out
