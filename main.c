#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "struct.h"
#include "poblacion.h"
#include "persona.h"
#include "parametros.h"

int main(int argc, char *argv[]){
    int i, j;
    printf("Inicializando simulación: ---------------\n");
    struct poblacion pb;

    srand(time(NULL));

    iniciarPoblacion(&pb); //Iniciamos la simulacion con todos los parametros de la poblacion
    metricas(&pb, "w");      //Crea las metricas de nuestra poblacion.

    clock_t t;
    
    t = clock();
    for(i=0; i<MAX_TIEMPO; i++){
        contagio(&pb);      //Actualiza la poblacion calculando las personas que se contagian.
        movimiento(&pb);    //Actauliza la posición de las personas segun el vector de velocidad y cambia dicho vector si toca.
        incubacion(&pb);    //Calcula los días de incubación de un individuo en estado 6 y lo pasara si ha llegado a dichos dias.
        mortalidad(&pb);    //Corre las probabilidades de mortalidad de las personas y cambia el estado a 5 si se da el caso.
        recuperacion(&pb);  //Cuenta los días de una persona infectada para la recuperación de esta.
        for(j=0; j<pb.ritmo_vacunacion; j++){
            if(pb.a_vacunar!=0){
                vacunarPoblacion(&pb);  //Vacuna la población.
            }
        }
    }
    metricas(&pb, "a");      //Crea las metricas de nuestra poblacion.
    t = clock() - t;
    double tiempo = ((double)t)/CLOCKS_PER_SEC;
    
    printf("TIEMPO FINAL: %d lo que sea\n", tiempo);
    printf("TIEMPO FINAL: %f segundos\n", tiempo);
    return 0;
}