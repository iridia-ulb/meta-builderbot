# The meta-builderbot layer for the Yocto build system

## Description
The [meta-builderbot repository](https://github.com/iridia-ulb/meta-builderbot) contains a layer for the Yocto build system, which generates a complete, bootable Linux OS ready to be run on the BuilderBot. This layer is based on the [meta-duovero](https://github.com/jumpnow/meta-duovero) layer by [Scott Ellis](mailto:scott@jumpnowtek.com). The system comes preinstalled with:

- ARGoS3 and a plugin for the BuilderBot
- Python3

## Quick start
To ensure reproducible builds on systems with varying configurations, the following steps will explain how to create an image using [Docker](https://docs.docker.com/get-docker/). Note that you will probably need to use sudo or to switch to a root user to install Docker or to create and run its containers.

To get started, you first need to decide where you want to build the system for the BuilderBot. Keep in mind that while the resulting image will be between 200-300 megabytes in size, the build system itself will require around **50 GB** of free disk space. The following steps will set up the build system. These steps assume that the build location is `/home/$(id -un)/yocto-pipuck` where `$(id -un)` will be replaced with the current username.
```sh
# Create a directory for the build system
mkdir /home/`id -un`/yocto-builderbot
```

We now need to clone the layers for the build system as follows:
```sh
# Switch to the build location
cd /home/`id -un`/yocto-builderbot
# Clone the Yocto repository
git clone git://git.yoctoproject.org/poky --branch sumo --single-branch
# Clone this layer inside the poky directory
cd poky
git clone https://github.com/iridia-ulb/meta-builderbot.git
```

### Create the Docker image
The following command will execute the Dockerfile in the meta-builderbot repository and create a Docker image based on Ubuntu 16.04 LTS. The image will contain a user and a group, which match the identifiers of current user and group. Setting the user and group in this way enables trivial access to the build system from the host.
```sh
sudo docker build -t yocto-builderbot:latest \
 https://github.com/iridia-ulb/meta-builderbot.git#:docker \
 --build-arg host_user_id=$(id -u) \
 --build-arg host_group_id=$(id -g)
```

Create the Docker container
Once the above command has completed successfully, you can run the following command to create a container from the image. Note the two paths given after the `-v` option. The format of this argument is `path/on/host:path/in/container` where `path/on/host` is a directory on your host system and `path/in/container` is a directory inside the Docker container. This command will map the home directory inside the container to a directory called `yocto-builderbot` under the current user's home directory on the host.
```sh
sudo docker create --tty --interactive \
 --volume /home/$(id -un)/yocto-builderbot:/home/developer \
 --name yocto-pipuck
 --hostname yocto-pipuck yocto-pipuck:latest
```
After executing this command, you should have a new container with the build environment. The following commands will start and attach to that container.

```sh
sudo docker start yocto-builderbot
sudo docker attach yocto-builderbot
```

### Start the build
After following the steps above, you should have a terminal that is attached to the docker container and be inside a directory called `build`. To build the entire image for the BuilderBot, just run the following command:
```sh
bitbake console-image-builderbot
```

Occasionally, the build can fail due to internet connectivity issues or due to an oversight in the dependency tree. These issues are normally resolved by just re-executing the command above.

## Copying the image
The instructions for creating a bootable microSD card are available on the [Gumstix website](https://www.gumstix.com/community/support/getting-started/create-bootable-microsd-card/).

## Booting the image and accessing the console
The easiest and most reliable way to get access to the BuilderBot is by using the on-board serial-to-USB converter. You can then connect to the board using a terminal application such as Picocom as follows:
```sh
picocom -b 115200 /dev/ttyUSBX
```
Where `ttyUSBX` is the serial-to-USB converter. Check `dmesg` while attaching the cable to confirm that you have the right device. Note that to access the serial port, you will either have to (i) use `sudo`, (ii) switch to the root user, or (iii) add yourself to the `dialout` group (do not forget to restart afterwards).

## Wifi configuration
Configuration for the wireless connection can be made by adding networks to [`wpa_supplicant.conf-sane`](https://github.com/iridia-ulb/meta-builderbot/blob/master/recipes-connectivity/wpa-supplicant/files/wpa_supplicant.conf-sane) before building the image. By default, the BuilderBot will connect to the network `MergeableNervousSystem` using PSK authentication with the password `uprising`. The network should automatically connect on boot and fetch an IP address using DHCP.

