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

typedef uint8_t ofp_wparams_stats_t;
typedef uint8_t ofp_wparams_config_t;

struct ofp13_wparams_stats_request {
    ofp_wparams_stats_t wparamsType;        
    char name[20];
};

struct ofp13_wparams_stats_reply {
    ofp_wparams_stats_t wparamsType;        
    char name[20];
};

struct wparams_config_tlv {
    ofp_wparams_config_t type;
    int length;
    char* value;
    struct wparams_config_tlv *next;
};

enum wparams_stat_types{
    DEVICES,
    INFO,
    ASSOICATED,
    UCI,
    FREQLIST,
    TXPOWERLIST,
    COUNTRYLIST,
    SCAN,
    SURVEY,
    SYSTEM_INFO,
    SYSTEM_BOARD,
};

enum wparams_config_types{
    RESET,
    SSID,
    TXPOWER,
    UCIs
};

struct ofconn;
struct ofpbuf;


// static enum ofperr handle_wparams_request(struct ofconn* ofconn, const struct ofp_header* oh);
#ifdef __cplusplus
extern "C" {
#endif

int 
ofputil_decode_wparams_stats_request(const struct ofp_header *request,
                                     ofp_wparams_stats_t* type, char deviceName[], int buf_len);
struct ofpbuf* ofputil_alloc_wparams_stats_reply(const struct ofp_header *request, ofp_wparams_stats_t type, char *deviceName, char *payload);
enum ofperr ofputil_decode_wparams_set_config(const struct ofp_header *oh, char *deviceName, struct wparams_config_tlv *start);

void
ofputil_free_wparams_set_config(struct wparams_config_tlv *start);
#ifdef __cplusplus
}
#endif

#endif