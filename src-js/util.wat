(module
	(import "clean" "memory" (memory 1))

	(global $start-heap (mut i32) (i32.const 0))
	(global $half-heap (mut i32) (i32.const 0))
	(global $end-heap (mut i32) (i32.const 0))
	(global $stack (mut i32) (i32.const 0))
	(global $in-first-semispace (mut i32) (i32.const 1))

	(func (export "strncmp") (param $s1 i32) (param $s2 i32) (param $n i32) (result i32)
		(local $n4 i32)
		(local $c1 i32)
		(local $c2 i32)

		(local.set $n4 (i32.shr_u (local.get $n) (i32.const 2)))

		(block $end-check-words
			(br_if $end-check-words (i32.eqz (local.get $n4)))
			(loop $check-words
				(br_if $end-check-words (i32.eqz (local.get $n4)))

				(local.set $c1 (i32.load8_s (local.get $s1)))
				(local.set $c2 (i32.load8_s (local.get $s2)))
				(if
					(i32.or (i32.eqz (local.get $c1)) (i32.ne (local.get $c1) (local.get $c2)))
					(then (return (i32.sub (local.get $c1) (local.get $c2))))
				)

				(local.set $c1 (i32.load8_s offset=1 (local.get $s1)))
				(local.set $c2 (i32.load8_s offset=1 (local.get $s2)))
				(if
					(i32.or (i32.eqz (local.get $c1)) (i32.ne (local.get $c1) (local.get $c2)))
					(then (return (i32.sub (local.get $c1) (local.get $c2))))
				)

				(local.set $c1 (i32.load8_s offset=2 (local.get $s1)))
				(local.set $c2 (i32.load8_s offset=2 (local.get $s2)))
				(if
					(i32.or (i32.eqz (local.get $c1)) (i32.ne (local.get $c1) (local.get $c2)))
					(then (return (i32.sub (local.get $c1) (local.get $c2))))
				)

				(local.set $c1 (i32.load8_s offset=3 (local.get $s1)))
				(local.set $c2 (i32.load8_s offset=3 (local.get $s2)))
				(if
					(i32.or (i32.eqz (local.get $c1)) (i32.ne (local.get $c1) (local.get $c2)))
					(then (return (i32.sub (local.get $c1) (local.get $c2))))
				)

				(local.set $s1 (i32.add (local.get $s1) (i32.const 4)))
				(local.set $s2 (i32.add (local.get $s2) (i32.const 4)))
				(local.set $n4 (i32.sub (local.get $n4) (i32.const 1)))

				(br $check-words)
			)
		)

		(local.set $n (i32.and (local.get $n) (i32.const 3)))
		(block $end-check-bytes
			(loop $check-bytes
				(br_if $end-check-bytes (i32.eqz (local.get $n)))

				(local.set $c1 (i32.load8_u (local.get $s1)))
				(local.set $c2 (i32.load8_u (local.get $s2)))
				(if
					(i32.or (i32.eqz (local.get $c1)) (i32.ne (local.get $c1) (local.get $c2)))
					(then (return (i32.sub (local.get $c1) (local.get $c2))))
				)

				(local.set $s1 (i32.add (local.get $s1) (i32.const 1)))
				(local.set $s2 (i32.add (local.get $s2) (i32.const 1)))
				(local.set $n (i32.sub (local.get $n) (i32.const 1)))

				(br $check-bytes)
			)
		)

		(i32.sub (local.get $c1) (local.get $c2))
	)

	(func (export "setup_gc") (param i32 i32 i32)
		(global.set $start-heap (local.get 0))
		(global.set $half-heap (i32.add (local.get 0) (local.get 1)))
		(global.set $end-heap (i32.add (local.get 0) (i32.add (local.get 1) (local.get 1))))
		(global.set $stack (local.get 2))
	)

	;; upper half of result is new hp pointer;
	;; lower half is hp-free
	(func (export "gc") (param $asp i32) (result i64)
		(local $old i32)
		(local $new i32)
		(local $n i32)
		(local $d i32)
		(local $a-arity i32)
		(local $ab-arity i32)
		(local $size i32)

		(if
			(global.get $in-first-semispace)
			(then
				(local.set $old (global.get $start-heap))
				(local.set $new (global.get $half-heap)))
			(else
				(local.set $old (global.get $half-heap))
				(local.set $new (global.get $start-heap)))
		)

		;; copy A stack pointers
		(block $end-copy-asp
			(loop $copy-asp
				(br_if $end-copy-asp (i32.eq (local.get $asp) (global.get $stack)))
				(local.set $new (call $update-ref (local.get $asp) (local.get $new)))
				(local.set $asp (i32.sub (local.get $asp) (i32.const 8)))
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
					;; hnf
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
							;; _STRING_+2
							(i32.eq (local.get $d) (i32.const 50))
							(then
								(local.set $size (i32.load offset=8 (local.get $n))) ;; size
								(local.set $size (i32.shr_u (i32.add (local.get $size) (i32.const 7)) (i32.const 3)))
								(local.set $n (i32.add (local.get $n) (i32.add (i32.shl (local.get $size) (i32.const 3)) (i32.const 16))))
								(br $update-refs)
							)
						)
						(if
							;; _ARRAY_+2
							(i32.eq (local.get $d) (i32.const 10))
							(then
								(local.set $size (i32.load offset=8 (local.get $n))) ;; size
								(local.set $d (i32.load offset=16 (local.get $n))) ;; child descriptor
								(if
									(i32.or ;; unboxed INT/REAL
										(i32.eq (local.get $d) (i32.const 170))
										(i32.eq (local.get $d) (i32.const 210))
									)
									(then
										(local.set $n (i32.add (local.get $n) (i32.add (i32.const 24)
											(i32.shl (local.get $size) (i32.const 3)))))
										(br $update-refs)
									)
								)
								(if ;; unboxed BOOL
									(i32.eq (local.get $d) (i32.const 90))
									(then
										(local.set $n (i32.add (local.get $n) (i32.add (i32.const 24)
											(i32.shl (local.get $size) (i32.const 6))))) ;; 3+3
										(br $update-refs)
									)
								)

								(if
									(i32.eqz (local.get $d))
									(then
										(local.set $ab-arity (i32.const 1))
										(local.set $a-arity (i32.const 1))
									)
									(else ;; unboxed record
										(local.set $ab-arity (i32.sub (i32.load16_s (i32.sub (local.get $d) (i32.const 2))) (i32.const 256)))
										(local.set $a-arity (i32.load16_s (local.get $d)))
									)
								)

								(if
									(i32.eqz (local.get $a-arity))
									(then
										(local.set $n (i32.add (local.get $n) (i32.add (i32.const 24)
											(i32.shl (i32.mul (local.get $size) (local.get $ab-arity)) (i32.const 3)))))
										(br $update-refs)
									)
								)

								(local.set $n (i32.add (local.get $n) (i32.const 24)))
								(local.set $ab-arity (i32.shl (local.get $ab-arity) (i32.const 3)))
								(local.set $a-arity (i32.shl (local.get $a-arity) (i32.const 3)))
								(loop $update-array
									(br_if $update-refs (i32.eqz (local.get $size)))

									(local.set $d (i32.const 0))
									(block $end-update-array-pointers
										(loop $update-array-pointers
											(br_if $end-update-array-pointers (i32.eq (local.get $d) (local.get $a-arity)))
											(local.set $new (call $update-ref (i32.add (local.get $n) (local.get $d)) (local.get $new)))
											(local.set $d (i32.add (local.get $d) (i32.const 8)))
											(br $update-array-pointers)
										)
									)

									(local.set $size (i32.sub (local.get $size) (i32.const 1)))
									(local.set $n (i32.add (local.get $n) (local.get $ab-arity)))
									(br $update-array)
								)
							)
						)
						;; not a basic type
						(local.set $ab-arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
						(local.set $a-arity (local.get $ab-arity))
						(if
							(i32.gt_s (local.get $ab-arity) (i32.const 256))
							(then
								(local.set $a-arity (i32.load16_s (local.get $d)))
								(local.set $ab-arity (i32.sub (local.get $ab-arity) (i32.const 256)))
							)
						)

						(if
							(i32.gt_s (local.get $ab-arity) (i32.const 2))
							(then
								;; large node
								(if
									(i32.gt_s (local.get $a-arity) (i32.const 0))
									(then
										(local.set $new (call $update-ref (i32.add (local.get $n) (i32.const 8)) (local.get $new)))

										(local.set $n (i32.add (local.get $n) (i32.const 16)))
										(local.set $a-arity (i32.shl (local.get $a-arity) (i32.const 3)))
										(block $end-update-large-hnf
											(loop $update-large-hnf
												(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 8)))
												(br_if $end-update-large-hnf (i32.eqz (local.get $a-arity)))
												(local.set $new (call $update-ref (i32.add (local.get $n) (local.get $a-arity)) (local.get $new)))
												(br $update-large-hnf)
											)
										)

										(local.set $n (i32.add (local.get $n) (i32.shl (local.get $ab-arity) (i32.const 3))))
										(br $update-refs)
									)
									(else
										(local.set $n (i32.add (local.get $n) (i32.shl (i32.add (i32.const 2) (local.get $ab-arity)) (i32.const 3))))
										(br $update-refs)
									)
								)
							)
							(else
								;; small node
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
					)
				)

				;; thunk
				(local.set $ab-arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
				(if
					(i32.lt_s (local.get $ab-arity) (i32.const 0))
					(then
						(local.set $a-arity (i32.const 1))
						(local.set $ab-arity (i32.const 1))
					)
					(else
						(local.set $a-arity (i32.sub (i32.and (local.get $ab-arity) (i32.const 0xff))
							(i32.and (i32.shr_u (local.get $ab-arity) (i32.const 8)) (i32.const 0xff))))
						(local.set $ab-arity (i32.and (local.get $ab-arity) (i32.const 0xff)))
					)
				)

				(local.set $d (i32.const 1)) ;; iterator
				(block $end-update-thunk-refs
					(loop $update-thunk-refs
						(br_if $end-update-thunk-refs (i32.gt_s (local.get $d) (local.get $a-arity)))
						(local.set $new (call $update-ref (i32.add (local.get $n) (i32.shl (local.get $d) (i32.const 3))) (local.get $new)))
						(local.set $d (i32.add (local.get $d) (i32.const 1)))
						(br $update-thunk-refs)
					)
				)
				(local.set $n
					(i32.add
						(local.get $n)
						(i32.shl
							(select
								(i32.add (local.get $ab-arity) (i32.const 1))
								(i32.const 3)
								(i32.gt_s (local.get $ab-arity) (i32.const 2))
							)
							(i32.const 3)
						)
					)
				)
				(br $update-refs)
			)
		)

		;; return
		(global.set $in-first-semispace (i32.sub (i32.const 1) (global.get $in-first-semispace)))
		(i64.or
			(i64.shl (i64.extend_i32_u (local.get $new)) (i64.const 32))
			(i64.extend_i32_u
				(i32.shr_u
					(i32.sub
						(select (global.get $half-heap) (global.get $end-heap) (global.get $in-first-semispace))
						(local.get $new)
					)
					(i32.const 3)
				)
			)
		)
	)

	(func $update-ref (param $ref i32) (param $hp i32) (result i32)
		(local $n i32)
		(local $d i32)
		(local $ab-arity i32)
		(local $size i32)

		(local.set $n (i32.load (local.get $ref)))

		(if
			(i32.or
				(i32.gt_s (global.get $start-heap) (local.get $n))
				(i32.lt_s (global.get $end-heap) (local.get $n))
			)
			(then (return (local.get $hp)))
		)

		(local.set $d (i32.load (local.get $n)))
		(if
			(i32.and (local.get $d) (i32.const 1))
			(then
				(i64.store (local.get $ref) (i64.extend_i32_u (i32.sub (local.get $d) (i32.const 1))))
				(return (local.get $hp))
			)
		)

		(i64.store (local.get $ref) (i64.extend_i32_u (local.get $hp)))

		(i64.store (local.get $hp) (i64.extend_i32_u (local.get $d)))
		(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 1))))

		(if (i32.and (local.get $d) (i32.const 2))
			;; hnf
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
						(i64.store offset=8 (local.get $hp) (i64.load offset=8 (local.get $n)))
						(return (i32.add (local.get $hp) (i32.const 16)))
					)
				)
				(if
					;; _STRING_+2
					(i32.eq (local.get $d) (i32.const 50))
					(then
						(local.set $size (i32.load offset=8 (local.get $n))) ;; size
						(i64.store offset=8 (local.get $hp) (i64.extend_i32_u (local.get $size)))
						(local.set $hp (i32.add (local.get $hp) (i32.const 16)))
						(local.set $n (i32.add (local.get $n) (i32.const 16)))
						(local.set $size (i32.shr_u (i32.add (local.get $size) (i32.const 7)) (i32.const 3)))
						(block $end-copy-string
							(loop $copy-string
								(br_if $end-copy-string (i32.eqz (local.get $size)))
								(i64.store (local.get $hp) (i64.load (local.get $n)))
								(local.set $size (i32.sub (local.get $size) (i32.const 1)))
								(local.set $hp (i32.add (local.get $hp) (i32.const 8)))
								(local.set $n (i32.add (local.get $n) (i32.const 8)))
								(br $copy-string)
							)
						)
						(return (local.get $hp))
					)
				)
				(if
					;; _ARRAY_+2
					(i32.eq (local.get $d) (i32.const 10))
					(then
						(local.set $size (i32.load offset=8 (local.get $n))) ;; size
						(local.set $d (i32.load offset=16 (local.get $n))) ;; element descriptor

						(i64.store offset=8 (local.get $hp) (i64.extend_i32_u (local.get $size)))
						(i64.store offset=16 (local.get $hp) (i64.extend_i32_u (local.get $d)))
						(local.set $hp (i32.add (local.get $hp) (i32.const 24)))
						(local.set $n (i32.add (local.get $n) (i32.const 24)))

						(if
							(i32.eqz ;; not
								(i32.or
									(i32.eqz (local.get $d)) ;; boxed
									(i32.or ;; unboxed INT/REAL
										(i32.eq (i32.const 170) (local.get $d))
										(i32.eq (local.get $d) (i32.const 210))
									)
								)
							)
							(then
								(if
									(i32.eq (local.get $d) (i32.const 90))
									(then ;; BOOL
										(local.set $size (i32.shr_u (i32.add (local.get $size) (i32.const 7)) (i32.const 3)))
									)
									(else ;; unboxed record
										(local.set $size (i32.mul (local.get $size)
											(i32.sub (i32.load16_s (i32.sub (local.get $d) (i32.const 2))) (i32.const 256))))
									)
								)
							)
						)

						(block $end-copy-array
							(loop $copy-array
								(br_if $end-copy-array (i32.eqz (local.get $size)))
								(i64.store (local.get $hp) (i64.load (local.get $n)))
								(local.set $size (i32.sub (local.get $size) (i32.const 1)))
								(local.set $hp (i32.add (local.get $hp) (i32.const 8)))
								(local.set $n (i32.add (local.get $n) (i32.const 8)))
								(br $copy-array)
							)
						)
						(return (local.get $hp))
					)
				)
				;; not a basic type
				(local.set $ab-arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
				(if
					(i32.gt_s (local.get $ab-arity) (i32.const 256))
					(then
						(local.set $ab-arity (i32.sub (local.get $ab-arity) (i32.const 256)))
					)
				)

				(i64.store offset=8 (local.get $hp) (i64.load offset=8 (local.get $n)))
				(if
					(i32.gt_s (local.get $ab-arity) (i32.const 2))
					(then
						;; hnf spread over two blocks
						(i64.store offset=16 (local.get $hp) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 24))))
						(local.set $hp (i32.add (local.get $hp) (i32.const 24)))
						(local.set $n (i32.load offset=16 (local.get $n)))

						(block $end-copy-large-hnf
							(loop $copy-large-hnf
								(local.set $ab-arity (i32.sub (local.get $ab-arity) (i32.const 1)))
								(br_if $end-copy-large-hnf (i32.eqz (local.get $ab-arity)))
								(i64.store (local.get $hp) (i64.load (local.get $n)))
								(local.set $hp (i32.add (local.get $hp) (i32.const 8)))
								(local.set $n (i32.add (local.get $n) (i32.const 8)))
								(br $copy-large-hnf)
							)
						)
						(return (local.get $hp))
					)
					(else
						;; small hnf
						(i64.store offset=16 (local.get $hp) (i64.load offset=16 (local.get $n)))
						(return (i32.add (local.get $hp) (i32.add (i32.shl (local.get $ab-arity) (i32.const 3)) (i32.const 8))))
					)
				)
			)
		)

		;; thunk
		(local.set $ab-arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
		(if
			(i32.lt_s (local.get $ab-arity) (i32.const 0))
			(then
				(local.set $ab-arity (i32.const 1)) ;; negative for selectors etc.
			)
			(else
				(local.set $ab-arity (i32.and (local.get $ab-arity) (i32.const 0xff)))
			)
		)

		(local.set $size (local.get $ab-arity)) ;; $size is just an iterator
		(block $end-copy-thunk
			(loop $copy-thunk
				(br_if $end-copy-thunk (i32.eqz (local.get $size)))
				(local.set $size (i32.sub (local.get $size) (i32.const 1)))
				(i64.store
					(local.tee $hp (i32.add (local.get $hp) (i32.const 8)))
					(i64.load (local.tee $n (i32.add (local.get $n) (i32.const 8)))))
				(br $copy-thunk)
			)
		)

		(select
			(i32.add (local.get $hp) (i32.shl (i32.sub (i32.const 3) (local.get $ab-arity)) (i32.const 3)))
			(i32.add (local.get $hp) (i32.const 8))
			(i32.lt_s (local.get $ab-arity) (i32.const 2))
		)
	)
)
