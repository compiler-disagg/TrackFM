for i in `cat /home/TrackFM-test/app_symbols`
do
foo=${i#"carm"}
objcopy --redefine-sym $i=$foo  maint.o
done
