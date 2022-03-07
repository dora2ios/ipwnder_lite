#IGCC		= xcrun -sdk iphoneos gcc
MGCC		= xcrun -sdk macosx gcc
MGCC_FLAGS	= -I./include -framework IOKit -framework CoreFoundation -Os -Wall $(CFLAGS) -DDEBUG
MGCC_FLAGS32	= -DApple_A6
#IGCC_FLAGS	= $(MGCC_FLAGS) -DIPHONEOS_ARM
#AARCH32	= -arch armv7
#AARCH64	= -arch arm64

SOURCE		= \
		main.c \
		io/iousb.c \
		exploit/checkm8/s5l8960x.c \
		exploit/checkm8/s8000.c \
		exploit/checkm8/t8010.c \
		exploit/limera1n/limera1n.c \
		common/common.c

SOURCE32	= \
		exploit/checkm8/s5l8950x.c \
		partialzip/partial.c

# only x86_64?
STATIC32	= \
		static/libcurl.a \
		static/libz.a \
		-framework Security \
		-framework LDAP

DYNAMIC		= \
		-lcurl\
		-lz

MOBJECT		= ipwnder_macosx
#IOBJECT	= ra1npoc_iphoneos
#IOBJECT32	= ra1npoc_iphoneos32
#IOBJECT64	= ra1npoc_iphoneos64
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
