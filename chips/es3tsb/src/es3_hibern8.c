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
#include <stddef.h>
#include <string.h>
#include <errno.h>
#include "bootrom.h"
#include "chipapi.h"
#include "unipro.h"
#include "tsb_unipro.h"
#include "debug.h"
#include "utils.h"
#include "tsb_scm.h"

int chip_enter_hibern8_client(void) {
    uint32_t tx_reset_offset, rx_reset_offset;
    uint32_t cportid;
    int rc;
    uint32_t tempval, unipro_rc;

    dbgprint("Wait for hibernate\n");
    do {
        rc = chip_unipro_attr_read(TSB_HIBERNATE_ENTER_IND, &tempval, 0,
                                   ATTR_LOCAL, &unipro_rc);
    } while (!rc && !unipro_rc && tempval == 0);
    if (DISJOINT_OR(rc, unipro_rc)) {
        return DISJOINT_OR(rc, unipro_rc);
    }

    for (cportid = 0; cportid < CPORT_MAX; cportid++) {
        tx_reset_offset = TX_SW_RESET_00 + (cportid << 2);
        rx_reset_offset = RX_SW_RESET_00 + (cportid << 2);

        putreg32(CPORT_SW_RESET_BITS,
                 (volatile unsigned int*)(AIO_UNIPRO_BASE + tx_reset_offset));
        putreg32(CPORT_SW_RESET_BITS,
                 (volatile unsigned int*)(AIO_UNIPRO_BASE + rx_reset_offset));
    }
    dbgprint("hibernate entered\n");

    while (0 != getreg32((volatile unsigned int*)UNIPRO_CLK_EN));
    tsb_clk_disable(TSB_CLK_UNIPROSYS);
    return 0;
}

int chip_exit_hibern8_client(void) {
    int rc;
    uint32_t tempval, unipro_rc;

    tsb_clk_enable(TSB_CLK_UNIPROSYS);
    dbgprint("Try to exit hibernate\n");
    tempval = 1;
    rc = chip_unipro_attr_write(TSB_HIBERNATE_EXIT_REQ, tempval, 0,
                                ATTR_LOCAL, &unipro_rc);
    if (DISJOINT_OR(rc, unipro_rc)) {
        return DISJOINT_OR(rc, unipro_rc);
    }

    do {
        rc = chip_unipro_attr_read(TSB_HIBERNATE_EXIT_IND, &tempval, 0,
                                   ATTR_LOCAL, &unipro_rc);
    } while (!rc && !unipro_rc && tempval == 0);
    if (DISJOINT_OR(rc, unipro_rc)) {
        return DISJOINT_OR(rc, unipro_rc);
    }
    dbgprint("hibernate exit\n");
    return 0;
}


int chip_enter_hibern8_server(void) {
    uint32_t tx_reset_offset, rx_reset_offset;
    uint32_t cportid;
    int rc;
    uint32_t tempval, unipro_rc;

    dbgprint("entering hibernate\n");
    for (cportid = 0; cportid < CPORT_MAX; cportid++) {
        tx_reset_offset = TX_SW_RESET_00 + (cportid << 2);
        rx_reset_offset = RX_SW_RESET_00 + (cportid << 2);

        putreg32(CPORT_SW_RESET_BITS,
                 (volatile unsigned int*)(AIO_UNIPRO_BASE + tx_reset_offset));
        putreg32(CPORT_SW_RESET_BITS,
                 (volatile unsigned int*)(AIO_UNIPRO_BASE + rx_reset_offset));
    }

    tempval = 1;
    rc = chip_unipro_attr_write(TSB_HIBERNATE_ENTER_REQ, tempval, 0,
                                ATTR_LOCAL, &unipro_rc);
    if (DISJOINT_OR(rc, unipro_rc)) {
        return DISJOINT_OR(rc, unipro_rc);
    }

    dbgprint("wait for hibernate\n");
    do {
        rc = chip_unipro_attr_read(TSB_HIBERNATE_ENTER_IND, &tempval, 0,
                                   ATTR_LOCAL, &unipro_rc);
    } while (!rc && !unipro_rc && tempval == 0);
    if (DISJOINT_OR(rc, unipro_rc)) {
        return DISJOINT_OR(rc, unipro_rc);
    }

    dbgprint("hibernate entered\n");

    dbgprint("wait for hibernate exit\n");
    do {
        rc = chip_unipro_attr_read(TSB_HIBERNATE_EXIT_IND, &tempval, 0,
                                   ATTR_LOCAL, &unipro_rc);
    } while (!rc && !unipro_rc && tempval == 0);
    if (DISJOINT_OR(rc, unipro_rc)) {
        return DISJOINT_OR(rc, unipro_rc);
    }
    dbgprint("hibernate exit\n");
    return 0;
}

