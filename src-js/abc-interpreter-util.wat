(module
	(import "clean" "memory" (memory 1))
	;;(func $debug (import "clean" "debug") (param i32 i32 i32 i32))

	;; to get host references during garbage collection
	;; has-host-reference should return a heap pointer when the requested index
	;;   is found, 0 if it is not found and no higher index is known, or -1 if
	;;   it is not found but higher indexes may be known
	(func $has-host-reference (import "clean" "has_host_reference") (param i32) (result i32))
	(func $update-host-reference (import "clean" "update_host_reference") (param i32 i32))

	(func $gc-start (import "clean" "gc_start"))
	(func $gc-end (import "clean" "gc_end"))
	;; When a node with the js-ref-constructor is found, the callback is called
	;; with its argument. This is used in iTasks to clean up the references from
	;; wasm to JS.
	(global $js-ref-constructor (mut i32) (i32.const 0))
	(func $js-ref-found (import "clean" "js_ref_found") (param i32))

	(func $set-hp (import "clean" "set_hp") (param i32))
	(func $set-hp-free (import "clean" "set_hp_free") (param i32))

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

	(func $d-to-a (param $d f64) (param $s i32)
		(local $i1 i32)
		(local $i2 i32)
		(local $n i32)
		(local $r i32)
		(local $d-i i64)
		(local $d-i-1000000000 i64)

		(local.set $d-i (i64.trunc_f64_s (local.get $d)))
		(local.set $d-i-1000000000 (i64.div_u (local.get $d-i) (i64.const 1000000000)))
		(local.set $n (i32.wrap_i64 (local.get $d-i-1000000000)))
		(local.set $r (i32.wrap_i64 (i64.sub (local.get $d-i)
			(i64.mul (local.get $d-i-1000000000) (i64.const 1000000000)))))

		(local.set $i2 (i32.add (i32.const 1) (i32.wrap_i64
			(i64.shr_u (i64.mul (i64.extend_i32_u (local.get $r)) (i64.const 0xabcc7712)) (i64.const 30)))))
		(i32.store8 offset=6 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 28)) (i32.const 48))) ;; '0'

		(local.set $i1 (i32.add (i32.const 1) (i32.wrap_i64
			(i64.shr_u (i64.mul (i64.extend_i32_u (local.get $n)) (i64.const 0x0a7c5ac48)) (i64.const 30)))))

		(local.set $i2 (i32.mul (i32.const 5) (i32.and (i32.const 0xfffffff) (local.get $i2))))
		(i32.store8 offset=7 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 27)) (i32.const 48)))

		(i32.store8 offset=0 (local.get $s) (i32.add (i32.shr_u (local.get $i1) (i32.const 18)) (i32.const 48)))

		(local.set $i2 (i32.mul (i32.const 5) (i32.and (i32.const 0x7ffffff) (local.get $i2))))
		(i32.store8 offset=8 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 26)) (i32.const 48)))

		(local.set $i1 (i32.mul (i32.const 5) (i32.and (i32.const 0x3ffff) (local.get $i1))))
		(i32.store8 offset=1 (local.get $s) (i32.add (i32.shr_u (local.get $i1) (i32.const 17)) (i32.const 48)))

		(local.set $i2 (i32.mul (i32.const 5) (i32.and (i32.const 0x3ffffff) (local.get $i2))))
		(i32.store8 offset=9 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 25)) (i32.const 48)))

		(local.set $i1 (i32.mul (i32.const 5) (i32.and (i32.const 0x1ffff) (local.get $i1))))
		(i32.store8 offset=2 (local.get $s) (i32.add (i32.shr_u (local.get $i1) (i32.const 16)) (i32.const 48)))

		(local.set $i2 (i32.mul (i32.const 5) (i32.and (i32.const 0x1ffffff) (local.get $i2))))
		(i32.store8 offset=10 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 24)) (i32.const 48)))

		(local.set $i1 (i32.mul (i32.const 5) (i32.and (i32.const 0xffff) (local.get $i1))))
		(i32.store8 offset=3 (local.get $s) (i32.add (i32.shr_u (local.get $i1) (i32.const 15)) (i32.const 48)))

		(local.set $i2 (i32.mul (i32.const 5) (i32.and (i32.const 0xffffff) (local.get $i2))))
		(i32.store8 offset=11 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 23)) (i32.const 48)))

		(local.set $i1 (i32.mul (i32.const 5) (i32.and (i32.const 0x7fff) (local.get $i1))))
		(i32.store8 offset=4 (local.get $s) (i32.add (i32.shr_u (local.get $i1) (i32.const 14)) (i32.const 48)))

		(local.set $i2 (i32.mul (i32.const 5) (i32.and (i32.const 0x7fffff) (local.get $i2))))
		(i32.store8 offset=12 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 22)) (i32.const 48)))

		(local.set $i1 (i32.mul (i32.const 5) (i32.and (i32.const 0x3fff) (local.get $i1))))
		(i32.store8 offset=5 (local.get $s) (i32.add (i32.shr_u (local.get $i1) (i32.const 13)) (i32.const 48)))

		(local.set $i2 (i32.mul (i32.const 5) (i32.and (i32.const 0x3fffff) (local.get $i2))))
		(i32.store8 offset=13 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 21)) (i32.const 48)))

		(i32.store8 offset=15 (local.get $s) (i32.const 0))

		(local.set $i2 (i32.mul (i32.const 5) (i32.and (i32.const 0x1fffff) (local.get $i2))))
		(i32.store8 offset=14 (local.get $s) (i32.add (i32.shr_u (local.get $i2) (i32.const 20)) (i32.const 48)))
	)

	(func (export "convert_real_to_string") (param $d f64) (param $s i32) (result i32)
		(local $scale-factor f64)
		(local $exponent i32)
		(local $n i32)
		(local $d-i i64)

		(local $exp-d i32)
		(local $p i32)
		(local $d-54-i i64)
		(local $p10 f64)
		(local $table-p i32)
		(local $d2 f64)
		(local $exponent-d10 i32)

		(local.set $d-i (i64.reinterpret_f64 (local.get $d)))

		(if
			(f64.lt (local.get $d) (f64.const 0))
			(then
				(local.set $d (f64.sub (f64.const 0) (local.get $d)))
				(i32.store8 (local.get $s) (i32.const 45)) ;; '-'
				(local.set $s (i32.add (local.get $s) (i32.const 1)))
			)
		)

		(if
			(f64.eq (local.get $d) (f64.const 0))
			(then
				(i32.store8 (local.get $s) (i32.const 48)) ;; '0'
				(return (i32.add (local.get $s) (i32.const 1)))
			)
		)

		(if
			(f64.lt (local.get $d) (f64.const 1e4))
			(then
				(if
					(f64.lt (local.get $d) (f64.const 1e0))
					(then
						(if
							(f64.lt (local.get $d) (f64.const 1e-4))
							(then
								(local.set $exp-d (i32.and (i32.wrap_i64 (i64.shr_u (local.get $d-i) (i64.const 52))) (i32.const 0x7ff)))
								(if
									(i32.eqz (local.get $exp-d))
									(then
										(local.set $d-54-i (i64.reinterpret_f64 (f64.mul (local.get $d) (f64.const 18014398509481984.0))))
										(local.set $exp-d (i32.sub (i32.const 1077) ;; 0x3ff+54
											(i32.and (i32.wrap_i64 (i64.shr_u (local.get $d-54-i) (i64.const 52))) (i32.const 0x7ff))))
									)
									(else
										(local.set $exp-d (i32.sub (i32.const 0x3ff) (local.get $exp-d)))
									)
								)

								(local.set $exponent (i32.wrap_i64
									(i64.shr_u (i64.mul (i64.extend_i32_u (local.get $exp-d)) (i64.const 2711437152599295)) (i64.const 53))))
								(local.set $p (i32.add (i32.const 14) (local.get $exponent)))
								(local.set $exponent (i32.sub (i32.const 0) (local.get $exponent)))

								(if
									(i32.ge_s (local.get $p) (i32.const 307))
									(then
										(local.set $d (f64.mul (local.get $d) (f64.const 1e30)))
										(local.set $p (i32.sub (local.get $p) (i32.const 30)))
									)
								)

								(if
									(i32.gt_s (local.get $p) (i32.const 0))
									(then
										(local.set $n (local.get $p))
										(local.set $p10 (f64.load (i32.add (i32.const 5352) ;; power10_table
											(i32.shl (i32.and (local.get $n) (i32.const 15)) (i32.const 3)))))
										(local.set $n (i32.shr_u (local.get $n) (i32.const 4)))
										(if
											(local.get $n)
											(then
												(local.set $table-p (i32.const 5480)) ;; power10_table2
												(if
													(i32.and (local.get $n) (i32.const 1))
													(then (local.set $p10 (f64.mul (local.get $p10) (f64.load (local.get $table-p)))))
												)
												(local.set $n (i32.shr_u (local.get $n) (i32.const 1)))
												(block
													(loop
														(br_if 1 (i32.eqz (local.get $n)))
														(local.set $table-p (i32.add (local.get $table-p) (i32.const 8)))
														(if
															(i32.and (local.get $n) (i32.const 1))
															(then (local.set $p10 (f64.mul (local.get $p10) (f64.load (local.get $table-p)))))
														)
														(local.set $n (i32.shr_u (local.get $n) (i32.const 1)))
														(br 0)
													)
												)
											)
										)
										(local.set $scale-factor (local.get $p10))
									)
									(else ;; p <= 0
										(if
											(i32.lt_s (local.get $p) (i32.const 0))
											(then
												(local.set $d (f64.div (local.get $d) (f64.load (i32.add (i32.const 5352) ;; power10_table
													(i32.shl (i32.sub (i32.const 0) (local.get $p)) (i32.const 3))))))
											)
										)
										(local.set $scale-factor (f64.const 1.0))
									)
								)
							)
							(else ;; d >= 1e-4
								(if
									(f64.lt (local.get $d) (f64.const 1e-2))
									(then
										(if
											(f64.lt (local.get $d) (f64.const 1e-3))
											(then
												(local.set $exponent (i32.const -4))
												(local.set $scale-factor (f64.const 1e18))
											)
											(else ;; d >= 1e-3
												(local.set $exponent (i32.const -3))
												(local.set $scale-factor (f64.const 1e17))
											)
										)
									)
									(else ;; d >= 1e-2
										(if
											(f64.lt (local.get $d) (f64.const 1e-1))
											(then
												(local.set $exponent (i32.const -2))
												(local.set $scale-factor (f64.const 1e16))
											)
											(else ;; d >= 1e-1
												(local.set $exponent (i32.const -1))
												(local.set $scale-factor (f64.const 1e15))
											)
										)
									)
								)
							)
						)
					)
					(else ;; d >= 1e0
						(if
							(f64.lt (local.get $d) (f64.const 1e2))
							(then
								(if
									(f64.lt (local.get $d) (f64.const 1e1))
									(then
										(local.set $exponent (i32.const 0))
										(local.set $scale-factor (f64.const 1e14))
									)
									(else ;; d >= 1e1
										(local.set $exponent (i32.const 1))
										(local.set $scale-factor (f64.const 1e13))
									)
								)
							)
							(else ;; d >= 1e2
								(if
									(f64.lt (local.get $d) (f64.const 1e3))
									(then
										(local.set $exponent (i32.const 2))
										(local.set $scale-factor (f64.const 1e12))
									)
									(else ;; d >= 1e3
										(local.set $exponent (i32.const 3))
										(local.set $scale-factor (f64.const 1e11))
									)
								)
							)
						)
					)
				)
			)
			(else ;; d >= 1e4
				(if
					(f64.lt (local.get $d) (f64.const 1e8))
					(then
						(if
							(f64.lt (local.get $d) (f64.const 1e6))
							(then
								(if
									(f64.lt (local.get $d) (f64.const 1e5))
									(then
										(local.set $exponent (i32.const 4))
										(local.set $scale-factor (f64.const 1e10))
									)
									(else ;; d >= 1e5
										(local.set $exponent (i32.const 5))
										(local.set $scale-factor (f64.const 1e9))
									)
								)
							)
							(else ;; d >= 1e6
								(if
									(f64.lt (local.get $d) (f64.const 1e7))
									(then
										(local.set $exponent (i32.const 6))
										(local.set $scale-factor (f64.const 1e8))
									)
									(else ;; d >= 1e7
										(local.set $exponent (i32.const 7))
										(local.set $scale-factor (f64.const 1e7))
									)
								)
							)
						)
					)
					(else ;; d >= 1e8
						(if
							(f64.lt (local.get $d) (f64.const 1e12))
							(then
								(if
									(f64.lt (local.get $d) (f64.const 1e10))
									(then
										(if
											(f64.lt (local.get $d) (f64.const 1e9))
											(then
												(local.set $exponent (i32.const 8))
												(local.set $scale-factor (f64.const 1e6))
											)
											(else ;; d >= 1e9
												(local.set $exponent (i32.const 9))
												(local.set $scale-factor (f64.const 1e5))
											)
										)
									)
									(else ;; d >= 1e10
										(if
											(f64.lt (local.get $d) (f64.const 1e11))
											(then
												(local.set $exponent (i32.const 10))
												(local.set $scale-factor (f64.const 1e4))
											)
											(else ;; d >= 1e11
												(local.set $exponent (i32.const 11))
												(local.set $scale-factor (f64.const 1e3))
											)
										)
									)
								)
							)
							(else ;; d >= 1e12
								(local.set $exp-d (i32.and (i32.wrap_i64 (i64.shr_u (local.get $d-i) (i64.const 52))) (i32.const 0x7ff)))

								(if
									(i32.eq (local.get $exp-d) (i32.const 0x7ff))
									(then
										(if
											(i32.and (i32.wrap_i64 (i64.shr_u (i64.reinterpret_f64 (local.get $d)) (i64.const 53))) (i32.const 1)) ;; signbit
											(then
												(i32.store8 (local.get $s) (i32.const 45)) ;; '-'
												(local.set $s (i32.add (local.get $s) (i32.const 1)))
											)
										)
										(if
											(i64.eqz (i64.and (local.get $d-i) (i64.const 0xfffffffffffff)))
											(then
												(i32.store8 offset=0 (local.get $s) (i32.const 105)) ;; 'i'
												(i32.store8 offset=1 (local.get $s) (i32.const 110)) ;; 'n'
												(i32.store8 offset=2 (local.get $s) (i32.const 102)) ;; 'f'
											)
											(else
												(i32.store8 offset=0 (local.get $s) (i32.const 110)) ;; 'n'
												(i32.store8 offset=1 (local.get $s) (i32.const  97)) ;; 'a'
												(i32.store8 offset=2 (local.get $s) (i32.const 110)) ;; 'n'
											)
										)
										(return (i32.add (local.get $s) (i32.const 3)))
									)
								)

								(local.set $exp-d (i32.sub (local.get $exp-d) (i32.const 0x3ff)))

								(local.set $exponent (i32.wrap_i64
									(i64.shr_u (i64.mul (i64.extend_i32_u (local.get $exp-d)) (i64.const 2711437152599295)) (i64.const 53))))
								(local.set $p (i32.sub (i32.const 14) (local.get $exponent)))

								(if
									(i32.lt_s (local.get $p) (i32.const 0))
									(then
										(local.set $n (i32.sub (i32.const 0) (local.get $p)))
										(local.set $p10 (f64.load offset=5352 ;; power10_table
											(i32.shl (i32.and (local.get $n) (i32.const 15)) (i32.const 3))))
										(local.set $n (i32.shr_u (local.get $n) (i32.const 4)))
										(if
											(local.get $n)
											(then
												(local.set $table-p (i32.const 5480)) ;; power10_table2
												(if
													(i32.and (local.get $n) (i32.const 1))
													(then (local.set $p10 (f64.mul (local.get $p10) (f64.load (local.get $table-p)))))
												)
												(local.set $n (i32.shr_u (local.get $n) (i32.const 1)))
												(block
													(loop
														(br_if 1 (i32.eqz (local.get $n)))
														(local.set $table-p (i32.add (local.get $table-p) (i32.const 8)))
														(if
															(i32.and (local.get $n) (i32.const 1))
															(then (local.set $p10 (f64.mul (local.get $p10) (f64.load (local.get $table-p)))))
														)
														(local.set $n (i32.shr_u (local.get $n) (i32.const 1)))
														(br 0)
													)
												)
											)
										)

										(local.set $d2 (f64.div (local.get $d) (local.get $p10)))
										(if
											(f64.ge (local.get $d2) (f64.sub (f64.const 1.0e15) (f64.const 0.5)))
											(then
												(local.set $exponent (i32.add (local.get $exponent) (i32.const 1)))
												(local.set $d (f64.div (local.get $d) (f64.mul (local.get $p10) (f64.const 10))))
											)
											(else
												(local.set $d (local.get $d2))
											)
										)

										(local.set $scale-factor (f64.const 1))
									)
									(else ;; p >= 0
										(if
											(i32.gt_s (local.get $p) (i32.const 0))
											(then
												(local.set $scale-factor (f64.load (i32.add (i32.const 5352) ;; power10_table
													(i32.shl (local.get $p) (i32.const 3)))))
												(if
													(f64.ge (f64.mul (local.get $d) (local.get $scale-factor)) (f64.const 1.0e15))
													(then
														(local.set $scale-factor (f64.load (i32.add (i32.const 5344) ;; power10_table-1
															(i32.shl (local.get $p) (i32.const 3)))))
														(local.set $exponent (i32.add (local.get $exponent) (i32.const 1)))
													)
												)
											)
											(else
												(local.set $scale-factor (f64.const 1.0))
												(if
													(f64.ge (local.get $d) (f64.const 1.0e15))
													(then
														(local.set $scale-factor (f64.const 0.1))
														(local.set $exponent (i32.add (local.get $exponent) (i32.const 1)))
													)
												)
											)
										)
									)
								)
							)
						)
					)
				)
			)
		)

		(local.set $d (f64.mul (local.get $d) (local.get $scale-factor)))
		(if
			(f64.lt (local.get $d) (f64.sub (f64.const 1e14) (f64.const 0.5)))
			(then
				(local.set $d (f64.mul (local.get $d) (f64.const 10.0)))
				(local.set $exponent (i32.sub (local.get $exponent) (i32.const 1)))
			)
			(else
				(if
					(f64.ge (local.get $d) (f64.sub (f64.const 1e15) (f64.const 0.5)))
					(then
						(local.set $d (f64.div (local.get $d) (f64.const 10.0)))
						(local.set $exponent (i32.add (local.get $exponent) (i32.const 1)))
					)
				)
			)
		)

		(call $d-to-a (local.get $d) (local.get $s))
		(local.set $s (i32.add (local.get $s) (i32.const 15)))

		(if
			(i32.or
				(i32.gt_s (local.get $exponent) (i32.const 14))
				(i32.lt_s (local.get $exponent) (i32.const -4)))
			(then
				(local.set $n (i32.const -1))
				(block
					(loop
						(br_if 1 (i32.lt_s (local.get $n) (i32.const -14)))
						(i32.store8 offset=1
							(i32.add (local.get $s) (local.get $n))
							(i32.load8_u (i32.add (local.get $s) (local.get $n))))
						(local.set $n (i32.sub (local.get $n) (i32.const 1)))
						(br 0)
					)
				)
				(i32.store8 (i32.sub (local.get $s) (i32.const 14)) (i32.const 46)) ;; '.'

				(local.set $s (i32.add (local.get $s) (i32.const 1)))
				(block
					(loop
						(br_if 1 (i32.ne (i32.load8_u (i32.sub (local.get $s) (i32.const 1))) (i32.const 48))) ;; '0'
						(local.set $s (i32.sub (local.get $s) (i32.const 1)))
						(br 0)
					)
				)

				(if
					(i32.eq (i32.load8_u (i32.sub (local.get $s) (i32.const 1))) (i32.const 46)) ;; '.'
					(then (local.set $s (i32.sub (local.get $s) (i32.const 1))))
				)

				(i32.store8 (local.get $s) (i32.const 101)) ;; 'e'
				(local.set $s (i32.add (local.get $s) (i32.const 1)))

				(if
					(i32.ge_s (local.get $exponent) (i32.const 0))
					(then
						(i32.store8 (local.get $s) (i32.const 43)) ;; '+'
					)
					(else
						(i32.store8 (local.get $s) (i32.const 45)) ;; '-'
						(local.set $exponent (i32.sub (i32.const 0) (local.get $exponent)))
					)
				)
				(local.set $s (i32.add (local.get $s) (i32.const 1)))

				(if
					(i32.ge_s (local.get $exponent) (i32.const 100))
					(then
						(local.set $exponent-d10 (i32.div_s (local.get $exponent) (i32.const 100)))
						(i32.store8 (local.get $s) (i32.add (i32.const 48) (local.get $exponent-d10))) ;; '0'+x
						(local.set $s (i32.add (local.get $s) (i32.const 1)))
						(local.set $exponent (i32.sub (local.get $exponent) (i32.mul (i32.const 100) (local.get $exponent-d10))))
					)
				)

				(local.set $exponent-d10 (i32.div_s (local.get $exponent) (i32.const 10)))
				(i32.store8 (local.get $s) (i32.add (i32.const 48) ;; '0'
					(local.get $exponent-d10)))
				(i32.store8 offset=1 (local.get $s) (i32.add (i32.const 48) ;; '0'
					(i32.sub (local.get $exponent) (i32.mul (local.get $exponent-d10) (i32.const 10)))))
				(local.set $s (i32.add (local.get $s) (i32.const 2)))
			)
			(else ;; -4 <= exponent <= 14
				(local.set $n (i32.const -1))
				(if
					(i32.ge_s (local.get $exponent) (i32.const 0))
					(then
						;; TODO: optimize loop
						(local.set $exponent (i32.sub (local.get $exponent) (i32.const 14)))
						(block
							(loop
								(br_if 1 (i32.lt_s (local.get $n) (local.get $exponent)))
								(i32.store8 offset=1
									(i32.add (local.get $s) (local.get $n))
									(i32.load8_u (i32.add (local.get $s) (local.get $n))))
								(local.set $n (i32.sub (local.get $n) (i32.const 1)))
								(br 0)
							)
						)
						(i32.store8 (i32.add (local.get $s) (local.get $exponent)) (i32.const 46)) ;; '.'
						(local.set $exponent (i32.add (local.get $exponent) (i32.const 14)))
						(local.set $s (i32.add (local.get $s) (i32.const 1)))
					)
					(else ;; exponent < 0
						(block
							(loop
								(br_if 1 (i32.lt_s (local.get $n) (i32.const -15)))
								(i32.store8 offset=1
									(i32.add (local.get $s) (i32.sub (local.get $n) (local.get $exponent)))
									(i32.load8_u (i32.add (local.get $s) (local.get $n))))
								(local.set $n (i32.sub (local.get $n) (i32.const 1)))
								(br 0)
							)
						)
						(i32.store8 (i32.sub (local.get $s) (i32.const 14)) (i32.const 46)) ;; '.'
						(i32.store8 (i32.sub (local.get $s) (i32.const 15)) (i32.const 48)) ;; '0'
						(local.set $n (local.get $exponent))
						(block
							(loop
								(br_if 1 (i32.ge_s (local.get $n) (i32.const -1)))
								(i32.store8
									(i32.sub (i32.add (i32.sub (local.get $s) (i32.const -13)) (local.get $n)) (local.get $exponent))
									(i32.const 48)) ;; '0'
								(local.set $n (i32.add (local.get $n) (i32.const 1)))
								(br 0)
							)
						)
						(local.set $s (i32.sub (i32.add (local.get $s) (i32.const 1)) (local.get $exponent)))
					)
				)

				(block
					(loop
						(br_if 1 (i32.ne (i32.load8_u (i32.sub (local.get $s) (i32.const 1))) (i32.const 48))) ;; '0'
						(local.set $s (i32.sub (local.get $s) (i32.const 1)))
						(br 0)
					)
				)

				(if
					(i32.eq (i32.load8_u (i32.sub (local.get $s) (i32.const 1))) (i32.const 46)) ;; '.'
					(then (local.set $s (i32.sub (local.get $s) (i32.const 1))))
				)
			)
		)

		(return (local.get $s))
	)

	(func (export "setup_gc") (param i32 i32 i32 i32)
		(global.set $start-heap (local.get 0))
		(global.set $half-heap (i32.add (local.get 0) (local.get 1)))
		(global.set $end-heap (i32.add (local.get 0) (i32.add (local.get 1) (local.get 1))))
		(global.set $stack (i32.sub (local.get 2) (i32.const 8)))
		(global.set $caf-list (local.get 3))
	)
	(func (export "set_js_ref_constructor") (param i32)
		(global.set $js-ref-constructor (local.get 0))
	)

	(func $get-unused-semispace (export "get_unused_semispace") (result i32)
		(select
			(global.get $half-heap)
			(global.get $start-heap)
			(global.get $in-first-semispace)
		)
	)

	(func (export "decode_prelinked_bytecode") (param $ptr i32)
		(local $section-len i32)

		(local $len i32)
		(local $i i32)
		(local $byte i64)

		(local $val i64)
		(local $shift i64)
		(local $neg i32)

		(local.set $len (local.get $ptr))
		(local.set $i (i32.const 0))

		(loop $sections
			(local.set $section-len (i32.load (local.get $ptr)))
			(if (i32.eqz (local.get $section-len)) (return))
			(local.set $ptr (i32.add (local.get $ptr) (i32.const 4)))

			(loop $decode
				(local.set $byte (i64.load8_u (local.get $ptr)))
				(local.set $val (i64.and (local.get $byte) (i64.const 0x3f)))
				(local.set $neg (i32.wrap_i64 (i64.and (local.get $byte) (i64.const 0x40))))
				(local.set $shift (i64.const -1))

				(block $end-value
					(loop $lp
						(local.set $ptr (i32.add (local.get $ptr) (i32.const 1)))
						(br_if $end-value (i32.eqz (i32.wrap_i64 (i64.and (local.get $byte) (i64.const 0x80)))))
						(local.set $shift (i64.add (local.get $shift) (i64.const 7)))
						(local.set $byte (i64.load8_u (local.get $ptr)))
						(local.set $val (i64.or (local.get $val)
							(i64.shl (i64.and (local.get $byte) (i64.const 0x7f)) (local.get $shift))))
						(br $lp)
					)
				)
				(if
					(local.get $neg)
					(then (local.set $val (i64.sub (i64.const 0) (local.get $val))))
				)
				(i64.store (local.get $i) (local.get $val))

				(local.set $i (i32.add (local.get $i) (i32.const 8)))
				(br_if $decode (local.tee $section-len (i32.sub (local.get $section-len) (i32.const 1))))
			)

			(local.set $ptr (i32.and (i32.add (local.get $ptr) (i32.const 7)) (i32.const 0xfffffff8)))
			(br $sections)
		)
	)

	(func (export "gc") (param $asp i32)
		(local $old i32)
		(local $new i32)
		(local $n i32)
		(local $d i32)
		(local $a-arity i32)
		(local $arity i32)
		(local $size i32)

		(call $gc-start)

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
				(local.set $n (i32.load (i32.sub (local.get $n) (i32.const 8))))
				(br_if $end-copy-cafs (i32.eqz (local.get $n)))
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

				(if
					(i32.gt_s (local.get $n) (i32.const 0))
					(then
						(i32.store (local.get $asp) (local.get $n))
						(local.set $new (call $update-ref (local.get $asp) (local.get $new)))
						(call $update-host-reference (local.get $d) (i32.load (local.get $asp)))
					)
				)

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
								(i32.eq (local.get $d) (i32.const 202)) ;; INT+2
								(i32.eq (local.get $d) (i32.const 162)) ;; REAL+2
								;; BOOL and CHAR are always static so cannot occur here
							)
							(then
								(local.set $n (i32.add (local.get $n) (i32.const 16)))
								(br $update-refs)
							)
						)
						(if
							;; _STRING_+2
							(i32.eq (local.get $d) (i32.const 42))
							(then
								(local.set $size (i32.load offset=8 (local.get $n))) ;; size
								(local.set $size (i32.shr_u (i32.add (local.get $size) (i32.const 7)) (i32.const 3)))
								(local.set $n (i32.add (local.get $n) (i32.add (i32.shl (local.get $size) (i32.const 3)) (i32.const 16))))
								(br $update-refs)
							)
						)
						(if
							;; _ARRAY_+2
							(i32.eq (local.get $d) (i32.const 2))
							(then
								(local.set $size (i32.load offset=8 (local.get $n))) ;; size
								(local.set $d (i32.load offset=16 (local.get $n))) ;; child descriptor
								(if
									(i32.or ;; unboxed INT/REAL
										(i32.eq (local.get $d) (i32.const 162))
										(i32.eq (local.get $d) (i32.const 202))
									)
									(then
										(local.set $n (i32.add (local.get $n) (i32.add (i32.const 24)
											(i32.shl (local.get $size) (i32.const 3)))))
										(br $update-refs)
									)
								)
								(if ;; unboxed BOOL
									(i32.eq (local.get $d) (i32.const 82))
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
		(call $set-hp (local.get $new))
		(call $set-hp-free
			(i32.shr_u
				(i32.sub
					(select (global.get $half-heap) (global.get $end-heap) (global.get $in-first-semispace))
					(local.get $new)
				)
				(i32.const 3)
			))

		(call $gc-end)
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

		(if (i32.and (local.get $d) (i32.const 2))
			;; hnf
			(then
				(if
					(i32.eq (local.get $d) (i32.const 202)) ;; INT+2
					(then
						(if
							(i64.lt_u (i64.load offset=8 (local.get $n)) (i64.const 33))
							(then
								(local.set $d (i32.add (i32.const 240) (i32.shl (i32.load offset=8 (local.get $n)) (i32.const 4)))) ;; small integer
								(i64.store (local.get $ref) (i64.extend_i32_u (local.get $d)))
								(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $d) (i32.const 1))))
								(return (local.get $hp))
							)
							(else
								(i64.store (local.get $hp) (i64.extend_i32_u (local.get $d)))
								(i64.store offset=8 (local.get $hp) (i64.load offset=8 (local.get $n)))
								(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 1))))
								(return (i32.add (local.get $hp) (i32.const 16)))
							)
						)
					)
				)
				(if
					(i32.eq (local.get $d) (i32.const 82)) ;; BOOL+2
					(then
						(local.set $d (select (i32.const 5296) (i32.const 5280) (i32.load offset=8 (local.get $n)))) ;; static FALSE/TRUE
						(i64.store (local.get $ref) (i64.extend_i32_u (local.get $d)))
						(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $d) (i32.const 1))))
						(return (local.get $hp))
					)
				)
				(if
					(i32.eq (local.get $d) (i32.const 122)) ;; CHAR+2
					(then
						(local.set $d (i32.add (i32.const 1168) (i32.shl
							(i32.and (i32.load8_u offset=8 (local.get $n)) (i32.const 0xff)) (i32.const 4)))) ;; static character
						(i64.store (local.get $ref) (i64.extend_i32_u (local.get $d)))
						(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $d) (i32.const 1))))
						(return (local.get $hp))
					)
				)
				(if
					(i32.eq (local.get $d) (i32.const 162)) ;; REAL+2
					(then
						(i64.store (local.get $hp) (i64.extend_i32_u (local.get $d)))
						(i64.store offset=8 (local.get $hp) (i64.load offset=8 (local.get $n)))
						(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 1))))
						(return (i32.add (local.get $hp) (i32.const 16)))
					)
				)
				(if
					;; _STRING_+2
					(i32.eq (local.get $d) (i32.const 42))
					(then
						(i64.store (local.get $hp) (i64.extend_i32_u (local.get $d)))
						(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 1))))

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
					(i32.eq (local.get $d) (i32.const 2))
					(then
						(i64.store (local.get $hp) (i64.extend_i32_u (local.get $d)))
						(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 1))))

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
										(i32.eq (i32.const 162) (local.get $d))
										(i32.eq (i32.const 202) (local.get $d))
									)
								)
							)
							(then
								(if
									(i32.eq (local.get $d) (i32.const 82))
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

				(if
					(i32.eq (local.get $d) (global.get $js-ref-constructor))
					(then
						(i64.store (local.get $hp) (i64.extend_i32_u (local.get $d)))
						(local.set $d (i32.load offset=8 (local.get $n)))
						(call $js-ref-found (local.get $d))
						(i64.store offset=8 (local.get $hp) (i64.extend_i32_u (local.get $d)))
						(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 1))))
						(return (i32.add (local.get $hp) (i32.const 16)))
					)
				)

				(local.set $arity (i32.load16_s (i32.sub (local.get $d) (i32.const 2))))
				(if
					(i32.gt_s (local.get $arity) (i32.const 256))
					(then
						(local.set $arity (i32.sub (local.get $arity) (i32.const 256)))
					)
				)

				(if
					(i32.eqz (local.get $arity))
					(then
						(local.set $d (i32.sub (local.get $d) (i32.const 10)))
						(i64.store (local.get $ref) (i64.extend_i32_u (local.get $d)))
						(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $d) (i32.const 1))))
						(return (local.get $hp))
					)
				)

				(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 1))))
				(i64.store (local.get $hp) (i64.extend_i32_u (local.get $d)))

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
		(i64.store (local.get $n) (i64.extend_i32_u (i32.add (local.get $hp) (i32.const 1))))
		(i64.store (local.get $hp) (i64.extend_i32_u (local.get $d)))

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

				(i32.store16 (local.get $a-size-stack) (i32.sub (i32.load16_s (local.get $a-size-stack)) (i32.const 1)))

				(local.set $desc (i32.load (i32.add (local.get $s) (local.get $i))))
				(i64.store (i32.add (local.get $s) (local.get $i)) (i64.extend_i32_u (local.get $hp)))

				;; "loop"
				;;(call $debug (local.get $desc) (local.get $i) (local.get $len) (local.get $hp))

				(if ;; redirection or predefined constructor
					(i32.lt_s (local.get $desc) (i32.const 0))
					(then
						(block $no-predefined-constructor
							(block $predefined-constructor
								;; predefined constructors: see ABC.Interpreter for the $desc values;
								;; the constructors written to the heap are from the interpreter generator.
								;; TODO: use small_integers, static_characters, and static_booleans here
								(if ;; BOOL
									(i32.eq (local.get $desc) (i32.const -22))
									(then
										(i64.store (local.get $hp) (i64.const 82))
										(i64.store offset=8 (local.get $hp) (i64.load offset=8 (i32.add (local.get $s) (local.get $i))))
										(br $predefined-constructor)
									)
								)
								(if ;; CHAR
									(i32.eq (local.get $desc) (i32.const -30))
									(then
										(i64.store (local.get $hp) (i64.const 122))
										(i64.store offset=8 (local.get $hp) (i64.load offset=8 (i32.add (local.get $s) (local.get $i))))
										(br $predefined-constructor)
									)
								)
								(if ;; REAL
									(i32.eq (local.get $desc) (i32.const -38))
									(then
										(i64.store (local.get $hp) (i64.const 162))
										(i64.store offset=8 (local.get $hp) (i64.load offset=8 (i32.add (local.get $s) (local.get $i))))
										(br $predefined-constructor)
									)
								)
								(if ;; INT
									(i32.eq (local.get $desc) (i32.const -46))
									(then
										(i64.store (local.get $hp) (i64.const 202))
										(i64.store offset=8 (local.get $hp) (i64.load offset=8 (i32.add (local.get $s) (local.get $i))))
										(br $predefined-constructor)
									)
								)
								(br_if ;; e__system__nind
									$loop (i32.eq (local.get $desc) (i32.const -54)))

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

								(i64.store (local.get $hp) (i64.const 42))
								(local.set $a (i32.load offset=8 (i32.add (local.get $s) (local.get $i))))
								(i64.store offset=8 (local.get $hp) (i64.extend_i32_u (local.get $a)))
								(local.set $a (i32.shr_u (i32.add (local.get $a) (i32.const 7)) (i32.const 3)))
								(call $copy
									(i32.add (local.get $hp) (i32.const 16))
									(i32.add (local.get $s) (i32.add (local.get $i) (i32.const 16)))
									(local.get $a))

								(local.set $hp (i32.add (local.get $hp) (i32.add (i32.const 16) (i32.shl (local.get $a) (i32.const 3)))))
								(local.set $i (i32.add (local.get $i) (i32.add (i32.const 8) (i32.shl (local.get $a) (i32.const 3)))))
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

								(i64.store (local.get $hp) (i64.const 2))
								(i64.store offset=8 (local.get $hp) (i64.extend_i32_u (local.get $a)))

								(if ;; INT elements
									(i32.eq (local.get $desc) (i32.const -46))
									(then
										(i64.store offset=16 (local.get $hp) (i64.const 202))
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
										(i64.store offset=16 (local.get $hp) (i64.const 162))
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
										(i64.store offset=16 (local.get $hp) (i64.const 82))
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
						;; thunk
						(local.set $arity (i32.load (i32.sub (local.get $desc) (i32.const 4))))
						(local.set $a-arity (local.get $arity))

						(if
							(i32.lt_s (local.get $arity) (i32.const 0))
							(then
								(local.set $arity (i32.const 1))
								(local.set $a-arity (i32.const 1))
							)
							(else
								(if
									(i32.gt_u (local.get $arity) (i32.const 256))
									(then
										(local.set $b-arity (i32.shr_u (local.get $arity) (i32.const 8)))
										(local.set $arity (i32.and (local.get $arity) (i32.const 0xff)))
										(local.set $a-arity (i32.sub (local.get $arity) (local.get $b-arity)))
									)
								)
							)
						)

						;; "arity"
						;;(call $debug (i32.const 3) (local.get $arity) (local.get $a-arity) (local.get $b-arity))

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

	(func (export "copy_to_string")
		(param $node i32)
		(param $code-offset i32)
		(result i32)

		(local $begin-s i32)
		(local $s i32)
		(local $desc i32)
		(local $arity i32)
		(local $a-arity i32)
		(local $b-arity i32)
		(local $args i32)
		(local $begin-stack i32)
		(local $stack i32)
		(local $i i32)

		(local.set $begin-s (call $get-unused-semispace))
		(local.set $s (i32.add (local.get $begin-s) (i32.const 8)))
		(local.set $begin-stack (select (global.get $end-heap) (global.get $half-heap) (global.get $in-first-semispace)))
		(local.set $stack (i32.sub (local.get $begin-stack) (i32.const 4)))

		(loop $loop
			(block $next-node
				(loop $continue
					(local.set $desc (i32.load (local.get $node)))

					(if ;; already copied; add a redirection
						(i32.and (local.get $desc) (i32.const 1))
						(then
							(i64.store (local.get $s) (i64.extend_i32_s (i32.sub (local.get $desc) (local.get $s))))
							(local.set $s (i32.add (local.get $s) (i32.const 8)))
							(br $next-node)
						)
					)

					;; new node
					(i64.store (local.get $node) (i64.extend_i32_u (i32.add (local.get $s) (i32.const 1))))
					(i64.store (local.get $s) (i64.extend_i32_u (i32.sub (local.get $desc) (local.get $code-offset))))
					(local.set $s (i32.add (local.get $s) (i32.const 8)))

					;;(call $debug (i32.const 1) (local.get $desc) (i32.const 0) (i32.const 0))

					(if ;; hnf
						(i32.and (local.get $desc) (i32.const 2))
						(then
							(local.set $arity (i32.load16_u (i32.sub (local.get $desc) (i32.const 2))))
							;;(call $debug (i32.const 2) (local.get $arity) (i32.const 0) (i32.const 0))

							(if
								(i32.eqz (local.get $arity))
								(then
									(block $predefined-constructor
										(if ;; BOOL
											(i32.eq (local.get $desc) (i32.const 82))
											(then
												(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -22))
												(i64.store (local.get $s) (i64.load offset=8 (local.get $node)))
												(br $predefined-constructor)
											)
										)
										(if ;; CHAR
											(i32.eq (local.get $desc) (i32.const 122))
											(then
												(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -30))
												(i64.store (local.get $s) (i64.load offset=8 (local.get $node)))
												(br $predefined-constructor)
											)
										)
										(if ;; REAL
											(i32.eq (local.get $desc) (i32.const 162))
											(then
												(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -38))
												(i64.store (local.get $s) (i64.load offset=8 (local.get $node)))
												(br $predefined-constructor)
											)
										)
										(if ;; INT
											(i32.eq (local.get $desc) (i32.const 202))
											(then
												(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -46))
												(i64.store (local.get $s) (i64.load offset=8 (local.get $node)))
												(br $predefined-constructor)
											)
										)
										(if ;; _STRING_
											(i32.eq (local.get $desc) (i32.const 42))
											(then
												(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -14))
												(local.set $arity (i32.load offset=8 (local.get $node))) ;; length
												(i64.store (local.get $s) (i64.extend_i32_u (local.get $arity)))
												(local.set $s (i32.add (local.get $s) (i32.const 8)))
												(local.set $arity (i32.shr_u (i32.add (local.get $arity) (i32.const 7)) (i32.const 3)))
												(call $copy (local.get $s) (i32.add (local.get $node) (i32.const 16)) (local.get $arity))
												(local.set $s (i32.add (local.get $s) (i32.shl (local.get $arity) (i32.const 3))))
												(br $next-node)
											)
										)
										(if ;; _ARRAY_
											(i32.eq (local.get $desc) (i32.const 2))
											(then
												(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -6))

												(local.set $args (i32.load offset=8 (local.get $node))) ;; length
												(local.set $desc (i32.load offset=16 (local.get $node)))
												(local.set $node (i32.add (local.get $node) (i32.const 24)))

												(i64.store (local.get $s) (i64.extend_i32_u (local.get $args)))
												(i64.store offset=8 (local.get $s) (i64.extend_i32_u (local.get $desc)))
												(local.set $s (i32.add (local.get $s) (i32.const 16)))

												(if ;; unboxed array
													(i32.eqz (local.get $desc))
													(then
														(local.set $node (i32.add (local.get $node) (i32.shl (local.get $args) (i32.const 3))))
														(loop $lp
															(local.set $node (i32.sub (local.get $node) (i32.const 8)))
															(i32.store (local.get $stack) (i32.load (local.get $node)))
															(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
															(local.set $args (i32.sub (local.get $args) (i32.const 1)))
															(br_if $lp (local.get $args))
														)
														(br $next-node)
													)
												)
												(if ;; {#Int}
													(i32.eq (local.get $desc) (i32.const 202))
													(then
														(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -46))
														(call $copy (local.get $s) (local.get $node) (local.get $args))
														(local.set $s (i32.add (local.get $s) (i32.shl (local.get $args) (i32.const 3))))
														(br $next-node)
													)
												)
												(if ;; {#Real}
													(i32.eq (local.get $desc) (i32.const 162))
													(then
														(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -38))
														(call $copy (local.get $s) (local.get $node) (local.get $args))
														(local.set $s (i32.add (local.get $s) (i32.shl (local.get $args) (i32.const 3))))
														(br $next-node)
													)
												)
												(if ;; {#Bool}
													(i32.eq (local.get $desc) (i32.const 82))
													(then
														(i64.store (i32.sub (local.get $s) (i32.const 8)) (i64.const -22))
														(local.set $args (i32.shr_u (i32.add (local.get $args) (i32.const 7)) (i32.const 3)))
														(call $copy (local.get $s) (local.get $node) (local.get $args))
														(local.set $s (i32.add (local.get $s) (i32.shl (local.get $args) (i32.const 3))))
														(br $next-node)
													)
												)

												;; unboxed record array
												(local.set $a-arity (i32.load16_u (local.get $desc)))
												(local.set $arity (i32.sub (i32.load16_u (i32.sub (local.get $desc) (i32.const 2))) (i32.const 256)))

												(i64.store
													(i32.sub (local.get $s) (i32.const 8))
													(i64.extend_i32_u (i32.sub (local.get $desc) (local.get $code-offset))))

												(if
													(i32.eqz (local.get $a-arity))
													(then
														(local.set $args (i32.mul (local.get $args) (local.get $arity)))
														(call $copy (local.get $s) (local.get $node) (local.get $args))
														(local.set $s (i32.add (local.get $s) (i32.shl (local.get $args) (i32.const 3))))
														(br $next-node)
													)
												)
												(local.set $b-arity (i32.sub (local.get $arity) (local.get $a-arity)))
												(if
													(i32.eqz (local.get $b-arity))
													(then
														(local.set $args (i32.mul (local.get $args) (local.get $arity)))
														(local.set $node (i32.add (local.get $node) (i32.shl (local.get $args) (i32.const 3))))
														(loop $lp
															(br_if $next-node (i32.eqz (local.get $args)))
															(local.set $node (i32.sub (local.get $node) (i32.const 8)))
															(i32.store (local.get $stack) (i32.load (local.get $node)))
															(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
															(local.set $args (i32.sub (local.get $args) (i32.const 1)))
															(br $lp)
														)
													)
												)
												(loop $lp
													(br_if $next-node (i32.eqz (local.get $args)))
													;; pointers
													(local.set $i (local.get $a-arity))
													(local.set $node (i32.add (local.get $node) (i32.shl (local.get $a-arity) (i32.const 3))))
													(loop
														(local.set $node (i32.sub (local.get $node) (i32.const 8)))
														(i32.store (local.get $stack) (i32.load (local.get $node)))
														(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
														(local.set $i (i32.sub (local.get $i) (i32.const 1)))
														(br_if 0 (local.get $i))
													)
													(local.set $node (i32.add (local.get $node) (i32.shl (local.get $a-arity) (i32.const 3))))
													;; unboxed arguments
													(call $copy (local.get $s) (local.get $node) (local.get $b-arity))
													(local.set $i (i32.shl (local.get $b-arity) (i32.const 3)))
													(local.set $s (i32.add (local.get $s) (local.get $i)))
													(local.set $node (i32.add (local.get $node) (local.get $i)))
													;; iterate
													(local.set $args (i32.sub (local.get $args) (i32.const 1)))
													(br $lp)
												)
											)
										)
										;; not a predefined constructor
										(br $next-node)
									)
									(local.set $s (i32.add (local.get $s) (i32.const 8)))
									(br $next-node)
								)
							)
							(if
								(i32.eq (local.get $arity) (i32.const 1))
								(then
									(local.set $node (i32.load offset=8 (local.get $node)))
									(br $continue)
								)
							)
							(if
								(i32.eq (local.get $arity) (i32.const 2))
								(then
									(i32.store (local.get $stack) (i32.load offset=16 (local.get $node)))
									(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
									(local.set $node (i32.load offset=8 (local.get $node)))
									(br $continue)
								)
							)
							(if ;; split hnf without unboxed arguments
								(i32.lt_s (local.get $arity) (i32.const 256))
								(then
									(local.set $arity (i32.sub (local.get $arity) (i32.const 2)))

									(local.set $args (i32.load offset=16 (local.get $node)))
									(local.set $args (i32.add (local.get $args) (i32.sub (i32.shl (local.get $arity) (i32.const 3)) (i32.const 16))))

									(i32.store (local.get $stack) (i32.load offset=16 (local.get $args)))
									(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
									(i32.store (local.get $stack) (i32.load offset=8 (local.get $args)))
									(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))

									(block $blk
										(loop $lp
											(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
											(br_if $blk (i32.eqz (local.get $arity)))
											(i32.store (local.get $stack) (i32.load (local.get $args)))
											(local.set $args (i32.sub (local.get $args) (i32.const 8)))
											(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
											(br $lp)
										)
									)

									(local.set $node (i32.load offset=8 (local.get $node)))
									(br $continue)
								)
							)

							;; hnf with unboxed arguments
							(local.set $a-arity (i32.load16_u (local.get $desc)))
							(local.set $arity (i32.sub (local.get $arity) (i32.const 256)))
							(if
								(i32.eq (local.get $arity) (i32.const 1))
								(then
									(if
										(i32.eqz (local.get $a-arity))
										(then
											(i64.store (local.get $s) (i64.load offset=8 (local.get $node)))
											(local.set $s (i32.add (local.get $s) (i32.const 8)))
											(br $next-node)
										)
									)
									(local.set $node (i32.load offset=8 (local.get $node)))
									(br $continue)
								)
							)
							(if
								(i32.eq (local.get $arity) (i32.const 2))
								(then
									(if
										(i32.eqz (local.get $a-arity))
										(then
											(i64.store (local.get $s) (i64.load offset=8 (local.get $node)))
											(i64.store offset=8 (local.get $s) (i64.load offset=16 (local.get $node)))
											(local.set $s (i32.add (local.get $s) (i32.const 16)))
											(br $next-node)
										)
									)
									(if
										(i32.eq (local.get $a-arity) (i32.const 1))
										(then
											(i64.store (local.get $s) (i64.load offset=16 (local.get $node)))
											(local.set $s (i32.add (local.get $s) (i32.const 8)))
										)
										(else
											(i32.store (local.get $stack) (i32.load offset=16 (local.get $node)))
											(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
										)
									)
									(local.set $node (i32.load offset=8 (local.get $node)))
									(br $continue)
								)
							)

							;; split hnf with unboxed arguments
							(local.set $args (i32.load offset=16 (local.get $node)))
							(if
								(i32.eqz (local.get $a-arity))
								(then ;; no pointers
									(i64.store (local.get $s) (i64.load offset=8 (local.get $node)))
									(local.set $s (i32.add (local.get $s) (i32.const 8)))
									(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
									(call $copy (local.get $s) (local.get $args) (local.get $arity))
									(local.set $s (i32.add (local.get $s) (i32.shl (local.get $arity) (i32.const 3))))
									(br $next-node)
								)
							)

							;; split hnf with unboxed argument and at least one pointer
							(local.set $b-arity (i32.sub (local.get $arity) (local.get $a-arity)))
							(if
								(i32.gt_s (local.get $b-arity) (i32.const 0))
								(then
									(call $copy
										(local.get $s)
										(i32.sub (i32.add (local.get $args) (i32.shl (local.get $a-arity) (i32.const 3))) (i32.const 8))
										(local.get $b-arity))
									(local.set $s (i32.add (local.get $s) (i32.shl (local.get $b-arity) (i32.const 3))))
								)
							)
							(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
							(if
								(i32.gt_s (local.get $a-arity) (i32.const 0))
								(then
									(local.set $args (i32.add (local.get $args) (i32.shl (local.get $a-arity) (i32.const 3))))
									(loop $lp
										(local.set $args (i32.sub (local.get $args) (i32.const 8)))
										(i32.store (local.get $stack) (i32.load (local.get $args)))
										(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
										(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
										(br_if $lp (local.get $a-arity))
									)
								)
							)
							(local.set $node (i32.load offset=8 (local.get $node)))
							(br $continue)
						)
					)

					;; thunk
					(local.set $arity (i32.load (i32.sub (local.get $desc) (i32.const 4))))

					(if
						(i32.le_s (local.get $arity) (i32.const 1))
						(then ;; arity 0, 1, or negative
							(br_if $next-node (i32.eqz (local.get $arity)))

							;; arity 1 or negative
							(if ;; skip indirection
								(i32.eq (local.get $arity) (i32.const -2))
								(then
									(local.set $s (i32.sub (local.get $s) (i32.const 8)))
								)
							)
							(local.set $node (i32.load offset=8 (local.get $node)))
							(br $continue)
						)
					)

					;; arity > 1 or with unboxed arguments
					(if
						(i32.lt_s (local.get $arity) (i32.const 256))
						(then ;; no unboxed arguments
							(local.set $args (i32.add (local.get $node) (i32.shl (local.get $arity) (i32.const 3))))
							(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
							(loop $lp
								(i32.store (local.get $stack) (i32.load (local.get $args)))
								(local.set $args (i32.sub (local.get $args) (i32.const 8)))
								(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
								(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
								(br_if $lp (local.get $arity))
							)
						)
						(else ;; unboxed arguments
							(local.set $b-arity (i32.shr_u (local.get $arity) (i32.const 8)))
							(local.set $a-arity (i32.sub (i32.and (local.get $arity) (i32.const 255)) (local.get $b-arity)))
							(call $copy
								(local.get $s)
								(i32.add (local.get $node) (i32.add (i32.const 1) (i32.shl (local.get $a-arity) (i32.const 3))))
								(local.get $b-arity))
							(local.set $s (i32.add (local.get $s) (i32.shl (local.get $b-arity) (i32.const 3))))

							(br_if $next-node (i32.eqz (local.get $a-arity)))

							(if
								(i32.gt_s (local.get $a-arity) (i32.const 1))
								(then
									(local.set $args (i32.add (local.get $node) (i32.shl (local.get $a-arity) (i32.const 3))))
									(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
									(loop $lp
										(i32.store (local.get $stack) (i32.load (local.get $args)))
										(local.set $args (i32.sub (local.get $args) (i32.const 8)))
										(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
										(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
										(br_if $lp (local.get $a-arity))
									)
								)
							)
						)
					)

					(local.set $node (i32.load offset=8 (local.get $node)))
					(br $continue)
				)
			)

			(local.set $stack (i32.add (local.get $stack) (i32.const 4)))
			(if
				(i32.eq (local.get $stack) (local.get $begin-stack))
				(then
					(i64.store (local.get $begin-s) (i64.extend_i32_u
						(i32.sub (i32.sub (local.get $s) (local.get $begin-s)) (i32.const 8))))
					(return (local.get $begin-s))
				)
			)
			(local.set $node (i32.load (local.get $stack)))

			(br $loop)
		)

		(i32.const 0)
	)

	(func (export "remove_forwarding_pointers_from_graph")
		(param $node i32)
		(param $code-offset i32)

		(local $desc i32)
		(local $arity i32)
		(local $a-arity i32)
		(local $b-arity i32)
		(local $args i32)
		(local $begin-stack i32)
		(local $stack i32)
		(local $i i32)

		(local.set $begin-stack (select (global.get $end-heap) (global.get $half-heap) (global.get $in-first-semispace)))
		(local.set $stack (i32.sub (local.get $begin-stack) (i32.const 4)))

		(loop $loop
			(block $next-node
				(loop $continue
					(local.set $desc (i32.load (local.get $node)))
					(br_if $next-node (i32.eqz (i32.and (local.get $desc) (i32.const 1))))

					(local.set $desc (i32.load (i32.sub (local.get $desc) (i32.const 1))))
					(block $desc-translation-done
						(if
							(i32.lt_s (local.get $desc) (i32.const 0))
							(then
								(if ;; INT
									(i32.eq (local.get $desc) (i32.const -46))
									(then (local.set $desc (i32.const 202)) (br $desc-translation-done)))
								(if ;; _STRING_
									(i32.eq (local.get $desc) (i32.const -14))
									(then (local.set $desc (i32.const 42)) (br $desc-translation-done)))
								(if ;; BOOL
									(i32.eq (local.get $desc) (i32.const -22))
									(then (local.set $desc (i32.const 82)) (br $desc-translation-done)))
								(if ;; REAL
									(i32.eq (local.get $desc) (i32.const -38))
									(then (local.set $desc (i32.const 162)) (br $desc-translation-done)))
								(if ;; CHAR
									(i32.eq (local.get $desc) (i32.const -30))
									(then (local.set $desc (i32.const 122)) (br $desc-translation-done)))
								(if ;; _ARRAY_
									(i32.eq (local.get $desc) (i32.const -6))
									(then (local.set $desc (i32.const 2)) (br $desc-translation-done)))
								(unreachable)
							)
							(else
								(local.set $desc (i32.add (local.get $desc) (local.get $code-offset)))
							)
						)
					)
					(i64.store (local.get $node) (i64.extend_i32_u (local.get $desc)))

					;;(call $debug (i32.const 1) (local.get $desc) (i32.const 0) (i32.const 0))

					(if ;; hnf
						(i32.and (local.get $desc) (i32.const 2))
						(then
							(local.set $arity (i32.load16_u (i32.sub (local.get $desc) (i32.const 2))))
							;;(call $debug (i32.const 2) (local.get $arity) (i32.const 0) (i32.const 0))

							(if
								(i32.eqz (local.get $arity))
								(then
									(br_if $next-node (i32.ne (local.get $desc) (i32.const 2))) ;; only _ARRAY_ can have pointer arguments

									(local.set $args (i32.load offset=8 (local.get $node))) ;; length
									(local.set $desc (i32.load offset=16 (local.get $node)))
									(local.set $node (i32.add (local.get $node) (i32.const 24)))

									(if ;; unboxed array
										(i32.eqz (local.get $desc))
										(then
											(local.set $node (i32.add (local.get $node) (i32.shl (local.get $args) (i32.const 3))))
											(loop $lp
												(local.set $node (i32.sub (local.get $node) (i32.const 8)))
												(i32.store (local.get $stack) (i32.load (local.get $node)))
												(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
												(local.set $args (i32.sub (local.get $args) (i32.const 1)))
												(br_if $lp (local.get $args))
											)
											(br $next-node)
										)
									)
									(br_if $next-node
										(i32.or
											(i32.eq (local.get $desc) (i32.const 202)) ;; INT
											(i32.or
												(i32.eq (local.get $desc) (i32.const 162)) ;; REAL
												(i32.eq (local.get $desc) (i32.const 82))))) ;; BOOL

									;; unboxed record array
									(local.set $a-arity (i32.load16_u (local.get $desc)))
									(br_if $next-node (i32.eqz (local.get $a-arity)))

									(local.set $arity (i32.sub (i32.load16_u (i32.sub (local.get $desc) (i32.const 2))) (i32.const 256)))
									(local.set $b-arity (i32.sub (local.get $arity) (local.get $a-arity)))
									(if
										(i32.eqz (local.get $b-arity))
										(then
											(local.set $args (i32.mul (local.get $args) (local.get $arity)))
											(loop $lp
												(br_if $next-node (i32.eqz (local.get $args)))
												(i32.store (local.get $stack) (i32.load (local.get $node)))
												(local.set $node (i32.add (local.get $node) (i32.const 8)))
												(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
												(local.set $args (i32.sub (local.get $args) (i32.const 1)))
												(br $lp)
											)
										)
									)
									(local.set $b-arity (i32.shl (local.get $b-arity) (i32.const 3)))
									(loop $lp
										(br_if $next-node (i32.eqz (local.get $args)))
										;; pointers
										(local.set $i (local.get $a-arity))
										(loop
											(i32.store (local.get $stack) (i32.load (local.get $node)))
											(local.set $node (i32.add (local.get $node) (i32.const 8)))
											(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
											(local.set $i (i32.sub (local.get $i) (i32.const 1)))
											(br_if 0 (local.get $i))
										)
										;; unboxed arguments
										(local.set $node (i32.add (local.get $node) (local.get $b-arity)))
										;; iterate
										(local.set $args (i32.sub (local.get $args) (i32.const 1)))
										(br $lp)
									)
								)
							)
							(if
								(i32.eq (local.get $arity) (i32.const 1))
								(then
									(local.set $node (i32.load offset=8 (local.get $node)))
									(br $continue)
								)
							)
							(if
								(i32.eq (local.get $arity) (i32.const 2))
								(then
									(i32.store (local.get $stack) (i32.load offset=16 (local.get $node)))
									(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
									(local.set $node (i32.load offset=8 (local.get $node)))
									(br $continue)
								)
							)
							(if ;; split hnf without unboxed arguments
								(i32.lt_s (local.get $arity) (i32.const 256))
								(then
									(local.set $arity (i32.sub (local.get $arity) (i32.const 2)))

									(local.set $args (i32.load offset=16 (local.get $node)))
									(local.set $args (i32.add (local.get $args) (i32.sub (i32.shl (local.get $arity) (i32.const 3)) (i32.const 16))))

									(i32.store (local.get $stack) (i32.load offset=16 (local.get $args)))
									(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
									(i32.store (local.get $stack) (i32.load offset=8 (local.get $args)))
									(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))

									(block $blk
										(loop $lp
											(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
											(br_if $blk (i32.eqz (local.get $arity)))
											(i32.store (local.get $stack) (i32.load (local.get $args)))
											(local.set $args (i32.sub (local.get $args) (i32.const 8)))
											(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
											(br $lp)
										)
									)

									(local.set $node (i32.load offset=8 (local.get $node)))
									(br $continue)
								)
							)

							;; hnf with unboxed arguments
							(local.set $a-arity (i32.load16_u (local.get $desc)))
							(br_if $next-node (i32.eqz (local.get $a-arity)))

							(if
								(i32.ge_s (local.get $a-arity) (i32.const 2))
								(then
									(local.set $args (i32.load offset=16 (local.get $node)))
									(if
										(i32.eq (local.get $a-arity) (i32.const 2))
										(then
											(i32.store (local.get $stack)
												(select
													(local.get $args)
													(i32.load (local.get $args))
													(i32.eq (i32.sub (local.get $arity) (i32.const 256)) (i32.const 2))))
											(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
										)
										(else
											(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
											(local.set $args (i32.add (local.get $args) (i32.shl (local.get $a-arity) (i32.const 3))))
											(loop $lp
												(local.set $args (i32.sub (local.get $args) (i32.const 8)))
												(i32.store (local.get $stack) (i32.load (local.get $args)))
												(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
												(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
												(br_if $lp (local.get $a-arity))
											)
										)
									)
								)
							)

							(local.set $node (i32.load offset=8 (local.get $node)))
							(br $continue)
						)
					)

					;; thunk
					(local.set $arity (i32.load (i32.sub (local.get $desc) (i32.const 4))))

					(if
						(i32.le_s (local.get $arity) (i32.const 1))
						(then ;; arity 0, 1, or negative
							(br_if $next-node (i32.eqz (local.get $arity)))

							;; arity 1 or negative
							(local.set $node (i32.load offset=8 (local.get $node)))
							(br $continue)
						)
					)

					;; arity > 1 or with unboxed arguments
					(if
						(i32.lt_s (local.get $arity) (i32.const 256))
						(then ;; no unboxed arguments
							(local.set $args (i32.add (local.get $node) (i32.shl (local.get $arity) (i32.const 3))))
							(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
							(loop $lp
								(i32.store (local.get $stack) (i32.load (local.get $args)))
								(local.set $args (i32.sub (local.get $args) (i32.const 8)))
								(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
								(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
								(br_if $lp (local.get $arity))
							)
						)
						(else ;; unboxed arguments
							(local.set $b-arity (i32.shr_u (local.get $arity) (i32.const 8)))
							(local.set $a-arity (i32.sub (i32.and (local.get $arity) (i32.const 255)) (local.get $b-arity)))

							(br_if $next-node (i32.eqz (local.get $a-arity)))

							(if
								(i32.gt_s (local.get $a-arity) (i32.const 1))
								(then
									(local.set $args (i32.add (local.get $node) (i32.shl (local.get $a-arity) (i32.const 3))))
									(local.set $a-arity (i32.sub (local.get $a-arity) (i32.const 1)))
									(loop $lp
										(i32.store (local.get $stack) (i32.load (local.get $args)))
										(local.set $args (i32.sub (local.get $args) (i32.const 8)))
										(local.set $stack (i32.sub (local.get $stack) (i32.const 4)))
										(local.set $arity (i32.sub (local.get $arity) (i32.const 1)))
										(br_if $lp (local.get $a-arity))
									)
								)
							)
						)
					)

					(local.set $node (i32.load offset=8 (local.get $node)))
					(br $continue)
				)
			)

			(local.set $stack (i32.add (local.get $stack) (i32.const 4)))
			(local.set $node (i32.load (local.get $stack)))
			(br_if $loop (i32.ne (local.get $stack) (local.get $begin-stack)))
		)
	)
)
