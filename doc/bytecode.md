# Byte code instructions and their arguments

### addI
Adds the top two integers of the B-stack.
Top two B-stack values are destroyed.
Result is stored on the top of the B-stack.

### add_empty_node2
Create a node with descriptor `__cycle__in__spine` on the heap and insert it at
position 2 on the A-stack.

### add_empty_node3
Create a node with descriptor `__cycle__in__spine` on the heap and insert it at
position 3 on the A-stack.

### andI
Bitwise and on the top two integers of the B-stack.
Top two B-stack values are destroyed.
Result is stored on the top of the B-stack.

### build *n* *d*, *n* &gt; 4
Create an *n*-ary node with descriptor *d* on the heap.
Pop *n* elements from the A-stack as its arguments.
Push a reference to the new node to the A-stack.

### build0 *d*
See `build 0 d`.

### build1 *d*
See `build 1 d`.

### build2
See `build 2 d`.

### build3
See `build 3 d`.

### build4
See `build 4 d`.

### buildAC

### buildh

### buildh0

### buildh1

### buildh2

### buildh3

### buildh4

### buildhr01

### buildhr02

### buildhr03

### buildhr10

### buildhr11

### buildhr12

### buildhr13

### buildhr1b

### buildhr20

### buildhr21

### buildhr22

### buildhr30

### buildhr31

### buildhr40

### build_r

### build_ra0

### build_ra1

### buildBFALSE

### buildBTRUE

### buildB_b

### buildC

### buildC_b

### buildI

### buildF_b

### buildI_b

### buildhr

### buildhra0

### buildhra1

### buildhr0b

### build_r01

### build_r02

### build_r03

### build_r04

### build_r10

### build_r11

### build_r12

### build_r13

### build_r1b

### build_r20

### build_r21

### build_r30

### build_r31

### build_r40

### build_u

### build_u01

### build_u02

### build_u11

### build_u12

### build_u13

### build_u21

### build_u22

### build_u31

### build_ua1

### create
Create a node with descriptor `__cycle__in__spine` on the heap and push its
address to the A-stack.

### creates

### create_array

### create_arrayBOOL

### create_arrayCHAR

### create_arrayINT

### create_array_

### create_array_BOOL

### create_array_CHAR

### create_array_INT

### create_array_r

### create_array_r_

### create_array_r_a

### create_array_r_b

### decI
`b[0] := b[0] - 1`

### divI
`b[1] := b[0] / b[1]`

### eqAC

### eqAC_a

### eqB
`b[1] := b[0] == b[1]`

### eqB_aFALSE *i*
Push `a[i] == FALSE` to the B-stack.

### eqB_aTRUE *i*
Push `a[i] == TRUE` to the B-stack.

### eqB_bFALSE
Push `b[i] == FALSE` to the B-stack.

### eqB_bTRUE
Push `b[i] == TRUE` to the B-stack.

### eqC
`b[1] := b[0] == b[1]`

### eqC_a *n* *i*
Push `a[n] == i` to the B-stack.

### eqC_b *n* *i*
Push `b[n] == i` to the B-stack.

### eqD_b

### eq_desc *n* *d*
Push whether the descriptor of `a[n]` equals *d* to the B-stack.

### eqI
`b[1] := b[0] == b[1]`

### eqI_a *n* *i*
Push `a[n] == i` to the B-stack.

### eqI_b *n* *i*
Push `b[n] == i` to the B-stack.

### fill *n* *m* *d*, *m* > 4
Set the descriptor of `a[n]` to *d*.
Pop *m* values from the A-stack and set them as arguments to the original `a[n]`.

### fill1 *n* *d*
See `fill n 1 d`.

### fill1001

### fill1010

### fill1011

### fill2 *n* *d*
See `fill n 2 d`.

### fill2a001

### fill2a011

### fill2a012

### fill2ab011

### fill2ab013

### fill2ab002

### fill2ab003

### fill2b001

### fill2b002

### fill2b011

### fill2b012

### fill3 *n* *d*
See `fill n 3 d`.

### fill3a10

### fill3a11

### fill3a12

### fill3aaab13

### fill4 *n* *d*
See `fill n 4 d`.

### fillcaf

### fillh

### fillh0

### fillh1 *n* *d*
Same as `fill1 n d` (TODO: check with John).

