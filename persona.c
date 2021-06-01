#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include "struct.h"
#include "poblacion.h"
#include "parametros.h"


struct persona* crearPersona(int i){
    struct persona *p;
    p = (struct persona*)malloc(sizeof(struct persona));
    p->id = i;
    p->edad = rand() % EDAD_MAX;
    p->estado = 0;
    if((p->edad >= 0) && (p->edad < 10)){
        p->mortalidad = 0.0;
    } else if((p->edad >= 10) && (p->edad < 40)){
        p->mortalidad = 0.002;
    } else if((p->edad >= 40) && (p->edad < 50)){
        p->mortalidad = 0.004;
    } else if((p->edad >= 50) && (p->edad < 60)){
        p->mortalidad = 0.013;
    } else if((p->edad >= 60) && (p->edad < 70)){
        p->mortalidad = 0.036;
    } else if((p->edad >= 70) && (p->edad < 80)){
        p->mortalidad = 0.08;
    } else if(p->edad >= 80){
        p->mortalidad = 0.148;
    }
    p->posicion[0] = rand() % ((NUM_PERSONAS/10)-1);
    p->posicion[1] = rand() % ((NUM_PERSONAS/10)-1);
    p->velocidad[0] = rand() % VELOCIDAD;
    p->velocidad[1] = rand() % VELOCIDAD;
    p->incubacion = 0;
    p->recuperacion = 0;
    return p;
}