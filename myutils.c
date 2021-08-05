

#include "myutils.h"

#define FILENAME_P "timeValuesPoisson_"
#define FILENAME_U "timeValuesUniform_"

#define FILENAME_D "timeValuesDet_"

//Crear packets de tamaño numBytes
packet create_packet(int numBytes, packet *pkt)
{

  pkt = (packet *)calloc(numBytes, sizeof(int));
  if (pkt == NULL)
  {
    exit(0);
  }

  pkt->length_bytes_pkt = numBytes;

  return *pkt;
}

//funcion que calcula valor aleatorio uniforme
double get_uniform_value()
{
  static double d = 0.0;

  d = drand48();
#if (DEBUG_UTILS)
  printf("valor uniforme d: %f\n", d);
#endif

  return (d);
}

//funcion para calcular el valor de lambda
double get_lambda(int L, int B)
{
#if (DEBUG_UTILS)
  printf("Longitud en bytes: %d\n", L);
  printf("Ancho de banda: %d\n", B);       
#endif

  L = L * 8; //Convirtiendo bytes a bits
  return (B / L);
}

/*funcion que calcula el tiempo entre paquetes
para el modelo Poisson*/
double time_between_poisson(double lambda)
{

  static double u = 0.0;
  static double time_ = 0.0;

  u = get_uniform_value();

  time_ = -log(u) / lambda;

  return time_;
}

////////// FUNCIONES PARA EL MODELO UNIFORME ///////////
double get_periodTime(double lambda)
{

  double T = 0.0;

  T = 1 / lambda;

  return (T);
}

double get_delta(double T, double alpha)
{

  double delta = 0.0;
  delta = alpha * T;

  return (delta);
}

double get_uniform_limits(double T, double delta)
{

  double low_limit;
  double high_limit;

  low_limit = T - delta;
  high_limit = T + delta;

  return (low_limit, high_limit);
}

double time_between_uniform(double T, double delta)
{

  //valor random uniforme
  double urandval = drand48();

  //Calculo de limites del rango [T-delta, T+delta]
  double low_limit;
  double max_limit;
  low_limit = T - delta;
  max_limit = T + delta;

#if (DEBUG)
  printf("T+Delta: %.8f\n", max_limit);
  printf("T-Delta: %.8f\n", low_limit);
#endif

  //////Definiendo valor dentro de un rango definido//////

  //Parte 1
  double result = ceil(urandval / low_limit);
  double mod = urandval - (double)(result)*low_limit;

  double uniformValueModel = mod - low_limit + (max_limit);
  
#if (DEBUG_INFO)
  printf("valor uniforme: %.8f\n", uniformValueModel);
#endif

  //Parte 2
  /*Verificar que el valor uniforme calculado está dentro del rango
  definido*/
  uniformValueModel = recursive_uniform(uniformValueModel, low_limit, max_limit);

  return uniformValueModel;
}

/*Función que define el valor final del tiempo uniforme
dentro del intervalo [T-delta, T + delta]*/
double recursive_uniform(double value, double low_limit, double high_limit)
{
  if (value > high_limit)
  {
    return (recursive_uniform(value - (low_limit / 11), low_limit, high_limit));
  }

  else if (value < low_limit)
  {
    return (recursive_uniform(value + (high_limit / 2), low_limit, high_limit));
  }
  else
  {
    return (value);
  }
}

double getRange(double T, double delta)
{
  double low_limit = T - delta;
  double high_limit = T + delta;

#if (DEBUG_INFO)
  printf("T+Delta: %.8f\n", high_limit);
  printf("T-Delta: %.8f\n", low_limit);
#endif

  return (low_limit, high_limit);
}

/////// FUNCIONES PARA LA MANIPULACIÓN DE FICHEROS   //////////

/*Función para crear un fichero .txt de la forma:
    timeValuesModelo_#pkts_longitudBytes_Bandwidth.txt
*/
void create_file(FILE *fp, int total_pkts, int lengthBytes, int B, char *model)
{
  char *buffer;
  buffer = construct_filename(total_pkts, lengthBytes, B, model);

  fp = fopen(buffer, "w");
  if (fp == NULL)
  {
    fputs("File error", stderr);
    exit(1);
  }

  fclose(fp);

  free(buffer);
}

void output_time_file(FILE *fp, double timesVec[], int total_pkts, int lengthBytes, int B,
                      double max_limit, double low_limit, char *option)
{

  char *buffer;
  buffer = construct_filename(total_pkts, lengthBytes, B, option);

  fp = fopen(buffer, "a");
  if (fp == NULL)
  {
    fputs("File error", stderr);
    exit(1);
  }
  else
  {

    if (strcmp(option, "U") == 0)
    {
      fprintf(fp, "%.8f\n", max_limit);
      fprintf(fp, "%.8f\n", low_limit);
    }

    for (int i = 0; i < total_pkts; i++)
    {
      fprintf(fp, "%.8f\n", timesVec[i]);
    }

    free(buffer);
    fclose(fp);
  }
}

/*Función para definir el nombre especifico de cada fichero que se genera
dependiendo del modelo de tráfico*/
char *construct_filename(int total_pkts, int length_bytes, int B, char *option)
{

  char *filename;
  if (strcmp(option, "P") == 0)
  {
    filename = FILENAME_P;
  }
  else if (strcmp(option, "U") == 0)
  {
    filename = FILENAME_U;
  }

  else
  {
    filename = FILENAME_D;
  }

  char *ext = ".txt";
  //num de pkts de int a string
  int length_num_pkts = snprintf(NULL, 0, "%d", total_pkts);
  char *num_string1 = malloc(length_num_pkts + 1);
  snprintf(num_string1, length_num_pkts + 1, "%d", total_pkts);

  //length en bytes de pkts a string
  int length_bytes_pkt = snprintf(NULL, 0, "%d", length_bytes);
  char *num_string2 = malloc(length_bytes_pkt + 1);
  snprintf(num_string2, length_bytes_pkt + 1, "%d", length_bytes);

  //Bandwidth a string
  int length_B_pkt = snprintf(NULL, 0, "%d", B);
  char *num_string3 = malloc(length_B_pkt + 1);
  snprintf(num_string3, length_B_pkt + 1, "%d", B);

  char *buffer = malloc(strlen(filename) + strlen(num_string1) + strlen("_") + strlen(num_string2) +
                        strlen("_") + strlen(num_string3) + strlen(ext) + 1);

  if (buffer == NULL)
  {
    // Out of memory.
    exit(0);
  }
  else
  {
    strcpy(buffer, filename);
    strcat(buffer, num_string1);
    strcat(buffer, "_");
    strcat(buffer, num_string2);
    strcat(buffer, "_");
    strcat(buffer, num_string3);
    strcat(buffer, ext);
  }

  return buffer;
}

double optimRange(double T, double min, double max, double d, double a)
{
  for (;;)
  {
    if (min < (max / 4) || min < (max / 3))
    {
      a = get_uniform_value();
      d = get_delta(T, a);

      min = T - d;
      max = T + d;
      printf("delta: %.8f\n", d);
      printf("low limit: %.8f\n", min);
      printf("max limit: %.8f\n\n", max);
    }
    else
    {
      break;
    }
  }

  return(min, max);
}