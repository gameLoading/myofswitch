#include "wireless/wdev.h"
#include "tools/myprocess.h"
#include "tools/mystrTools.h"
#include "tools/myUnixSocketCli.h"
// #include "tools/myleak_detector_c.h"
#include "tools/myjson.h"
#include <string.h>
#include <syslog.h>
#include <json-c/json.h>
#include <ctype.h>
#include <stdio.h>

/*Control Wireless Device*/
int InitialWirelessDevice(void);

/*Query information for Wireless Device */
void getAllWirelessDeviceWithJSON(char *buf, int buf_len);
void getWirelessDeviceInfo(char *deviceName, char *buf, int buf_len);
void getAssocicatedHosts(char *deviceName, char buf[], int buf_len);
int getWirelessUcis(char *deviceName, char buf[], int buf_len);
void getFreqList(char *deviceName, char buf[], int buf_len);
void getTxpowerList(char *deviceName, char buf[], int buf_len);
void getCountryList(char *deviceName, char buf[], int buf_len);
void getScannedSSIDList(char *deviceName, char buf[], int buf_len);
void getFreqSurveyInfo(char *deviceName, char buf[], int buf_len);
void getSystemInfo(char *deviceName, char buf[], int buf_len);
void getSystemBoard(char *deviceName, char buf[], int buf_len);

/*Managing function for Wireless Device */
void setSSID(char *deviceName, char *ssid, int buf_len);
void setTxPower(char *deviceName, int txpower);
void setWirelessUcis(char *deviceName, char *buf);

/*For utils*/
int isAvailableSSID(char *ssid);
int getBaseUci(char *buffer, char deviceName[], int buf_size);

int InitialWirelessDevice(void)
{
	// char buf[MAX_UCI_LEN] = "";

	// int res = 1;
	// char deviceName[MAX_DEVICE_NAME_LEN] = "";
	// char setCommand[MAX_UCI_LEN] = "";
	// char baseUci[MAX_UCI_LEN] = "";

	// /*All wifi device delete*/
	// int i=0;

	// const int BASE_CMD_LEN = 5;
	// char command[5][MAX_UCI_LEN] = {
	// 	"=wifi-iface",
	// 	".mode='ap'",
	// 	".ssid='OpenWrt-public'",
	// 	".encryption='none'",
	// 	".disabled='1'",
	// };

	// while(1){
	// 	sprintf(deviceName, "wlan%d", i++);
    // 	int ret = getBaseUci(baseUci, deviceName, MAX_UCI_LEN);
	// 	if (ret == -1)break;
	// 	ret = uci_command_del(baseUci);
	// 	for (int j = 0; j < BASE_CMD_LEN; j++){
	// 		sprintf(setCommand, "%s%s", baseUci, command[j]);
	// 		ret &= uci_command_set(command[j]);
	// 	}
	// 	ret &= uci_command_commit();
	// 	if (ret == 0){
	// 		printf("Wireless Device Initial Complete\n");
	// 		restartWifi();
	// 	}
	// 	else{
	// 		printf("Wireless Device Adding result code : %d\n", ret);
	// 	}
	// }
	return 0;
}

void setSSID(char *deviceName, char *ssid, int buf_len){
	openlog("ofproto.c", LOG_CONS, LOG_USER);
	syslog(LOG_INFO, "dev : %s / ssid : %s / buf_len : %d \n", deviceName, ssid, buf_len);
	closelog();
	if (!isAvailableSSID(ssid))
	{
		openlog("ofproto.c", LOG_CONS, LOG_USER);
		syslog(LOG_INFO, "this ssid is not available \n");
		closelog();
		return;
	}
	openlog("ofproto.c", LOG_CONS, LOG_USER);
	// syslog(LOG_INFO, "this ssid is available \n");
	closelog();

	openlog("ofproto.c", LOG_CONS, LOG_USER);
	syslog(LOG_INFO, "dev : %s / ssid : %s / buf_len : %d \n", deviceName, ssid, buf_len);
	closelog();

	char baseUci[MAX_UCI_LEN] = "";
	int ret = getBaseUci(baseUci, deviceName, MAX_UCI_LEN);
	if (ret == -1) {
		openlog("ofproto.c", LOG_CONS, LOG_USER);
		syslog(LOG_INFO, "There is not available device \n");
		closelog();
		return;
	}


	char command[MAX_UCI_LEN+20+MAX_DEVICE_NAME_LEN] = "";
	sprintf(command, "uci set %s.ssid=%s", baseUci, ssid);

	
	openlog("ofproto.c", LOG_CONS, LOG_USER);
	syslog(LOG_INFO, "dev : %s / ssid : %s / buf_len : %d \n", deviceName, ssid, buf_len);
	closelog();
	send_unix_msg_onetime(UNIX_SOCK_FILE, command);
}

