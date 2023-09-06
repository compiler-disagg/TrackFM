export LLVM_COMPILER=clang
cp make_opt config/make.def
make clean
make cg CLASS=D
make ft CLASS=C
make mg CLASS=D
make is CLASS=D
make sp CLASS=D
