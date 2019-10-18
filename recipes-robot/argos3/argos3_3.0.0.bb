DESCRIPTION = "ARGoS: a parallel, multi-engine simulator for heterogeneous \
swarm robotics"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${WORKDIR}/git/doc/ARGoS_LICENSE.txt;md5=a1f9fe093c34486e45d07b2d33fb5325"
HOMEPAGE = "http://www.argos-sim.info/"

DEPENDS = "lua"

inherit cmake

SRC_URI = "git://github.com/ilpincy/argos3;protocol=http \
           file://0001-Workaround-for-hardware.patch \
"

SRCREV = "8a2279d92bc527552650735f499aefd2ea68a9bc"

S = "${WORKDIR}/git"

OECMAKE_SOURCEPATH = "${S}/src"

EXTRA_OECMAKE += "-DARGOS_BUILD_FOR=hardware -DARGOS_DOCUMENTATION=OFF"

FILES_${PN} += "${prefix}/*"

