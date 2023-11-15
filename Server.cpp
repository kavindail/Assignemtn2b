#include "FormUpload.cpp"
#include "UploadServer.cpp"
#include <dirent.h>
#include <iostream>
#include <netinet/in.h>
#include <pthread.h>
#include <resolv.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/dir.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>

FormUpload formUpload;
UploadServer uploadServer;
static void *handleRequests(void *);
pthread_t thread;

int main() {
  int sock;
  int length;
  struct sockaddr_in server;
  int msgsock;
  int i;

  sock = socket(AF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("error opening stream socket: In Server.cpp");
  }

  server.sin_family = AF_INET;
  server.sin_addr.s_addr = INADDR_ANY;
  server.sin_port = htons(8888);

  if (bind(sock, (struct sockaddr *)&server, sizeof server) < 0) {
    perror("error binding stream socket: In Server.cpp");
  }

  listen(sock, 5);

  while (1) {
    /*Make msgSock accept new connections taking in the socket binded to 8888*/
    msgsock = accept(sock, (struct sockaddr *)0, (socklen_t *)0);

    if (msgsock == -1) {
      perror("msgsock:: error accepting new socket");
    }
    pthread_create(
        &thread /*Pass in thread adress---------*/,
        NULL /*Attribute is null------------*/,
        handleRequests /*Function which will be called from pthread*/,
        (void *)(intptr_t)msgsock) /*Argument being passed to handleRequests*/;
    printf("Thread created\n");

    pthread_join(thread, NULL);
  }
  return 0;
};

static void *handleRequests(void *arg) {

  int returnVal;
  int mainSocket;
  char buf[2048]; /* Buffer set to size 2048 */

  /*Assigning mainSocket to the argment passed in (msgSock)*/
  mainSocket = (int)(intptr_t)(arg);
  printf("Assigned mainSocket in handleRequests\n");

  read(mainSocket, buf, sizeof(buf) - 1);
  buf[sizeof(buf) - 1] = '\0';

  if (strstr(buf, "GET") != NULL) {
    std::cout << "GET called in server" << std::endl;
    formUpload.sendTCP(mainSocket);
  }

  if (strstr(buf, "POST") != NULL) {
    std::cout << "POST called in server" << std::endl;
    uploadServer.readBuffer(mainSocket);
    std::cout << "After file start" << std::endl;
    pthread_exit(0);
    close(mainSocket);
  }
  return nullptr;
};
