for i in `cat /home/CARM/app_symbols`
do
	foo=${i#"carm"}
	objcopy --redefine-sym $i=$foo  maint.o
done
