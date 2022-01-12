inherit systemd

SUMMARY = "Fernbedienung"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
RDEPENDS_${PN} = "python3"

SRC_URI = " \
   git://github.com/iridia-ulb/fernbedienung-python.git;protocol=http;branch=builderbot \
   file://fernbedienung.service \
"

SRCREV = "${AUTOREV}"

SYSTEMD_SERVICE_${PN} = "fernbedienung.service"

S = "${WORKDIR}/git"

do_install() {
   install -d ${D}${libdir}/fernbedienung/
   install -m 0644 ${S}/service.py ${D}${libdir}/fernbedienung/
   install -d ${D}${systemd_unitdir}/system/
   install -m 0644 ${WORKDIR}/fernbedienung.service ${D}${systemd_unitdir}/system/
}

FILES_${PN} = "${libdir}/fernbedienung/service.py ${systemd_unitdir}/system/fernbedienung.service"

