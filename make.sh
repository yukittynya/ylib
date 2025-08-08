rm -rvf build >/dev/null
mkdir -p build/bin

clang -O3 -c test/test.c -o build/test.o
clang -O3 build/test.o -o build/bin/test

./build/bin/test
