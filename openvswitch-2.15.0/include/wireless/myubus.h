#include <stdio.h>

#define MAX_JSON_SIZE 500

int ubus_cmd_call(char* path, char* item, char* buffer, char* arg);