### fillh2 *n* *d*
Same as `fill2 n d` (TODO: check with John).

### fillh3

### fillh4

### fillB_b

### fillC_b	

### fillF_b

### fillI_b

### fill_a *n* *m*
Copy the contents of `a[n]` into `a[m]`.

### fill_r

### fill_r01

### fill_r02

### fill_r03

### fill_r10

### fill_r11

### fill_r12

### fill_r13

### fill_r1b

### fill_r20

### fill_r21

### fill_r22

### fill_r30

### fill_r31

### fill_r40

### fill_ra0

### fill_ra1

### get_node_arity

### gtI
`b[1] := b[0] > b[1]`

### halt
Halts the program.

### incI
`b[0] = b[0] + 1`

### jesr *n*
Jumps to a *trap* subroutine. The traps are defined in `traps.c`.
Which trap is used depends on *n*:

 1\. `catAC`  
 2\. `sliceAC`  
 3\. `ItoAC`  
 5\. `print_string_`  
 6\. `openF`  
 8\. `closeF`  
 9\. `readLineF`  
10\. `endF`  
11\. `cmpAC`  
12\. `writeFI`  
13\. `writeFS`  
14\. `writeFC`

### jmp *l*
`pc := l`

### jmp_eval *n*
Jump to `a[n][0]` if `a[n]` is not in HNF (see `jsr_eval`).

### jmp_eval_upd

### jmp_false *l*
Jump to *l* if `b[0]` is false.

### jmp_true *l*
Jump to *l* if `b[0]` is true.

### jsr *l*
Push the instruction *after* the `jsr` to the C-stack and jump to *l*.

### jsr_eval *n*
Evaluate `a[n]` to HNF. That is: If `a[n][0]` is marked as in HNF (the 2nd bit
is set), do nothing. Otherwise, jump to `a[n][0]` as a subroutine.

### jsr_eval0
Evaluate `a[0]` to HNF. See `jsr_eval`.

### jsr_eval1
Evaluate `a[1]` to HNF. See `jsr_eval`.

### jsr_eval2
Evaluate `a[2]` to HNF. See `jsr_eval`.

### jsr_eval3
Evaluate `a[3]` to HNF. See `jsr_eval`.

### ltC
`b[1] := b[0] < b[1]`

### ltI
`b[1] := b[0] < b[1]`

### mulI
`b[1] := b[0] * b[1]`

### negI
`b[0] := ~b[0]` (integer negation)

### notB
`b[0] := !b[0]` (boolean not)

### notI
`b[0] := !b[0]` (bitwise not)

### orI
`b[0] := b[0] | b[1]` (bitwise or)

### pop_a *n*
Pop *n* elements from the A-stack.

### pop_b *n*
Pop *n* elements from the B-stack.

### print

### print_symbol_sc

### pushcaf10

### pushcaf11

### pushcaf20

### pushcaf31

### pushBFALSE
Push FALSE to the B-stack.

### pushBTRUE
Push TRUE to the B-stack.

### pushB_a *b*
Push the boolean *b* to the A-stack.

### pushC *c*
Push the character *c* to the B-stack.

### pushC_a *c*
Push the character *c* to the A-stack.

### pushD

### pushD_a

### pushF_a
Push the float *n* to the B-stack.

### pushI *n*
Push the integer *n* to the B-stack.

### pushI_a *n*
Push the integer *n* to the A-stack.

### push_a *i*
Push `a[i]` to the A-stack.

### push_arg *n* *m*, *m* &gt; 4
Push argument *m* of `a[n]` to the A-stack.

### push_arg1 *n*
Push argument 1 of `a[n]` to the A-stack.

### push_arg2 *n*
Push argument 2 of `a[n]` to the A-stack.

### push_arg2l

### push_arg3 *n*
Push argument 3 of `a[n]` to the A-stack.

### push_arg4 *n*
Push argument 4 of `a[n]` to the A-stack.

### push_arg_b

### push_args

### push_args1 *n*
Push argument 1 of `a[n]` to the A-stack.

### push_args2

### push_args3

### push_args4

### push_array

### push_arraysize

### push_a_b
Push `a[i]` to the B-stack.

### push_b
Push `b[i]` to the B-stack.

### push_node

### push_node0

### push_node1

### push_node2

### push_node3

### push_node4

