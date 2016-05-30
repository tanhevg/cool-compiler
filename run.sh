#!/bin/bash

FILE=$1

output/Debug/cool-compiler -o "output/$FILE" "test/$FILE.cl"
spim -exception_file asm/trap/trap.handler.asm -file "output/$FILE.asm"

# output/Debug/cool-compiler -o output/primes test/primes.cl
# output/Debug/cool-compiler -o output/list test/list.cl
# output/Debug/cool-compiler -o output/hello-world test/hello-world.cl
# output/Debug/cool-compiler -o output/atoi test/atoi.cl
# output/Debug/cool-compiler -o output/sort-list test/sort-list.cl
# output/Debug/cool-compiler -o output/print-cool test/print-cool.cl
# output/Debug/cool-compiler -o output/new-complex test/new-complex.cl
# output/Debug/cool-compiler -o output/hs test/hs.cl
# output/Debug/cool-compiler -o output/cells test/cells.cl
# output/Debug/cool-compiler -o output/arith test/arith.cl 
