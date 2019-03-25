definition module interpretergen

from StdMaybe import :: Maybe
from target import :: Expr

:: Options =
	{ instructions_order :: !Maybe [String]
	, debug_instructions :: !Bool
	}

:: TWord  = TWord
:: TChar  = TChar
:: TShort = TShort
:: TInt   = TInt
:: TReal  = TReal

:: TPtr t = TPtr !t

class to_word  t :: !(Expr t) -> Expr TWord
class to_char  t :: !(Expr t) -> Expr TChar
class to_short t :: !(Expr t) -> Expr TShort
class to_int   t :: !(Expr t) -> Expr TInt
class to_real  t :: !(Expr t) -> Expr TReal

class to_word_ptr t  :: !(Expr t) -> Expr (TPtr TWord)
class to_char_ptr t  :: !(Expr t) -> Expr (TPtr TChar)
class to_short_ptr t :: !(Expr t) -> Expr (TPtr TShort)
class to_int_ptr t   :: !(Expr t) -> Expr (TPtr TInt)
