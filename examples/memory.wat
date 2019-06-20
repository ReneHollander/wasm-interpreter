;;tests function calls, constants, add function
;;expected result: 43

(module
  (import "env" "memory" (memory $mem 1))
  (func (export "calculate") (result i32)
    (i32.const 1)
    (i32.const 0)
    (i32.load)
    (i32.const 20)
    (i32.add)
    (i32.store)
    (i32.const 1)
    (i32.load)
  )
)
