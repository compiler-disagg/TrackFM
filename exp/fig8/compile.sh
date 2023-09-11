export LLVM_COMPILER=clang
sudo ln -sf /home/llvm-project/build/bin/llvm-objcopy /usr/bin/objcopy
wllvm -fPIC -O1 -g -fno-vectorize -fno-builtin kmeans.c kmeans_test.c -o main -I inc/ -lm 
extract-bc main
