#!/bin/sh

function test {
    ../output/Debug/cool-parser -o ../output/$1.tree ../../cool/cool-examples/$1.cl  > ../output/$1.out 2>../output/$1.err
}
#test arith
#test cells
#test hello-world
#test hs
test list
test new-complex
test primes
test print-cool
test sort-list.cl