SUMMARY = "Linux kernel"
SECTION = "kernel"
LICENSE = "GPLv2"

LIC_FILES_CHKSUM ?= "file://COPYING;md5=d7810fab7487fb0aad327b76f1be7cd7"

DEPENDS += "bc-native bison-native openssl-native util-linux-native xz-native"

inherit kernel
