SUMMARY = "Library for interfacing with IIO devices"
HOMEPAGE = "https://wiki.analog.com/resources/tools-software/linux-software/libiio"
SECTION = "libs"
LICENSE = "LGPLv2.1+"
LIC_FILES_CHKSUM = "file://COPYING.txt;md5=7c13b3376cea0ce68d2d2da0a1b3a72c"

SRCREV = "6ecff5d46e1b12c2859f0b63a73282940e3402bb"
PV = "0.15"

SRC_URI = "git://github.com/analogdevicesinc/libiio.git"

S = "${WORKDIR}/git"

DEPENDS = "flex-native bison-native avahi libaio libusb1 libxml2"

inherit cmake pythonnative

EXTRA_OECMAKE += "-DLOG_LEVEL=Error"

PACKAGES =+ "${PN}-iiod ${PN}-tests ${PN}-python"

RDEPENDS_${PN}-python = "${PN} python-ctypes python-stringold"

FILES_${PN}-iiod = "${sbindir}/iiod"
FILES_${PN}-tests = "${bindir}"
FILES_${PN}-python = "${PYTHON_SITEPACKAGES_DIR}"
