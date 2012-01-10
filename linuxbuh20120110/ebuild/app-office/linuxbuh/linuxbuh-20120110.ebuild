# Copyright 1999-2011 Gentoo Foundation
# Distributed under the terms of the GNU General Public License v3
# $Header: $

inherit eutils 

DESCRIPTION="Accounting system LinuxBuh.RU"
HOMEPAGE="http://www.linuxbuh.ru"
SRC_URI="https://github.com/downloads/zaharchuktv/linuxbuh/${P}.tar.gz"
LICENSE="GPL-2"
SLOT="0"
KEYWORDS="~amd64 ~x86"
IUSE=""
DEPEND="dev-db/mysql"

RDEPEND="${DEPEND}"

src_unpack() {
	    unpack ${A}
	    
}


src_compile() {
	emake || die "make failed"
}

src_install() {
cd ${WORKDIR}
mkdir -p ${D}/usr/share/doc/linuxbuh
mkdir -p ${D}/usr/share/doc/linuxbuh/bx
mkdir -p ${D}/usr/share/doc/linuxbuh/doc
mkdir -p ${D}/usr/share/pixmaps
mkdir -p ${D}/usr/share/applications
mkdir -p ${D}/usr/bin

cp -r ${WORKDIR}/labels/*.desktop ${D}/usr/share/applications
cp -r ${WORKDIR}/INSTALL ${D}/usr/share/doc/linuxbuh
cp -r ${WORKDIR}/doc/*.txt ${D}/usr/share/doc/linuxbuh/doc
cp -r ${WORKDIR}/bx/*.alx ${D}/usr/share/doc/linuxbuh/bx
cp -r ${WORKDIR}/resources/linuxbuhupdate ${D}/usr/bin
dodir /usr/bin
emake BINDIR=${D}/usr/bin install || die "emake failed"


}

#pkg_postinst() {
#	elog "The program scans the update overlays and displays a message about the updates to the desktop."
#	elog "If necessary, updates the world."
#	elog "After installation, edit the following files:"
#	elog "1) in /etc/update-system-notify/desktop.cfg enter your DE option"
#	elog "for GNOME - gnome-panel parameter for KDE - plasma-desktop"
#	elog "2) the file update-system-notify.cron move the folder /etc/cron.hourly /etc/cron.weekly folder or any other"
#	elog "Read for more information /usr/share/doc/update-system-notify/README.txt
#}
