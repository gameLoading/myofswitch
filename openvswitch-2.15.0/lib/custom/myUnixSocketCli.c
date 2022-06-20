#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/un.h>
#include <sys/types.h>
#include <unistd.h>
#include <errno.h>
#include <netinet/in.h>
#include <netdb.h>
#include <arpa/inet.h>
#include "tools/myUnixSocketCli.h"


int send_unix_msg_onetime(unix_socket_file file, char* msg);
int recv_unix_msg(unix_socket_file file, char* buffer, int buf_size);

int send_unix_msg_onetime(unix_socket_file file, char* msg){
	int client_len;
    int client_sockfd;
    struct sockaddr_un clientaddr;

    client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sockfd == -1) {
        perror("error : ");
		goto fail;
    }

    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sun_family = AF_UNIX;
    strcpy(clientaddr.sun_path, file);
    client_len = sizeof(clientaddr);

    if (connect(client_sockfd, (struct sockaddr *)&clientaddr, client_len) < 0)
    {
        perror("Connect error: ");
		goto fail;
    }
    write(client_sockfd, msg, strlen(msg));
	goto success;
success:
    close(client_sockfd);
	return 0;
fail:
    close(client_sockfd);
	return -1;
}

int recv_unix_msg(unix_socket_file file, char* buffer, int buf_size){
	int client_len;
    int client_sockfd;
    struct sockaddr_un clientaddr;

    client_sockfd = socket(AF_UNIX, SOCK_STREAM, 0);
    if (client_sockfd == -1) {
        perror("error : ");
		goto fail;
    }

    bzero(&clientaddr, sizeof(clientaddr));
    clientaddr.sun_family = AF_UNIX;
    strcpy(clientaddr.sun_path, file);
    client_len = sizeof(clientaddr);

    if (connect(client_sockfd, (struct sockaddr *)&clientaddr, client_len) < 0)
    {
        perror("Connect error: ");
		goto fail;
    }
    read(client_sockfd, buffer, buf_size);
	goto success;
success:
    close(client_sockfd);
	return 0;
fail:
    close(client_sockfd);
	return -1;
}
