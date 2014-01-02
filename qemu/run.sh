rm disk.img

if [[ ! -f "disk.img" ]]
then
	./imagegen.sh
fi

qemu-system-x86_64 -hda disk.img
