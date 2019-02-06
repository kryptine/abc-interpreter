module sudoku

import StdEnv
import StdMaybe

:: SudokuLike val :== {Node val}

:: Node val = Node NodeId [NodeId] [val] // Id; neighbours; possible values
:: NodeId :== Int

:: Solution val :== [(NodeId, val)]

instance < (Node val) where (<) (Node a _ _) (Node b _ _) = a < b

makeSudokuLike :: [Node val] -> SudokuLike val
makeSudokuLike ns = {n \\ n <- sort ns}

makeSudoku :: [[Maybe Int]] -> SudokuLike Int
makeSudoku mat = makeSudokuLike
	[ let i = 9*ri + ci in Node i (nbs i) (if (isJust v) [fromJust v] [1..9])
		\\ r <- mat & ri <- [0..], v <- r & ci <- [0..]]
where
	nbs :: NodeId -> [NodeId]
	nbs i = row ++ col ++ block
	where
		row   = let base = (i / 9) * 9 in [base..base+8]
		col   = let base = i rem 9 in [base,base+9..base+72]
		block = let base = (i / 27) * 27 + ((i rem 9) / 3) * 3 in
			[ base     , base +  1, base +  2
			, base +  9, base + 10, base + 11
			, base + 18, base + 19, base + 20
			]

makeSudokuWithZeroes :: ([[Int]] -> SudokuLike Int)
makeSudokuWithZeroes = makeSudoku o map (map (\v.if (v==0) Nothing (Just v)))

solve :: [NodeId] (SudokuLike val) -> [Solution val] | Eq val
solve [] _ = [[]]
solve [n:ns] pz = case pz.[n] of
	(Node i nb ps) = [[(i,p):sol] \\ p <- ps,
		sol <- let pz` = upd nb p pz in solve (eval pz` ns) pz`]
where
	upd :: [NodeId] val (SudokuLike val) -> SudokuLike val | Eq val
	upd []     v pz = pz
	upd [i:is] v pz = upd is v
		{if (i`==i) (remove v p) p \\ p <-: pz & i` <- [0..]}

	remove :: val (Node val) -> Node val | Eq val
	remove v (Node id nb vs) = Node id nb (removeMember v vs)

	eval :: (SudokuLike val) -> [NodeId] -> [NodeId]
	eval pz = sortBy (\a b -> poss pz.[a] < poss pz.[b])
	where poss (Node _ _ ps) = length ps

Start = map snd (sort (hd (solve [0..80] (makeSudokuWithZeroes sudoku))))
where
	// https://en.wikipedia.org/wiki/Sudoku
	sudoku :: [[Int]]
	sudoku =
		[ [5,3,0, 0,7,0, 0,0,0]
		, [6,0,0, 1,9,5, 0,0,0]
		, [0,9,8, 0,0,0, 0,6,0]

		, [8,0,0, 0,6,0, 0,0,3]
		, [4,0,0, 8,0,3, 0,0,1]
		, [7,0,0, 0,2,0, 0,0,6]

		, [0,6,0, 0,0,0, 2,8,0]
		, [0,0,0, 4,1,9, 0,0,5]
		, [0,0,0, 0,8,0, 0,7,9]]
