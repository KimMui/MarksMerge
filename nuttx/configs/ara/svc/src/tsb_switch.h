/*
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

/**
 * @author: Perry Hung
 * @author: Jean Pihet
 */

#ifndef  _TSB_SWITCH_H_
#define  _TSB_SWITCH_H_

#include <pthread.h>

#include <nuttx/list.h>

#include "tsb_unipro.h"
#include "unipro.h"

/* Switch internal attributes */
#define SWVER                       (0x0000)
#define SWSTA                       (0x0003)
#define SWINT                       (0x0010)

/* NCP commands */
#define NCP_SETREQ                  (0x00)
#define NCP_SETCNF                  (0x01)
#define NCP_PEERSETREQ              (0x02)
#define NCP_PEERSETCNF              (0x03)
#define NCP_GETREQ                  (0x04)
#define NCP_GETCNF                  (0x05)
#define NCP_PEERGETREQ              (0x06)
#define NCP_PEERGETCNF              (0x07)
#define NCP_LUTSETREQ               (0x10)
#define NCP_LUTSETCNF               (0x11)
#define NCP_LUTGETREQ               (0x12)
#define NCP_LUTGETCNF               (0x13)
#define NCP_SETDEVICEIDMASKREQ      (0x14)
#define NCP_SETDEVICEIDMASKCNF      (0x15)
#define NCP_GETDEVICEIDMASKREQ      (0x16)
#define NCP_GETDEVICEIDMASKCNF      (0x17)
#define NCP_SWITCHATTRSETREQ        (0x20)
#define NCP_SWITCHATTRSETCNF        (0x21)
#define NCP_SWITCHATTRGETREQ        (0x22)
#define NCP_SWITCHATTRGETCNF        (0x23)
#define NCP_SWITCHIDSETREQ          (0x24)
#define NCP_SWITCHIDSETCNF          (0x25)

/* NCP field values */
#define NCP_RESERVED                (0x00)
#define NCP_SELINDEX_NULL           (0x00)

/* Unipro attributes values */
#define PA_CONN_TX_DATA_LANES_NR        (2)
#define PA_CONN_RX_DATA_LANES_NR        (2)
#define PA_ACTIVE_TX_DATA_LANES_NR      (2)
#define PA_ACTIVE_RX_DATA_LANES_NR      (2)
#define MAX_SEGMENT_CONFIG              (280)
#define PA_FASTMODE_RXTX                (0x11)
#define PA_FASTAUTOMODE_RXTX            (0x44)
#define PA_GEAR                         (0x1)
#define TSB_DME_POWERMODEIND_SUCCESS    (2)

/* Number of external ports, excluding the internal switch port */
#define SWITCH_UNIPORT_MAX          (14)
#define SWITCH_PORT_ID              SWITCH_UNIPORT_MAX
/* Number of ports, including the internal switch port */
#define SWITCH_PORT_MAX             (SWITCH_UNIPORT_MAX + 1)
#define INVALID_PORT                (0xFF)

#define SWITCH_DEVICE_ID            (0)

/**
 * @brief Toshiba-specific L2 timer configuraion to apply to the local
 *        end of the link during a power mode change.
 *
 * A particular attribute is only reconfigured if the corresponding
 * flag is set. E.g., `tsb_fc0_protection_timeout` is used to set
 * TSB_DME_FC0PROTECTIONTIMEOUTVAL if and only if TSB_LOCAL_L2_FC0 is
 * set in `flags'.
 *
 * @see switch_configure_link()
 */
struct tsb_local_l2_timer_cfg {
    /* Set TSB_DME_FC0PROTECTIONTIMEOUTVAL */
#   define TSB_LOCALL2F_FC0  (1U << 0)
    /* Set TSB_DME_TC0REPLAYTIMEOUTVAL */
#   define TSB_LOCALL2F_TC0  (1U << 1)
    /* Set TSB_DME_AFC0REQTIMEOUTVAL */
#   define TSB_LOCALL2F_AFC0 (1U << 2)
    /* Set TSB_DME_FC1PROTECTIONTIMEOUTVAL */
#   define TSB_LOCALL2F_FC1  (1U << 3)
    /* Set TSB_DME_TC1REPLAYTIMEOUTVAL */
#   define TSB_LOCALL2F_TC1  (1U << 4)
    /* Set TSB_DME_AFC1REQTIMEOUTVAL */
#   define TSB_LOCALL2F_AFC1 (1U << 5)
    unsigned int tsb_flags;

    uint16_t tsb_fc0_protection_timeout;
    uint16_t tsb_tc0_replay_timeout;
    uint16_t tsb_afc0_req_timeout;
    uint16_t tsb_fc1_protection_timeout;
    uint16_t tsb_tc1_replay_timeout;
    uint16_t tsb_afc1_req_timeout;
};

