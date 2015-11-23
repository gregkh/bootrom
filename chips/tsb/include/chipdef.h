/**
 * Copyright (c) 2015 Google Inc.
 * All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions are met:
 * 1. Redistributions of source code must retain the above copyright notice,
 * this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright notice,
 * this list of conditions and the following disclaimer in the documentation
 * and/or other materials provided with the distribution.
 * 3. Neither the name of the copyright holder nor the names of its
 * contributors may be used to endorse or promote products derived from this
 * software without specific prior written permission.
 *
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
 * AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO,
 * THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR
 * PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR
 * CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL,
 * EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO,
 * PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
 * OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
 * WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR
 * OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF
 * ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */

#ifndef __ARCH_ARM_TSB_CHIPDEF_H
#define __ARCH_ARM_TSB_CHIPDEF_H

/* chip revision:
    higher 4bits are production chip revision, so it is 0 for all ESx.
    lower 4bits are minor revisions, here is stands for the x in ESx
*/
#define CHIP_REVISION_ES1 0x01
#define CHIP_REVISION_ES2 0x02
#define CHIP_REVISION_ES3 0x03

/* AP/GP Bridge peripherals memory map */

/* Include chip-specific elements of the memory map */
#if CONFIG_CHIP_REVISION == CHIP_REVISION_ES2
#include "es2_chipdef.h"
#elif CONFIG_CHIP_REVISION == CHIP_REVISION_ES3
#include "es3_chipdef.h"
#endif

#define SYSCTL_BASE     0x40000000
#define SYSCTL_SIZE     0x1000

#define I2C_BASE        0x40001000
#define I2C_SIZE        0x100

#define TMR_BASE        0x40002000
#define TMR_SIZE        0x800

#define GPIO_BASE       0x40003000
#define GPIO_SIZE       0x40

#define PWMOD_BASE      0x40004000
#define PWMOD_SIZE      0x400

#define UART_BASE       0x40005000
#define UART_SIZE       0x100

#define MBOX_BASE       0x40006000
#define MBOX_SIZE       0x1000

#define I2SLP_SC_BASE   0x40007000
#define I2SLP_SC_SIZE   0x100

#define I2SLP_SO_BASE   0x40008000
#define I2SLP_SO_SIZE   0x200

#define I2SLP_SI_BASE   0x40009000
#define I2SLP_SI_SIZE   0x200

#define CDSI0_BASE      0x40010000
#define CDSI0_SIZE      0x2000

#define CDSI1_BASE      0x40012000
#define CDSI1_SIZE      0x2000

#define GDMAC_BASE      0x40014000
#define GDMAC_SIZE      0x1000

#define GDMACN_BASE     0x40015000
#define GDMACN_SIZE     0x1000

#define UHSSD_BASE      0x40019000
#define UHSSD_SIZE      0x100

#define AIO_UNIPRO_BASE 0x40020000
#define AIO_UNIPRO_SIZE 0x10000

#define HSIC_BASE       0x40040000
#define HSIC_SIZE       0x1000

#define CM3UP_BASE      0xE000E000
#define CM3UP_SIZE      0x1000

#define ISAA_BASE       0x40084000
#define ISAA_SIZE       0x1000

#define PMU_BASE        0x40090000
#define PMU_SIZE        64

/* PMU registers */
#define ISO_EN          (PMU_BASE + 0)
#define ISO_FOR_IO_EN   (PMU_BASE + 0x04)
#define RETSRAMRET      (PMU_BASE + 0x10)
#define RETSRAMCENCONT  (PMU_BASE + 0x14)
#define RETSRAMCLKCONT  (PMU_BASE + 0x18)
#define HB8CLK_EN       (PMU_BASE + 0x1C)
#define WAKEUPSRC       (PMU_BASE + 0x24)
#define BOOTRET_O       (PMU_BASE + 0x2C)
#define RETFFSAVE       (PMU_BASE + 0x40)
#define RETFFSTR        (PMU_BASE + 0x44)

#define UNIPRO_CLK_EN               (SYSCTL_BASE + 0x0E10)
#define SOFTRESETRELEASE1           (SYSCTL_BASE + 0x0104)
    #define SRSTRELEASE_UNIPRO_SYSRESET_N         (1 << 2)

/* ISAA registers */
#define _DISABLE_IMS_ACCESS         (ISAA_BASE + 0x00000400)
#define _DISABLE_CMS_ACCESS         (ISAA_BASE + 0x00000404)
#define _JTAG_DISABLE               (ISAA_BASE + 0x0000040c)
#define DISABLE_JTAG_IMS_CMS_ACCESS (0x00000001)

#include "chipcfg.h"

#endif /* __ARCH_ARM_TSB_CHIPDEF_H */
