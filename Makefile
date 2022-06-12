#IGCC		= xcrun -sdk iphoneos gcc
MGCC		= xcrun -sdk macosx gcc
MGCC_FLAGS	= -I./include -I./ra1npoc/src/include -framework IOKit -framework CoreFoundation -Os -Wall $(CFLAGS) -DDEBUG
#IGCC_FLAGS	= $(MGCC_FLAGS) -DIPHONEOS_ARM
#AARCH32	= -arch armv7
#AARCH64	= -arch arm64

SOURCE		= \
		main.c \
		src/common/payload.c \
		src/exploit/limera1n.c \
		src/exploit/s5l8950x.c \
		ra1npoc/src/io/iousb.c \
		ra1npoc/src/common/common.c \
		ra1npoc/src/exploit/checkm8_arm64.c

MOBJECT		= ipwnder_macosx
#IOBJECT	= ra1npoc_ios
#IOBJECT32	= ra1npoc_ios32
#IOBJECT64	= ra1npoc_ios64
#ICODESIGN	= ldid -S

#PayloadDir	= payload/

.PHONY: all static clean

all:
	$(MGCC) $(MGCC_FLAGS) $(SOURCE) -o $(MOBJECT)

static:
	$(MGCC) $(MGCC_FLAGS) $(SOURCE) $(STATIC32) -o $(MOBJECT)

clean:
	-$(RM) $(MOBJECT)
