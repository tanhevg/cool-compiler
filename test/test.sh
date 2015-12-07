#!/bin/sh

function testex {
    ../output/Debug/cool-parser -o ../output/$1.tree ../../cool/cool-examples/$1.cl  > ../output/$1.out 2>../output/$1.err
}

function test {
    ../output/Debug/cool-parser -o ../output/$1.tree $1.cl  > ../output/$1.out 2>../output/$1.err
}

testex arith
testex cells
testex hello-world
testex hs
testex list
testex new-complex
testex primes
testex print-cool
testex sort-list.cl
test good_semant
test bad_semant