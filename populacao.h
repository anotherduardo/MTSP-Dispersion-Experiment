#include<stdio.h>
#include<stdlib.h>

#include"grafo-matriz.h"
#include"code-tle.h"
#include"code-mse.h"

typedef struct populacao Populacao;

Populacao *cria_Populacao(int pop_size, GrafoMatriz *dag, int qtd_cpus);

TLE *recebe_TLE(Populacao *pop, int indice);

MSE *recebe_MSE(Populacao *pop, int indice);

int recebe_pop_size(Populacao *pop);

void calcula_makespan_Populacao(Populacao *pop, GrafoMatriz *dag, int debug);

void libera_Populacao(Populacao *pop);

void debug_Populacao(Populacao *pop);