/**
 * Extra Toshiba extensions for UniPro link configuration.
 */
struct tsb_link_cfg {
    struct tsb_local_l2_timer_cfg tsb_l2tim_cfg;
};

/**
 * Switch structs
 */
struct tsb_switch;

struct tsb_switch_ops {
    int (*init_comm)(struct tsb_switch *);

    int (*set)(struct tsb_switch *,
               uint8_t portid,
               uint16_t attrid,
               uint16_t select_index,
               uint32_t attr_value);
    int (*get)(struct tsb_switch *,
               uint8_t portid,
               uint16_t attrid,
               uint16_t select_index,
               uint32_t *attr_value);
    int (*peer_set)(struct tsb_switch *,
                    uint8_t portid,
                    uint16_t attrid,
                    uint16_t select_index,
                    uint32_t attr_value);
    int (*peer_get)(struct tsb_switch *,
                    uint8_t portid,
                    uint16_t attrid,
                    uint16_t select_index,
                    uint32_t *attr_value);
    int (*port_irq_enable)(struct tsb_switch *sw,
                           uint8_t port_id,
                           bool enable);

    int (*lut_set)(struct tsb_switch *,
                   uint8_t unipro_portid,
                   uint8_t addr,
                   uint8_t dst_portid);
    int (*lut_get)(struct tsb_switch *,
                   uint8_t unipro_portid,
                   uint8_t addr,
                   uint8_t *dst_portid);
    int (*dump_routing_table)(struct tsb_switch*);
    int (*switch_attr_get)(struct tsb_switch *,
                           uint16_t attrid,
                           uint32_t *val);
    int (*switch_attr_set)(struct tsb_switch *,
                           uint16_t attrid,
                           uint32_t val);
    int (*switch_id_set)(struct tsb_switch *,
                         uint8_t cportid,
                         uint8_t peer_cportid,
                         uint8_t dis,
                         uint8_t irt);
    int (*dev_id_mask_get)(struct tsb_switch *,
                           uint8_t unipro_portid,
                           uint8_t *dst);
    int (*dev_id_mask_set)(struct tsb_switch *,
                           uint8_t unipro_portid,
                           uint8_t *mask);
    int (*switch_irq_enable)(struct tsb_switch *sw,
                             bool enable);
    int (*switch_irq_handler)(struct tsb_switch *sw);
};

struct tsb_switch {
    void                    *priv;
    struct tsb_switch_ops   *ops;
    unsigned int            vreg_1p1;
    unsigned int            vreg_1p8;
    unsigned int            irq;
    unsigned int            reset;
    sem_t                   sw_irq_lock;
    pthread_t               sw_irq_thread;
    bool                    sw_irq_thread_exit;
    uint8_t                 dev_ids[SWITCH_PORT_MAX];
};

int switch_if_dev_id_set(struct tsb_switch *sw,
                         uint8_t port_id,
                         uint8_t dev_id);
int switch_connection_create(struct tsb_switch *sw,
                             uint8_t port_id1,
                             uint8_t cport_id1,
                             uint8_t port_id2,
                             uint8_t cport_id2,
                             uint8_t tc,
                             uint8_t flags);

static inline int switch_connection_std_create(struct tsb_switch *sw,
                                               uint8_t port_id1,
                                               uint8_t cport_id1,
                                               uint8_t port_id2,
                                               uint8_t cport_id2) {

    return switch_connection_create(sw,
                                    port_id1,
                                    cport_id1,
                                    port_id2,
                                    cport_id2,
                                    CPORT_TC0,
                                    CPORT_FLAGS_CSD_N | CPORT_FLAGS_CSV_N);
};

int switch_configure_link(struct tsb_switch *sw,
                          uint8_t port_id,
                          const struct unipro_link_cfg *cfg,
                          const struct tsb_link_cfg *tcfg);

/**
 * @brief "Auto" mode variant selection during link configuration.
 *
 * If set when configuring a link for an HS or PWM gear, choose the
 * "auto" mode variant. This provides lower power with automatic
 * transitions between BURST and SLEEP M-PHY states, at the cost of
 * extra latency to come out of SLEEP. */
#define UNIPRO_LINK_CFGF_AUTO         (1U << 0)

#define TSB_DEFAULT_PWR_USER_DATA                               \
    {                                                           \
        .flags = UPRO_PWRF_FC0,                                 \
        .upro_pwr_fc0_protection_timeout = 0x1FFF,              \
    }

/**
 * @brief Configure both directions of a link to the given HS gear.
 *
 * @param sw Switch handle
 * @param port_id Port whose link to configure
 * @param gear HS gear to use (HS-G1=1, HS-G2=2, HS-G3=3)
 * @param nlanes Number of lanes to use (1, 2, ...)
 * @param flags UNIPRO_LINK_CFGF_xxx
 */
