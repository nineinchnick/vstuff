
astmod_LTLIBRARIES = chan_vgsm.la
chan_vgsm_la_SOURCES = chan_vgsm.c chan_vgm.h

INCLUDES =					\
	-I$(top_builddir)			\
	-I$(top_builddir)/include/		\
	-I$(top_builddir)/modules/include/	\
	-I$(astincdir)

chan_vgsm_la_LIBADD = 
chan_vgsm_la_LDFLAGS = -module -avoid-version -no-undefined

AM_CFLAGS = -D_REENTRANT -D_GNU_SOURCE -Wall