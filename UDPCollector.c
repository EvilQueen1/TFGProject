/*#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include "unistd.h"*/

#include "myutils.h"

#define MAX 80
#define PORT 43454
#define SA struct sockaddr


int main()
{

  char buff[] = "true"; //variable definida para indicarle al generador que inicie la transmisión

  clock_t start, end;
  int sockfd, len, n = 0;
  int total_pkts, length_bytes;
  struct sockaddr_in servaddr;
  sockfd = socket(AF_INET, SOCK_DGRAM, 0);

  if (sockfd == -1)
  {
    printf("Fallo de creación del socket...\n");
    exit(0);
  }
  else
    printf("Socket creado con éxito..\n");
  bzero(&servaddr, sizeof(len));

  ////////////////////////////////////////////////////

  //establecemos valores de las variables necesarias para conectar el socket
  servaddr.sin_family = AF_INET;
  //servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
  servaddr.sin_addr.s_addr = inet_addr("192.168.1.110"); //--> IP del generador
  servaddr.sin_port = htons(PORT);
  len = sizeof(servaddr);

  ///////////////////////////////////////////////////

  /*si el socket se ha creado correctamente, le notificamos al generador para
    que comience la transmision de pkts*/
  sendto(sockfd, buff, sizeof(buff), 0, (SA *)&servaddr, len);

  if (strncmp("true", buff, 4) == 0)
  {

    //informando desde el generador longitud de los pkts
    recvfrom(sockfd, &length_bytes, sizeof(length_bytes), 0, (SA *)&servaddr, &len);

    packet pkt;
    start = clock();

    int cont = 0;
    while (recvfrom(sockfd, &pkt, (sizeof(pkt)), 0, (SA *)&servaddr, &len) > 0)
    {
#if (DEBUG)
      printf("Paquete recibido del servidor: %d. Longitud del pkt: %d. tiempo entre pkts: %.8f seg\n", pkt.id, length_bytes, pkt.interarrival_time);
      cont++;
#endif
    }

    end = clock() - start;

    printf("\nLa transmisión ha tomado %ld clicks (%.6f segundos)\n", end, ((float)end) / CLOCKS_PER_SEC);
    printf("\nNúmero de paquetes recibidos: %d\n\n", cont);
    close(sockfd);
    exit(EXIT_SUCCESS);
  }
  else
  {
    printf("The transmission couldn't be defined\n");
    close(sockfd);
    exit(EXIT_FAILURE);
  }

  return 0;
}
