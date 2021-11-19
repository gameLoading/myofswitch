#define MAX_DEVICE_SIZE 10
#define MAX_DEVICE_NAME_LEN 20

/*Control Wireless Device*/
void InitialWirelessDevice();

/*Query information for Wireless Device */
void getAllWirelessDevice(char buf[][MAX_DEVICE_NAME_LEN]);
void getAllWirelessDeviceWithJSON(char *buf);
void getWirelessDeviceInfo(char *deviceName, char *buf);
void getAssocicatedHosts(char *deviceName, char *buf);

// enum Txpower {

// }