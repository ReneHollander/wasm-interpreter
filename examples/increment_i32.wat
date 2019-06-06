;;tests function calls, constants, add function
;;expected result: 43

(module
  (func $getAnswer (result i32) i32.const 42)
  (func (export "increment") (result i32)
    call $getAnswer
    i32.const 1
    i32.add))
