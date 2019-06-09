;; should jump out of the inner block and add 4, 10
;; expected result: 14
(func (export "br_test") (result i32)
    (block (result i32)
    (block (result i32) (i32.const 2) (i32.const 3) (i32.const 4) (br 0)) (i32.const 10) i32.add)
)

;; should return, i.e. jump out of all blocks including the containing function
;; expected result: 4
(func (export "return_test") (result i32)
    (block (result i32)
    (block (result i32) (i32.const 2) (i32.const 3) (i32.const 4) (return)) (i32.const 10) i32.add)
)

;; should return, i.e. jump out of all blocks including the containing function
;; expected result: 4
(func (export "return_test2") (result i32)
    (block (result i32) (i32.const 2) (i32.const 3) (i32.const 4) (return) (i32.const 5))
)

;; should return, i.e. jump out of all blocks including the containing function
;; expected result: 4
(func (export "return_test3") (result i32)
  (i32.const 2) (i32.const 3) (i32.const 4) (return) (i32.const 5)
)

;; should not jump at all
;; expected result: 12
(func (export "br_if_test1") (result i32)
    (block (result i32)
    (block (result i32) (i32.const 2) (i32.const 1) (br_if 1)) (i32.const 10) i32.add)
)

;; should jump to the end of the outer block into the containing function
;; expected result: 2
(func (export "br_if_test2") (result i32)
    (block (result i32)
    (block (result i32) (i32.const 2) (i32.const 1) (br_if 1)) (i32.const 10) i32.add)
)

;; should take the if branch
;; expexted result: 10
(func (export "if_test") (result i32)
    (i32.const 100)
    (if (result i32)
        (then i32.const 10)
        (else i32.const 11)
    )
)

;; should take the if branch
;; expexted result: 11
(func (export "else_test") (result i32)
    (i32.const 0)
    (if (result i32)
        (then i32.const 10)
        (else i32.const 11)
    )
)

(func (export "loop_test1") (result i32) (local i32) (local i32)
    (i32.const 10)  ;; loop counter
    (set_local 0)
    (i32.const 1)   ;; product
    (set_local 1)
    (loop
        (i32.const 2)
        (get_local 1)
        (i32.mul)       ;; multiply local by 2
        (set_local 1)   ;; save result
        (get_local 0)
        (i32.const 1)
        (i32.sub)       ;; decrement loop counter
        (tee_local 0)
        (i32.const 0)
        (i32.ne)
        (br_if 0)
    )
    (get_local 1)
)

(func (export "loop_test2") (result i32) (local i32) (local i32)
    (loop (result i32)
        (i32.const 0)
        (br_if 0)
        (i32.const 1)
    )
)

(func (export "factorial") (param i64) (result i64)
    (get_local 0)
    (i64.const 0)
    (i64.eq)

    (if (result i64)
        (then (i64.const 1))
        (else
            get_local 0
            get_local 0
            i64.const 1
            i64.sub
            call 10
            i64.mul
         )
     )
)

(func (export "call-factorial") (result i64)
    (i64.const 5)
    (call 10)
)