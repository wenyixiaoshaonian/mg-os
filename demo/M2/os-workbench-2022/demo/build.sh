# rm a.out
# gcc -O1 -c sum.c -lpthread && objdump -d sum.o
# gcc -O1 sum.c -lpthread && objdump -d a.out
gcc peterson-barrier.c -lpthread 
./a.out
