rm disk.img

pushd ../toolchain/vagrant
./build.sh
popd

if [[ ! -f "disk.img" ]]
then
	./imagegen.sh
fi