### push_node_u

### push_node_u01

### push_node_u02

### push_node_u11

### push_node_u12

### push_node_u13

### push_node_u21

### push_node_u22

### push_node_u31

### push_node_ua1

### push_r_args

### push_r_args01

### push_r_args02

### push_r_args03

### push_r_args04

### push_r_args0b

### push_r_args10

### push_r_args11

### push_r_args12

### push_r_args13

### push_r_args1b

### push_r_args20

### push_r_args21

### push_r_args22

### push_r_args30

### push_r_args31

### push_r_args40

### push_r_argsa0

### push_r_argsa1

### push_r_args_a1

### push_r_args_a2l

### push_r_args_a3

### push_r_args_a4

### push_r_args_aa1

### push_r_args_b0b11

### push_r_args_b0221

### push_r_args_b1

### push_r_args_b1111

### push_r_args_b2l1

### push_r_args_b31

### push_r_args_b41

### push_r_args_b1l2

### push_r_args_b2

### push_r_args_b22

### replace

### replaceBOOL

### replaceCHAR

### replaceINT

### replace_ra

### repl_args

### repl_args1

### repl_args2

### repl_args3

### repl_args4

### repl_r_args

### repl_r_args01

### repl_r_args02

### repl_r_args03

### repl_r_args04

### repl_r_args10

### repl_r_args11

### repl_r_args12

### repl_r_args13

### repl_r_args1b

### repl_r_args20

### repl_r_args30

### repl_r_args21

### repl_r_args22

### repl_r_args31

### repl_r_args40

### repl_r_argsa0

### repl_r_argsa1

### repl_r_args_aab11

### repl_r_args_a2021

### repl_r_args_a21

### repl_r_args_a31

### repl_r_args_a41

### repl_r_args_aa1

### remI
`b[1] := b[0] % b[1]` (remainder after division)

### rtn
Pop `c[0]` and jump to it.

### select

### selectBOOL

### selectCHAR

### selectINT

### select_r

### select_r02

### select_r12

### select_r20

### select_r21

### select_r40

### select_ra

### select_ra1

### shiftlI
`b[1] := b[0] << b[1]`

### shiftrI
`b[1] := b[0] >> b[1]`

### subI
`b[1] := b[0] - b[1]`

### testcaf

### update

### updateBOOL

### updateCHAR

### updateINT

### updatepop_a

### updatepop_b

### update_a *n* *m*
`a[m] := a[n]`

### update_b *n* *m*
`b[m] := b[n]`

### update_r

### update_r02

### update_r12

### update_r20

### update_r21

### update_r30

### update_r40

### update_ra

### update_ra1

### xorI
`b[1] := b[0] ^ b[1]` (bitwise xor)

### CtoAC

### ItoC
`b[0] := b[0] & 0xff` (cast integer to character)

### jsr_ap4

### jmp_ap4

### jsr_ap3

### jmp_ap3

### jsr_ap2

### jmp_ap2

### jsr_ap1

### jmp_ap1

### add_arg0

### add_arg1

### add_arg2

### add_arg3

### add_arg4

### add_arg5

### add_arg6

### add_arg7

### add_arg8

### add_arg9

### add_arg10

### add_arg11

### add_arg12

### add_arg13

### add_arg14

### add_arg15

### add_arg16

### eval_upd0

### eval_upd1

### eval_upd2

### eval_upd3

### eval_upd4

### fill_a01_pop_rtn

### swap_a1
Swap `a[0]` and `a[1]`.

### swap_a2
Swap `a[0]` and `a[2]`.

### swap_a3
Swap `a[0]` and `a[3]`.

### swap_a

### addIi *n*
`b[0] := b[0] + n`

### andIi *n*
`b[0] := b[0] & n` (bitwise and)

### andIio *i* *n*
Push `b[i] & n` (bitwise and) to the B-stack.

### buildh0_dup_a

### buildh0_dup2_a

### buildh0_dup3_a

### buildh0_put_a

### buildh0_put_a_jsr

### buildho2

### buildo1

### buildo2

### dup_a *n*
`a[n] := a[0]`

### dup2_a *n*
`a[n+1] := a[n] := a[0]`

### dup3_a *n*
`a[n+2] := a[n+1] := a[n] := a[0]`

### exchange_a *n* *m*
Swap `a[n]` and `a[m]`.

