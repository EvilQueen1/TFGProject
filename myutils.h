#include <stdio.h>
#include <netinet/in.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <stdlib.h>
#include <time.h>
#include <string.h>
#include <ctype.h>
#include <math.h>

#include <arpa/inet.h>
#include <netinet/in.h>
#include "unistd.h"

#define DEBUG_INFO 0
#define DEBUG_UTILS 0
#define DEBUG 1
#define LENGTH_PKT 1000
#define PADDING 50
#define MTU 1500

/*Estructura que contiene todos los campos necesarias
para definir un paquete*/
typedef struct{
  int    id;                  // packet ID
  double interarrival_time;   //tiempo entre paquetes
  char data[PADDING];         //padding
  int length_bytes_pkt;       //longitud del paquete
}packet;

//generales
packet create_packet(int numBytes, packet *pkt);
double get_uniform_value();
double get_lambda(int L, int B); 

//for possion generator
double time_between_poisson(double lambda);

//for uniform generator
double time_between_uniform(double T, double delta);
double get_uniform_limits(double T, double delta);
double get_periodTime(double lambda);
double get_delta(double T, double alpha);
double recursive_uniform(double u, double low_limit, double high_limit);

double getRange(double T, double delta);

//FILE descriptor

void create_file(FILE *fp, int total_pkts, int lengthBytes, int B, char *model);


void output_time_file(FILE *fp, double timesVec[], int total_pkts, int lengthBytes, int B,
                      double max_limit, double low_limit, char *option);


char *construct_filename(int num_pkts, int lengthBytes, int B, char *option);

double optimRange(double T, double min, double max, double d, double a);