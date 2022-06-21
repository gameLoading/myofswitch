#include "openvswitch/ofp-wireless.h"
#include <stdint.h>
#include <syslog.h>
#include <config.h>
#include <stdint.h>
#include <ctype.h>
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
#include "tools/mystrTools.h"
// #include "tools/myleak_detector_c.h"

int 
ofputil_decode_wparams_stats_request(const struct ofp_header *request,
                                     ofp_wparams_stats_t* type, char deviceName[], int buf_len)
{
    const struct ofp13_wparams_stats_request* wsr;
    switch ((enum ofp_version)request->version)
    {
    case OFP13_VERSION:
        wsr = ofpmsg_body(request);
        openlog("ofproto.c", LOG_CONS, LOG_USER);
        // syslog(LOG_INFO, "%d %s", wsr->wparamsType, wsr->name);
        closelog();
        *type = wsr->wparamsType;
        int res = copyString_a(deviceName, wsr->name, MAX_DEVICE_NAME_LEN-1, buf_len);
        return res;
    default:
        abort();
    }
}


struct ofpbuf* ofputil_alloc_wparams_stats_reply(const struct ofp_header *request, ofp_wparams_stats_t type, 
                                                char *deviceName, char *payload)
{
    struct ofpbuf* buf;
    struct ofp13_wparams_stats_reply *reply;

    buf = ofpraw_alloc_reply(OFPRAW_OFPST13_WPARAMS_STATS_REPLY, request, 0);
    reply = ofpbuf_put_zeros(buf, sizeof(*reply));

    if (deviceName != 0){
        int dev_length = strnlen(deviceName, MAX_DEVICE_NAME_LEN-1);
        copyString_a(reply->name, deviceName, dev_length, MAX_DEVICE_NAME_LEN);
    }
    reply->wparamsType = type;

    int payload_length = strnlen(payload, MAX_JSON_SIZE-1);
    char* information = ofpbuf_put_zeros(buf, sizeof(char)*payload_length+2);
    copyString_a(information, payload, payload_length+2, payload_length+2);
    return buf;
}

enum ofperr
ofputil_decode_wparams_set_config(const struct ofp_header *oh, char *deviceName, struct wparams_config_tlv *start)
{
    uint8_t* bytes = (uint8_t *) oh;
    ofp_wparams_config_t type;
    uint8_t length;

    bytes+=2;
    int oflength = (bytes[0] << 8) + (bytes[1]);
    openlog("ofproto.c", LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "bytes[0]:%d, bytes[1]:%d", bytes[0], bytes[1]);
    closelog();
    bytes+=6;

    memcpy(deviceName, bytes, MAX_DEVICE_NAME_LEN);
    deviceName[MAX_DEVICE_NAME_LEN-1] = 0;
    openlog("ofproto.c", LOG_CONS, LOG_USER);
    syslog(LOG_INFO, "deviceName:%s, oflength:%d", deviceName, oflength);
    closelog();

    bytes+=24;
    int tlvs_len = 0;
    int flag=0, tlvOffset=0;
    struct wparams_config_tlv *current = NULL;
    struct wparams_config_tlv *before = NULL;
    
    switch ((enum ofp_version)oh->version)
    {
    case OFP13_VERSION:
        while(tlvOffset<(oflength-32)){
            //way to get 'Type'
            if (flag == 0){
                type = *bytes;
                bytes++;
                flag++;
                tlvOffset++;
                if (tlvs_len != 0 && type == RESET){
                    openlog("ofproto.c", LOG_CONS, LOG_USER);
                    syslog(LOG_INFO, "packets cannot have anything other tlv type RESET ");
                    closelog();
                    break;
                }
            }
            //way to get 'Length'
            else if (flag == 1){
                length = (bytes[0] << 8) + (bytes[1]);
                bytes+=2;
                flag++;
                tlvOffset+=2;
                if (!(type == RESET) && length == 0)break;
            }
            //way to get 'Value'
            else if (flag == 2){
                char* str = (char*)malloc(sizeof(char)*(length+1));
                memset(str, 0, length+1);
                copyString_m(str, (char*)bytes, length+1);
                flag=0;
                bytes+=length;
                tlvOffset+=length;
                openlog("ofproto.c", LOG_CONS, LOG_USER);
                syslog(LOG_INFO, "type:%d / length:%d / value:%s", type, length, str);
                closelog();
                tlvs_len++;

                if (before == NULL){
                    start->length = length;
                    start->type = type;
                    start->value = str;
                    start->next = NULL;
                    before = start;
                }else{
                    current = (struct wparams_config_tlv *)malloc(sizeof(struct wparams_config_tlv));
                    memset(current, 0, sizeof(struct wparams_config_tlv));
                    current->length = length;
                    current->type = type;
                    current->value = str;
                    current->next = NULL;
                    before->next = current;
                    before = current;
                }
            }
        }
        return 0;
    default:
        abort();
    }
}

void
ofputil_free_wparams_set_config(struct wparams_config_tlv *start)
{
    if (start == NULL) return;

    struct wparams_config_tlv *current;
    current = start;
    struct wparams_config_tlv *next = NULL;
    do{
        openlog("ofproto.c", LOG_CONS, LOG_USER);
        syslog(LOG_INFO, "current : %p / next : %p", current, current->next);
        closelog();
        next = current->next;
        openlog("ofproto.c", LOG_CONS, LOG_USER);
        syslog(LOG_INFO, "current Value : %s", current->value);
        closelog();
        if (current->value!=NULL) {
            openlog("ofproto.c", LOG_CONS, LOG_USER);
            syslog(LOG_INFO, "free(current->value);");
            closelog();
            free(current->value);
            current->value = NULL;
        }
        openlog("ofproto.c", LOG_CONS, LOG_USER);
        syslog(LOG_INFO, "free(current);");
        closelog();
        if (current != NULL) free(current);
        current = next;
    }while(current != NULL);
}