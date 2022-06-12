#include <dirent.h>
#include <sys/stat.h>
#include <getopt.h>

#include <io/iousb.h>
#include <common/common.h>
#include <common/payload.h>

#include <exploit/limera1n.h>
#include <exploit/checkm8_arm64.h>
#include <exploit/s5l8950x.h>

io_client_t client;
checkra1n_payload_t payload;
extern bool debug_enabled;

static void list(void)
{
    printf("Devices list:\n");
    printf("\t\x1b[36ms5l8920x\x1b[39m - \x1b[35miPhone 3GS\x1b[39m\n");
    printf("\t\x1b[36ms5l8922x\x1b[39m - \x1b[35miPod touch 3G\x1b[39m\n");
    printf("\t\x1b[36ms5l8930x\x1b[39m - \x1b[35mApple A4\x1b[39m\n");
    printf("\t\x1b[36ms5l8950x\x1b[39m - \x1b[35mApple A6\x1b[39m\n");
    //printf("\t\x1b[36ms5l8955x\x1b[39m - \x1b[35mApple A6X\x1b[39m\n");
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
                DEBUGLOG("enabled: debug log");
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
    
    LOG("Waiting for device in DFU mode...");
    while(get_device(DEVICE_DFU, true) != 0) {
        sleep(1);
    }
    
    LOG("CONNECTED");
    
    if(client->hasSerialStr == false) {
        read_serial_number(client); // For iOS 10 and lower
    }

    if(client->hasSerialStr != true) {
        ERROR("Serial number was not found!");
        return -1;
    }
    
    LOG("CPID: 0x%02x, BDID: 0x%02x, STRG: [%s]", client->devinfo.cpid, client->devinfo.bdid, client->devinfo.srtg != NULL ? client->devinfo.srtg : "none");
    
    if(!client->devinfo.srtg) {
        if(client->devinfo.cpid != 0x8950) {
            ERROR("Not DFU mode!");
            return -1;
        }
        ERROR("Not DFU mode! Already pwned iBSS mode?");
        return -1;
    }
    
    if((client->devinfo.cpfm & 0x1) == 0) {
        ERROR("Already have a development device flag!");
        return 0;
    }
    
    if(client->devinfo.hasPwnd == true) {
        if(!strcmp(client->devinfo.pwnstr, "demoted")) {
            ERROR("Already demoted!");
            return 0;
        }
        ERROR("Already pwned!");
        return 0;
    }
    
    client->isDemotion = demotionFlag;
    
    int flags = client->devinfo.checkm8_flag; // because it will be lost
    
    if(flags & (CHECKM8_A7|CHECKM8_A8_A9|CHECKM8_A9X_A11)) {
        if(payload_gen(client, &payload, eclipsaStyle) != 0)
            return -1;
    }
    
    if(eclipsaStyle && (flags & CHECKM8_A8_A9)) {
        flags |= USE_HEAP_SPRAY_A8_A9;
    }
    
    if(client->devinfo.cpid == 0x8920 ||
       client->devinfo.cpid == 0x8922 ||
       client->devinfo.cpid == 0x8930) {
        if(client->isDemotion == true) {
            ERROR("demotion is only compatible with checkm8 exploit");
            return -1;
        }
        // limera1n devices
        ret = limera1n(client);
    } else if(flags & CHECKM8_A6) {
        ret = checkm8_s5l8950x(client);
    } else {
        ret = checkm8_arm64(client, payload, flags);
    }
    
    
    return ret;
}
