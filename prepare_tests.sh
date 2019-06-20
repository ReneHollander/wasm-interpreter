#!/bin/bash

cd test/

rm -Rf build/

# Run wast2json on all test files to generate JSON describing the test and the corresponding wasm files.
for wast_file in *.wast; do
    suite_name=${wast_file%.*}
    mkdir -p "build/$suite_name"
    wast2json --no-check "$wast_file" -o "build/$suite_name/$suite_name.json"
done

cd ../
