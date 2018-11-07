# The Yocto Layer for the BuilderBot Robot
This repository is a layer for the Yocto build system. It is based on and updated from the [meta-duovero](https://github.com/jumpnow/meta-duovero) layer by [Scott Ellis](mailto:scott@jumpnowtek.com).

## Instructions
1. Clone the yocto project repository and check out version 2.5 into a new branch.
```bash
git clone git://git.yoctoproject.org/poky.git
cd poky
git checkout tags/yocto-2.5 -b my-yocto-2.5
```
2. Clone this repository into the poky directory.
```bash
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
