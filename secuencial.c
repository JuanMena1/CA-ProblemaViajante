#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "timer.h"

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

Rec_en_profund(tour_struct *tour_t){
}

int main(int argc, char *argv[]){
    double inicio, fin;
    int n;
    FILE *diagraph_file;
    diagraph_file = fopen(argv[1], "r");
    fscanf(diagraph_file, "%d,", &n);
    
    GET_TIME(inicio);
    Rec_en_profund(tour_t);
    GET_TIME(fin);
}
