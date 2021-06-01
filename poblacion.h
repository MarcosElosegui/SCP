#ifndef POBLACION_H
#define POBLACION_H


void iniciarPoblacion(struct poblacion *pb);
void contagio(struct poblacion *pb);
void mortalidad(struct poblacion *pb);
void recuperacion(struct poblacion *pb);
void movimiento(struct poblacion *pb);
void incubacion(struct poblacion *pb);
void vacunarPoblacion(struct poblacion *pb);
void metricas(struct poblacion *pb, const char * restrict modo);

#endif