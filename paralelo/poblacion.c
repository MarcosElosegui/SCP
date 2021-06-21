#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "persona.h"
#include "struct.h"
#include "parametros.h"
#include "poblacion.h"

void iniciarPoblacion(struct poblacion *pb, int reparto, int world_rank, int world_size){
    int i, edadmedia,ritmo, personas_a_vacunar,first, id;
    struct persona *p;
    struct nodo *act;
    struct nodo *aux;
    pb->tamano = reparto;
    pb->radio = RAD;
    edadmedia = 0;
    pb->sanos = pb->tamano;
    pb->paciente_cero = rand() % NUM_PERSONAS;

    id = world_rank*reparto;

    for(i=0; i<reparto; i++){
        act = (struct nodo*) malloc(sizeof(struct nodo));
        p = crearPersona(id);
        edadmedia += p->edad;
        if(i==pb->paciente_cero) p->estado = 1;
        if(p->id==10) p->estado = 1;
        if(p->id==30) p->estado = 1;
        printf("ID: %d, EDAD: %d, ESTADO: %d, MORTALIDAD: %.3f, POSICION: [%d, %d], VELOCIDAD[%d, %d]\n", p->id, p->edad, p->estado, p->mortalidad, p->posicion[0], p->posicion[1], p->velocidad[0], p->velocidad[1]);
        act->data = p;
        if(act->data->estado == 1 || act->data->estado == 2) pb->sanos--;
        if(i==0){
            pb->primero = act;
            pb->ultimo = act;
        } else {
            aux->sig = act;
            pb->ultimo = act;
        }
        aux = act;
        id++;
    }
    personas_a_vacunar = (INMUNIZACION*NUM_PERSONAS)/100;
    if((MAX_TIEMPO/personas_a_vacunar)==0){
        ritmo = 1;
    } else {
        ritmo = MAX_TIEMPO/personas_a_vacunar;
    }
    pb->contagiados = 1;
    pb->a_vacunar = personas_a_vacunar/world_size;
    pb->vacunados = 0;
    pb->recuperados = 0;
    pb->muertos = 0;
    pb->r0 = R0;
    pb->ritmo_vacunacion = ritmo;
    pb->eje_x = (int) sqrt(NUM_PERSONAS)+1;
    pb->eje_y = pb->eje_x;
    pb->mediaEdad = edadmedia/pb->tamano;
    pb->probContagio = CONTAGIO;
    pb->periodoIncubacion = INCUBACION;
    pb->periodoRecuperacion = RECUPERACION;
    pb->probCambioVec = CAMBIO_VEC;
}


void contagio(struct poblacion *pb, int *contagiados, int size){
    int i,j,infectado;
    struct nodo * aux;
    for(i=0; i<(size/2);i++){
            aux = pb->primero;
            for(j=0; j<pb->tamano; j++){ //Comparamos persona que puede infectar con las demas de la lista
                if(aux->data->estado == 0){ //Solo se podran contagiar los individuos con estado 0.  && act->data->contagiado != R0
                    if(((aux->data->posicion[i] - contagiados[i])<=RAD) || ((aux->data->posicion[1] - contagiados[i+1])<=RAD)){ //Comprobamos que la persona este en rango de infeccion.
                        infectado = rand() % 100; //Sacamos un numero aleatorio, el cual indicara si el individuo dentro del rango se infecta.
                        if(infectado <= CONTAGIO){
                            aux->data->estado = 6; //Cambiamos estado a un estado que nos indica que esta infectado, pero en periodo de incubación.
                            pb->sanos--;
                            pb->contagiados++;
                            printf("¡LA PERSONA %d HA SIDO INFECTADA!\nEDAD: %d, ESTADO: %d, MORTALIDAD: %.3f, POSICION: [%d, %d], VELOCIDAD[%d, %d]\n", aux->data->id, aux->data->edad, aux->data->estado, aux->data->mortalidad, aux->data->posicion[0], aux->data->posicion[1], aux->data->velocidad[0], aux->data->velocidad[1]);
                        }            
                    }
                }
                aux = aux->sig;
            }
            i++;
    }
}

int contagiados(struct poblacion *pb, int world_rank, int  size, int * contagio){
    int i;
    int indice = 0;
    struct nodo *act;
    act = pb->primero;  
    for(i=0; i<pb->tamano; i++){
        if(act->data->estado == 1 || act->data->estado == 2){
            contagio[indice] = act->data->posicion[0];
            contagio[indice+1] = act->data->posicion[1];
            indice = indice+2;
        }
        act = act->sig;
    }
    size=indice;
    return size;
}

