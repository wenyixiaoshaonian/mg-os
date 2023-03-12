rm test.i test
gcc -E -static test.c -o test.i
gcc -static test.c -o test