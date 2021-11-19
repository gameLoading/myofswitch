#include <stdint.h>
#include <syslog.h>
#include <config.h>
#include <stdint.h>
#include "openvswitch/ofp-port.h"
#include "openflow/intel-ext.h"
#include "openvswitch/ofp-errors.h"
#include "openvswitch/ofp-msgs.h"
#include "openvswitch/ofp-print.h"
#include "openvswitch/ofp-prop.h"
#include "openvswitch/ofpbuf.h"
#include "openvswitch/vlog.h"

#ifndef OPENVSWITCH_OFP_WIRELESS_H
#define OPENVSWITCH_OFP_WIRELESS_H 1

typedef uint8_t ofp_wparams_t;



struct ofp13_wparams_stats_request {
    ofp_wparams_t wparamsType;        
    char name[20];
};

struct ofp13_wparams_stats_reply {
    ofp_wparams_t wparamsType;        
    char name[20];
};

enum wparams_types{
    DEVICES,
    INFO,
    ASSOICATED
};

struct ofconn;
struct ofpbuf;


// static enum ofperr handle_wparams_request(struct ofconn* ofconn, const struct ofp_header* oh);
#ifdef __cplusplus
extern "C" {
#endif

enum ofperr ofputil_decode_wparams_stats_request(const struct ofp_header *request, ofp_wparams_t* type, char *deviceName);
struct ofpbuf* ofputil_alloc_wparams_stats_reply(
    const struct ofp_header *request, ofp_wparams_t type, char *deviceName, char *payload);

#ifdef __cplusplus
}
#endif

#endif