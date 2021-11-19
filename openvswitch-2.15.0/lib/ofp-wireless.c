#include "openvswitch/ofp-wireless.h"
#include <stdint.h>
#include <syslog.h>
#include <config.h>
#include <stdint.h>
#include "openvswitch/ofp-port.h"
#include "byte-order.h"
#include "openflow/intel-ext.h"
#include "openvswitch/json.h"
#include "openvswitch/ofp-errors.h"
#include "openvswitch/ofp-msgs.h"
#include "openvswitch/ofp-print.h"
#include "openvswitch/ofp-prop.h"
#include "openvswitch/ofpbuf.h"
#include "openvswitch/vlog.h"
#include "wireless/wdev.h"
#include "wireless/myubus.h"

enum ofperr
ofputil_decode_wparams_stats_request(const struct ofp_header *request,
                                     ofp_wparams_t* type, char *deviceName)
{
    const struct ofp13_wparams_stats_request* wsr;
    switch ((enum ofp_version)request->version)
    {
    case OFP13_VERSION:
        wsr = ofpmsg_body(request);
        openlog("ofproto.c", LOG_CONS, LOG_USER);
        syslog(LOG_INFO, "%d %s", wsr->wparamsType, wsr->name);
        closelog();
        *type = wsr->wparamsType;
        strncpy(deviceName, wsr->name, MAX_DEVICE_NAME_LEN-1);
        deviceName[MAX_DEVICE_NAME_LEN-1] = '\0';
        return 0;
    default:
        abort();
    }
}


struct ofpbuf* ofputil_alloc_wparams_stats_reply(const struct ofp_header *request, ofp_wparams_t type, 
                                                char *deviceName, char *payload)
{
    openlog("ofproto.c", LOG_CONS, LOG_USER);
    struct ofpbuf* buf;
    struct ofp13_wparams_stats_reply *reply;

    buf = ofpraw_alloc_reply(OFPRAW_OFPST13_WPARAMS_REPLY, request, 0);
    reply = ofpbuf_put_zeros(buf, sizeof(struct ofp13_wparams_stats_reply));

    if (deviceName != 0){
        int dev_length = strnlen(deviceName, MAX_DEVICE_NAME_LEN-1);
        strncpy(reply->name, deviceName, dev_length);
        reply->name[dev_length] = '\0';
    }
    reply->wparamsType = type;

    int payload_length = strnlen(payload, MAX_JSON_SIZE-1);
    char* information = ofpbuf_put_zeros(buf, sizeof(char)*payload_length);
    strncpy(information, payload, payload_length);
    information[payload_length] = '\0';
    closelog();

    return buf;
}

