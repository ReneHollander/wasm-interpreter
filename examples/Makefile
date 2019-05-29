.PHONY: all test

all: test.wasm

test.wasm: test.c
	emcc test.c -v -g -O3 -s WASM=1 -s SIDE_MODULE=1 -o test.wasm

clean:
	rm -f *.wasm *.map *.wast
