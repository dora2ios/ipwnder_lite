#/bin/sh

LIBCURL="dynamic/iphoneos-arm64/libcurl.dylib"
#LIBCURL="-lcurl"

SYSROOT="/Applications/Xcode.app/Contents/Developer/Platforms/iPhoneOS.platform/Developer/SDKs/iPhoneOS9.2.sdk"
FRAMEWORKS="-framework IOKit -framework CoreFoundation"
FLAGS="-Os -DDEBUG -DIPHONEOS_ARM -DUSE_A6EXP"

clang -isysroot $SYSROOT main.c iousb.c checkm8_s5l8950x.c checkm8_s5l8960x.c checkm8_t8010.c common.c partialzip/partial.c -I./dynamic/include -I./include -lz $LIBCURL $FRAMEWORKS $FLAGS -arch arm64 -o ipwnder_iphoneos64
strip ipwnder_iphoneos64
ldid -S ipwnder_iphoneos64
