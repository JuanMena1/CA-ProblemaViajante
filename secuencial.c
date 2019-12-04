#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include <limits.h>

//Estructuras
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

//Variables Globales
tour_t besttour;
mystack stack;
int **digraph;
int n;
int contador = 0;

//Métodos
void printTour(tour_t tour, int id){
    printf("~~~~~~Tour:%d~~~~~~\nNº poblaciones: %d\nPoblaciones: ", id, tour->cont);
    for(int i = 0; i< tour->cont; i++){
        printf("%d, ", tour->pobl[i]);
    }
    printf("\nCoste: %d\n~~~~~~~~~~~~~~~~~~\n", tour->coste);
}

void printStack(){
    printf("-----------STACK------------\n");
    printf(" - Tamaño: %d\n", stack->list_sz);
    for(int i = 0; i< stack->list_sz;i++){
        printTour(stack->list[i],i);
    }
    printf("----------------------------\n\n");
}

void push(tour_t tour){
    tour_t tour2;
    tour2 = (tour_t) malloc(sizeof(tour_struct));

    *tour2 = *tour;

    stack->list[stack->list_sz] = tour2;
    stack->list_sz = stack->list_sz + 1;
}

tour_t pop(){
    tour_t tour;
    tour = (tour_t) malloc(sizeof(tour_struct));

    stack->list_sz = stack->list_sz - 1;
    *tour = *stack->list[stack->list_sz];
    return tour;
}

int factible(tour_t tour, int poblacion){
    if((tour->coste + digraph[tour->pobl[tour->cont-1]][poblacion]) >= besttour->coste){
        return 0;
    }

    for(int i = 0; i< tour->cont;i++){
        if(tour->pobl[i] == poblacion){
            return 0;
        }
        for(int j = 0; j< tour->cont;j++){
            if(tour->pobl[i] == tour->pobl[j] && i!=j){
                return 0;
            }
        }
    }

    return 1;
}

void Rec_en_profund(tour_t tour){
    int* pobl = (int *) malloc(n*sizeof(int));
    int pob = 0;

    push(tour);
    while(stack->list_sz != 0){
        printStack();
        tour=pop();
        if(tour->cont == n){
            if(tour->coste < besttour->coste){
                besttour = tour;
            }
        }else{ 
            for(int i = n-1; i!=0 ; i--){
                if(factible(tour,i) == 1){         
                    pobl = tour->pobl;
                    pob = tour->pobl[tour->cont];
                    tour->pobl[tour->cont] = i;
                    tour->cont = tour->cont + 1;
                    tour->coste = tour->coste + digraph[tour->pobl[tour->cont-2]][tour->pobl[tour->cont-1]];
                    //printf("Poblacion que entra %d, numero ciudades %d, coste %d \n",i, tour->cont, tour->coste);
                    push(tour);
                    tour->coste = tour->coste - digraph[tour->pobl[tour->cont-2]][tour->pobl[tour->cont-1]];
                    tour->cont = tour->cont - 1;
                    tour->pobl = (int *) malloc(n*sizeof(int));
                    tour->pobl[tour->cont] = pob;
                    tour->pobl = pobl;
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

    //Leemos el fichero y lo pasamos a la variable digraph
    digraph_file = fopen(argv[1], "r");
    fscanf(digraph_file, "%d,", &n);
    
    //Reservamos memoria para el digraph
    digraph = (int**)malloc(n*sizeof(int*));
    for(i=0;i<n;i++){
       digraph[i] = (int*)malloc(n*sizeof(int*)); 
    }
    //Rellenamos el digraph con los valores del fichero
    for(i=0;i<n;i++){
        for(j=0;j<n;j++){
            fscanf(digraph_file, "%d,", &digraph[i][j]);
        }
    }
    //Reservamos espacio para tour y besttour
    //tour = malloc((n+1)*sizeof(tour));
    tour = (tour_t) malloc(sizeof(tour_struct));
    besttour = (tour_t) malloc(sizeof(tour_struct));
    stack =  (mystack) malloc(sizeof(stack_struct));
     stack->list = malloc(INT_MAX);


    //stack->list = (tour_t*) malloc((2+n*(n-3)/2)*sizeof(tour_t));
    besttour->pobl = (int *) malloc(n*sizeof(int));
    //besttour->pobl = malloc((n+1)*sizeof(int));
    
    /*tour->pobl = 0;
    tour->cont = 0;
    tour->coste = 0;*/

    besttour->pobl = 0;
    besttour->cont = 0;
    besttour->coste = INT_MAX;
    stack->list_sz = 0;

    tour->pobl = (int *) malloc(n*sizeof(int));
    tour->pobl[0] = 0;
    tour->cont = 1;
    tour->coste = 0;
    
    GET_TIME(inicio);
    Rec_en_profund(tour);
    GET_TIME(fin);

   //printTour(besttour,0);
}
