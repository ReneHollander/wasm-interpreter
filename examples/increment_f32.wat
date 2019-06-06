;;tests function calls, constants, add function
;;expected result: 43

(module
  (func $getAnswer (result f32) f32.const 42)
  (func (export "increment") (result f32)
    call $getAnswer
    f32.const 1
    f32.add))