### geC
`b[1] := b[0] >= b[1]`

### jmp_b_false *n* *l*
Jump to *l* if `b[n]` is false.

### jmp_eqACio

### jmp_eqC_b *n* *m* *l*
Jump to *l* if `b[n] == b[m]`.

### jmp_eqC_b2 *m* *n* *l* *n'* *l'*
Jump to *l* if `b[m] == b[n]`.  
If not, jump to *l'* if `b[m] == b[n']`.

### jmp_eqD_b

### jmp_eqD_b2

### jmp_eqI *l*
Jump to *l* if `b[0] == b[1]`.

### jmp_eqI_b *n* *m* *l*
Jump to *l* if `b[n] == b[m]`.

### jmp_eqI_b2 *m* *n* *l* *n'* *l'*
Jump to *l* if `b[m] == b[n]`.  
If not, jump to *l'* if `b[m] == b[n']`.

### jmp_eq_desc

### jmp_geI *l*
Jump to *l* if `b[0] >= b[1]`.

### jmp_ltI *l*
Jump to *l* if `b[0] < b[1]`.

### jmp_neC_b *n* *c* *l*
Jump to *l* if `b[n] <> c`.

### jmp_neI *l*
Jump to *l* if `b[0] <> b[1]`.

### jmp_neI_b *n* *i* *l*
Jump to *l* if `b[n] <> i`.

### jmp_ne_desc

### jmp_o_geI

### jmp_o_geI_arraysize_a

### ltIi *n*
`b[1] := b[0] < n`

### neI
`b[1] := b[0] != b[1]`

### swap_b1
Swap `b[0]` and `b[1]`.1

### pop_a_jmp *n* *l*
Pop *n* elements from the A-stack and jump to *l*.

### pop_a_jsr *n* *l*
Pop *n* elements from the A-stack and jump to *l* as a subroutine.

### pop_a_rtn *n*
Pop *n* elements from the A-stack and return to `c[0]`.

### pop_ab_rtn *n* *m*
Pop *n* elements from the A-stack, *m* from the B-stack and return to `c[0]`.

### pop_b_jmp *n* *l*
Pop *n* elements from the B-stack and jump to *l*.

### pop_b_jsr *n* *l*
Pop *n* elements from the B-stack and jump to *l* as a subroutine.

### pop_b_pushBFALSE *n*
Pop *n+1* elements from the B-stack and push False.

### pop_b_pushBTRUE *n*
Pop *n+1* elements from the B-stack and push True.

### pop_b_rtn *n*
Pop *n* elements from the B-stack and return to `c[0]`.

### pushD_a_jmp_eqD_b2

### push_a_jsr *n* *l*
Push `a[n]` to the A-stack and jump to *l* as a subroutine.

### push_b_incI *n*
Push `b[n]+1` to the B-stack.

### push_b_jsr *n* *l*
Push `b[n]` to the B-stack and jump to *l* as a subroutine.

### push_arraysize_a

### push_jsr_eval

### push_a2 *n* *m*
Push `a[n]` and `a[m]` to the A-stack.

### push_ab *n* *m*
Push `a[n]` to the A-stack and `b[m]` to the B-stack.

### push_b2 *n* *m*
Push `b[n]` and `b[m]` to the A-stack.

### push2_a *n*
Push `a[n]` and `a[n-1]` to the A-stack.

### push2_b *n*
Push `b[n]` and `b[n-1]` to the B-stack.

### push3_a *n*
Push `a[n]`, `a[n-1]` and `a[n-2]` to the A-stack.

### push3_b *n*
Push `b[n]`, `b[n-1]` and `b[n-2]` to the B-stack.

### push_update_a

### put_a *n*
`a[n] := a[0]` and pop one element from the A-stack.

### put_b *n*
`b[n] := b[0]` and pop one element from the B-stack.

### selectCHARoo

### selectoo

### update2_a

### update2_b

### update2pop_a

### update2pop_b

### update3_a

### update3_b

### update3pop_a

### update3pop_b

### update4_a

### updates2_a

### updates2_a_pop_a

### updates2_b

### updates2pop_a

### updates2pop_b

### updates3_a

### updates3_b

### updates3pop_a

### updates3pop_b

### updates4_a

### jsr_stack_check

### stack_check

### build_r0b