void setWirelessUcis(char *deviceName, char *buf){
	char *ptr = strtok(buf, ",");
	while (ptr != NULL){
		openlog("ofproto.c", LOG_CONS, LOG_USER);
		syslog(LOG_INFO, "ptr : %s\n", ptr);
		closelog();
		send_unix_msg_onetime(UNIX_SOCK_FILE, ptr);
		ptr = strtok(NULL, ",");
	}
}

void getAllWirelessDeviceWithJSON(char *buf, int buf_len)
{
	// ubus_cmd_call("iwinfo", "devices", buf, NULL);
	systemEx_oneline((char *)"ubus -S call iwinfo devices", buf, buf_len);
}

void getWirelessDeviceInfo(char *deviceName, char *buf, int buf_len){
	// char arg[MAX_DEVICE_NAME_LEN + 10];
	// sprintf(arg, "{\"device\":\"%s\"}", deviceName);
	// ubus_cmd_call("iwinfo", "info", buf, arg);

	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call iwinfo info '{\"device\":\"%s\"}'", deviceName);
	systemEx_oneline((char *)cmd, buf, buf_len);
}


void getAssocicatedHosts(char *deviceName, char buf[], int buf_len) {
	// char arg[MAX_DEVICE_NAME_LEN + 10];
	// sprintf(arg, "{\"device\":\"%s\"}", deviceName);
	// ubus_cmd_call("iwinfo", "assoclist", buf, arg);

	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call iwinfo assoclist '{\"device\":\"%s\"}'", deviceName);
	systemEx_oneline((char *)cmd, buf, buf_len);
}
int getWirelessUcis(char *deviceName, char buf[], int buf_len){
	int nResult = 0;
	char cmd[COMMAND_LEN] = "";
	char **result = NULL;

	/*Get Radio Options*/
	result = (char**)malloc(sizeof(char*)*MAX_LINE);
	memset(result, 0, sizeof(char*)*MAX_LINE);
	memset(buf, 0, buf_len);

	const char start[] = "{\"configs\":[";
	const char end[] = "\"last\"]}";
	if (copyString_a(buf, (char*)start, strlen(start), buf_len)==-1){
		goto fail;
	}

	sprintf(cmd, "uci show wireless.radio%c", deviceName[4]);
	systemEx(cmd, result, &nResult);

	char tmp[MAX_UCI_LEN+10] = "";
	for (int i=0;i < nResult;i++){
		printf("nResult:%d %s\n", i, result[i]);
		sprintf(tmp, "\"%s\",", result[i]);
		if (strcat_safe(buf, tmp, strlen(tmp), buf_len)==-1) goto fail;
	}
	result = freeStrings(result, nResult);
	printf("1----%s\n", buf);

	/*Get Radio Options*/
	nResult = 0;
	result = (char**)malloc(sizeof(char*)*MAX_LINE);
	memset(result, 0, sizeof(char*)*MAX_LINE);
	
	char baseUci[MAX_UCI_LEN] = "";
	int ret = getBaseUci(baseUci, deviceName, MAX_UCI_LEN);
	if (ret != -1){
		sprintf(cmd, "uci show %s", baseUci);
		systemEx(cmd, result, &nResult);

		for (int i=0;i < nResult;i++){
			sprintf(tmp, "\"%s\",", result[i]);
			if (strcat_safe(buf, tmp, strlen(tmp),buf_len)==-1) goto fail;
		}
	}
	printf("2----%s\n", buf);
	
	if (strcat_safe(buf, (char*)end, strlen(end), buf_len)==-1) goto fail;
	goto success;
success:
	result = freeStrings(result, nResult);
	return 0;
fail:
	result = freeStrings(result, nResult);
	memset(buf, 0, buf_len);
	return -1;
}

void getFreqList(char *deviceName, char buf[], int buf_len) {
	// char arg[MAX_DEVICE_NAME_LEN + 10];
	// sprintf(arg, "{\"device\":\"%s\"}", deviceName);
	// ubus_cmd_call("iwinfo", "assoclist", buf, arg);

	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call iwinfo freqlist '{\"device\":\"%s\"}'", deviceName);
	systemEx_oneline((char *)cmd, buf, buf_len);
}

