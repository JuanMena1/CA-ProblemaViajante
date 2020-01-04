#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include <limits.h>
#include "mpi.h"
#include <math.h>

/*Estructuras*/
typedef struct
{
    int *pobl;
    int cont;
    int coste;
} tour_struct;

typedef tour_struct *tour_t;

typedef struct
{
    tour_t *list;
    int list_sz;
} stack_struct;

typedef stack_struct *mystack;

/*Variables Globales*/
int **digraph; // Prof.: para matrices de enteros suele ser más recomendable usar int* que int**
int n;
int size, rank;
MPI_Status status;

/*Métodos*/
tour_t compararTour(tour_t t1, tour_t t2){
    if (t1->coste < t2->coste)
    {
        *t2 = *t1;
    }
    return t2;
}

void tourAArray(tour_t tour, int arr[]){
    arr[0]=tour->coste;
    arr[1]=tour->cont;
    for (int j = 0; j < tour->cont; j++)
    {
        arr[2+j]=tour->pobl[j];
    }
}

tour_t arrayATour(tour_t tour, int arr[]){
    tour->coste=arr[0];
    tour->cont=arr[1];
    for (int j = 0; j < tour->cont; j++)
    {
        tour->pobl[j]=arr[j+2];
    }
    return tour;
}

void printTour(tour_t tour)
{
    printf("~~~~~~Besttour:~~~~~~\nNº poblaciones: %d\nPoblaciones: ", tour->cont - 1);
    for (int i = 0; i < tour->cont; i++)
    {
        printf("%d ", tour->pobl[i]);
    }
    printf("\nCoste: %d\n~~~~~~~~~~~~~~~~~~\n", tour->coste);
}

void printStack(mystack stack)
{
    printf("-----------STACK------------\n");
    printf(" - Tamaño: %d\n", stack->list_sz);
    for (int i = 0; i < stack->list_sz; i++)
    {
        printTour(stack->list[i]);
    }
    printf("----------------------------\n\n");
}

void push(tour_t tour, mystack stack)
{
    /*Creamos un tour auxiliar que es el que metemos a la pila*/
    tour_t tour2;
    tour2 = (tour_t)malloc(sizeof(tour_struct));
    tour2->pobl = (int *)malloc((n + 1) * sizeof(int));

    /*Solo copiamos el nº de poblaciones que tenga el tour en lugar de todo el vector*/
    memcpy(tour2->pobl, tour->pobl, (tour->cont) * sizeof(int));
    tour2->cont = tour->cont;
    tour2->coste = tour->coste;

    stack->list[stack->list_sz] = tour2;
    (stack->list_sz)++;
}

tour_t pop(mystack stack)
{
    /*Creamos un tour_t y lo rellenamos con el primer tour de la pila*/
    tour_t tour;

    (stack->list_sz)--;
    tour = stack->list[stack->list_sz];
    return tour;
}

int factible(tour_t tour, int poblacion, tour_t besttour)
{
    /*Vemos si el tour que vamos a meter a la pila tiene un coste menor que el besttour*/
    if ((tour->coste + digraph[tour->pobl[tour->cont - 1]][poblacion]) >= besttour->coste)
    {
        return 0;
    }
    /*Comprobamos si la población se ha visitado previamente*/
    for (int i = 0; i < tour->cont; i++)
    {
        if (tour->pobl[i] == poblacion)
        {
            return 0;
        }
    }
    return 1;
}

tour_t Rec_en_profund(mystack stack, tour_t besttour)
{
    tour_t tour;
    while (stack->list_sz != 0)
    {
        tour = pop(stack);
        if (tour->cont == n)
        {   /*Comprobamos si ya hemos visitado todas las poblaciones*/
            /*Añadimos la población 0 para acabar el tour, actualizamos el coste y el cont*/
            tour->pobl[tour->cont] = 0;
            tour->coste = tour->coste + digraph[tour->pobl[tour->cont - 1]][0];
            (tour->cont)++;
            besttour=compararTour(tour, besttour);
        }
        else
        {
            for (int i = n - 1; i > 0; i--)
            {
                if (factible(tour, i, besttour))
                {
                    /*Actualizamos los valores con el tour nuevo*/
                    tour->pobl[tour->cont] = i;
                    tour->coste = tour->coste + digraph[tour->pobl[tour->cont - 1]][i];
                    (tour->cont)++;
                    push(tour, stack);
                    /*Deshacemos los cambios anteriores para explorar un nuevo tour*/
                    (tour->cont)--;
                    tour->coste = tour->coste - digraph[tour->pobl[tour->cont - 1]][i];
                }
            }
        }
    }
    return besttour;
}

void obtenerRecorridos(tour_t tour, mystack stack, tour_t besttour)
{
    push(tour, stack);
    while (stack->list_sz < size)
    {
        tour = pop(stack);
        
        for (int i = n - 1; i > 0; i--)
        {
            if (factible(tour, i, besttour))
            {
                /*Actualizamos los valores con el tour nuevo*/
                tour->pobl[tour->cont] = i;
                tour->coste = tour->coste + digraph[tour->pobl[tour->cont - 1]][i];
                (tour->cont)++;
                push(tour, stack);
                /*Deshacemos los cambios anteriores para explorar un nuevo tour*/
                (tour->cont)--;
                tour->coste = tour->coste - digraph[tour->pobl[tour->cont - 1]][i];
            }
        }
    }
}

