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
#include "chipdef.h"
#include "debug.h"
#include "data_loading.h"
#include "unipro.h"
#include "greybus.h"
#include "utils.h"

/**
 * @brief Synchronously read from our local mailbox.
 * @return 0 on success, <0 on internal error, >0 on UniPro error
 */
int read_mailbox(uint32_t *val) {
    int rc;
    uint32_t mbox = ARA_MAIL_RESET, irq_status;

    if (!val) {
        return -EINVAL;
    }

    /**
     * The following loop does not have a time-out built in, and can fail to
     * terminate if it never sees the mailbox interrupt pend.  This is because
     * mailbox reading and writing are synchronous barrier operations: the code
     * is meant to arrive to the point of reading/writing the mailbox and wait
     * for a notification from the SVC (supervisory controller).
     */
    do {
        rc = chip_unipro_attr_read(ARA_INTERRUPTSTATUS, &irq_status, 0,
                                   ATTR_LOCAL);
    } while (!rc && !(irq_status & ARA_INTERRUPTSTATUS_MAILBOX));
    if (rc) {
        return rc;
    }

    rc = chip_unipro_attr_read(ARA_MAILBOX, &mbox, 0, ATTR_LOCAL);
    if (rc) {
        return rc;
    }

    if (mbox >= UINT16_MAX) {
        dbgprint("Mailbox protocol only supports 16-bit values\n");
        return -EINVAL;
    }

    *val = mbox;

    return 0;
}

/**
 * @brief Acknowledge that we've read our local mailbox, clearing it.
 * @return 0 on success, <0 on internal error, >0 on UniPro error
 */
int ack_mailbox(uint16_t val) {
    return chip_unipro_attr_write(ARA_MBOX_ACK_ATTR, val, 0, ATTR_LOCAL);
}

/**
 * @brief Synchronously write to the peer mailbox, polling for it to be cleared
 * once we've written it.
 * @return 0 on success, <0 on internal error, >0 on UniPro error
 */
int write_mailbox(uint32_t val) {
    int rc;
    uint32_t irq_status = 0;

    rc = chip_unipro_attr_write(ARA_MAILBOX, val, 0, ATTR_PEER);
    if (rc) {
        return rc;
    }
    /**
     * Poll the interrupt-assert line on the switch until we know the SVC has
     * picked up our mail.  This is a synchronous barrier operation, so no
     * timeout has been included.
     */
    do {
        rc = chip_unipro_attr_read(ARA_INTERRUPTSTATUS, &irq_status, 0,
                                   ATTR_PEER);
    } while (!rc && (irq_status & ARA_INTERRUPTSTATUS_MAILBOX));

    return rc;
}

/**
 * Common code for advertising readiness to boot firmware to the switch
 */
int advertise_ready(void) {
    int rc;

    /**
     * This should be the first time need to talk to the peer,
     * so need to wait for link up
     */
    chip_wait_for_link_up();

    chip_reset_before_ready();

    /**
     * Write that we're a ready non-AP module to the switch's mailbox
     * attribute.
     */
    rc = write_mailbox(ARA_MAIL_READY_OTHER);
    if (rc) {
        return rc;
    }

    dbgprint("Module ready advertised.\n");
    return 0;
}
