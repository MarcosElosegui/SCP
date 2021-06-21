#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <mpi.h>
#include "struct.h"
#include "poblacion.h"
#include "persona.h"
#include "parametros.h"

int main(int argc, char *argv[]){
    int i, j, k, l, reparto, world_rank, world_size, lista_size, size, suma_aux;
    FILE * missi;
    struct poblacion pb;
    printf("Inicializando simulación: ---------------\n");


    MPI_Init(NULL, NULL);
    MPI_Comm_rank(MPI_COMM_WORLD, &world_rank);
	MPI_Comm_size(MPI_COMM_WORLD, &world_size);

    reparto = NUM_PERSONAS/world_size;

    if(world_rank == 0){ //Hacemos que el procesador con rank 0 reparta las personas de la lista correspondientes a cada procesador.
        srand(3*world_rank);
        iniciarPoblacion(&pb, reparto, world_rank, world_size); //Iniciamos la simulacion con todos los parametros de la poblacion
        for(k=1; k<world_size; k++){
                MPI_Send(&reparto, 1, MPI_INT, k, 0, MPI_COMM_WORLD);
        }
        missi=fopen("simulacion.pos", "w");
    } else {
        srand(3*world_rank);
        MPI_Recv(&lista_size, 1, MPI_INT, 0, 0, MPI_COMM_WORLD, MPI_STATUS_IGNORE);
        iniciarPoblacion(&pb, lista_size, world_rank, world_size);
        printf("%d", pb.primero->data->id);
    }
    metricas(&pb, "a", world_size, world_rank);      //Crea las metricas de nuestra poblacion.

    clock_t t;
    
    t = clock();


    int * send_contagiado = (int *) malloc(sizeof(int)*2*reparto);
    int * recv_contagiado = (int *) malloc(sizeof(int)*2*reparto*world_size);

	int send_count[world_size];	
	int recv_count[world_size];
    int send_display[world_size];
    int recv_display[world_size];

    int * buftamano = (int *) malloc(sizeof(int)*world_size);
    int * tamanos = (int *) malloc(sizeof(int)*world_size);
    int * rbufcontagiados = (int *) malloc(sizeof(int)*world_size*2*reparto);

    int * contagiado;

    
    for(i=0; i<MAX_TIEMPO; i++){
        size = contagiados(&pb, world_rank, size, send_contagiado);
      
        for(l=0;l<world_size;l++){
            buftamano[l]=size;
            send_count[l]=1;	
            send_display[l]=0;
            recv_display[l]=l;				
			recv_count[l]=1;
		}

        MPI_Alltoallv(buftamano, send_count, send_display, MPI_INT, tamanos, recv_count, recv_display, MPI_INT ,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        int * sbufcontagiados = (int *) malloc(sizeof(int)*size*world_size);
        int * rbufcontagiados = (int *) malloc(sizeof(int)*world_size*2*reparto);

        int index=0;
        for(l=0;l<size;l+=2){
            sbufcontagiados[index]=send_contagiado[l];
            sbufcontagiados[index+1]=send_contagiado[l+1];
            index+=2;
        }

        int acumulado =0;
        for(l=0;l<world_size;l++){
            send_count[l]= size;	
            send_display[l]=0;
            recv_display[l]=acumulado;			
            acumulado += tamanos[l];		
		}
        
        MPI_Alltoallv(sbufcontagiados, send_count, send_display,MPI_INT, rbufcontagiados, tamanos, recv_display, MPI_INT ,MPI_COMM_WORLD);
        MPI_Barrier(MPI_COMM_WORLD);

        int suma_tam =0;
        for(l=0; l<world_size;l++){
            suma_tam += tamanos[l];
        }

        contagio(&pb,rbufcontagiados, suma_tam);      //Actualiza la poblacion calculando las personas que se contagian.
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
    
    if (world_rank==0)t = clock() - t;
    MPI_Reduce(&pb.sanos,&sanos,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Reduce(&pb.contagiados,&contagiaos,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Reduce(&pb.recuperados,&recuperados,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    MPI_Reduce(&pb.muertos,&fallecidos,1,MPI_INT,MPI_SUM,0,MPI_COMM_WORLD);
    metricas(&pb, "a", world_size, world_rank);      //Crea las metricas de nuestra poblacion.
    double tiempo = ((double)t)/CLOCKS_PER_SEC;
    MPI_Barrier(MPI_COMM_WORLD);

    if(world_rank==0) printf("TIEMPO FINAL: %f segundos\n", tiempo);
    MPI_Finalize();
    return 0;
}