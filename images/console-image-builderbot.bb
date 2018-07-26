SUMMARY = "Image recipe for the BuilderBot"
HOMEPAGE = "http://code.ulb.ac.be/iridia.home.about.php"

IMAGE_FEATURES += "package-management"
IMAGE_LINGUAS = "en-us"

inherit image

BUILDERBOT_INSTALL = " \
   argos3-builderbot \
"

CORE_OS = " \
    busybox-hwclock \
    openssh openssh-keygen openssh-sftp-server \
    packagegroup-core-boot \
    tzdata \
"

KERNEL_EXTRA_INSTALL = " \
    kernel-modules \
    kernel-module-bb-avr-dds \
    kernel-module-bb-avr-ems \
    kernel-module-bb-avr-i2c \
    kernel-module-bb-avr-las \
    kernel-module-bb-avr-nfc \
    kernel-module-bb-avr-poweroff \
    kernel-module-bb-avr-regulator \
    kernel-module-bb-avr-uptime \
    kernel-module-ov5640 \
"

WIFI_SUPPORT = " \
    iw \
    linux-firmware-sd8787 \
    wireless-tools \
    wpa-supplicant \
"

EXTRA_TOOLS_INSTALL = " \
    bzip2 \
    dosfstools \
    ethtool \
    findutils \
    i2c-tools \
    less \
    nano \
    picocom \
    procps \
    sysfsutils \
    tcpdump \
    unzip \
    wget \
    zip \
"

IMAGE_INSTALL += " \
    ${CORE_OS} \
    ${KERNEL_EXTRA_INSTALL} \
    ${WIFI_SUPPORT} \
    ${EXTRA_TOOLS_INSTALL} \
    ${BUILDERBOT_INSTALL} \
"

set_local_timezone() {
    ln -sf /usr/share/zoneinfo/Europe/Brussels ${IMAGE_ROOTFS}/etc/localtime
}

disable_bootlogd() {
    echo BOOTLOGD_ENABLE=no > ${IMAGE_ROOTFS}/etc/default/bootlogd
}

ROOTFS_POSTPROCESS_COMMAND += " \
    set_local_timezone ; \
    disable_bootlogd ; \
 "

export IMAGE_BASENAME = "console-image-builderbot"
