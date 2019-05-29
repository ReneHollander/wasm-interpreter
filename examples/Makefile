.PHONY: all test

all: test.wasm

test.wasm: test.c
	emcc test.c -g -s WASM=1 -s ONLY_MY_CODE=1 -s EXPORTED_FUNCTIONS='["_simple_function"]' -o test.wasm

clean:
	rm -f *.map *.wast
