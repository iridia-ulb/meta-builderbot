SUMMARY = "AprilTag Library"
LICENSE = "BSD"
LIC_FILES_CHKSUM = "file://src/apriltag.h;endline=31;md5=50390d72e72d3e3d1b440748f252ff47"

inherit cmake

PR = "r0"

SRC_URI = "file://src \
           file://CMakeLists.txt \
          "

SRC_URI[md5sum] = "5607f8c4eba3dfa0c62bcd77b751aef3"
SRC_URI[sha256sum] = "2ade0ebf094d4b666e02a1be9e094a4938223a921b2d48c6a7e2e826d31f1054"

S = "${WORKDIR}"

