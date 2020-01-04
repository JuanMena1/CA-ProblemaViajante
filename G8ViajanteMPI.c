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
tour_t besttour;
mystack stack;
int **digraph; // Prof.: para matrices de enteros suele ser más recomendable usar int* que int**
int n;
int size, rank, sbuf, root;
MPI_Status status;

/*Métodos*/
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

tour_t Rec_en_profund(tour_t lista_tours[], int tamano, mystack stack, tour_t besttour)
{
    for (int i = 0; i < tamano; i++)
        push(lista_tours[i], stack);
    tour_t tour;
    while (stack->list_sz != 0)
    {
        tour = pop(stack);
        if (tour->cont == n)
        { /*Comprobamos si ya hemos visitado todas las poblaciones*/
            /*Añadimos la población 0 para acabar el tour, actualizamos el coste
            y el cont*/
            tour->pobl[tour->cont] = 0;
            tour->coste = tour->coste + digraph[tour->pobl[tour->cont - 1]][0];
            (tour->cont)++;
            if (tour->coste < besttour->coste)
            {
                besttour = tour;
            }
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

void repartirRecorridos(tour_t tour, mystack stack, tour_t besttour)
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
    for (i = 0; i < n; i++)
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
    double inicio, fin;
    tour_t tour;
    mystack stack;
    tour_t besttour;
    MPI_Datatype type_pobl;
    MPI_Init(&argc, &argv);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm comm;
    MPI_Request request;
    int *scounts = malloc(sizeof(int)*size);
    int *displs = malloc(sizeof(int)*size);

    MPI_Type_contiguous(n + 3, MPI_INT, &type_pobl);
    MPI_Type_commit(&type_pobl);
    int rbuf[100];
    int num_recv[1];

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
    int lista_repartir[(n + 3) * stack->list_sz];
    int num_enviar[2];
    int aux[n+3];
    int contador = 0;
    int receptor = 1;

    if (rank == 0)
    {
        leerMatriz(argv[1]);
        repartirRecorridos(tour, stack, besttour);
        num_enviar[0] = stack->list_sz - (size - 2) * (int)ceil((float)stack->list_sz / (float)(size-1));
        num_enviar[1] = n+3;
        MPI_Send(&num_enviar, 2, MPI_INT, size-1, 0, MPI_COMM_WORLD);

        num_enviar[0] = (int)ceil((float)stack->list_sz / (float)(size-1));
        for(int i=1;i<size-1;i++){
            MPI_Send(&num_enviar, 2, MPI_INT, i, 0, MPI_COMM_WORLD);
        }
        int vueltas_for = stack->list_sz;
        for(int i=0;i<vueltas_for;i++){
            tour = pop(stack);
            aux[0]=tour->coste;
            aux[1]=tour->cont;
            for (int j = 0; j < tour->cont; j++)
            {
                aux[2+j]=tour->pobl[j];
            }
            if(contador==num_enviar[0]){
                receptor++;
                contador=0;
            }
            MPI_Isend(&aux, n+3, MPI_INT, receptor, 0, MPI_COMM_WORLD, &request);
            contador++;
        }
    } else {
        MPI_Recv(&num_recv, 2, MPI_INT, 0 ,MPI_ANY_TAG, MPI_COMM_WORLD, &status);
        int tam_buff = num_recv[1];
        for(int i=0;i<num_recv[0];i++){
            MPI_Recv(&rbuf, tam_buff, MPI_INT, 0 ,MPI_ANY_TAG, MPI_COMM_WORLD, &status);

            tour->coste=rbuf[0];
            tour->cont=rbuf[1];
            for (int j = 0; j < tour->cont; j++)
            {
                tour->pobl[j]=rbuf[j+2];
            }
            push(tour, stack);
        }
        //printf("Proceso %d, stack:\n", rank);
        //printStack(stack);
    }
        

    /*Liberamos el espacio asignado a las estructuras y al grafo*/
    free(tour->pobl);
    free(tour);
    free(besttour->pobl);
    free(besttour);
    free(stack->list);
    free(stack);
    free(digraph);

    MPI_Finalize();
    return 0;
}
