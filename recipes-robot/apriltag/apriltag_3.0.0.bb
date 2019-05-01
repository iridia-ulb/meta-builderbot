SUMMARY = "The AprilTag Library"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://apriltag.h;endline=26;md5=a84642dbd9d801540a1b3c2f02e2f3e3"
HOMEPAGE = "https://april.eecs.umich.edu/software/apriltag"

inherit cmake

SRC_URI = "git://github.com/AprilRobotics/apriltag;protocol=http"

SRCREV = "b6af709e0616e33b0d07e2a857b383dbb91a67ab"

S = "${WORKDIR}/git"

FILES_${PN} += "${prefix}/*"
