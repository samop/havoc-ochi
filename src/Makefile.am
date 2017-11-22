#ochidir=/usr/local/bin
bin_PROGRAMS=ochi
ochi_SOURCES=main.c imageio.c imageio.h img_process.c img_process.h sid_process.c sid_process.h ocrlib/network.c ocrlib/sample.c ocrlib/interpolation.c ocrlib/lwneuralnet.h ocrlib/ocr.h bar_process.c bar_process.h filesystem.c filesystem.h localization.c localization.h ans_process.c ans_process.h daemon.c daemon.h database.c database.h nnrok.c nnrok.h
ochi_LDFLAGS=-DNO_CONSOLE_IO
#ochi_CFLAGS=-DCONFDIR=\"$(sysconfdir)/$(PACKAGE)\" -DDATADIR=\"$(pkgdatadir)\"
AM_CFLAGS=-DCONFDIR=\"$(sysconfdir)/$(PACKAGE)\" -DDATADIR=\"$(pkgdatadir)\" -I/usr/include/postgresql/
