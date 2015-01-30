.include <bsd.compiler.mk>

PROG = sysinfo
NO_MAN =

SRCS =  sysinfo.c

CSTD = c11
CFLAGS  = -pipe -pedantic -Werror -march=native
LDFLAGS = -lgeom -ldevstat

.if make(debug)
CFLAGS += -O0 -g
.else
CFLAGS += -O3 -fomit-frame-pointer -DNDEBUG
LDFLAGS += -s
.endif

.if ${COMPILER_TYPE} == "clang"
CFLAGS += -Weverything -fcolor-diagnostics
.else # assume gcc
CFLAGS += -Wall -Wextra
.endif


debug: all


.include <bsd.prog.mk>
