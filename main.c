#include <dirent.h>
#include <sys/stat.h>
#include <getopt.h>

#include <io/iousb.h>
#include <common/common.h>
#include <common/payload.h>

#include <soc/limera1n.h>
#include <soc/ipwndfu_a8_a9.h>
#include <soc/s5l8960x.h>
#include <soc/t7000_s8000.h>
#include <soc/t8010_t8015.h>

#ifdef Apple_A6
#include <partialzip/partial.h>
#include <soc/s5l8950x.h>

ipwnder_payload_t data;
#endif /* Apple_A6 */

io_client_t client;
checkra1n_payload_t payload;
extern bool debug_enabled;

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
#else /* !IPHONEOS_ARM */
char *outdir = "/tmp/image3/";
const char *n41_ibss = "/tmp/image3/ibss.n41";
const char *n42_ibss = "/tmp/image3/ibss.n42";
const char *n48_ibss = "/tmp/image3/ibss.n48";
const char *n49_ibss = "/tmp/image3/ibss.n49";
const char *p101_ibss = "/tmp/image3/ibss.p101";
const char *p102_ibss = "/tmp/image3/ibss.p102";
const char *p103_ibss = "/tmp/image3/ibss.p103";
#endif /* IPHONEOS_ARM */

static int dl_file(const char* url, const char* path, const char* realpath)
{
    int r;
    LOG("[%s] Downloading image: %s ...", __FUNCTION__, realpath);
    r = partialzip_download_file(url, path, realpath);
    if(r != 0){
        ERROR("[%s] ERROR: Failed to get image!", __FUNCTION__);
        return -1;
    }
    return 0;
}
#endif /* Apple_A6 */

static void list(void)
{
    printf("Devices list:\n");
    printf("\t\x1b[36ms5l8920x\x1b[39m - \x1b[35miPhone 3GS\x1b[39m\n");
    printf("\t\x1b[36ms5l8922x\x1b[39m - \x1b[35miPod touch 3G\x1b[39m\n");
    printf("\t\x1b[36ms5l8930x\x1b[39m - \x1b[35mApple A4\x1b[39m\n");
#ifdef Apple_A6
    printf("\t\x1b[36ms5l8950x\x1b[39m - \x1b[35mApple A6\x1b[39m\n");
    printf("\t\x1b[36ms5l8955x\x1b[39m - \x1b[35mApple A6X\x1b[39m\n");
#endif /* Apple_A6 */
    printf("\t\x1b[36ms5l8960x\x1b[39m - \x1b[35mApple A7\x1b[39m\n");
    //printf("\t\x1b[36mt7000   \x1b[39m - \x1b[35mApple A8\x1b[39m\n");
    //printf("\t\x1b[36mt7001   \x1b[39m - \x1b[35mApple A8X\x1b[39m\n");
    printf("\t\x1b[36ms8000   \x1b[39m - \x1b[35mApple A9 (Samsung)\x1b[39m\n");
    printf("\t\x1b[36ms8003   \x1b[39m - \x1b[35mApple A9 (TSMC)\x1b[39m\n");
    //printf("\t\x1b[36ms8001   \x1b[39m - \x1b[35mApple A9X\x1b[39m\n");
    printf("\t\x1b[36mt8010   \x1b[39m - \x1b[35mApple A10 Fusion\x1b[39m\n");
    printf("\t\x1b[36mt8011   \x1b[39m - \x1b[35mApple A10X Fusion\x1b[39m\n");
    //printf("\t\x1b[36mt8012   \x1b[39m - \x1b[35mApple T2\x1b[39m\n");
    printf("\t\x1b[36mt8015   \x1b[39m - \x1b[35mApple A11 Bionic\x1b[39m\n");
}

static void usage(char** argv)
{
    printf("Usage: %s [option]\n", argv[0]);
    printf("  -h, --help\t\t\t\x1b[36mshow usage\x1b[39m\n");
    printf("  -l, --list\t\t\t\x1b[36mshow list of supported devices\x1b[39m\n");
    printf("  -c, --cleandfu\t\t\x1b[36muse cleandfu [BETA]\x1b[39m\n");
    printf("  -d, --debug\t\t\t\x1b[36menable debug log\x1b[39m\n");
    printf("  -j, --demote\t\t\t\x1b[36menable jtag/swd\x1b[39m\n");
    printf("  -e, --eclipsa\t\t\t\x1b[36muse eclipsa/checkra1n style\x1b[39m\n");
    printf("\n");
}

