#ifndef POBLACION_H
#define POBLACION_H

int sanos;
int contagiaos;
int recuperados;
int fallecidos;

void iniciarPoblacion(struct poblacion *pb, int reparto, int world_rank, int world_size);
void contagio(struct poblacion *pb, int *contagiados, int size);
void mortalidad(struct poblacion *pb);
void recuperacion(struct poblacion *pb);
void movimiento(struct poblacion *pb);
void incubacion(struct poblacion *pb);
void vacunarPoblacion(struct poblacion *pb);
void metricas(struct poblacion *pb, const char * modo, int world_size, int world_rank);
int contagiados(struct poblacion *pb, int world_rank, int size, int * contagio);

#endif