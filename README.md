# The Yocto Layer for the BuilderBot Robot
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
