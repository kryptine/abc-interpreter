(module
	(import "clean" "memory" (memory 1))

	(func $debug (import "clean" "debug") (param i32 i32 i32 i32))

	(global $start-heap (mut i32) (i32.const 0))
	(global $half-heap (mut i32) (i32.const 0))
	(global $end-heap (mut i32) (i32.const 0))
	(global $in-first-semispace (mut i32) (i32.const 1))

	(func (export "setup") (param i32 i32)
		(global.set $start-heap (local.get 0))
		(global.set $half-heap (i32.add (local.get 0) (local.get 1)))
		(global.set $end-heap (i32.add (local.get 0) (i32.add (local.get 1) (local.get 1))))
	)

	;; upper half of result is new hp pointer;
	;; lower half is hp-free
	(func (export "gc") (param $asp i32) (result i64)
		(local $org-asp i32)
		(local $old i32)
		(local $new i32)
		(local $n i32)
		(local $d i32)
		(local $a-arity i32)
		(local $b-arity i32)
		(local $ab-arity i32)

		(local.set $org-asp (local.get $asp))

		(if
			(global.get $in-first-semispace)
			(then
				(local.set $old (global.get $start-heap))
				(local.set $new (global.get $half-heap)))
			(else
				(local.set $old (global.get $half-heap))
				(local.set $new (global.get $start-heap)))
		)

		;; "garbage collection!"
		;;(call $debug (i32.const 0) (local.get $old) (local.get $new) (global.get $end-heap))

		;; copy A stack pointers
		(block $end-copy-asp
			(loop $copy-asp
				(local.set $n (i32.load (local.get $asp)))
				(br_if $end-copy-asp (i32.eqz (local.get $n)))

				(local.set $d (i32.load (local.get $n)))
				;; "copy-a"
				;;(call $debug (i32.const 1) (local.get $asp) (local.get $n) (local.get $new))

				(local.set $asp (i32.sub (local.get $asp) (i32.const 8)))
				(if
					(i32.and (local.get $d) (i32.const 1))
					(then
						(i32.store offset=8 (local.get $asp) (i32.sub (local.get $d) (i32.const 1)))
						(br $copy-asp)
					)
				)

				(br_if $copy-asp
					(i32.or
						(i32.gt_s (global.get $start-heap) (local.get $n))
						(i32.lt_s (global.get $end-heap) (local.get $n))
					)
				)

				(local.set $d (local.get $new))
				(local.set $new (call $copy (local.get $n) (local.get $new)))
				(i32.store offset=8 (local.get $asp) (local.get $d))
				(br $copy-asp)
			)
		)

		;; update references
		(local.set $n (select (global.get $half-heap) (global.get $start-heap) (global.get $in-first-semispace)))
		(block $end-update-refs
			(loop $update-refs
				(br_if $end-update-refs (i32.ge_u (local.get $n) (local.get $new)))

				(local.set $d (i32.load (local.get $n)))

				(if
					(i32.and (local.get $d) (i32.const 2))
					;; HNF
					(then
						(if
							(i32.or
								(i32.eq (local.get $d) (i32.const 210)) ;; INT+2
								(i32.or
									(i32.eq (local.get $d) (i32.const 130)) ;; CHAR+2
									(i32.or
										(i32.eq (local.get $d) (i32.const 90)) ;; BOOL+2
										(i32.eq (local.get $d) (i32.const 170)) ;; REAL+2
									)
								)
							)
							(then
								(local.set $n (i32.add (local.get $n) (i32.const 16)))
								(br $update-refs)
							)
						)
						(if
							;; _ARRAY_+2
							(i32.eq (local.get $d) (i32.const 10))
							(then
								(local.set $a-arity (i32.load offset=8 (local.get $n))) ;; length
								(local.set $d (i32.load offset=16 (local.get $n))) ;; child descriptor
								(if
									(i32.and ;; unboxed INT/BOOL/REAL
										(i32.le_u (i32.const 90) (local.get $d))
										(i32.le_u (local.get $d) (i32.const 210))
									)
									(then
										(local.set $n (i32.add (local.get $n) (i32.add (i32.shl (local.get $a-arity) (i32.const 3)) (i32.const 24))))
										(br $update-refs)
									)
								)
								(if
									(i32.eqz (i32.eqz (local.get $d)))
									(then (unreachable)) ;; TODO: unboxed record arrays
								)
								(local.set $n (i32.add (local.get $n) (i32.const 24)))
								(loop $update-array
									(br_if $update-refs (i32.eqz (local.get $a-arity)))
									(local.set $new (call $update-ref (local.get $n) (local.get $new)))
									(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
									(local.set $n (i32.add (local.get $n) (i32.const 8)))
									(br $update-array)
								)
							)
						)
						(if
							(i32.lt_u (local.get $d) (i32.mul (i32.const 131) (i32.const 8))) ;; built-in
							(then
								;; TODO: add more basic types
								(unreachable)
							)
						)
						;; No basic type
						(local.set $a-arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
						(local.set $b-arity (i32.const 0))
						(if
							(i32.gt_s (local.get $a-arity) (i32.const 256))
							(then
								(local.set $a-arity (i32.load16_s (local.get $d)))
								(local.set $b-arity (i32.sub (i32.sub (i32.load16_s (i32.sub (local.get $d) (i32.const 2))) (local.get $a-arity)) (i32.const 256)))
							)
						)
						(local.set $ab-arity (i32.add (local.get $a-arity) (local.get $b-arity)))
						;; "update-hnf"
						;;(call $debug (i32.const 6) (local.get $d) (local.get $a-arity) (local.get $b-arity))

						(if
							(i32.le_s (local.get $ab-arity) (i32.const 2))
							;; small node
							(then
								(block $skip-b-elems
									(br_if $skip-b-elems (i32.eqz (local.get $a-arity)))
									(local.set $new (call $update-ref (i32.add (local.get $n) (i32.const 8)) (local.get $new)))
									(br_if $skip-b-elems (i32.eq (local.get $a-arity) (i32.const 1)))
									(local.set $new (call $update-ref (i32.add (local.get $n) (i32.const 16)) (local.get $new)))
								)
								(local.set $n (i32.add (local.get $n) (i32.add (i32.shl (local.get $ab-arity) (i32.const 3)) (i32.const 8))))
								(br $update-refs)
							)
						)
						;; large node
						(if
							(i32.eqz (local.get $a-arity))
							(then
								(local.set $n (i32.add (local.get $n) (i32.add (i32.shl (local.get $ab-arity) (i32.const 3)) (i32.const 16))))
								(br $update-refs)
							)
						)
						(local.set $new (call $update-ref (i32.add (local.get $n) (i32.const 8)) (local.get $new)))
						(local.set $n (i32.load offset=16 (local.get $n)))
						(block $end-update-large-hnf
							(loop $update-large-hnf
								(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
								(br_if $end-update-large-hnf (i32.eqz (local.get $a-arity)))
								(local.set $new (call $update-ref (local.get $n) (local.get $new)))
								(local.set $n (i32.add (local.get $n) (i32.const 8)))
								(br $update-large-hnf)
							)
						)
						(local.set $n (i32.add (local.get $n) (i32.shl (local.get $b-arity) (i32.const 3))))
						(br $update-refs)
					)
				)

				;; thunk
				(local.set $a-arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2)))) ;; TODO optimize (see in $copy)
				(if
					(i32.lt_s (local.get $a-arity) (i32.const 0))
					(then
						(local.set $a-arity (i32.const 1))
						(local.set $b-arity (i32.const 0))
					)
					(else
						(local.set $b-arity (i32.shr_u (local.get $a-arity) (i32.const 8)))
						(local.set $a-arity (i32.sub (i32.and (local.get $a-arity) (i32.const 0xff)) (local.get $b-arity)))
					)
				)
				;; "update-thunk"
				;;(call $debug (i32.const 5) (local.get $n) (local.get $a-arity) (local.get $b-arity))

				(local.set $d (local.get $a-arity))
				(block $end-update-thunk-refs
					(loop $update-thunk-refs
						(local.set $n (i32.add (local.get $n) (i32.const 8)))
						(br_if $end-update-thunk-refs (i32.eqz (local.get $d)))
						(local.set $new (call $update-ref (local.get $n) (local.get $new)))
						(local.set $d (i32.sub (local.get $d) (i32.const 1)))
						(br $update-thunk-refs)
					)
				)
				(if
					(i32.lt_s (i32.add (local.get $a-arity) (local.get $b-arity)) (i32.const 2))
					(then
						(local.set $n (i32.add (local.get $n) (i32.shl (i32.sub (i32.const 2) (i32.add (local.get $a-arity) (local.get $b-arity))) (i32.const 3))))
					)
					(else
						(local.set $n (i32.add (local.get $n) (i32.shl (local.get $b-arity) (i32.const 3))))
					)
				)

				(br $update-refs)
			)
		)

		;; return
		(if
			(result i64)
			(global.get $in-first-semispace)
			(then
				(global.set $in-first-semispace (i32.const 0))
				(i64.or
					(i64.shl (i64.extend_i32_u (local.get $new)) (i64.const 32))
					(i64.extend_i32_u (i32.div_u (i32.sub (global.get $end-heap) (local.get $new)) (i32.const 8)))
				)
			)
			(else
				(global.set $in-first-semispace (i32.const 1))
				(i64.or
					(i64.shl (i64.extend_i32_u (local.get $new)) (i64.const 32))
					(i64.extend_i32_u (i32.div_u (i32.sub (global.get $half-heap) (local.get $new)) (i32.const 8)))
				)
			)
		)
	)

	(func $copy (param $from i32) (param $to i32) (result i32)
		(local $d i32)
		(local $a-arity i32)
		(local $b-arity i32)
		(local $ab-arity i32)
		(local.set $d (i32.load (local.get $from)))

		(i32.store (local.get $to) (local.get $d))
		(i32.store (local.get $from) (i32.add (local.get $to) (i32.const 1)))

		(if (i32.and (local.get $d) (i32.const 2))
			;; HNF
			(then
				(if
					(i32.or
						(i32.eq (local.get $d) (i32.const 210)) ;; INT+2
						(i32.or
							(i32.eq (local.get $d) (i32.const 130)) ;; CHAR+2
							(i32.or
								(i32.eq (local.get $d) (i32.const 90)) ;; BOOL+2
								(i32.eq (local.get $d) (i32.const 170)) ;; REAL+2
							)
						)
					)
					(then
						(i64.store offset=8 (local.get $to) (i64.load offset=8 (local.get $from)))
						(return (i32.add (local.get $to) (i32.const 16)))
					)
				)
				(if
					;; _ARRAY_+2
					(i32.eq (local.get $d) (i32.const 10))
					(then
						(local.set $a-arity (i32.load offset=8 (local.get $from))) ;; length
						(local.set $d (i32.load offset=16 (local.get $from))) ;; child descriptor
						(if
							(i32.eqz
								(i32.or
									(i32.eqz (local.get $d)) ;; boxed
									(i32.and ;; unboxed INT/BOOL/REAL
										(i32.le_u (i32.const 90) (local.get $d))
										(i32.le_u (local.get $d) (i32.const 210))
									)
								)
							)
							(then
								;;(call $debug (i32.const 4) (local.get $d) (i32.const -1) (i32.const -2))
								(unreachable) ;; TODO: unboxed record arrays
							)
						)
						(i32.store offset=8 (local.get $to) (local.get $a-arity))
						(i32.store offset=16 (local.get $to) (local.get $d))
						(local.set $to (i32.add (local.get $to) (i32.const 24)))
						(local.set $from (i32.add (local.get $from) (i32.const 24)))
						(block $end-copy-array
							(loop $copy-array
								(br_if $end-copy-array (i32.eqz (local.get $a-arity)))
								(i32.store (local.get $to) (i32.load (local.get $from)))
								(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
								(local.set $to (i32.add (local.get $to) (i32.const 8)))
								(local.set $from (i32.add (local.get $from) (i32.const 8)))
								(br $copy-array)
							)
						)
						(return (local.get $to))
					)
				)
				(if
					(i32.lt_u (local.get $d) (i32.mul (i32.const 131) (i32.const 8))) ;; built-in
					(then
						;; "copy-hnf"
						;;(call $debug (i32.const 4) (local.get $d) (i32.const -1) (i32.const -1))
						;; TODO: add more basic types
						(unreachable)
					)
				)
				;; No basic type
				(local.set $a-arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
				(if
					(i32.gt_s (local.get $a-arity) (i32.const 256))
					(then
						(local.set $a-arity (i32.load16_s (local.get $d)))
						(local.set $b-arity (i32.sub (i32.sub (i32.load16_s (i32.sub (local.get $d) (i32.const 2))) (local.get $a-arity)) (i32.const 256)))
					)
					(else
						(local.set $b-arity (i32.const 0))
					)
				)
				;; "copy-hnf"
				;;(call $debug (i32.const 4) (local.get $d) (local.get $a-arity) (local.get $b-arity))
				;; large node
				(if
					(i32.gt_s (i32.add (local.get $a-arity) (local.get $b-arity)) (i32.const 2))
					(then
						(i32.store offset=8 (local.get $to) (i32.load offset=8 (local.get $from)))
						(i32.store offset=16 (local.get $to) (i32.add (local.get $to) (i32.const 24)))

						(local.set $ab-arity (i32.add (local.get $a-arity) (local.get $b-arity)))
						(local.set $to (i32.add (local.get $to) (i32.const 24)))
						(local.set $from (i32.load offset=16 (local.get $from)))
						(block $end-copy-large-hnf
							(loop $copy-large-hnf
								(local.set $ab-arity (i32.sub (local.get $ab-arity) (i32.const 1)))
								(br_if $end-copy-large-hnf (i32.eqz (local.get $ab-arity)))
								(i32.store (local.get $to) (i32.load (local.get $from)))
								(local.set $to (i32.add (local.get $to) (i32.const 8)))
								(local.set $from (i32.add (local.get $from) (i32.const 8)))
								(br $copy-large-hnf)
							)
						)
						(return (local.get $to))
					)
				)
				;; small node
				(i64.store offset=8  (local.get $to) (i64.load offset=8  (local.get $from)))
				(i64.store offset=16 (local.get $to) (i64.load offset=16 (local.get $from)))
				(return (i32.add (local.get $to) (i32.add (i32.shl (i32.add (local.get $a-arity) (local.get $b-arity)) (i32.const 3)) (i32.const 8))))
			)
		)

		;; thunk, "copy-thunk"
		(local.set $ab-arity (i32.and (i32.load16_s (i32.sub (local.get $d) (i32.const 2))) (i32.const 0xff)))
		;;(call $debug (i32.const 3) (local.get $from) (local.get $d) (local.get $ab-arity))

		(local.set $d (local.get $ab-arity))
		(block $end-copy-thunk
			(loop $copy-thunk
				(br_if $end-copy-thunk (i32.eqz (local.get $d)))
				(local.set $d (i32.sub (local.get $d) (i32.const 1)))
				(i32.store
					(local.tee $to (i32.add (local.get $to) (i32.const 8)))
					(i32.load (local.tee $from (i32.add (local.get $from) (i32.const 8)))))
				(br $copy-thunk)
			)
		)

		(if
			(i32.lt_s (local.get $ab-arity) (i32.const 2))
			(then
				(local.set $to (i32.add (local.get $to) (i32.shl (i32.sub (i32.const 2) (local.get $ab-arity)) (i32.const 3))))
			)
		)

		(i32.add (local.get $to) (i32.const 8))
	)

	(func $update-ref (param $ref i32) (param $hp i32) (result i32)
		(local $n i32)
		(local $d i32)
		(local.set $n (i32.load (local.get $ref)))
		(local.set $d (i32.load (local.get $n)))

		(if
			(i32.or
				(i32.gt_s (global.get $start-heap) (local.get $n))
				(i32.lt_s (global.get $end-heap) (local.get $n))
			)
			(then (return (local.get $hp)))
		)

		(if
			(i32.and (local.get $d) (i32.const 1))
			(then
				(i32.store (local.get $ref) (i32.sub (local.get $d) (i32.const 1)))
				(return (local.get $hp))
			)
		)

		(i32.store (local.get $ref) (local.get $hp))
		(call $copy (local.get $n) (local.get $hp))
	)
)
