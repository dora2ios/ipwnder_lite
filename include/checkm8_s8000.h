#ifndef CHECKM8_S8000_H
#define CHECKM8_S8000_H
#include <iousb.h>

// 0x7ff's eclipsa
// original: https://github.com/0x7ff/eclipsa
// s8000 && s8003
#define SYNOPSYS_ROUTINE_ADDR        (0x100006718)
#define IO_BUFFER_ADDR               (0x18010D500)
#define VROM_PAGE_TABLE_ADDR         (0x1800C8400)
#define ARCH_TASK_TRAMP_ADDR         (0x10000D998)

// https://github.com/hack-different/ipwndfu/blob/main/docs/sigcheck_patches.txt
#define PATCH_ADDR                   (0x10000812C) // Universal sigcheck patch

#define gDemotionRegister            (0x2102BC000)
#define gUSBSerialNumber             (0x180087958)
#define gUSBSRNMStringDescriptor     (0x1800807DA)
#define gUSBDescriptors              (0x1800877E0)
#define usb_create_string_descriptor (0x10000E354)

int checkm8_s8000(io_client_t client);

#endif
