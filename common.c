#include <iousb.h>

int boot_checkm8_32(io_client_t client, unsigned char* data, size_t sz)
{
    transfer_t result;
    size_t blanksz;
    
    blanksz = 16;
    unsigned char blank[blanksz];
    
    memset(&blank, '\0', blanksz);
    
    LOG_PROGRESS("[%s] reconnecting", __FUNCTION__);
    io_reset(client);
    io_close(client);
    client = NULL;
    usleep(1000);
    get_device_time_stage(&client, 5, DEVICE_DFU, false);
    if(!client) {
        ERROR("[%s] ERROR: Failed to reconnect to device", __FUNCTION__);
        return -1;
    }
    
    result = usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, blank, blanksz);
    DEBUGLOG("[%s] (1/5) %x", __FUNCTION__, result.ret);
    result = usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, NULL, 0);
    DEBUGLOG("[%s] (2/5) %x", __FUNCTION__, result.ret);
    result = usb_ctrl_transfer(client, 0xA1, 3, 0x0000, 0x0000, blank, 6);
    DEBUGLOG("[%s] (3/5) %x", __FUNCTION__, result.ret);
    result = usb_ctrl_transfer(client, 0xA1, 3, 0x0000, 0x0000, blank, 6);
    DEBUGLOG("[%s] (4/5) %x", __FUNCTION__, result.ret);
    
    LOG_PROGRESS("[%s] sending payload", __FUNCTION__);
    
    {
        size_t len = 0;
        size_t size;
        while(len < sz) {
            size = ((sz - len) > 0x800) ? 0x800 : (sz - len);
            result = usb_ctrl_transfer(client, 0x21, 1, 0x0000, 0x0000, (unsigned char*)&data[len], size);
            if(result.wLenDone != size || result.ret != kIOReturnSuccess){
                ERROR("[%s] ERROR: Failed to send payload [%x, %x]", __FUNCTION__, result.ret, (unsigned int)result.wLenDone);
                return -1;
            }
            len += size;
        }
    }
    
    result = usb_ctrl_transfer_with_time(client, 0xA1, 2, 0xFFFF, 0x0000, NULL, 0, 100);
    DEBUGLOG("[%s] (5/5) %x", __FUNCTION__, result.ret);
    
    return 0;
}
