

#include "myutils.h"

#define MAX 80
#define PORT 43454
#define SA struct sockaddr
clock_t start, end;

void transmission(int sockfd, int total_pkts, int lengthBytes, int B, char *option)
{

    FILE *fp;
    char buff[MAX];
    int n, clen;
    struct sockaddr_in cli; //socket del cliente/colector
    clen = sizeof(cli);

    //uniform
    double T = 0.0;
    double delta = 0.0;
    int cont = 0; //contador para bucle while

    //inicio de parametros para modelo uniforme
    double alpha = 0.0;
    double low_limit = 0.0;
    double max_limit = 0.0;

    B = B * pow(10, 6); //Bandwitdh en Mbps

#if (DEBUG)
    //
    printf("Número de paquetes que se generarán: %d\n", total_pkts);
    //
#endif

    packet pkts[total_pkts]; //vector que guarda todos de pkts
    packet pkt, aux;         //instancia de un pkt

    double timesVec[total_pkts]; //vector que guarda el tiempo entre paquetes

    //crear fichero .txt
    create_file(fp, total_pkts, lengthBytes, B, option);

    for (;;)
    {

        bzero(buff, MAX);
        recvfrom(sockfd, buff, sizeof(buff), 0, (SA *)&cli, &clen);
        //printf("From client %s.\n ", buff);
        if (strncmp("true", buff, 4) == 0)
        {

            //informamos longitud de pkts
            sendto(sockfd, &lengthBytes, sizeof(lengthBytes), 0, (SA *)&cli, clen);

            //Inicializando el pseudo generador de números aleatorios
            double seed = 0.0;
            srand48(time(NULL));


            float lambda = get_lambda(lengthBytes, B);

#if (DEBUG)
            printf("valor alpha aleatorio [0, 1]: %f\n", alpha);
            printf("Numero de paquetes informado...\n\n");
#endif
            start = clock();

            //parámetros para modelo uniforme
            T = get_periodTime(lambda);
            alpha = get_uniform_value();
            delta = get_delta(T, alpha);

            //intervalo [T-delta, T+ delta]
            low_limit = T - delta;
            max_limit = T + delta;

            for (;;)
            {
                if (low_limit < (max_limit / 4) || low_limit < (max_limit / 3))
                {
                    alpha = get_uniform_value();
                    delta = get_delta(T, alpha);

                    low_limit = T - delta;
                    max_limit = T + delta;
#if (DEBUG_INFO)
                    printf("delta: %.8f\n", delta);
                    printf("valor mínimo del intervalo: %.8f\n", low_limit);
                    printf("valor máximo del intervalo: %.8f\n\n", max_limit);
#endif
                }
                else
                {
                    break;
                }
            }


            while (cont < total_pkts)
            {
                ////////////////////////////////////////////////////
                if (strcmp(option, "P") == 0)
                {
                    //creación del paquete y cálculo de tiempo entre pkts
                    pkt = create_packet(lengthBytes, &aux);
                    pkt.interarrival_time = time_between_poisson(lambda);
                }
                else if (strcmp(option, "U") == 0)
                {
                    //creación del paquete y cálculo de tiempo entre pkts
                    pkt = create_packet(lengthBytes, &aux);
                    pkt.interarrival_time = time_between_uniform(T, delta);
                }
               
                //datos del paquete
                pkt.id = cont + 1;
                timesVec[cont] = pkt.interarrival_time;
                cont++;

                ///////////////////////////////////////////////////

#if (DEBUG)
                printf("\nEnviando paquete %d\n", pkt.id);
                printf("Tiempo entre paquete %d y %d: %.8f\n", pkt.id, pkt.id + 1, pkt.interarrival_time);
#endif

                ///////////////////////////////////////////////////
                //enviamos el pkt a través del socket
                sendto(sockfd, &pkt, (pkt.length_bytes_pkt), 0, (SA *)&cli, clen);

                //tiempo de espera para generar nuevo pkt (inter-arrival time)
                sleep(pkt.interarrival_time);
                seed = lrand48();
                srand48(seed);
                ///////////////////////////////////////////////////
            }

            //enviamos mensaje con 0 bytes para finalizar conexión con el colector
            sendto(sockfd, NULL, 0, 0, (SA *)&cli, clen);

            //calculando tiempo final de ejecución
            end = clock() - start;

            //rellenar fichero .txt
            output_time_file(fp, timesVec, total_pkts, lengthBytes, B, T + delta, T - delta, option);

            break;
        }
        else
        {
            printf("Impossible to transmit...\n");
            //break;
            close(sockfd);
            exit(EXIT_FAILURE);
        }
    }
}

int main(int argc, char *argv[])
{

    int total_pkts, length_pkts, bandwidth, arg;
    char *option;

    //obtenemos parametros por consola
    if (argc > 1 && argc == 5)
    {
        printf("Argumento 1: %s\n", argv[1]);
        total_pkts = atoi(argv[1]);
        printf("Argumento 2: %s\n", argv[2]);
        length_pkts = atoi(argv[2]);
        printf("Argumento 3: %s\n", argv[3]);
        bandwidth = atoi(argv[3]);
        printf("Argumento 4: %s\n", argv[4]);
        option = argv[4];
    }
    else if (argc == 4)
    {
        printf("No olvide especificar el modelo...\n");
        exit(0);
    }
    else if (argc == 3)
    {
        printf("No olvide especificar el ancho de banda (B) y el modelo de tráfico\n");
        exit(0);
    }
    else if (argc == 2)
    {
        printf("Después del num de pkts, especifique la longitud de los pkts, Bandwidth y modelo de tráfico!\n");
        exit(0);
    }
    else
    {
        printf("Por favor esepcificar los parámetros de entrada:\nnum de pkts, longitud de pkts (Bytes), Bandwitdh (Mbps) y modelo de tráfico:\nPossion --> Introduzca 'P'\nUniform --> Introduzca 'U'\n");
        exit(0);
    }

    if (length_pkts < LENGTH_PKT || length_pkts >= MTU)
    {
        printf("La longitud de los pkts debe ser como mínimo de 1000 pero menor que 1400");
        exit(0);
    }

    int sockfd;
    struct sockaddr_in servaddr;

    sockfd = socket(AF_INET, SOCK_DGRAM, 0);

    
    if (sockfd == -1)
    {
        printf("Fallo de creación del socket...\n");
        exit(0);
    }
    else
        printf("Socket creado con éxito...\n");
    bzero(&servaddr, sizeof(servaddr));

    servaddr.sin_family = AF_INET;
    servaddr.sin_addr.s_addr = htonl(INADDR_ANY);
    //servaddr.sin_addr.s_addr = inet_addr("192.168.1.110"); //--> portatil como generador
    servaddr.sin_port = htons(PORT);

    if ((bind(sockfd, (SA *)&servaddr, sizeof(servaddr))) != 0)
    {
        printf("Bind de socket fallido...\n");
        exit(0);
    }
    else
        printf("Bind de socket con éxito...\n");

    //comenzar transmisión de pkts
    transmission(sockfd, total_pkts, length_pkts, bandwidth, option);

    printf("\nLa transmisión ha tomado %ld clicks (%.6f segundos)\n", end, ((double)(end) / CLOCKS_PER_SEC));
    //printf("Transmission successfully..\n\n");

    close(sockfd);
    exit(0);
}
