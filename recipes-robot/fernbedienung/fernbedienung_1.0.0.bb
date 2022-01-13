SUMMARY = "Fernbedienung"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/MIT;md5=0835ade698e0bcf8506ecda2f7b4f302"
RDEPENDS_${PN} = "python3"

SRC_URI = " \
   git://github.com/iridia-ulb/fernbedienung-python.git;protocol=http;branch=builderbot \
   file://fernbedienung \
"

SRCREV = "${AUTOREV}"

S = "${WORKDIR}/git"

do_install() {
   install -d ${D}${libdir}/fernbedienung/
   install -m 0644 ${S}/service.py ${D}${libdir}/fernbedienung/
   install -d ${D}${sysconfdir}/init.d
   install -d ${D}${sysconfdir}/rc5.d
   install -m 0755 ${WORKDIR}/fernbedienung ${D}${sysconfdir}/init.d/
   ln -sf ../init.d/fernbedienung ${D}${sysconfdir}/rc5.d/K90fernbedienung
   ln -sf ../init.d/fernbedienung ${D}${sysconfdir}/rc5.d/S90fernbedienung
}

FILES_${PN} = " \
   ${libdir}/fernbedienung/service.py \
   ${sysconfdir}/init.d/fernbedienung \
   ${sysconfdir}/rc5.d/K90fernbedienung \
   ${sysconfdir}/rc5.d/S90fernbedienung \
"

