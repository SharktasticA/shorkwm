CC ?= gcc
AR ?= ar
RANLIB ?= ranlib
STRIP ?= strip

CFLAGS += -I. -Wall -Wextra
#LDFLAGS += -static -lX11
LDFLAGS += -lX11

SRC = src/*.c

shorkwm: $(SRC)
	$(CC) $(CFLAGS) $(SRC) -o shorkwm $(LDFLAGS)
	$(STRIP) shorkwm

PREFIX ?= /usr
BINDIR = $(PREFIX)/bin

install: shorkwm
	install -d $(DESTDIR)$(BINDIR)
	install -m 755 shorkwm $(DESTDIR)$(BINDIR)

uninstall:
	rm -f $(DESTDIR)$(BINDIR)/shorkwm

clean:
	rm -f shorkwm

.PHONY: install uninstall clean
