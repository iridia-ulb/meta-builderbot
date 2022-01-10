# The meta-builderbot layer for the Yocto build system

## Description
This repository contains a layer for the Yocto build system, which generates a complete, bootable Linux OS ready to be run on the BuilderBot. It is based on the [meta-duovero](https://github.com/jumpnow/meta-duovero) layer by [Scott Ellis](mailto:scott@jumpnowtek.com). The system comes preinstalled with:
- ARGoS3 and a plugin for the BuilderBot
- Python3

## Quick start
We have listed two ways to build the system below. The reccomended method is to use Docker which will ensure a clean and predictable build environment. The only drawback of the Docker method is that you will need to use sudo or to switch user to root to use Docker. However, this is also a requirement for writing the built image to a USB stick.

To get started, you need to decide where you want to build the system for the BuilderBot. Keep in mind that while the resulting image will be approximately 1 GB in size, the build itself will require around **50 GB** of free disk space. In the following steps, we assume that the build location is ``/home/`id -un`/yocto-builderbot`` where `` `id -un` `` will be replaced with the current username.
```bash
# Create a directory for the build system
mkdir /home/`id -un`/yocto-builderbot
```

We now need to clone the layers for the build system as follows:
```bash
# Switch to the build location
cd /home/`id -un`/yocto-builderbot
# Clone the Yocto repository
git clone git://git.yoctoproject.org/poky --branch sumo --single-branch
# Clone this layer inside the poky directory
cd poky
git clone https://github.com/iridia-ulb/meta-builderbot.git
```

### Set up the build system
The easiest way to build the system for the BuilderBot is to use Docker. Given you have Docker installed, the following command will execute the Dockerfile in this repository and create a Docker image based on Ubuntu 16.04.7 LTS. The image will contain a user and a group, which match the current user and group. Setting the user and group enables trivial access to the build system from the host.
```bash
sudo docker build -t yocto-builderbot:latest https://github.com/iridia-ulb/meta-builderbot.git#:docker \
 --build-arg host_usrid=`id -u` \
 --build-arg host_usrname=`id -un` \
 --build-arg host_grpid=`id -g` \
 --build-arg host_grpname=`id -gn`
```
Once the above command has completed sucessfully, you can run the following command to create a container from the image. Note the two paths given after the `-v` option. The format of this argument is `path/on/host:path/in/container` where `path/on/host` is a directory on your host system and `path/in/container` is a directory inside the Docker container. This command will map the home directory inside the container to a directory called `yocto-builderbot` under the current users home directory on the host.
```bash
sudo docker create --tty --interactive --volume /home/`id -un`/yocto-builderbot:/home/`id -un` \
 --name yocto-builderbot --hostname yocto-builderbot yocto-builderbot:latest
```
After executing this command, you should have a new container with the build environment. The following commands will start and attach to that container.

```
sudo docker start yocto-builderbot
sudo docker attach yocto-builderbot
```


### Start the build
After following the steps above, you should have a terminal that is attached to the docker container and be inside a directory called `build`. To build the entire image for the BuilderBot, just run the following command:
```bash
bitbake console-image-builderbot
```

Occasionally, the build can fail due to internet connectivity issues or due to an oversight in the dependency tree. These issues are normally resolved by just re-executing the command above.

## Copying the image
The instructions for creating a bootable microSD card are available on the [Gumstix website](https://www.gumstix.com/community/support/getting-started/create-bootable-microsd-card/).

## Booting the image and accessing the console
TODO

## Wifi configuration
Configuration for the wireless connection can be made by adding networks to `meta-builderbot/recipes-connectivity/wpa-supplicant/files/wpa_supplicant.conf-sane` before building the image. By default, the robot will connect to the network `MergeableNervousSystem` using PSK authentication (password `uprising`).



