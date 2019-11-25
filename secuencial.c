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

//Métodos
void push(tour_t tour){
    stack->list[stack->list_sz] = tour;
    stack->list_sz = stack->list_sz + 1;
}

void pop(tour_t tour){
    stack->list_sz = stack->list_sz - 1;
    tour = stack->list[stack->list_sz];
}

int factible(tour_t tour, int poblacion){
    if((tour->coste + digraph[tour->pobl[tour->cont-1]][poblacion]) >= besttour->coste){
        return 0;
    }
    for(int i = 0; i< tour->cont;i++){
        if(tour->pobl[i] == poblacion){
            return 0;
        }
    }
    return 1;
}

void printTour(tour_t tour){
    for(int i = 0; i< tour->cont; i++){
        printf("Población: %d ", tour->pobl[i]);
    }
    printf("\nCoste: %d\n", tour->coste);
}

void printStack(){
    for(int i = 0; i< stack->list_sz;i++){
        for(int j = 0; j< stack->list[i]->cont; j++){
            printf("Población: %d ", stack->list[i]->pobl[j]);
        }
        printf("\nCoste: %d\n", stack->list[i]->coste);
    }
}


void Rec_en_profund(tour_t tour, int n){
    int poblaciones[n];
    for(int i = 0; i< n ; i++){
        poblaciones[i] = -1;
    }
    poblaciones[0] = 0;
    tour->pobl = poblaciones;
    tour->cont = 1;
    tour->coste = 0;
    push(tour);
    while(stack->list_sz != 0){
        pop(tour);
        if(tour->cont == n){
            if(tour->coste < besttour->coste){
                besttour = tour;
            }
        }else{ 
            for(int i = n-1; i!=0 ; i--){
                if(factible(tour,i) == 1){
                    poblaciones[tour->cont] = i;
                    tour->pobl = poblaciones;
                    tour->cont = tour->cont + 1;
                    tour->coste = tour->coste + digraph[tour->pobl[tour->cont-2]][tour->pobl[tour->cont-1]];
                    push(tour);
                    tour->coste = tour->coste - digraph[tour->pobl[tour->cont-2]][tour->pobl[tour->cont-1]];
                    tour->cont = tour->cont - 1;
                    poblaciones[tour->cont] = -1;  
                    tour->pobl = poblaciones;
                }              
            }
        }
    }
}

int main(int argc, char *argv[]){
    double inicio, fin;
    int i, j, n;
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
    tour = malloc((n+1)*sizeof(tour_t));
    besttour = malloc((n+1)*sizeof(tour_t));
    stack = malloc(INT_MAX);
    stack->list = malloc(INT_MAX);
    tour->pobl = malloc((n+1)*sizeof(int));
    besttour->pobl = malloc((n+1)*sizeof(int));
    
    tour->pobl = 0;
    tour->cont = 1;
    tour->coste = 0;
    besttour->pobl = 0;
    besttour->cont = 0;
    besttour->coste = INT_MAX;
    stack->list_sz = 0;
    
    GET_TIME(inicio);
    Rec_en_profund(tour,n);
    GET_TIME(fin);
    
    /*
    for(int k = 0; k< besttour->cont; k++){
        printf("Población: %d", besttour->pobl[k]);
    }
    */
}
