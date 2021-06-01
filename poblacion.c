#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "persona.h"
#include "struct.h"
#include "parametros.h"

void iniciarPoblacion(struct poblacion *pb){
    int i, edadmedia,ritmo, personas_a_vacunar;
    struct persona *p;
    struct nodo *act;
    struct nodo *aux;
    pb->tamano = NUM_PERSONAS;
    pb->radio = RAD;
    edadmedia = 0;
    pb->sanos = NUM_PERSONAS;
    pb->paciente_cero = rand() % NUM_PERSONAS;

    for(i=0; i<NUM_PERSONAS; i++){
        act = (struct nodo*) malloc(sizeof(struct nodo));
        p = crearPersona(i);
        edadmedia = edadmedia + p->edad;
        if(i==pb->paciente_cero) p->estado = 1;
        printf("ID: %d, EDAD: %d, ESTADO: %d, MORTALIDAD: %.3f, POSICION: [%d, %d], VELOCIDAD[%d, %d]\n", p->id, p->edad, p->estado, p->mortalidad, p->posicion[0], p->posicion[1], p->velocidad[0], p->velocidad[1]);
        act->data = p;
        if(act->data->estado == 1 || act->data->estado == 2) pb->sanos--;
        if(p->id==0){
            pb->primero = act;
            pb->ultimo = act;
        } else {
            aux->sig = act;
            pb->ultimo = act;
        }
        aux = act;
    }
    personas_a_vacunar = (INMUNIZACION*NUM_PERSONAS)/100;
    if((MAX_TIEMPO/personas_a_vacunar)==0){
        ritmo = 1;
    } else {
        ritmo = MAX_TIEMPO/personas_a_vacunar;
    }
    pb->contagiados = 1;
    pb->a_vacunar = personas_a_vacunar;
    pb->vacunados = 0;
    pb->recuperados = 0;
    pb->muertos = 0;
    pb->r0 = R0;
    pb->ritmo_vacunacion = ritmo;
    pb->eje_x = (int) sqrt(NUM_PERSONAS)+1;
    pb->eje_y = pb->eje_x;
    pb->mediaEdad = edadmedia/NUM_PERSONAS;
    pb->probContagio = CONTAGIO;
    pb->periodoIncubacion = INCUBACION;
    pb->periodoRecuperacion = RECUPERACION;
    pb->probCambioVec = CAMBIO_VEC;
}

void contagio(struct poblacion *pb){
    int i,j,infectado;
    struct nodo *act;
    act = pb->primero;
    struct nodo *aux = act->sig;
    for(i=0; i<pb->tamano-1; i++){
        if(act->data->estado == 1 || act->data->estado == 2){ //Persona que va a infectar
            for(j=i; j<pb->tamano-1; j++){ //Comparamos persona que puede infectar con las demas de la lista
                //printf("%d\t", act->data->id);
                //printf("%d\n", aux->data->id);
                if(aux->data->estado == 0 && act->data->contagiado != R0){ //Solo se podran contagiar los individuos con estado 0.
                    if(((aux->data->posicion[0] - act->data->posicion[0])<=RAD) || ((aux->data->posicion[1] - act->data->posicion[1])<=RAD)){ //Comprobamos que la persona este en rango de infeccion.
                        infectado = rand() % 100; //Sacamos un numero aleatorio, el cual indicara si el individuo dentro del rango se infecta.
                        if(infectado <= CONTAGIO){
                            //act->data->contagiado++;
                            aux->data->estado = 6; //Cambiamos estado a un estado que nos indica que esta infectado, pero en periodo de incubación.
                            pb->sanos--;
                            pb->contagiados++;
                            printf("¡LA PERSONA %d HA SIDO INFECTADA!\nEDAD: %d, ESTADO: %d, MORTALIDAD: %.3f, POSICION: [%d, %d], VELOCIDAD[%d, %d]\n", aux->data->id, aux->data->edad, aux->data->estado, aux->data->mortalidad, aux->data->posicion[0], aux->data->posicion[1], aux->data->velocidad[0], aux->data->velocidad[1]);
                        }                      
                    }
                }
                aux = aux->sig;
            }
        }
        act = act->sig;
        aux = act->sig;
    }
}

void mortalidad(struct poblacion *pb){
    int i;
    float mort;
    struct nodo *act;
    act = pb->primero;

    for(i=0; i<pb->tamano; i++){
        if((act->data->estado == 1) || (act->data->estado == 2)){
            mort = ((float) rand() / (float) (RAND_MAX)) * 1.0;
            //printf("%f\t", muerto);
            //printf("%f\n", mort);
            if(mort<=act->data->mortalidad){
                act->data->estado = 5;
                pb->muertos++;
                pb->contagiados--;
                printf("¡LA PERSONA %d HA ESTIRADO LA PATA!\n", act->data->id);
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

void metricas(struct poblacion *pb, const char * restrict modo){
    int i;
    struct nodo *act;
    act = pb->primero;
    FILE *pos;
    FILE *metric;

    pos = fopen("simulacion.pos", modo);
    metric = fopen("simulacion.metricas", modo);

    for(i=0; i<pb->tamano; i++){
        fprintf(pos, "ID: %d, ESTADO: %d, POSICION: [%d, %d]\n", act->data->id, act->data->estado, act->data->posicion[0], act->data->posicion[1]);
        act = act->sig;
    }
    fclose(pos);
    fprintf(metric, "SANOS: %d \nCONTAGIADOS: %d \nRECUPERADOS: %d \nFALLECIDOS: %d \nR0: %d\n\n", pb->sanos, pb->contagiados, pb->recuperados, pb->muertos, pb->r0);
    fclose(metric);
}
