# wasm-interpreter
An interpreter for WASM for the Abstract Machines course at TU Vienna

- All commands assume the current directory to be the project root
- Building the binary (requires [cmake](https://cmake.org/))
  - `cmake --build . --target interpreter -- -j 4`

- Test execution (needs Python 3)
  - Prepare tests: `./prepare_tests.sh`
  - Execute specific test: `python run_tests.py wasm_interpreter [test_wat_file]`
  - Execute all tests: `python run_tests.py wasm_interpreter`
    
- Interpreting a wasm or wat file requires [wabt](https://github.com/WebAssembly/wabt)
  - In case of a wat file, it first has to be converted to a wasm file: `wat2wasm file.wat`
  - The wasm file can then be given as program argument to the interpreter, together with the 
  name of the function to be called: `wasm_interpreter -p ../examples/func1.wasm -f func`.
  - Alternatively, you can also provide arguments for the function by using the flag -a in the form `type:value` like so: 
  `wasm_interpreter -p ../examples/increment_i32.wasm -f increment -a i32:10 -a i32:11`
