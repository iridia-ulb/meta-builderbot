# The Yocto Layer for the BuilderBot Robot
This repository is a layer for the Yocto build system. It is based on and updated from the [meta-duovero](https://github.com/jumpnow/meta-duovero) layer by [Scott Ellis](mailto:scott@jumpnowtek.com).

## Instructions
1. Clone the yocto project repository and check out the sumo branch.
```bash
git clone --single-branch --branch sumo git://git.yoctoproject.org/poky.git poky-sumo
```
2. Clone this repository into the poky directory.
```bash
cd poky-sumo
git clone https://github.com/allsey87/meta-builderbot.git
```
3. Set up the build environment using the configuration from the meta-builderbot layer. Executing this line will move you into the build directory.
```bash
TEMPLATECONF=meta-builderbot/conf source oe-init-build-env
```
4. Build the main image.
```bash
bitbake console-image-builderbot
```
5. Make the sdcard, use dmesg to check its id, for example: sdb
	1. Create a mount point
	```bash
	sudo mkdir /media/card
	```
	2. Specify target directory in environment variable
	```bash
	source OETMP=tmp
	```
	3. Partition the SD card into 2 partitions. 
	Check your sdcard label very carefully!!! The "sdb"s in the following commands are just examples!!!
	```bash
	sudo ../meta-builderbot/scripts/mk2parts.sh sdb
	```

	4. Copy boot loader
	```bash
	../meta-builderbot/scripts/copy_boot.sh sdb
	```
	5. Copy root file system
	```bash
	../meta-builderbot/scripts/copy_rootfs.sh sdb
	```
```bash
