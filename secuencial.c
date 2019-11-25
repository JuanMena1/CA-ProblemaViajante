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
mystack stack = NULL;
int **digraph;

//Métodos
void push(tour_t tour){
    stack->list[stack->list_sz] = tour;
    stack->list_sz = stack->list_sz + 1;
}

void pop(tour_t tour){
    stack->list = stack->list_sz - 1;
    tour = stack->list[stack->list_sz];
}


void Rec_en_profund(tour_t tour, int n){
    tour = (0,1,0);
    push(tour);
    while(stack->list_sz != 0){
        pop(tour);
        if(tour->cont == n){
            if(tour->coste < besttour->coste){
                besttour = tour;
            }
        }else{
            for(int i = n-1; i==0; i--){
                if((tour->coste + digraph[tour->pobl[tour->cont]][i])< besttour->coste){
                    tour->pobl[tour->cont+1] = i;
                    tour->coste = tour->coste + digraph[tour->pobl[tour->cont]][i];
                    push(tour);
                    tour->coste = tour->coste - digraph[tour->pobl[tour->cont]][i];
                    tour->pobl[tour->cont+1] = 0;
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

    tour = (0,0,0);
    besttour = (0,0, INT_MAX);

    GET_TIME(inicio);
    Rec_en_profund(tour,n);
    GET_TIME(fin);
}
