#include <dirent.h>
#include <sys/stat.h>

#include <io/iousb.h>
#include <common/common.h>

#ifdef Apple_A6
#include <partialzip/partial.h>
#include <exploit/checkm8/s5l8950x.h>
#endif
#include <exploit/limera1n.h>
#include <exploit/checkm8/s5l8960x.h>
#include <exploit/checkm8/s8000.h>
#include <exploit/checkm8/t8010.h>

io_client_t client;
ipwnder_payload_t payload;

#ifdef Apple_A6
#ifndef IPHONEOS_ARM
char *outdir = "image3/";
const char *n41_ibss = "image3/ibss.n41";
const char *n42_ibss = "image3/ibss.n42";
const char *n48_ibss = "image3/ibss.n48";
const char *n49_ibss = "image3/ibss.n49";
const char *p101_ibss = "image3/ibss.p101";
const char *p102_ibss = "image3/ibss.p102";
const char *p103_ibss = "image3/ibss.p103";
#else
char *outdir = "/tmp/image3/";
const char *n41_ibss = "/tmp/image3/ibss.n41";
const char *n42_ibss = "/tmp/image3/ibss.n42";
const char *n48_ibss = "/tmp/image3/ibss.n48";
const char *n49_ibss = "/tmp/image3/ibss.n49";
const char *p101_ibss = "/tmp/image3/ibss.p101";
const char *p102_ibss = "/tmp/image3/ibss.p102";
const char *p103_ibss = "/tmp/image3/ibss.p103";
#endif

static int dl_file(const char* url, const char* path, const char* realpath){
    int r;
    LOG("[%s] Downloading image: %s ...", __FUNCTION__, realpath);
    r = partialzip_download_file(url, path, realpath);
    if(r != 0){
        ERROR("[%s] ERROR: Failed to get image!", __FUNCTION__);
        return -1;
    }
    return 0;
}
#endif

static void usage(char** argv)
{
    printf("Usage: %s [-p/-e/-d]\n", argv[0]);
    printf("\n");
}