void getTxpowerList(char *deviceName, char buf[], int buf_len) {
	// char arg[MAX_DEVICE_NAME_LEN + 10];
	// sprintf(arg, "{\"device\":\"%s\"}", deviceName);
	// ubus_cmd_call("iwinfo", "assoclist", buf, arg);

	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call iwinfo txpowerlist '{\"device\":\"%s\"}'", deviceName);
	systemEx_oneline((char *)cmd, buf, buf_len);
}

void getCountryList(char *deviceName, char buf[], int buf_len) {
	// char arg[MAX_DEVICE_NAME_LEN + 10];
	// sprintf(arg, "{\"device\":\"%s\"}", deviceName);
	// ubus_cmd_call("iwinfo", "assoclist", buf, arg);

	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call iwinfo countrylist '{\"device\":\"%s\"}'", deviceName);
	systemEx_oneline((char *)cmd, buf, buf_len);
}

void getScannedSSIDList(char *deviceName, char buf[], int buf_len) {
	// char arg[MAX_DEVICE_NAME_LEN + 10];
	// sprintf(arg, "{\"device\":\"%s\"}", deviceName);
	// ubus_cmd_call("iwinfo", "assoclist", buf, arg);

	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call iwinfo scan '{\"device\":\"%s\"}'", deviceName);
	systemEx_oneline((char *)cmd, buf, buf_len);
}

void getFreqSurveyInfo(char *deviceName, char buf[], int buf_len) {
	// char arg[MAX_DEVICE_NAME_LEN + 10];
	// sprintf(arg, "{\"device\":\"%s\"}", deviceName);
	// ubus_cmd_call("iwinfo", "assoclist", buf, arg);

	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call iwinfo survey '{\"device\":\"%s\"}'", deviceName);
	systemEx_oneline((char *)cmd, buf, buf_len);
}


void getSystemBoard(char *deviceName, char buf[], int buf_len){
	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call system board");
	systemEx_oneline((char *)cmd, buf, buf_len);
}

void getSystemInfo(char *deviceName, char buf[], int buf_len){
	char cmd[MAX_JSON_SIZE] = "";
	sprintf(cmd, "ubus -S call system info");
	systemEx_oneline((char *)cmd, buf, buf_len);
}

void setTxPower(char *deviceName, int txpower){
	// char cmd[MAX_JSON_SIZE] = "";
	// sprintf(cmd, "iw dev %s set txpower fixed %d", deviceName, txpower);
	// system(cmd);

	openlog("ofproto.c", LOG_CONS, LOG_USER);
	syslog(LOG_INFO, "dev : %s / txpower : %d\n", deviceName, txpower);
	closelog();

	char command[MAX_UCI_LEN+20+MAX_DEVICE_NAME_LEN] = "";
	if (!isdigit(deviceName[4])) {
		printf("deviceName length is not normal : %s\n", deviceName);
		return -1;
	}
	int devIdx = deviceName[4]-'0';
	sprintf(command, "uci set wireless.radio%d.txpower=%d", devIdx, txpower);
	send_unix_msg_onetime(UNIX_SOCK_FILE, command);
}

int isAvailableSSID(char *ssid)
{
	for (int i = 0; ssid[i] != '\0';i++)
	{
		if (!(isalnum(ssid[i]) || ssid[i] == '.' || ssid[i] == '_'))
			return 0;
	}
	return 1;
}

/*radio index must be 0~9*/
int getBaseUci(char *buffer, char deviceName[], int buf_size){
	int devLen = strlen_safe(deviceName, MAX_DEVICE_NAME_LEN);
	if (devLen != 5) {
		printf("deviceName length is not 5 but %d\n", devLen);
		return -1;
	}
	if (!isdigit(deviceName[4])) return -1;

	int devIdx = deviceName[4]-'0';
    char result[MAX_UCI_LEN] = "";

	char cmd[COMMAND_LEN] = "";
	sprintf(cmd, "uci show wireless | grep \"='radio%d'\"", devIdx);
    systemEx_oneline(cmd, result, MAX_UCI_LEN);

	int dotCnt = 0;
	int len = strlen(result);	
	int lastlen = -1;
	for (int i=0;i < len;i++){
		if (result[i] == '.'){
			dotCnt++;
			if (dotCnt == 2){
				lastlen = i;
				break;
			}
		}
	}
	if (lastlen == -1) return -1;
	int isSucess = copyString_a(buffer, result, lastlen, buf_size);
	if (isSucess != -1) return 0; 
	else return -1;
}