int main(int argc, char** argv)
{
    int ret = 0;
    
    memset(&payload, '\0', sizeof(checkra1n_payload_t));
    
    bool useRecovery = false;
    bool demotionFlag = false;
    bool eclipsaStyle = false;
    
    int opt = 0;
    static struct option longopts[] = {
        { "help",       no_argument,    NULL, 'h' },
        { "list",       no_argument,    NULL, 'l' },
        { "cleandfu",   no_argument,    NULL, 'c' },
        { "debug",      no_argument,    NULL, 'd' },
        { "demote",     no_argument,    NULL, 'j' },
        { "eclipsa",    no_argument,    NULL, 'e' },
        { NULL, 0, NULL, 0 }
    };
    
    while ((opt = getopt_long(argc, argv, "hlcdje", longopts, NULL)) > 0) {
        switch (opt) {
            case 'h':
                usage(argv);
                return 0;
                
            case 'l':
                list();
                return 0;
                
            case 'd':
                debug_enabled = true;
                DEBUGLOG("[%s] enabled: debug log", __FUNCTION__);
                break;
                
            case 'c':
                useRecovery = true;
                break;
                
            case 'j':
                demotionFlag = true;
                break;
                
            case 'e':
                eclipsaStyle = true;
                break;
                
            default:
                usage(argv);
                return -1;
        }
    }
    
    if(useRecovery) {
        if(enter_dfu_via_recovery(client) != 0) {
            return -1;
        }
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
        
        memset(&data, '\0', sizeof(ipwnder_payload_t));
        
        if(client->devinfo.cpid == 0x8950) {
            if(client->devinfo.bdid == 0x00) {
                // iPhone5,1
                data.path = n41_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/5ada2e6df3f933abde79738967960a27371ce9f3.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.n41ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x02) {
                // iPhone5,2
                data.path = n42_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/a05a5e2e6c81df2c0412c51462919860b8594f75.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.n42ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x0a || client->devinfo.bdid == 0x0b) {
                // iPhone5,3
                data.path = n48_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/71ece9ff3c211541c5f2acbc6be7b731d342e869.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.n48ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x0e) {
                // iPhone5,4
                data.path = n49_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/455309571ffb5ca30c977897d75db77e440728c1.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.n49ap.RELEASE.dfu";
            } else {
                ERROR("[%s] ERROR: Unknown device!", __FUNCTION__);
                return -1;
            }
                
        } else if(client->devinfo.cpid == 0x8955) {
            if(client->devinfo.bdid == 0x00) {
                // iPad3,4
                data.path = p101_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/c0cbed078b561911572a09eba30ea2561cdbefe6.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.p101ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x02) {
                // iPad3,5
                data.path = p102_ibss;
                url = "http://appldnld.apple.com/iOS7.1/031-4897.20140627.JCWhk/3e0efaf1480c74195e4840509c5806cc83c99de2.zip";
                path = "AssetData/boot/Firmware/dfu/iBSS.p102ap.RELEASE.dfu";
            } else if(client->devinfo.bdid == 0x04) {
                // iPad3,6
                data.path = p103_ibss;
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
            ret = mkdir(outdir, 0755);
            if(ret != 0){
                ERROR("[%s] ERROR: Failed to make dir: %s!", __FUNCTION__, outdir);
                return -1;
            }
        }
        
        FILE *fd = fopen(data.path, "r");
        if(!fd){
            if(dl_file(url, path, data.path) != 0) return -1;
            fd = fopen(data.path, "r");
            if(!fd) {
                ERROR("[%s] ERROR: Failed to open file!", __FUNCTION__);
                return -1;
            }
        }
        
        fseek(fd, 0, SEEK_END);
        data.len = ftell(fd);
        fseek(fd, 0, SEEK_SET);
        
        data.payload = malloc(data.len);
        if (!data.payload) {
            ERROR("[%s] ERROR: Failed to allocating file buffer!", __FUNCTION__);
            fclose(fd);
            return -1;
        }
        
        fread(data.payload, data.len, 1, fd);
        fclose(fd);
    }
#endif /* Apple_A6 */
    
    int flags = client->devinfo.checkm8_flag; // because it will be lost
    
    if(flags & (CHECKM8_A7|CHECKM8_A8_A9|CHECKM8_A9X_A11)) {
        if(payload_gen(client, &payload, eclipsaStyle) != 0)
            return -1;
    }
    
    if(flags & CHECKM8_A7) {
        ret = checkm8_s5l8960x(client, payload);
    } else if(flags & CHECKM8_A8_A9) {
        if(eclipsaStyle) {
            ret = checkm8_t7000_s8000(client, payload);
        } else {
            ret = ipwndfu_a8_a9(client, payload);
        }
    } else if(flags & CHECKM8_A9X_A11) {
        ret = checkm8_t8010_t8015(client, payload);
#ifdef Apple_A6
    } else if(flags & CHECKM8_A6) {
        ret = checkm8_s5l8950x(client, data);
#endif /* Apple_A6 */
    } else if(client->devinfo.cpid == 0x8920 ||
              client->devinfo.cpid == 0x8922 ||
              client->devinfo.cpid == 0x8930) {
        if(client->isDemotion == true) {
            ERROR("[%s] ERROR: demotion is only compatible with checkm8 exploit", __FUNCTION__);
            return -1;
        }
        // limera1n devices
        ret = limera1n(client);
    }
    
    return ret;
}
