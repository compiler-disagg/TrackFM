AIFM_PATH=/home/AIFM/aifm/exp/fig7/aifm_no_offload/
figpath="/home/TrackFM/exp/fig14a/AIFM"
sudo mkdir /mnt/
sudo ln -sf /home/datasets/all.csv /mnt/all.csv
cd $AIFM_PATH
./run.sh
mv log.* $TRACKFM_PATH/plotgen/scripts/figgen/results/fig14a/AIFM/
cd  $figpath
cd ../../../plotgen
python3 scripts/figgen/fig14a.py $figpath fig14a
