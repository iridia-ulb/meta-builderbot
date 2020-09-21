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
5. Refer to the instructions on the [Gumstix website](https://www.gumstix.com/support/getting-started/create-bootable-microsd-card/) for preparing an SD card for the Duovero COM on the BuilderBot robot
