#include <sys/socket.h>
#include <netinet/in.h>
#include <string.h>
#include <assert.h>
#include <unistd.h> // close

void socket_test()
{
  /// do POSIX things
  auto sockfd = socket(AF_INET, SOCK_STREAM, 0);

  struct sockaddr_in serv_addr;
  memset(&serv_addr, 0, sizeof(serv_addr));
  serv_addr.sin_family = AF_INET;
  serv_addr.sin_port   = 6667;

  if (connect(sockfd, (struct sockaddr *) &serv_addr, sizeof(serv_addr)) < 0)
      assert(0 && "ERROR connecting");

  const char buffer[] = "KKona Amerifats KKona";

  int len = send(sockfd, buffer, strlen(buffer), 0);
  if (len < 0) assert(0 && "ERROR writing to socket");

  if (close(sockfd) < 0)
      assert(0 && "Failed to close socket");
}
