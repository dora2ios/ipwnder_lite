#IGCC		= xcrun -sdk iphoneos gcc
MGCC		= xcrun -sdk macosx gcc
MGCC_FLAGS	= -I./include -I./ra1npoc/src/include -framework IOKit -framework CoreFoundation -Os -Wall $(CFLAGS) -DDEBUG
MGCC_FLAGS32	= -DApple_A6
#IGCC_FLAGS	= $(MGCC_FLAGS) -DIPHONEOS_ARM
#AARCH32	= -arch armv7
#AARCH64	= -arch arm64

SOURCE		= \
		main.c \
		src/common/payload.c \
		src/exploit/limera1n.c \
		ra1npoc/src/io/iousb.c \
		ra1npoc/src/common/common.c \
		ra1npoc/src/exploit/checkm8_arm64.c


SOURCE32	= \
		src/common/usb_0xa1_2.c \
		src/exploit/s5l8950x.c \
		lib/partialzip/partial.c

# only x86_64?
STATIC32	= \
		lib/static/libcurl.a \
		lib/static/libz.a \
		-framework Security \
		-framework LDAP

DYNAMIC		= \
		-lcurl\
		-lz

MOBJECT		= ipwnder_macosx
#IOBJECT	= ra1npoc_ios
#IOBJECT32	= ra1npoc_ios32
#IOBJECT64	= ra1npoc_ios64
#ICODESIGN	= ldid -S

#PayloadDir	= payload/

.PHONY: all noA6 static clean

all:
	$(MGCC) $(MGCC_FLAGS) $(MGCC_FLAGS32) $(SOURCE) $(SOURCE32) $(DYNAMIC) -o $(MOBJECT)

noA6:
	$(MGCC) $(MGCC_FLAGS) $(SOURCE) -o $(MOBJECT)

static:
	$(MGCC) $(MGCC_FLAGS) $(MGCC_FLAGS32) $(SOURCE) $(SOURCE32) $(STATIC32) -o $(MOBJECT)

clean:
	-$(RM) $(MOBJECT)
