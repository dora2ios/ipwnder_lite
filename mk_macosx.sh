#/bin/sh

clang main.c iousb.c checkm8_s5l8950x.c checkm8_s5l8960x.c checkm8_t8010.c common.c partialzip/partial.c static/*.a -o ipwnder_macosx -I./include -framework IOKit -framework CoreFoundation -framework Security -framework LDAP -Os -DDEBUG
strip ipwnder_macosx
