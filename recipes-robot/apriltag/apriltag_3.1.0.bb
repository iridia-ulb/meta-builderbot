SUMMARY = "The AprilTag Library"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://apriltag.h;endline=26;md5=a84642dbd9d801540a1b3c2f02e2f3e3"
HOMEPAGE = "https://april.eecs.umich.edu/software/apriltag"

inherit cmake

SRC_URI = "git://github.com/AprilRobotics/apriltag;tag=3.1.0;protocol=http"

S = "${WORKDIR}/git"

FILES_${PN} += "${prefix}/*"
