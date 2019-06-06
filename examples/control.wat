(func (export "br_test") (result i32)
    (block (result i32)
    (block (result i32) (i32.const 2) (i32.const 3) (i32.const 4) (br 0)) (i32.const 10) i32.add)
  )

(func (export "return_test") (result i32)
    (block (result i32)
    (block (result i32) (i32.const 2) (i32.const 3) (i32.const 4) (return)) (i32.const 10) i32.add)
  )

(func (export "return_test2") (result i32)
    (block (result i32) (i32.const 2) (i32.const 3) (i32.const 4) (return) (i32.const 5))
  )

(func (export "return_test3") (result i32)
  (i32.const 2) (i32.const 3) (i32.const 4) (return) (i32.const 5)
)