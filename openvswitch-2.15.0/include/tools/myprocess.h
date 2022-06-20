#define LINE_BUF 2048
#define MAX_LINE 100
#define COMMAND_LEN 200

int systemEx(char *cmd, char** result, int* nResult);
int systemEx_oneline(char *cmd, char* buf, int buf_len);