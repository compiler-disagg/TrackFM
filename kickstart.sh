cd kickstart/cstream/TrackFM/
./run.sh
wc="cat log.1 | grep \"Solution Validates: avg error less than 1.000000e-13 on all three arrays\"|wc -l"
wco=$(eval "$wc")
if [[ "$wco" == *"1"* ]];
then
echo "TrackFM installed sucessfully"
else
echo "TrackFM installation failed"
fi
