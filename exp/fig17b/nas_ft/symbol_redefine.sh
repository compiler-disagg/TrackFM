for i in `cat /home/TrackFM/app_symbols`
do
foo=${i#"carm"}
objcopy --redefine-sym $i=$foo  maint.o
done
