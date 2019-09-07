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
	new_local TReal (to_real (B @ 0)) \r ->
	new_local TPtrOffset (ex_to_expr (Ecall "clean_RtoAC_words_needed" [expr_to_ex r])) \lw ->
	ensure_hp lw :.
	A @ 1 .= to_word Hp :.
	Hp .= (ex_to_expr (Ecall "clean_RtoAC" [expr_to_ex Hp,expr_to_ex r]) ::: TPtr TWord) :.
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
