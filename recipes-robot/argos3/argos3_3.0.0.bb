DESCRIPTION = "ARGoS: a parallel, multi-engine simulator for heterogeneous \
swarm robotics"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${WORKDIR}/git/doc/ARGoS_LICENSE.txt;md5=a1f9fe093c34486e45d07b2d33fb5325"
HOMEPAGE = "http://www.argos-sim.info/"

DEPENDS = "lua"

inherit cmake

SRC_URI = "git://github.com/ilpincy/argos3;protocol=http \
           file://0001-Add-hardware-boilerplate-code-to-ARGoS.patch \
           file://0002-Add-the-rate-class.patch \
"

SRCREV = "402cf2fc1dfa03935518062e7ba342b3b3ac87d6"

S = "${WORKDIR}/git"

OECMAKE_SOURCEPATH = "${S}/src"

EXTRA_OECMAKE += "-DARGOS_BUILD_FOR=hardware -DARGOS_DOCUMENTATION=OFF"

FILES_${PN} += "${prefix}/*"

