PROG=	r600-t
MAN=

SRCS=	main.c r600-lib.c cp.c 2d-triangle.c

VERSION= -DVERSION=\"`git rev-parse --short HEAD`\"
CFLAGS+= -Wall ${VERSION} -I/usr/local/include -I/usr/local/include/libdrm

LDFLAGS+= -ldrm -ldrm_radeon -L/usr/local/lib

LDADD+=

.include <bsd.prog.mk>

