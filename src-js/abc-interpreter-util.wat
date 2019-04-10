(module
	(import "clean" "memory" (memory 1))
	;;(func $debug (import "clean" "debug") (param i32 i32 i32 i32))

	;; to get host references during garbage collection
	(func $has-host-reference (import "clean" "has_host_reference") (param i32) (result i32))
	(func $update-host-reference (import "clean" "update_host_reference") (param i32 i32))

	(global $start-heap (mut i32) (i32.const 0))
	(global $half-heap (mut i32) (i32.const 0))
	(global $end-heap (mut i32) (i32.const 0))
	(global $stack (mut i32) (i32.const 0))
	(global $caf-list (mut i32) (i32.const 0))
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

	(func (export "memcpy") (param $dest i32) (param $src i32) (param $nbytes i32)
		(local $i i32)
		(local.set $i (i32.shr_u (local.get $nbytes) (i32.const 3)))
		(local.set $nbytes (i32.and (local.get $nbytes) (i32.const 0x7)))

		(block $end-copy-words
			(loop $copy-words
				(br_if $end-copy-words (i32.eqz (local.get $i)))
				(i64.store (local.get $dest) (i64.load (local.get $src)))
				(local.set $dest (i32.add (local.get $dest) (i32.const 8)))
				(local.set $src (i32.add (local.get $src) (i32.const 8)))
				(local.set $i (i32.sub (local.get $i) (i32.const 1)))
				(br $copy-words)
			)
		)

		(block $end
			(br_if $end (i32.eqz (local.get $nbytes)))
			(i64.store8 (local.get $dest) (i64.load8_u (local.get $src)))
			(br_if $end (i32.eq (local.get $nbytes) (i32.const 1)))
			(i64.store8 offset=1 (local.get $dest) (i64.load8_u offset=1 (local.get $src)))
			(br_if $end (i32.eq (local.get $nbytes) (i32.const 2)))
			(i64.store8 offset=2 (local.get $dest) (i64.load8_u offset=2 (local.get $src)))
			(br_if $end (i32.eq (local.get $nbytes) (i32.const 3)))
			(i64.store8 offset=3 (local.get $dest) (i64.load8_u offset=3 (local.get $src)))
			(br_if $end (i32.eq (local.get $nbytes) (i32.const 4)))
			(i64.store8 offset=4 (local.get $dest) (i64.load8_u offset=4 (local.get $src)))
			(br_if $end (i32.eq (local.get $nbytes) (i32.const 5)))
			(i64.store8 offset=5 (local.get $dest) (i64.load8_u offset=5 (local.get $src)))
			(br_if $end (i32.eq (local.get $nbytes) (i32.const 6)))
			(i64.store8 offset=6 (local.get $dest) (i64.load8_u offset=6 (local.get $src)))
		)
	)

	(func (export "setup_gc") (param i32 i32 i32 i32)
		(global.set $start-heap (local.get 0))
		(global.set $half-heap (i32.add (local.get 0) (local.get 1)))
		(global.set $end-heap (i32.add (local.get 0) (i32.add (local.get 1) (local.get 1))))
		(global.set $stack (local.get 2))
		(global.set $caf-list (local.get 3))
	)

	(func (export "get_unused_semispace") (result i32)
		(select
			(global.get $half-heap)
			(global.get $start-heap)
			(global.get $in-first-semispace)
		)
	)

	;; upper half of result is new hp pointer;
	;; lower half is hp-free
	(func (export "gc") (param $asp i32) (result i64)
		(local $old i32)
		(local $new i32)
		(local $n i32)
		(local $d i32)
		(local $a-arity i32)
		(local $arity i32)
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

		;; copy CAF pointers
		(local.set $n (i32.add (global.get $caf-list) (i32.const 8)))
		(block $end-copy-cafs
			(loop $copy-cafs
				(br_if $end-copy-cafs (i32.eqz (i32.load (i32.sub (local.get $n) (i32.const 8)))))
				(local.set $n (i32.load (i32.sub (local.get $n) (i32.const 8))))
				(local.set $a-arity (i32.load16_s (local.get $n)))
				(block $end-copy-caf-pointers
					(loop $copy-caf-pointers
						(br_if $end-copy-caf-pointers (i32.eqz (local.get $a-arity)))
						(local.set $new (call $update-ref (i32.add (local.get $n) (i32.shl (local.get $a-arity) (i32.const 3))) (local.get $new)))
						(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
						(br $copy-caf-pointers)
					)
				)
				(br $copy-cafs)
			)
		)

		;; copy host references
		(local.set $arity (i32.load (local.get $asp))) ;; to restore
		(local.set $d (i32.const 0))
		(block $end-copy-host-references
			(loop $copy-host-references
				(local.set $n (call $has-host-reference (local.get $d)))
				(br_if $end-copy-host-references (i32.eqz (local.get $n)))
				(i32.store (local.get $asp) (local.get $n))
				(local.set $new (call $update-ref (local.get $asp) (local.get $new)))
				(call $update-host-reference (local.get $d) (i32.load (local.get $asp)))
				(local.set $d (i32.add (local.get $d) (i32.const 1)))
				(br $copy-host-references)
			)
		)
		(i32.store (local.get $asp) (local.get $arity))

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
										(local.set $arity (i32.const 1))
										(local.set $a-arity (i32.const 1))
									)
									(else ;; unboxed record
										(local.set $arity (i32.sub (i32.load16_s (i32.sub (local.get $d) (i32.const 2))) (i32.const 256)))
										(local.set $a-arity (i32.load16_s (local.get $d)))
									)
								)

								(if
									(i32.eqz (local.get $a-arity))
									(then
										(local.set $n (i32.add (local.get $n) (i32.add (i32.const 24)
											(i32.shl (i32.mul (local.get $size) (local.get $arity)) (i32.const 3)))))
										(br $update-refs)
									)
								)

								(local.set $n (i32.add (local.get $n) (i32.const 24)))
								(local.set $arity (i32.shl (local.get $arity) (i32.const 3)))
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
									(local.set $n (i32.add (local.get $n) (local.get $arity)))
									(br $update-array)
								)
							)
						)
						;; not a basic type
						(local.set $arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
						(local.set $a-arity (local.get $arity))
						(if
							(i32.gt_s (local.get $arity) (i32.const 256))
							(then
								(local.set $a-arity (i32.load16_s (local.get $d)))
								(local.set $arity (i32.sub (local.get $arity) (i32.const 256)))
							)
						)

						(if
							(i32.gt_s (local.get $arity) (i32.const 2))
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

										(local.set $n (i32.add (local.get $n) (i32.shl (local.get $arity) (i32.const 3))))
										(br $update-refs)
									)
									(else
										(local.set $n (i32.add (local.get $n) (i32.shl (i32.add (i32.const 2) (local.get $arity)) (i32.const 3))))
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
								(local.set $n (i32.add (local.get $n) (i32.add (i32.shl (local.get $arity) (i32.const 3)) (i32.const 8))))
								(br $update-refs)
							)
						)
					)
				)

				;; thunk
				(local.set $arity (i32.load (i32.sub (local.get $d) (i32.const 4))))
				(if
					(i32.lt_s (local.get $arity) (i32.const 0))
					(then
						(local.set $a-arity (i32.const 1))
						(local.set $arity (i32.const 1))
					)
					(else
						(local.set $a-arity (i32.sub (i32.and (local.get $arity) (i32.const 0xff))
							(i32.and (i32.shr_u (local.get $arity) (i32.const 8)) (i32.const 0xff))))
						(local.set $arity (i32.and (local.get $arity) (i32.const 0xff)))
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
								(i32.add (local.get $arity) (i32.const 1))
								(i32.const 3)
								(i32.gt_s (local.get $arity) (i32.const 2))
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
		(local $arity i32)
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
				(local.set $arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
				(if
					(i32.gt_s (local.get $arity) (i32.const 256))
					(then
						(local.set $arity (i32.sub (local.get $arity) (i32.const 256)))
					)
				)

				(i64.store offset=8 (local.get $hp) (i64.load offset=8 (local.get $n)))
				(if
					(i32.gt_s (local.get $arity) (i32.const 2))
					(then
						;; hnf spread over two blocks
						(i64.store offset=16 (local.get $hp) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 24))))
						(local.set $hp (i32.add (local.get $hp) (i32.const 24)))
						(local.set $n (i32.load offset=16 (local.get $n)))

						(block $end-copy-large-hnf
							(loop $copy-large-hnf
								(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
								(br_if $end-copy-large-hnf (i32.eqz (local.get $arity)))
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
						(return (i32.add (local.get $hp) (i32.add (i32.shl (local.get $arity) (i32.const 3)) (i32.const 8))))
					)
				)
			)
		)

		;; thunk
		(local.set $arity (i32.load (i32.sub (local.get $d) (i32.const 4))))
		(if
			(i32.lt_s (local.get $arity) (i32.const 0))
			(then
				(local.set $arity (i32.const 1)) ;; negative for selectors etc.
			)
			(else
				(local.set $arity (i32.and (local.get $arity) (i32.const 0xff)))
			)
		)

		(local.set $size (local.get $arity)) ;; $size is just an iterator
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
			(i32.add (local.get $hp) (i32.shl (i32.sub (i32.const 3) (local.get $arity)) (i32.const 3)))
			(i32.add (local.get $hp) (i32.const 8))
			(i32.lt_s (local.get $arity) (i32.const 2))
		)
	)

	(func $copy (param $to i32) (param $from i32) (param $n-words i32)
		(block $end
			(loop $loop
				(local.set $n-words (i32.sub (local.get $n-words) (i32.const 1)))
				(br_if $end (i32.lt_s (local.get $n-words) (i32.const 0)))
				(i64.store
					(i32.add (local.get $to) (i32.shl (local.get $n-words) (i32.const 3)))
					(i64.load (i32.add (local.get $from) (i32.shl (local.get $n-words) (i32.const 3)))))
				(br $loop)
			)
		)
	)

	(func (export "copy_from_string")
		(param $s i32) (param $len i32)
		(param $asp i32) (param $bsp i32) (param $hp i32)
		(param $code-offset i32)
		(result i32)

		(local $ptr-stack i32)
		(local $a-size-stack i32)
		(local $i i32)
		(local $desc i32)
		(local $a-arity i32)
		(local $b-arity i32)
		(local $arity i32)
		(local $a i32)
		(local $j i32)
		(local $k i32)

		(local.set $ptr-stack (local.get $asp))
		(local.set $a-size-stack (local.get $bsp))

		(i32.store (local.tee $ptr-stack (i32.add (local.get $ptr-stack) (i32.const 4))) (local.get $s)) ;; dummy
		(i32.store (local.tee $a-size-stack (i32.sub (local.get $a-size-stack) (i32.const 2))) (i32.const 1))

		(local.set $i (i32.const -8))
		(local.set $len (i32.shl (local.get $len) (i32.const 3)))
		(block $end
			(loop $loop
				(local.set $i (i32.add (local.get $i) (i32.const 8)))
				(br_if $end (i32.ge_u (local.get $i) (local.get $len)))

				(if
					(i32.eqz (i32.load16_s (local.get $a-size-stack)))
					(then
						(local.set $i (i32.sub (local.get $i) (i32.const 8)))
						(local.set $a-size-stack (i32.add (local.get $a-size-stack) (i32.const 2)))
						(br $loop)
					)
				)

				;; "loop"
				;;(call $debug (i32.const 0) (local.get $i) (local.get $len) (local.get $hp))

				(i32.store16 (local.get $a-size-stack) (i32.sub (i32.load16_s (local.get $a-size-stack)) (i32.const 1)))

				(local.set $desc (i32.load (i32.add (local.get $s) (local.get $i))))
				(i64.store (i32.add (local.get $s) (local.get $i)) (i64.extend_i32_u (local.get $hp)))

				(if ;; redirection or predefined constructor
					(i32.lt_s (local.get $desc) (i32.const 0))
					(then
						(block $no-predefined-constructor
							(block $predefined-constructor
								;; predefined constructors: see ABC.Interpreter for the $desc values;
								;; the constructors written to the heap are from the interpreter generator.
								(if ;; BOOL
									(i32.eq (local.get $desc) (i32.const -22))
									(then
										(i64.store (local.get $hp) (i64.const 90))
										(i64.store offset=8 (local.get $hp) (i64.load offset=8 (i32.add (local.get $s) (local.get $i))))
										(br $predefined-constructor)
									)
								)
								(if ;; CHAR
									(i32.eq (local.get $desc) (i32.const -30))
									(then
										(i64.store (local.get $hp) (i64.const 130))
										(i64.store offset=8 (local.get $hp) (i64.load offset=8 (i32.add (local.get $s) (local.get $i))))
										(br $predefined-constructor)
									)
								)
								(if ;; REAL
									(i32.eq (local.get $desc) (i32.const -38))
									(then
										(i64.store (local.get $hp) (i64.const 170))
										(i64.store offset=8 (local.get $hp) (i64.load offset=8 (i32.add (local.get $s) (local.get $i))))
										(br $predefined-constructor)
									)
								)
								(if ;; INT
									(i32.eq (local.get $desc) (i32.const -46))
									(then
										(i64.store (local.get $hp) (i64.const 210))
										(i64.store offset=8 (local.get $hp) (i64.load offset=8 (i32.add (local.get $s) (local.get $i))))
										(br $predefined-constructor)
									)
								)

								(br $no-predefined-constructor)
							)

							(i64.store (i32.load (local.get $ptr-stack)) (i64.extend_i32_u (local.get $hp)))
							(local.set $ptr-stack (i32.sub (local.get $ptr-stack) (i32.const 4)))
							(local.set $hp (i32.add (local.get $hp) (i32.const 16)))
							(local.set $i (i32.add (local.get $i) (i32.const 8)))
							(br $loop)
						)

						(if ;; _STRING_
							(i32.eq (local.get $desc) (i32.const -14))
							(then
								(i64.store (i32.load (local.get $ptr-stack)) (i64.extend_i32_u (local.get $hp)))
								(local.set $ptr-stack (i32.sub (local.get $ptr-stack) (i32.const 4)))

								(i64.store (local.get $hp) (i64.const 50))
								(local.set $a (i32.load offset=8 (i32.add (local.get $s) (local.get $i))))
								(i64.store offset=8 (local.get $hp) (i64.extend_i32_u (local.get $a)))
								(local.set $a (i32.shr_u (i32.add (local.get $a) (i32.const 7)) (i32.const 3)))
								(call $copy
									(i32.add (local.get $hp) (i32.const 16))
									(i32.add (local.get $s) (i32.add (local.get $i) (i32.const 16)))
									(local.get $a))

								(local.set $hp (i32.add (local.get $hp) (i32.add (i32.const 8) (i32.shl (local.get $a) (i32.const 3)))))
								(local.set $i (i32.add (local.get $i) (i32.add (i32.const 16) (i32.shl (local.get $a) (i32.const 3)))))
								(br $loop)
							)
						)

						(if ;; _ARRAY_
							(i32.eq (local.get $desc) (i32.const -6))
							(then
								(i64.store (i32.load (local.get $ptr-stack)) (i64.extend_i32_u (local.get $hp)))
								(local.set $ptr-stack (i32.sub (local.get $ptr-stack) (i32.const 4)))

								(local.set $a (i32.load offset=8 (i32.add (local.get $s) (local.get $i))))
								(local.set $desc (i32.load offset=16 (i32.add (local.get $s) (local.get $i))))

								(i64.store (local.get $hp) (i64.const 10))
								(i64.store offset=8 (local.get $hp) (i64.extend_i32_u (local.get $a)))

								(if ;; INT elements
									(i32.eq (local.get $desc) (i32.const -46))
									(then
										(i64.store offset=16 (local.get $hp) (i64.const 210))
										(call $copy
											(i32.add (local.get $hp) (i32.const 24))
											(i32.add (local.get $s) (i32.add (local.get $i) (i32.const 24)))
											(local.get $a))
										(local.set $a (i32.shl (local.get $a) (i32.const 3)))
										(local.set $hp (i32.add (local.get $hp) (i32.add (i32.const 24) (local.get $a))))
										(local.set $i (i32.add (local.get $i) (i32.add (i32.const 16) (local.get $a))))
										(br $loop)
									)
								)
								(if ;; REAL elements
									(i32.eq (local.get $desc) (i32.const -38))
									(then
										(i64.store offset=16 (local.get $hp) (i64.const 170))
										(call $copy
											(i32.add (local.get $hp) (i32.const 24))
											(i32.add (local.get $s) (i32.add (local.get $i) (i32.const 24)))
											(local.get $a))
										(local.set $a (i32.shl (local.get $a) (i32.const 3)))
										(local.set $hp (i32.add (local.get $hp) (i32.add (i32.const 24) (local.get $a))))
										(local.set $i (i32.add (local.get $i) (i32.add (i32.const 16) (local.get $a))))
										(br $loop)
									)
								)
								(if ;; BOOL elements
									(i32.eq (local.get $desc) (i32.const -22))
									(then
										(i64.store offset=16 (local.get $hp) (i64.const 90))
										(local.set $a (i32.shr_u (i32.add (local.get $a) (i32.const 7)) (i32.const 3)))
										(call $copy
											(i32.add (local.get $hp) (i32.const 24))
											(i32.add (local.get $s) (i32.add (local.get $i) (i32.const 24)))
											(local.get $a))
										(local.set $a (i32.shl (local.get $a) (i32.const 3)))
										(local.set $hp (i32.add (local.get $hp) (i32.add (i32.const 24) (local.get $a))))
										(local.set $i (i32.add (local.get $i) (i32.add (i32.const 16) (local.get $a))))
										(br $loop)
									)
								)

								(if
									(i32.eqz (local.get $desc))
									(then
										(local.set $arity (i32.const 1))
										(local.set $a-arity (i32.const 1))
									)
									(else
										(local.set $desc (i32.add (local.get $desc) (local.get $code-offset)))
										(local.set $arity (i32.sub (i32.load16_u (i32.sub (local.get $desc) (i32.const 2))) (i32.const 256)))
										(local.set $a-arity (i32.load16_u (local.get $desc)))
									)
								)
								(local.set $b-arity (i32.sub (local.get $arity) (local.get $a-arity)))

								(i64.store offset=16 (local.get $hp) (i64.extend_i32_u (local.get $desc)))
								(local.set $hp (i32.add (local.get $hp) (i32.const 24)))
								(local.set $i (i32.add (local.get $i) (i32.const 16)))

								(local.set $ptr-stack (i32.add (local.get $ptr-stack)
									(i32.shl (i32.mul (local.get $a) (local.get $a-arity)) (i32.const 2))))
								(i32.store16 (local.tee $a-size-stack (i32.sub (local.get $a-size-stack) (i32.const 2)))
									(i32.mul (local.get $a) (local.get $a-arity)))

								(local.set $arity (i32.shl (local.get $arity) (i32.const 3)))
								(local.set $j (i32.const 0))
								(block $end-copy-array-elements
									(loop $copy-array-elements
										(br_if $end-copy-array-elements (i32.eq (local.get $j) (local.get $a)))

										(local.set $k (i32.const 0))
										(block $end-push-pointers
											(loop $push-pointers
												(br_if $end-push-pointers (i32.eq (local.get $k) (local.get $a-arity)))
												(i32.store
													(i32.sub (local.get $ptr-stack)
														(i32.shl (i32.add (i32.mul (local.get $j) (local.get $a-arity)) (local.get $k)) (i32.const 2)))
													(i32.add (local.get $hp) (i32.shl (local.get $k) (i32.const 3))))
												(local.set $k (i32.add (local.get $k) (i32.const 1)))
												(br $push-pointers)
											)
										)

										(call $copy
											(i32.add (local.get $hp) (i32.shl (local.get $a-arity) (i32.const 3)))
											(i32.add (local.get $s) (i32.add (local.get $i) (i32.const 8)))
											(local.get $b-arity))
										(local.set $i (i32.add (local.get $i) (i32.shl (local.get $b-arity) (i32.const 3))))

										(local.set $hp (i32.add (local.get $hp) (local.get $arity)))
										(local.set $j (i32.add (local.get $j) (i32.const 1)))
										(br $copy-array-elements)
									)
								)

								(br $loop)
							)
						)

						;; not a predefined constructor; redirection
						(i64.store
							(i32.load (local.get $ptr-stack))
							(i64.load (i32.add (local.get $s) (i32.add (local.get $i) (i32.sub (local.get $desc) (i32.const 1))))))
						(local.set $ptr-stack (i32.sub (local.get $ptr-stack) (i32.const 4)))
						(br $loop)
					)
				)

				(local.set $desc (i32.add (local.get $desc) (local.get $code-offset)))

				;; "desc"
				;;(call $debug (i32.const 1) (local.get $desc) (i32.const 0) (i32.const 0))

				(if
					(i32.and (local.get $desc) (i32.const 2))
					(then
						;; hnf
						(local.set $arity (i32.load16_s (i32.sub (local.get $desc) (i32.const 2))))
						(local.set $a-arity (local.get $arity))

						;; "arity"
						;;(call $debug (i32.const 2) (local.get $arity) (i32.const 0) (i32.const 0))

						(if
							(i32.gt_u (local.get $arity) (i32.const 256))
							(then
								(local.set $a-arity (i32.load16_s (local.get $desc)))
								(local.set $arity (i32.sub (local.get $arity) (i32.const 256)))
							)
						)

						(if
							(i32.eqz (local.get $arity))
							(then
								(local.set $desc (i32.sub (local.get $desc) (i32.const 10)))
								(i64.store (i32.load (local.get $ptr-stack)) (i64.extend_i32_u (local.get $desc)))
								(local.set $ptr-stack (i32.sub (local.get $ptr-stack) (i32.const 4)))
								(i64.store (i32.add (local.get $s) (local.get $i)) (i64.extend_i32_u (local.get $desc)))
								(i32.store16 (local.tee $a-size-stack (i32.sub (local.get $a-size-stack) (i32.const 2))) (i32.const 0))
								(br $loop)
							)
						)

						(i64.store (i32.load (local.get $ptr-stack)) (i64.extend_i32_u (local.get $hp)))
						(local.set $ptr-stack (i32.sub (local.get $ptr-stack) (i32.const 4)))

						(i64.store (local.get $hp) (i64.extend_i32_u (local.get $desc)))

						(if
							(i32.eq (local.get $arity) (i32.const 1))
							(then
								(if
									(i32.eq (local.get $a-arity) (i32.const 1))
									(then
										(i32.store
											(local.tee $ptr-stack (i32.add (local.get $ptr-stack) (i32.const 4)))
											(i32.add (local.get $hp) (i32.const 8)))
									)
									(else
										(i64.store offset=8 (local.get $hp)
											(i64.load (i32.add (local.get $s)
												(local.tee $i (i32.add (local.get $i) (i32.const 8))))))
									)
								)
								(local.set $hp (i32.add (local.get $hp) (i32.const 16)))
								(i32.store16 (local.tee $a-size-stack (i32.sub (local.get $a-size-stack) (i32.const 2))) (local.get $a-arity))
								(br $loop)
							)
						)
						(if
							(i32.eq (local.get $arity) (i32.const 2))
							(then
								(if
									(i32.eq (local.get $a-arity) (i32.const 2))
									(then
										(i32.store offset=8 (local.get $ptr-stack) (i32.add (local.get $hp) (i32.const 8)))
										(i32.store offset=4 (local.get $ptr-stack) (i32.add (local.get $hp) (i32.const 16)))
										(local.set $ptr-stack (i32.add (local.get $ptr-stack) (i32.const 8)))
									)
									(else
										(if
											(i32.eq (local.get $a-arity) (i32.const 1))
											(then
												(i32.store offset=4 (local.get $ptr-stack) (i32.add (local.get $hp) (i32.const 8)))
												(local.set $ptr-stack (i32.add (local.get $ptr-stack) (i32.const 4)))
												(i64.store offset=16 (local.get $hp)
													(i64.load (i32.add (local.get $s)
														(local.tee $i (i32.add (local.get $i) (i32.const 8))))))
											)
											(else ;; b-arity=2
												(i64.store offset=8 (local.get $hp)
													(i64.load (i32.add (local.get $s) (i32.add (local.get $i) (i32.const 8)))))
												(i64.store offset=16 (local.get $hp)
													(i64.load (i32.add (local.get $s) (i32.add (local.get $i) (i32.const 16)))))
												(local.set $i (i32.add (local.get $i) (i32.const 16)))
											)
										)
									)
								)
								(local.set $hp (i32.add (local.get $hp) (i32.const 24)))
								(i32.store16 (local.tee $a-size-stack (i32.sub (local.get $a-size-stack) (i32.const 2))) (local.get $a-arity))
								(br $loop)
							)
						)

						;; large hnf
						(i64.store offset=16 (local.get $hp) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 24))))

						(if
							(i32.eqz (local.get $a-arity))
							(then
								(i64.store offset=8 (local.get $hp)
									(i64.load (i32.add (local.get $s)
										(local.tee $i (i32.add (local.get $i) (i32.const 8))))))
								(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
								(call $copy
									(i32.add (local.get $hp) (i32.const 24))
									(i32.add (local.get $s) (local.get $i))
									(local.get $arity))
								(local.set $i (i32.add (local.get $i) (i32.shl (local.get $arity) (i32.const 3))))
							)
							(else
								(i32.store
									(i32.add (local.get $ptr-stack) (i32.shl (local.get $a-arity) (i32.const 2)))
									(i32.add (local.get $hp) (i32.const 8)))
								(local.set $a (i32.const 1))
								(block $end-push-pointers
									(loop $push-pointers
										(br_if $end-push-pointers (i32.eq (local.get $a) (local.get $a-arity)))
										(i32.store
											(i32.add (local.get $ptr-stack) (i32.shl (local.get $a) (i32.const 2)))
											(i32.add (local.get $hp) (i32.add (i32.const 16) (i32.shl (i32.sub (local.get $a-arity) (local.get $a)) (i32.const 3)))))
										(local.set $a (i32.add (local.get $a) (i32.const 1)))
										(br $push-pointers)
									)
								)
								(local.set $ptr-stack (i32.add (local.get $ptr-stack) (i32.shl (local.get $a-arity) (i32.const 2))))
								(call $copy
									(i32.add (local.get $hp) (i32.add (i32.const 16) (i32.shl (local.get $a-arity) (i32.const 3))))
									(i32.add (local.get $s) (i32.add (i32.const 8) (local.get $i)))
									(i32.sub (local.get $arity) (local.get $a-arity)))
								(local.set $i (i32.add (local.get $i) (i32.shl (i32.sub (local.get $arity) (local.get $a-arity)) (i32.const 3))))
							)
						)

						(local.set $hp (i32.add (local.get $hp) (i32.add (i32.shl (local.get $arity) (i32.const 3)) (i32.const 16))))
						(i32.store16 (local.tee $a-size-stack (i32.sub (local.get $a-size-stack) (i32.const 2))) (local.get $a-arity))
						(br $loop)
					)
					(else
						(local.set $arity (i32.load (i32.sub (local.get $desc) (i32.const 4))))
						(local.set $a-arity (local.get $arity))

						;; "arity"
						;;(call $debug (i32.const 3) (local.get $arity) (i32.const 0) (i32.const 0))

						(if
							(i32.lt_s (local.get $arity) (i32.const 0))
							(then
								(local.set $arity (i32.const 1))
							)
							(else
								(if
									(i32.gt_u (local.get $arity) (i32.const 256))
									(then
										(local.set $a-arity (i32.shr_u (local.get $arity) (i32.const 8)))
										(local.set $arity (i32.and (local.get $arity) (i32.const 0xff)))
									)
								)
							)
						)

						(i64.store (i32.load (local.get $ptr-stack)) (i64.extend_i32_u (local.get $hp)))
						(local.set $ptr-stack (i32.sub (local.get $ptr-stack) (i32.const 4)))

						(i64.store (local.get $hp) (i64.extend_i32_u (local.get $desc)))
						(local.set $hp (i32.add (local.get $hp) (i32.const 8)))

						(i32.store16 (local.tee $a-size-stack (i32.sub (local.get $a-size-stack) (i32.const 2))) (local.get $a-arity))

						(local.set $a-arity (i32.shl (local.get $a-arity) (i32.const 2)))
						(local.set $a (i32.const 0))
						(block $end-push-pointers
							(loop $push-pointers
								(br_if $end-push-pointers (i32.eq (local.get $a) (local.get $a-arity)))
								(i32.store
									(i32.add (local.get $ptr-stack) (i32.sub (local.get $a-arity) (local.get $a)))
									(i32.add (local.get $hp) (i32.shl (local.get $a) (i32.const 1))))
								(local.set $a (i32.add (local.get $a) (i32.const 4)))
								(br $push-pointers)
							)
						)
						(local.set $ptr-stack (i32.add (local.get $ptr-stack) (local.get $a-arity)))

						(local.set $a (i32.shl (local.get $a) (i32.const 1)))
						(local.set $arity (i32.shl (local.get $arity) (i32.const 3)))
						(block $end-add-thunk-values
							(loop $add-thunk-values
								(br_if $end-add-thunk-values (i32.eq (local.get $a) (local.get $arity)))
								(i64.store
									(i32.add (local.get $hp) (local.get $a))
									(i64.load (i32.add (local.get $s)
										(local.tee $i (i32.add (local.get $i) (i32.const 8))))))
								(local.set $a (i32.add (local.get $a) (i32.const 8)))
								(br $add-thunk-values)
							)
						)

						(local.set $hp (i32.add (local.get $hp)
							(select
								(i32.const 16)
								(local.get $arity)
								(i32.lt_s (local.get $arity) (i32.const 24)))))
						(br $loop)
					)
				)

				(unreachable)
			)
		)

		(local.get $hp)
	)
)
