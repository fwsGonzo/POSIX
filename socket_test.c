#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <unistd.h> // close
#include <stdio.h>
#include <errno.h>

void fail(const char* reason)
{
  perror(reason);
  assert(0 && reason);
}

void socket_test()
{
  /// do POSIX things
  int sockfd = socket(AF_INET, SOCK_STREAM, 0);
  printf("socket() returned %d\n", sockfd);

  //in_addr shit_addr;
  //inet_aton("10.0.0.1", shit_addr);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_addr.s_addr   = 16777226;
  serv_addr.sin_port   = 6667;

  int ret;
  ret = connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr));
  if (ret < 0) fail("connect() failed");
  printf("connect() success\n");

  const char buffer[] = "KKona Amerifats KKona";

  int len = send(sockfd, buffer, strlen(buffer), 0);
  if (len < 0) fail("send() failed");
  assert(len == strlen(buffer));
  printf("send() success\n");

  char readbuf[1024];
  len = recv(sockfd, readbuf, sizeof(readbuf), 0);
  if (len < 0) fail("recv() failed");
  printf("recv(): %.*s\n", len, readbuf);

  ret = close(sockfd);
  if (ret < 0) fail("close() failed");
}
