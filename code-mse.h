#include<stdio.h>
#include<stdlib.h>
#include<time.h>

#include"grafo-matriz.h"

#define TRUE 1   // S1M
#define FALSE 0  // NA0

typedef struct mse MSE;

MSE *cria_MSE(GrafoMatriz *dag, int qtd_processador);

void libera_MSE(MSE *code);

void distribui_Sequencia_MSE(MSE *code, GrafoMatriz *dag);

void calcula_makespan_MSE(MSE *code, GrafoMatriz *dag, short debug);

void atribuir_ID_MSE(MSE *code, int id);

int recebe_makespan_MSE(MSE *code);

void debug_MSE(MSE *code);
