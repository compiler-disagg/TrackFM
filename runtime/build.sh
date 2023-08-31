git submodule init
git submodule update
cd AIFM/
git checkout master
./build_all.sh
cd ..
sed "s/\/\/update_cache_object(ptr);.*/update_cache_object(ptr);/g" AIFM/aifm/src/manager.cpp -i
