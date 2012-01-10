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
mkdir -p ${D}/usr/share/doc/linuxbuh
#mkdir -p ${D}/usr/sbin
mkdir -p ${D}/usr/share/pixmaps
mkdir -p ${D}/usr/share/applications

#cp -r ${WORKDIR}/${PN}/shscripts/update-system-notify ${D}/usr/sbin
#cp -r ${WORKDIR}/${PN}/shscripts/update-system-notify-daemon ${D}/usr/sbin
#cp -r ${WORKDIR}/${PN}/shscripts/update-system-updater ${D}/usr/sbin
#cp -r ${WORKDIR}/${PN}/conf/desktop.cfg ${D}/etc/update-system-notify
#cp -r ${WORKDIR}/${PN}/conf/update-system-notify.cron ${D}/etc/cron.hourly
#cp -r ${WORKDIR}/${PN}/README.txt ${D}/usr/share/doc/${PN}
#cp -r ${WORKDIR}/${PN}/Changelog.txt ${D}/usr/share/doc/${PN}
#cp -r ${WORKDIR}/${PN}/TODO.txt ${D}/usr/share/doc/${PN}
#cp -r ${WORKDIR}/${PN}/resources/images/dialog-error.png ${D}/usr/share/doc/${PN}
#cp -r ${WORKDIR}/${PN}/resources/images/dialog-warning.png ${D}/usr/share/doc/${PN}
#cp -r ${WORKDIR}/${PN}/resources/images/update-system-updater.png ${D}/usr/share/pixmaps
cp -r ${WORKDIR}/labels/*.desktop ${D}/usr/share/applications
cp -r ${WORKDIR}/INSTALL ${D}/usr/share/doc/linuxbuh
#cp -r ${WORKDIR}/${PN}/resources/images/update-system-updater-kde.desktop ${D}/usr/share/doc/${PN}

cd ${WORKDIR}
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
