do_install_append() {
    echo "include ld.so.conf.d/*.conf" >> ${D}${sysconfdir}/ld.so.conf
}
