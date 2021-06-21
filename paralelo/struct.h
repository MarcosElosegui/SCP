#ifndef STRUCT_H
#define STRUCT_H

struct persona{
    int         id;
	int 		edad;
	int 		estado;             //(0) Individuo sano, (1) afectado pero sin síntomas, (2) afectado con síntomas, (3) recuperado, (4) vacunado, (5) fallecido, (6) incubando
    float       mortalidad;         //Probabilidad de morir una vez que se ha infectado.
	int 		posicion[2];        //Posición del individuo en el escenario.
    int         velocidad[2];       //Dirección y velocidad del movimiento del individuo.
    int         incubacion;         //Dias que lleva de incubacion.
    int         recuperacion;       //Dias que lleva infectado.
    int         contagiado;         //Número de porsonas que ha infectado.
};

struct nodo{
    struct nodo *sig;
	struct persona *data;
};

struct poblacion{
	int 	tamano;                 //Tamaño de la población.
    int     eje_x;                  //Tamaño eje x.
    int     eje_y;                  //Tamaño eje y.
    int     mediaEdad;              //Edad media de la población.
    int     radio;                  //Radio en la que una persona puede contagiar a otra.
    float   probContagio;           //Probabilidad de una persona de contagiarse estando dentro del radio de contagio.
    int     periodoIncubacion;      //Tiempo que tiene que pasar desde que un individuo se contagio hasta que es contagioso.
    int     periodoRecuperacion;    //Tiempo desde que un individuo se contagia hasta que se recupera.
    int     probCambioVec;          //Probabilidad de cambiar la dirección y velocidad del movimiento de un individuo.
    int     ritmo_vacunacion;       //Gente a vacunar por unidad de tiempo.
    int     a_vacunar;              //Gente a vacunar.
    int     sanos;                  //Gente sana.
    int     vacunados;              //Gente vacunada.
    int     recuperados;            //Gente recuperada del virus.
    int     muertos;                //Gente que ha muerto.
    int     contagiados;
    int     r0;                     //Capacidad de contagio.
    int     paciente_cero;          //Paciente cero de la población.
    struct  nodo *primero;          //Lista ligada para las personas.
	struct  nodo *ultimo;           //

};


struct auxiliar{
    struct  nodo *primero;          //Lista ligada para las personas.
	struct  nodo *ultimo;
};

#endif