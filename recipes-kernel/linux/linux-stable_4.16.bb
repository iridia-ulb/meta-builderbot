require linux-stable.inc

COMPATIBLE_MACHINE = "builderbot"

KERNEL_DEVICETREE_builderbot = "omap4-duovero-builderbot.dtb"

LINUX_VERSION = "4.16"
LINUX_VERSION_EXTENSION = "-builderbot"

FILESEXTRAPATHS_prepend := "${THISDIR}/linux-stable-${LINUX_VERSION}:"

S = "${WORKDIR}/git"

PV = "4.16.5"
SRCREV = "e5ce9f6879d3fe20435f34dfd86fb76c36072916"
SRC_URI = " \
    git://git.kernel.org/pub/scm/linux/kernel/git/stable/linux-stable.git;branch=linux-${LINUX_VERSION}.y \
    file://omap4-duovero-builderbot.dts;subdir=git/arch/arm/boot/dts \
    file://0000-kernel-GPL-export-symbol-freeze_secondary_cpus.patch \
    file://0001-mwifiex-Skip-unsupported-CHAN_REGION_CFG-cmd.patch \
    file://0002-mwifiex-Remove-unsupported-GTK_REKEY_OFFLOAD_CFG-cmd.patch \
    file://0003-iio-Add-support-for-low-speed-output-buffers.patch \
    file://0004-vcnl4000-Add-device-tree-and-software-buffer-support.patch \
    file://0005-sc16is7xx-Improve-stability-for-use-with-serdev.patch \
    file://0006-serdev-Add-support-for-multi-UART-devices.patch \
    file://0007-mfd-Add-support-for-the-BuilderBot-AVRs.patch \
    file://defconfig \
"

