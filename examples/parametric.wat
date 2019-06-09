;; should select the first (pushed) param on the stack
;; expected result: 1
(func (export "select_test1") (result i32)
    (i32.const 1)
    (i32.const 2)
    (i32.const 5)
    (select)
)

;; should select the second (pushed) param on the stack
;; expected result: 2
(func (export "select_test2") (result i32)
    (i32.const 1)
    (i32.const 2)
    (i32.const 0)
    (select)
)

;; should drop the top value from the stack
;; expected result: 100
(func (export "drop_test") (result i32)
    (i32.const 100)
    (i32.const 200)
    (drop)
)
