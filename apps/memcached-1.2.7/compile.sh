export LLVM_COMPILER=clang
cp make_O1 Makefile
make clean
make
extract-bc memcached