void leerMatriz(char *nombre_archivo)
{
    FILE *digraph_file;
    int i, j;
    /*Leemos el fichero y lo pasamos a la variable digraph*/
    digraph_file = fopen(nombre_archivo, "r");
    fscanf(digraph_file, "%d,", &n);

    /*Reservamos memoria para el digraph*/
    digraph = (int **)malloc(n * sizeof(int *));
    for (int i = 0; i < n; i++)
    {
        digraph[i] = (int *)malloc(n * sizeof(int));
    }

    /*Rellenamos el digraph con los valores del fichero*/
    for (i = 0; i < n; i++)
    {
        for (j = 0; j < n; j++)
        {
            fscanf(digraph_file, "%d,", &digraph[i][j]);
        }
    }
    fclose(digraph_file);
}

int main(int argc, char *argv[])
{
    double inicio, fin, suma;
    tour_t tour,besttour;
    mystack stack;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm comm;

    inicio = MPI_Wtime();
    if (rank == 0)
    {
        leerMatriz(argv[1]);
    }

    MPI_Bcast(&n,1,MPI_INT,0,MPI_COMM_WORLD); //Enviamos el número de problaciones a todos los procesos

    fin = MPI_Wtime();
    suma = fin-inicio;

    /*Reservamos memoria para el digraph en los procesos que no han leido la matriz*/
    if(rank != 0){
        digraph = (int **)malloc(n * sizeof(int *));
        for (int i = 0; i < n; i++)
        {
            digraph[i] = (int *)malloc(n * sizeof(int));
        }
    }

    inicio = MPI_Wtime();

    for(int i = 0; i< n; i++){ //Enviamos el grafo de poblaciones a los procesos que no lo han leido
        for(int j = 0;j<n;j++){
            MPI_Bcast(&digraph[i][j],1,MPI_INT,0,MPI_COMM_WORLD);
        }
    }

    /*Reservamos espacio para tour, besttour y la pila*/
    tour = (tour_t)malloc(sizeof(tour_struct));
    besttour = (tour_t)malloc(sizeof(tour_struct));
    stack = (mystack)malloc(sizeof(stack_struct));

    stack->list = (tour_t *)malloc((2 + n * (n - 3) / 2) * sizeof(tour_t));
    besttour->pobl = (int *)malloc((n + 1) * sizeof(int));
    tour->pobl = (int *)malloc((n + 1) * sizeof(int));

    /*Inicializamos tour, besttour y la pila*/
    tour->pobl[0] = 0;
    tour->cont = 1;
    tour->coste = 0;

    besttour->pobl = 0;
    besttour->cont = 1;
    besttour->coste = INT_MAX;
    stack->list_sz = 0;

    //Declaramos variables para paralelizar
    int num_env, num_recv;
    int sbuf[n+3],rbuf[n+3];
    int contador = 0;
    int receptor = 1;

    if (rank == 0)
    {
        obtenerRecorridos(tour, stack, besttour); //Recorremos el while hasta que tenemos tantos o más tour como procesos
        num_env = stack->list_sz - (size - 2) * (int)ceil((float)stack->list_sz / (float)(size-1));
        MPI_Send(&num_env, 1, MPI_INT, size-1, 0, MPI_COMM_WORLD);

        num_env = (int)ceil((float)stack->list_sz / (float)(size-1));
        for(int i=1;i<size-1;i++){ //Enviamos el numero de tour que tocan a cada proceso
            MPI_Send(&num_env, 1, MPI_INT, i, 0, MPI_COMM_WORLD);
        }

        int vueltas_for = stack->list_sz;
        for(int i=0;i<vueltas_for;i++){ //Enviamos a cada proceso sus tour
            tour = pop(stack);
            tourAArray(tour, sbuf);
            if(contador==num_env){
                receptor++;
                contador=0;
            }
            MPI_Send(&sbuf, n+3, MPI_INT, receptor, 0, MPI_COMM_WORLD);
            contador++;
        }

        //Recibimos los besttour de cada proceso y los comparamos
        for(int i = 1; i<size;i++){
            MPI_Recv(&rbuf, n+3, MPI_INT, i ,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
            tour = arrayATour(tour, rbuf);
            besttour = compararTour(tour, besttour);
        }
        fin = MPI_Wtime();
        suma = suma + (fin-inicio);
        printTour(besttour);
        printf("El programa paralelo tarda %lfs.\n", suma);
    } else {
        MPI_Recv(&num_recv, 1, MPI_INT, 0 ,MPI_ANY_TAG, MPI_COMM_WORLD, &status); //Recibimos cuantos tour le tocan a cada proceso
        for(int i=0;i<num_recv;i++){
            MPI_Recv(&rbuf, n+3, MPI_INT, 0 ,MPI_ANY_TAG, MPI_COMM_WORLD, &status); //Recibimos los tour
            tour = arrayATour(tour, rbuf);
            push(tour, stack);
        }

        besttour = Rec_en_profund(stack,besttour); //Ejecutamos el algoritmo
        tourAArray(besttour,sbuf);
        MPI_Send(&sbuf, n+3, MPI_INT, 0, 0, MPI_COMM_WORLD); //Enviamos los besttour de cada proceso al 0
    }
        
    /*Liberamos el espacio asignado a las estructuras y al grafo*/
    
    free(tour->pobl);
    free(tour);
    free(stack->list);
    free(stack);
    free(digraph);
    
    MPI_Finalize();
    return 0;
}