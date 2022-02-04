#IGCC		= xcrun -sdk iphoneos gcc
MGCC		= xcrun -sdk macosx gcc
MGCC_FLAGS	= -I./include -framework IOKit -framework CoreFoundation -Os -Wall $(CFLAGS) -DDEBUG
MGCC_FLAGS32	= -DUSE_A6EXP
#IGCC_FLAGS	= $(MGCC_FLAGS) -DIPHONEOS_ARM
#AARCH32	= -arch armv7
#AARCH64	= -arch arm64

SOURCE		= \
                main.c \
                iousb.c \
                checkm8_s5l8960x.c \
                checkm8_t8010.c \
                common.c

SOURCE32	= \
                checkm8_s5l8950x.c \
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

.PHONY: all no32bit static clean

all:
	$(MGCC) $(MGCC_FLAGS) $(MGCC_FLAGS32) $(SOURCE) $(SOURCE32) $(DYNAMIC) -o $(MOBJECT)

no32bit:
	$(MGCC) $(MGCC_FLAGS) $(SOURCE) -o $(MOBJECT)

static:
	$(MGCC) $(MGCC_FLAGS) $(MGCC_FLAGS32) $(SOURCE) $(SOURCE32) $(STATIC32) -o $(MOBJECT)

clean:
	-$(RM) $(MOBJECT)
