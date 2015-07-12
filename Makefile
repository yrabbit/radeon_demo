PROG=	r600-t
MAN=

SRCS=	main.c

VERSION= -DVERSION=\"`git rev-parse --short HEAD`\"
CFLAGS+= ${VERSION} -I/usr/local/include -I/usr/local/include/libdrm

LDFLAGS+= -ldrm -L/usr/local/lib

LDADD+=

.include <bsd.prog.mk>