int main(int argc, char** argv)
{
    int r=0;
    
    bool demotionFlag = false;
    bool eclipsaStyle = false;
    
    if(argc == 1) {
        usage(argv);
        return -1;
    }
    
    if(!strcmp(argv[1], "-d")) {
        demotionFlag = true;
    } else if(!strcmp(argv[1], "-p")) {
        demotionFlag = false;
    } else if(!strcmp(argv[1], "-e")) {
        demotionFlag = false;
        eclipsaStyle = true;
    } else {
        usage(argv);
        return -1;
    }
    
    LOG("[%s] Waiting for device in DFU mode...", __FUNCTION__);
    
    while(get_device(DEVICE_DFU, true) != 0) {
        sleep(1);
    }
    
    LOG("[%s] CONNECTED", __FUNCTION__);
    
    if(client->hasSerialStr == false) {
        read_serial_number(client); // For iOS 10 and lower
    }

    if(client->hasSerialStr != true) {
        ERROR("[%s] ERROR: Serial number was not found!", __FUNCTION__);
        return -1;
    }
    
    LOG("[%s] CPID: 0x%02x, BDID: 0x%02x, STRG: [%s]", __FUNCTION__, client->devinfo.cpid, client->devinfo.bdid, client->devinfo.srtg != NULL ? client->devinfo.srtg : "none");
    
    if(!client->devinfo.srtg) {
        if(client->devinfo.cpid != 0x8950) {
            ERROR("[%s] ERROR: Not DFU mode!", __FUNCTION__);
            return -1;
        }
        ERROR("[%s] ERROR: Not DFU mode! Already pwned iBSS mode?", __FUNCTION__);
        return -1;
    }
    
    if((client->devinfo.cpfm & 0x1) == 0) {
        ERROR("[%s] ERROR: Already have a development device flag!", __FUNCTION__);
        return 0;
    }
    
    if(client->devinfo.hasPwnd == true) {
        if(!strcmp(client->devinfo.pwnstr, "demoted")) {
            ERROR("[%s] ERROR: Already demoted!", __FUNCTION__);
            return 0;
        }
        ERROR("[%s] ERROR: Already pwned!", __FUNCTION__);
        return 0;
    }
    
    client->isDemotion = demotionFlag;
    
#ifdef Apple_A6
    if(client->isDemotion == false && (client->devinfo.cpid == 0x8950 || client->devinfo.cpid == 0x8955)) {
        const char* url;
        const char* path;
        
        memset(&payload, '\0', sizeof(ipwnder_payload_t));
        
        if(client->devinfo.cpid == 0x8950) {
            if(client->devinfo.bdid == 0x00) {
                // iPhone5,1
                payload.path = n41_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/5ada2e6df3f933abde79738967960a27371ce9f3.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.n41ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x02) {
                // iPhone5,2
                payload.path = n42_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/a05a5e2e6c81df2c0412c51462919860b8594f75.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.n42ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x0a || client->devinfo.bdid == 0x0b) {
                // iPhone5,3
                payload.path = n48_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/71ece9ff3c211541c5f2acbc6be7b731d342e869.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.n48ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x0e) {
                // iPhone5,4
                payload.path = n49_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/455309571ffb5ca30c977897d75db77e440728c1.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.n49ap.RELEASE.dfu";
            } else {
                ERROR("[%s] ERROR: Unknown device!", __FUNCTION__);
                return -1;
            }
                
        } else if(client->devinfo.cpid == 0x8955) {
            if(client->devinfo.bdid == 0x00) {
                // iPad3,4
                payload.path = p101_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/c0cbed078b561911572a09eba30ea2561cdbefe6.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.p101ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x02) {
                // iPad3,5
                payload.path = p102_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/3e0efaf1480c74195e4840509c5806cc83c99de2.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.p102ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x04) {
                // iPad3,6
                payload.path = p103_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/238641fd4b8ca2153c9c696328aeeedabede6174.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.p103ap.RELEASE.dfu";
            } else {
                ERROR("[%s] ERROR: Unknown device!", __FUNCTION__);
                return -1;
            }
        } else {
            ERROR("[%s] ERROR: Unknown device!", __FUNCTION__);
            return -1;
        }
        
        DIR *d = opendir(outdir);
        if (!d) {
            LOG("[%s] Making directory: %s", __FUNCTION__, outdir);
            r = mkdir(outdir, 0755);
            if(r != 0){
                ERROR("[%s] ERROR: Failed to make dir: %s!", __FUNCTION__, outdir);
                return -1;
            }
        }
        
        FILE *fd = fopen(payload.path, "r");
        if(!fd){
            if(dl_file(url, path, payload.path) != 0) return -1;
            fd = fopen(payload.path, "r");
            if(!fd) {
                ERROR("[%s] ERROR: Failed to open file!", __FUNCTION__);
                return -1;
            }
        }
        
        fseek(fd, 0, SEEK_END);
        payload.len = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        
        payload.payload = malloc(payload.len);
        if (!payload.payload) {
            ERROR("[%s] ERROR: Failed to allocating file buffer!", __FUNCTION__);
            fclose(fd);
            return -1;
        }
        
        fread(payload.payload, payload.len, 1, fd);
        fclose(fd);
        
    }
#endif
    
    if(client->devinfo.cpid == 0x8960){
        r = checkm8_s5l8960x(client);
    } else if(client->devinfo.cpid == 0x8010){
        r = checkm8_t8010(client);
    } else if(client->devinfo.cpid == 0x8000 ||
              client->devinfo.cpid == 0x8003 ||
              client->devinfo.cpid == 0x7000){
        r = checkm8_s8000(client, eclipsaStyle);
#ifdef Apple_A6
    } else if(client->devinfo.cpid == 0x8950 ||
              client->devinfo.cpid == 0x8955){
        r = checkm8_s5l8950x(client, payload);
#endif
    } else if(client->devinfo.cpid == 0x8920 ||
              client->devinfo.cpid == 0x8922 ||
              client->devinfo.cpid == 0x8930){
        if(client->isDemotion == true) {
            ERROR("[%s] ERROR: demotion is only compatible with checkm8 exploit!", __FUNCTION__);
            return -1;
        }
        r = limera1n(client);
    }
    
    return r;
}