static inline int switch_configure_link_hs(struct tsb_switch *sw,
                                           uint8_t port_id,
                                           unsigned int gear,
                                           unsigned int nlanes,
                                           unsigned int flags) {
    int auto_variant = flags & UNIPRO_LINK_CFGF_AUTO;
    const struct unipro_link_cfg lcfg = {
        .upro_hs_ser = UNIPRO_HS_SERIES_UNCHANGED,
        .upro_tx_cfg = UNIPRO_FAST_PWR_CFG(auto_variant, gear, nlanes),
        .upro_rx_cfg = UNIPRO_FAST_PWR_CFG(auto_variant, gear, nlanes),
        .upro_user   = TSB_DEFAULT_PWR_USER_DATA,
        .flags       = 0,
    };
    return switch_configure_link(sw, port_id, &lcfg, NULL);
}

/**
 * @brief Configure both directions of a link to the given PWM gear.
 *
 * @param sw Switch handle
 * @param port_id Port whose link to configure
 * @param gear PWM gear to use (PWM-G1=1, PWM-G2=2, ..., PWM-G7=7)
 * @param nlanes Number of lanes to use (1, 2, ...)
 * @param flags UNIPRO_LINK_CFGF_xxx
 */
static inline int switch_configure_link_pwm(struct tsb_switch *sw,
                                            uint8_t port_id,
                                            unsigned int gear,
                                            unsigned int nlanes,
                                            unsigned int flags) {
    int auto_variant = flags & UNIPRO_LINK_CFGF_AUTO;
    const struct unipro_link_cfg lcfg = {
        .upro_hs_ser = UNIPRO_HS_SERIES_UNCHANGED,
        .upro_tx_cfg = UNIPRO_SLOW_PWR_CFG(auto_variant, gear, nlanes),
        .upro_rx_cfg = UNIPRO_SLOW_PWR_CFG(auto_variant, gear, nlanes),
        .upro_user   = TSB_DEFAULT_PWR_USER_DATA,
        .flags       = UPRO_LINKF_TX_TERMINATION | UPRO_LINKF_RX_TERMINATION,
    };
    return switch_configure_link(sw, port_id, &lcfg, NULL);
}

int switch_lut_get(struct tsb_switch *sw,
                   uint8_t unipro_portid,
                   uint8_t addr,
                   uint8_t *dst_portid);

int switch_lut_set(struct tsb_switch *sw,
                   uint8_t unipro_portid,
                   uint8_t addr,
                   uint8_t dst_portid);

int switch_dev_id_mask_get(struct tsb_switch *sw,
                           uint8_t unipro_portid,
                           uint8_t *dst);

int switch_dev_id_mask_set(struct tsb_switch *sw,
                           uint8_t unipro_portid,
                           uint8_t *mask);

int switch_setup_routing_table(struct tsb_switch *sw,
                               uint8_t device_id_0,
                               uint8_t port_id_0,
                               uint8_t device_id_1,
                               uint8_t port_id_1);

int switch_dump_routing_table(struct tsb_switch *sw);

struct tsb_switch *switch_init(struct tsb_switch *,
                               unsigned int vreg_1p1,
                               unsigned int vreg_1p8,
                               unsigned int reset,
                               unsigned int irq);
void switch_exit(struct tsb_switch*);

/*
 * Low-level DME access
 */

int switch_dme_set(struct tsb_switch *sw,
                   uint8_t portid,
                   uint16_t attrid,
                   uint16_t select_index,
                   uint32_t attr_value);

int switch_dme_get(struct tsb_switch *sw,
                   uint8_t portid,
                   uint16_t attrid,
                   uint16_t select_index,
                   uint32_t *attr_value);

int switch_dme_peer_set(struct tsb_switch *sw,
                        uint8_t portid,
                        uint16_t attrid,
                        uint16_t select_index,
                        uint32_t attr_value);

int switch_dme_peer_get(struct tsb_switch *sw,
                        uint8_t portid,
                        uint16_t attrid,
                        uint16_t select_index,
                        uint32_t *attr_value);

int switch_port_irq_enable(struct tsb_switch *sw,
                           uint8_t portid,
                           bool enable);

int switch_internal_getattr(struct tsb_switch *sw,
                            uint16_t attrid,
                            uint32_t *val);

int switch_internal_setattr(struct tsb_switch *sw,
                            uint16_t attrid,
                            uint32_t val);

int switch_irq_enable(struct tsb_switch *sw,
                      bool enable);
int switch_post_irq(struct tsb_switch *sw);

#endif
