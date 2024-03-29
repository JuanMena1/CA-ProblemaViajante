#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include <limits.h>

/*Estructuras*/
typedef struct {
    int* pobl;
    int cont;
    int coste;
} tour_struct;

typedef tour_struct *tour_t;

typedef struct {
    tour_t *list;
    int list_sz;
} stack_struct;

typedef stack_struct *mystack;

/*Variables Globales*/
tour_t besttour;
mystack stack;
int **digraph;		// Prof.: para matrices de enteros suele ser más recomendable usar int* que int**
int n;
int contador = 0;

/*Métodos*/
void printTour(tour_t tour){
    printf("~~~~~~Besttour:~~~~~~\nNº poblaciones: %d\nPoblaciones: ", tour->cont-1);
    for(int i = 0; i< tour->cont; i++){
        printf("%d ", tour->pobl[i]);
    }
    printf("\nCoste: %d\n~~~~~~~~~~~~~~~~~~\n", tour->coste);
}

void printStack(){
    printf("-----------STACK------------\n");
    printf(" - Tamaño: %d\n", stack->list_sz);
    for(int i = 0; i< stack->list_sz;i++){
        printTour(stack->list[i]);
    }
    printf("----------------------------\n\n");
}

void push(tour_t tour){
    /*Creamos un tour auxiliar que es el que metemos a la pila*/
    tour_t tour2;
    tour2 = (tour_t) malloc(sizeof(tour_struct));
    tour2->pobl = (int *) malloc((n+1)*sizeof(int));

    /*Solo copiamos el nº de poblaciones que tenga el tour en lugar de todo el vector*/
    memcpy(tour2->pobl, tour->pobl, (tour->cont)*sizeof(int));
    tour2->cont = tour->cont;
    tour2->coste = tour->coste;

    stack->list[stack->list_sz] = tour2;
    (stack->list_sz)++;
}

tour_t pop(){
    /*Creamos un tour_t y lo rellenamos con el primer tour de la pila*/
    tour_t tour;

    (stack->list_sz)--;
    tour = stack->list[stack->list_sz];	
    return tour;
}

int factible(tour_t tour, int poblacion){
    /*Vemos si el tour que vamos a meter a la pila tiene un coste menor que el besttour*/
    if((tour->coste + digraph[tour->pobl[tour->cont-1]][poblacion]) >= besttour->coste){
        return 0;
    }
    /*Comprobamos si la población se ha visitado previamente*/
    for(int i = 0; i<tour->cont;i++){
        if(tour->pobl[i] == poblacion){
            return 0;
        }
    }
    return 1;
}

void Rec_en_profund(tour_t tour){

    push(tour);
    while(stack->list_sz != 0){
        tour=pop();
        if(tour->cont == n){/*Comprobamos si ya hemos visitado todas las poblaciones*/
            /*Añadimos la población 0 para acabar el tour, actualizamos el coste
            y el cont*/
            tour->pobl[tour->cont] = 0;
            tour->coste = tour->coste + digraph[tour->pobl[tour->cont-1]][0];
            (tour->cont)++;
            if(tour->coste < besttour->coste){
                besttour = tour;
            }
        }else{ 
            for(int i = n-1; i>0 ; i--){
                if(factible(tour,i)){
                    /*Actualizamos los valores con el tour nuevo*/
                    tour->pobl[tour->cont] = i;
                    tour->coste = tour->coste + digraph[tour->pobl[tour->cont-1]][i];
                    (tour->cont)++;
                    push(tour);
                    /*Deshacemos los cambios anteriores para explorar un nuevo tour*/
                    (tour->cont)--;
                    tour->coste = tour->coste - digraph[tour->pobl[tour->cont-1]][i];
                }
            }                   
        }
    }
}

int main(int argc, char *argv[]){
    double inicio, fin;
    int i, j;
    FILE *digraph_file;
    tour_t tour;

    /*Leemos el fichero y lo pasamos a la variable digraph*/
    digraph_file = fopen(argv[1], "r");
    fscanf(digraph_file, "%d,", &n);
    
    /*Reservamos memoria para el digraph*/
    digraph = (int**)malloc(n*sizeof(int*));
    for(i=0;i<n;i++){
       digraph[i] = (int*)malloc(n*sizeof(int));
    }
    /*Rellenamos el digraph con los valores del fichero*/
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            fscanf(digraph_file, "%d,", &digraph[i][j]);
        }
    }
    fclose(digraph_file);
    /*Reservamos espacio para tour, besttour y la pila*/
    tour = (tour_t) malloc(sizeof(tour_struct));
    besttour = (tour_t) malloc(sizeof(tour_struct));
    stack =  (mystack) malloc(sizeof(stack_struct));


    stack->list = (tour_t*) malloc((2+n*(n-3)/2)*sizeof(tour_t));
    besttour->pobl = (int *) malloc((n+1)*sizeof(int));
    tour->pobl = (int *) malloc((n+1)*sizeof(int));

    /*Inicializamos tour, besttour y la pila*/
    tour->pobl[0] = 0;
    tour->cont = 1;
    tour->coste = 0;

    besttour->pobl = 0;
    besttour->cont = 1;
    besttour->coste = INT_MAX;
    stack->list_sz = 0;
    
    GET_TIME(inicio);
    Rec_en_profund(tour);
    GET_TIME(fin);

    printTour(besttour);
    printf("Tiempo de ejecución: %lfs segundos\n", fin-inicio);

    /*Liberamos el espacio asignado a las estructuras y al grafo*/
    free(tour->pobl);
    free(tour);
    free(besttour->pobl);
    free(besttour);
    free(stack->list);
    free(stack);
    free(digraph);
}
