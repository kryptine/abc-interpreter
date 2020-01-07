implementation module specialized

import target
import wasm

instr_halt :: !Target -> Target
instr_halt t = (
	append (Ecall "clean_halt" [expr_to_ex Pc, expr_to_ex Hp_free, Eget (Global "g-hp-size")]) :.
	append (Ereturn (Econst I32 0))
	) t

instr_divLU :: !Target -> Target
instr_divLU t = instr_unimplemented t // TODO

instr_mulUUL :: !Target -> Target
instr_mulUUL t = instr_unimplemented t // TODO

instr_RtoAC :: !Target -> Target
instr_RtoAC t = (
	ensure_hp 5 :. // upper bound; sometimes as little as 3 words are needed
	new_local TReal (to_real (B @ 0)) \r ->
	A @ 1 .= to_word Hp :.
	Hp @ 0 .= STRING__ptr + lit_word 2 :.
	new_local TWord (to_word Hp) \old_hp ->
	advance_ptr Hp 2 :.
	new_local TWord (to_word (ex_to_expr (Ecall "clean_RtoAC" [expr_to_ex r,expr_to_ex Hp]) ::: TPtrOffset)) \new_hp ->
	Hp @ -1 .= new_hp - to_word Hp :.
	Hp .= to_word_ptr ((new_hp + lit_word 7) &. lit_word -8) :.
	// correct hp-free counter because we use an upper bound above
	Hp_free += lit_hword 5 - to_ptr_offset ((to_word Hp - old_hp) >>. lit_word 3) :.
	advance_ptr Pc 1 :.
	advance_ptr A 1 :.
	advance_ptr B 1
	) t

instr_closeF :: !Target -> Target
instr_closeF t = instr_unimplemented t

instr_endF :: !Target -> Target
instr_endF t = instr_unimplemented t

instr_errorF :: !Target -> Target
instr_errorF t = instr_unimplemented t

instr_flushF :: !Target -> Target
instr_flushF t = instr_unimplemented t

instr_openF :: !Target -> Target
instr_openF t = instr_unimplemented t

instr_positionF :: !Target -> Target
instr_positionF t = instr_unimplemented t

instr_readFC :: !Target -> Target
instr_readFC t = instr_unimplemented t

instr_readFI :: !Target -> Target
instr_readFI t = instr_unimplemented t

instr_readFR :: !Target -> Target
instr_readFR t = instr_unimplemented t

instr_readFS :: !Target -> Target
instr_readFS t = instr_unimplemented t

instr_readLineF :: !Target -> Target
instr_readLineF t = instr_unimplemented t

instr_seekF :: !Target -> Target
instr_seekF t = instr_unimplemented t

instr_stderrF :: !Target -> Target
instr_stderrF t = instr_unimplemented t

instr_stdioF :: !Target -> Target
instr_stdioF t = instr_unimplemented t

instr_writeFC :: !Target -> Target
instr_writeFC t = instr_unimplemented t

instr_writeFI :: !Target -> Target
instr_writeFI t = instr_unimplemented t

instr_writeFR :: !Target -> Target
instr_writeFR t = instr_unimplemented t

instr_writeFS :: !Target -> Target
instr_writeFS t = instr_unimplemented t

instr_writeFString :: !Target -> Target
instr_writeFString t = instr_unimplemented t
