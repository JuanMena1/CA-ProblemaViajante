#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"
#include <limits.h>

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

void push(stack_struct stack, tour_struct tour){
    
}


Rec_en_profund(tour_t *tour, int poblaciones[]){
    
}

int main(int argc, char *argv[]){
    double inicio, fin;
    int i, j, n;
    int **digraph;
    tour_t tour, besttour;
    FILE *digraph_file;
    digraph_file = fopen(argv[1], "r");
    fscanf(digraph_file, "%d,", &n);
    int poblaciones[n];
    for(i=0; i<n;i++){
        poblaciones[i]=i;
    }
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
    tour = malloc(sizeof(tour_t));
    besttour = malloc(sizeof(tour_t));

    tour = (0,1,0);
    besttour = (0,1, INT_MAX);

    GET_TIME(inicio);
    Rec_en_profund(tour, &poblaciones);
    GET_TIME(fin);
}
