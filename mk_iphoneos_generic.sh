#/bin/sh
rm ipwnder_lite
lipo -create -output ipwnder_lite -arch armv7 ipwnder_iphoneos -arch arm64 ipwnder_iphoneos64
codesign -f -s - -i ipwnder_lite ipwnder_lite
rm ipwnder_iphoneos*
