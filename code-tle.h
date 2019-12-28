#include<stdio.h>
#include<stdlib.h>
#include <time.h>

#include"grafo-matriz.h"

#define TRUE 1   // S1M
#define FALSE 0  // NA0

typedef struct tle TLE;

TLE *cria_TLE(GrafoMatriz *dag, int qtd_processador);

void libera_TLE(TLE *code);

void distribui_Sequencia_TLE(TLE *code, GrafoMatriz *dag, short tipo);

void atribuir_ID_TLE(TLE *code, int id);

int recebe_makespan_TLE(TLE *code);

void calcula_makespan_TLE(TLE *code, GrafoMatriz *dag, short debug);

void debug_TLE(TLE *code);
