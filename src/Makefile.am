#ochidir=/usr/local/bin
bin_PROGRAMS=ochi
ochi_SOURCES=main.c imageio.c img_process.c sid_process.c ocrlib/network.c ocrlib/sample.c bar_process.c filesystem.c localization.c ans_process.c daemon.c
ochi_LDFLAGS=-DNO_CONSOLE_IO
ochi_CFLAGS=-DCONFDIR=\"$(sysconfdir)/$(PACKAGE)\" -DDATADIR=\"$(pkgdatadir)\"
