definition module specialized

import target

instr_halt :: !Target -> Target

instr_divLU :: !Target -> Target
instr_mulUUL :: !Target -> Target

instr_RtoAC :: !Target -> Target

instr_closeF :: !Target -> Target
instr_endF :: !Target -> Target
instr_errorF :: !Target -> Target
instr_flushF :: !Target -> Target
instr_openF :: !Target -> Target
instr_positionF :: !Target -> Target
instr_readFC :: !Target -> Target
instr_readFI :: !Target -> Target
instr_readFR :: !Target -> Target
instr_readFS :: !Target -> Target
instr_readLineF :: !Target -> Target
instr_seekF :: !Target -> Target
instr_stderrF :: !Target -> Target
instr_stdioF :: !Target -> Target
instr_writeFC :: !Target -> Target
instr_writeFI :: !Target -> Target
instr_writeFR :: !Target -> Target
instr_writeFS :: !Target -> Target
instr_writeFString :: !Target -> Target
