#include <stdio.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <unistd.h>
#include <ctype.h>
#include <stdbool.h>
#include <strings.h>
#include <string.h>
#include <sys/stat.h>
#if USE_PTHREAD
#include <pthread.h>
#endif
#include <sys/wait.h>
#include <stdlib.h>
#include <stdint.h>
#include "yhttpd.h"
#include "ylib.h"

#define SERVER_STRING "Server: yhttpd/0.0.1\r\n"
#define MAXLINE 1024

void accept_request(void *);
void error_die(const char *);
int get_line(int, char *, int);
void headers(int, const char *);
void handle_url(int, const char *);
int startup(u_short *);

void accept_request(void *arg) {
    int client = (int)((intptr_t)arg);
    char method[255];
    char url[255];
    char path[512];
    char buf[MAXLINE];
    bool cgi = false;
    size_t numchars = get_line(client, buf, sizeof(buf));
    size_t i = 0;
    while (!isspace(buf[i]) && (i < sizeof(method) - 1)) {
        method[i] = buf[i];
        i++;
    }
    size_t j = i;
    method[i] = '\0';
//    if (strcasecmp(method, "GET") && strcasecmp(method, "POST")) {
//        return;
//    }
    if (strcasecmp(method, "POST") == 0)
        cgi = true;
    i = 0;
    while (isspace(buf[j]) && (j < numchars)) j++;
    while (!isspace(buf[j]) && (i < sizeof(url) - 1) && (j < numchars)) {
        url[i++] = buf[j++];
    }
    url[i] = '\0';
    if (strcasecmp(method, "GET") == 0) {
        char *query_string = url;
        while ((*query_string != '?') && (*query_string != '\0'))
            query_string++;
        if (*query_string == '?') {
            cgi = true;
            *query_string = '\0';
            query_string++;
        }
    }
    sprintf(path, " %s", url);
    struct stat st;
    if (stat(path, &st) == -1) {
        while ((numchars > 0) && strcmp("\n", buf)) numchars = get_line(client, buf, sizeof(buf));
        handle_url(client, path);
    } else {
        // todo
    }
    close(client);
}

void error_die(const char *sc) {
    perror(sc);
    exit(1);
}

int get_line(int sock, char *buf, int size) {
    int i = 0;
    char c = '\0';
    while ((i < size - 1) && (c != '\n')) {
        ssize_t n = recv(sock, &c, 1, 0);
        if (n > 0) {
            if (c == '\r') {
                n = recv(sock, &c, 1, MSG_PEEK);
                if ((n > 0) && (c == '\n')) recv(sock, &c, 1, 0);
                else c = '\n';
            }
            buf[i] = c;
            i++;
        } else {
            c = '\n';
        }
    }
    buf[i] = '\0';
    return i;
}

#define SEND(_client, _str) strcpy(buf, _str); send(_client, buf, strlen(buf), 0);

void headers(int client, const char *filename) {
    char buf[MAXLINE];
    SEND(client, "HTTP/1.0 200 OK\r\n")
    SEND(client, SERVER_STRING)
    SEND(client, "Content-Type: text/plain\r\n")
    SEND(client, "\r\n")
}

void handle_url(int client, const char *path) {
    headers(client, path);
    char buf[MAXLINE];
    char sub_str[MAXLINE];
    if (strlen(path) == 2 && *(path + 1) == '/') {
        sprintf(buf, "hello world");
    } else if (strncmp(path, " /msg/", 6) == 0) {
        substring(sub_str, path, 6, strlen(path));
        sprintf(buf, "%s", sub_str);
    }
    send(client, buf, strlen(buf), 0);
}

int startup(u_short *port) {
    struct sockaddr_in name;
    int httpd = socket(PF_INET, SOCK_STREAM, 0);
    if (httpd == -1)
        error_die("socket");
    bzero(&name, sizeof(name));
    name.sin_family = AF_INET;
    name.sin_port = htons(*port);
    name.sin_addr.s_addr = htonl(INADDR_ANY);
    int on = 1;
    if ((setsockopt(httpd, SOL_SOCKET, SO_REUSEADDR, &on, sizeof(on))) < 0) {
        error_die("setsockopt failed");
    }
    if (bind(httpd, (struct sockaddr *)&name, sizeof(name)) < 0) {
        error_die("bind");
    }
    if (*port == 0) {
        socklen_t namelen = sizeof(name);
        if (getsockname(httpd, (struct sockaddr *)&name, &namelen) == -1)
            error_die("getsockname");
        *port = ntohs(name.sin_port);
    }
    if (listen(httpd, 5) < 0) error_die("listen");
    return (httpd);
}
#if TEST == 0
int main(void) {
    u_short port = SERV_PORT;
    int client_sock = -1;
    struct sockaddr_in client_name;
    socklen_t  client_name_len = sizeof(client_name);
    int server_sock = startup(&port);
    printf("httpd running on port %d\n", port);
    for ( ; ; ) {
        client_sock = accept(server_sock, (struct sockaddr *)&client_name, &client_name_len);
        if (client_sock == -1) error_die("accept");
#if USE_PTHREAD
        pthread_t newthread;
        if (pthread_create(&newthread , NULL, (void *)accept_request, (void *)(intptr_t)client_sock) != 0) {
            perror("pthread_create");
        }
#else
        accept_request((void *)(intptr_t)client_sock);
#endif
    }
    return (0);
}
#endif