void mortalidad(struct poblacion *pb){
    int i;
    float mort;
    struct nodo *act;
    act = pb->primero;

    for(i=0; i<pb->tamano; i++){
        if((act->data->estado == 1) || (act->data->estado == 2)){
            mort = ((float) rand() / (float) (RAND_MAX)) * 1.0;
            if(mort<=act->data->mortalidad){
                act->data->estado = 5;
                pb->muertos++;
                pb->contagiados--;
                printf("¡LA PERSONA %d HA MUERTO!\n", act->data->id);
            }
        }
        act = act->sig;
    }
}

void recuperacion(struct poblacion *pb){
    int i;
    struct nodo *act;
    act = pb->primero;

    for(i=0; i<pb->tamano; i++){
        if(((act->data->estado == 1) || (act->data->estado == 2)) && act->data->recuperacion >= RECUPERACION){
           act->data->estado = 3;
           pb->recuperados++;
           pb->sanos++;
           pb->contagiados--;
           printf("¡LA PERSONA %d SE HA RECUPERADO!\n", act->data->id);
        } else if((act->data->estado == 1) || (act->data->estado == 2)){
            act->data->recuperacion++;
        }
        act = act->sig;
    }
}


void movimiento(struct poblacion *pb){
    int i, cambio;
    struct nodo *act;
    act = pb->primero;

    for(i=0; i<pb->tamano; i++){
        if(((act->data->posicion[0] + act->data->velocidad[0]) < pb->eje_x) && ((act->data->posicion[1] + act->data->velocidad[1]) < pb->eje_y)){
            act->data->posicion[0] = act->data->posicion[0] + act->data->velocidad[0];
            act->data->posicion[1] = act->data->posicion[1] + act->data->velocidad[1];
        } else if (((act->data->posicion[0] + act->data->velocidad[0]) < pb->eje_x) && ((act->data->posicion[1] + act->data->velocidad[1]) >= pb->eje_y)){
            act->data->posicion[0] = act->data->posicion[0] + act->data->velocidad[0];
            act->data->posicion[1] = act->data->posicion[1] - act->data->velocidad[1];
        } else if (((act->data->posicion[0] + act->data->velocidad[0]) >= pb->eje_x) && ((act->data->posicion[1] + act->data->velocidad[1]) < pb->eje_y)){
            act->data->posicion[0] = act->data->posicion[0] - act->data->velocidad[0];
            act->data->posicion[1] = act->data->posicion[1] + act->data->velocidad[1];
        } else {
            act->data->posicion[0] = act->data->posicion[0] - act->data->velocidad[0];
            act->data->posicion[1] = act->data->posicion[1] - act->data->velocidad[1];
        }
        cambio = rand() % 100;
        if(cambio <= CAMBIO_VEC){
            act->data->velocidad[0] = rand() % VELOCIDAD;
            act->data->velocidad[1] = rand() % VELOCIDAD;
        }
        act = act->sig;
    }
}

void incubacion(struct poblacion *pb){
    int i;
    struct nodo *act;
    act = pb->primero;

    for(i=0; i<pb->tamano; i++){
        if(act->data->estado == 6){
            if(act->data->incubacion==INCUBACION){
                act->data->incubacion=0;
                act->data->estado=1;
                pb->contagiados++;
                pb->sanos--;
                printf("¡LA PERSONA %d SE HA VUELTO INFECCIOSA!\n", act->data->id);
            } else {
                act->data->incubacion++;
            }
        }
        act = act->sig;
    }
}

void vacunarPoblacion(struct poblacion *pb){
    int i;
    struct nodo *act;
    act = pb->primero;

    for(i=0; i<pb->tamano; i++){
        if(act->data->estado == 0 || act->data->estado == 3){
            act->data->estado = 4;
            pb->vacunados++;
            pb->a_vacunar--;
            printf("¡LA PERSONA %d HA SIDO INMUNIZADA!\n", act->data->id);
            break;
        }
        act = act->sig;
    }
}

void metricas(struct poblacion *pb, const char * modo, int world_size, int world_rank){
    int i, j;
    struct nodo *act;
    act = pb->primero;
    FILE *pos;
    FILE *metric;

    for(j=0; j<world_size; j++){
        if(world_rank==j){
            pos = fopen("simulacion.pos", modo);

            for(i=0; i<pb->tamano; i++){
                fprintf(pos, "ID: %d, ESTADO: %d, POSICION: [%d, %d]\n", act->data->id, act->data->estado, act->data->posicion[0], act->data->posicion[1]);
                act = act->sig;
            }

            fclose(pos);
        }
    }
    if(world_rank==0){
        metric = fopen("simulacion.metricas", "w");
        fprintf(metric, "SANOS: %d \nCONTAGIADOS: %d \nRECUPERADOS: %d \nFALLECIDOS: %d \n\n\n", sanos, contagiaos, recuperados, fallecidos);
        fclose(metric);
    }
}