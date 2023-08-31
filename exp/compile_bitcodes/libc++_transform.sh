noelle-simplification  libc++.so.ll -o mainn.bc 
noelle-load -load ~/CAT/lib/TRACKFMCHECKS.so -CAT mainn.bc -o main_check.bc 
noelle-load -load ~/CAT/lib/TRACKFMTRANSFORM.so -CAT main_check.bc -o main_transform.bc 
noelle-load -load ~/CAT/lib/TRACKFMLIBCTRANSFORM.so -CAT main_transform.bc -o main_libctransform.bc
opt -O3 -disable-simplify-libcalls main_libctransform.bc -o opt.bc
llc -relocation-model=pic -filetype=obj opt.bc -o libc++.so.1.0.o
objcopy --prefix-symbols=carm libc++.so.1.0.o
for i in `cat /home/TrackFM/libc++_symbols`
do
	foo=${i#"carm"}
	objcopy --redefine-sym $i=$foo  libc++.so.1.0.o
done
objcopy --redefine-sym carmmain=carmsmain  libc++.so.1.0.o
