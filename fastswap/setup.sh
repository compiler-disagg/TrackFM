rm -r /home/cfm
cp -r cfm /home/
export USERNAME=$1
export IP=$2
cd /home
sudo swapoff swap.img
sudo swapoff /dev/sda3
sudo rm -r /home/swap.img
sudo dd if=/dev/zero of=swap.img bs=31M count=1000
sudo mkswap swap.img
sudo swapon swap.img
