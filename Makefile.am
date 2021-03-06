#
# vStuff main Makefile
#
# Copyright (C) 2004-2007 Daniele Orlandi
#
# Authors: Daniele "Vihai" Orlandi <daniele@orlandi.com>
#
# This program is free software and may be modified and distributed
# under the terms and conditions of the GNU General Public License.
#

noinst_DATA =					\
	include/list.h				\
	include/longtime.h

EXTRA_DIST =					\
	$(sysconf_DATA)				\
	$(astconf_DATA)				\
	$(noinst_DATA)				\
	rpm/vstuff-centos.spec			\
	rpm/vstuff-centos-kmod-el5.spec		\
	rpm/vstuff-suse.spec			\
	visdn-init				\
	visdn-sleep.diff			\
	visdn-sysconfig				\
	modules/include/compat/*		\
	modules/include/kernel_config.h.in	\
	modules/include/linux/lapd.h		\
	modules/include/linux/vgsm.h		\
	modules/include/linux/vgsm2.h		\
	modules/include/linux/kstreamer/*.h	\
	modules/include/linux/visdn/*.h		\
	docs/*					\
	vgsm2_firmware/*.fw			\
	vgsm2_firmware/README

SUBDIRS = tools

if kernel_modules
SUBDIRS += modules
endif

SUBDIRS += libskb
SUBDIRS += libkstreamer
SUBDIRS += libq931
SUBDIRS += kstool
SUBDIRS += vgsmctl
SUBDIRS += visdn_netdev_ctl
SUBDIRS += scripts samples unsupported

if asterisk_modules
SUBDIRS += res_kstreamer
SUBDIRS += chan_visdn
SUBDIRS += chan_vgsm
SUBDIRS += app_pipe
endif

if pppd_plugin
SUBDIRS += pppd_plugin
endif

if tests
SUBDIRS += vgsm2test tests
endif

dist-rpm: dist-bzip2
	rpmbuild -ta @PACKAGE_TARNAME@-@PACKAGE_VERSION@.tar.bz2

.PHONY: ChangeLog
ChangeLog:
	bzr log --log-format gnu > ChangeLog

#release: dist-gzip dist-bzip2 dist-rpm
release: ChangeLog distdir
	tardir=$(distdir) && $(am__tar) | GZIP=$(GZIP_ENV) gzip -c >$(distdir).tar.gz
	tardir=$(distdir) && $(am__tar) | bzip2 -9 -c >$(distdir).tar.bz2
	$(am__remove_distdir)
	-gpg --sign -ab $(distdir).tar.gz
	-gpg --sign -ab $(distdir).tar.bz2
	scp $(distdir).tar.gz $(distdir).tar.gz.asc \
	    $(distdir).tar.bz2 $(distdir).tar.bz2.asc \
	    vihai@stfu.orlandi.com:/srv/www/www.visdn.org/htdocs/download/

doxy:
	doxygen doxygen.conf

