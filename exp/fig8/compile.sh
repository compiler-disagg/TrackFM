export LLVM_COMPILER=clang
sudo ln -sf /proj/rmt-PG0/noelle_setup/llvm-project/build/bin/llvm-objcopy /usr/bin/objcopy
sudo ln -sf /proj/rmt-PG0/noelle_setup/llvm-project/build/bin/llvm-link /usr/bin/llvm-link
wllvm -fPIC -O1 -Xclang -disable-llvm-passes -g -fno-vectorize -fno-builtin kmeans.c kmeans_test.c -o main -I inc/ -lm 
extract-bc main
