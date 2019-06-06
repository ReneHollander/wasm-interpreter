;;tests globals, locals, params and function calls, expected result: 35
;;exported function: func

(module
  (global (mut i32) (i32.const 50))
  (global (mut i32) (i32.const 20))
  (func $sub (param i32) (param i32) (result i32) (local i32)
    get_local 0
    get_local 1
    i32.sub
    i32.const 10
    set_local 2
    get_local 2
    i32.add)
  (func (export "func") (result i32)
    i32.const 4
    i32.const 5
    call $sub
    i32.const 6
    i32.add
    get_global 1
    i32.add))
