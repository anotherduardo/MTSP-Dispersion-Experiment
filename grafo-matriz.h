#include<stdio.h>
#include<stdlib.h>
#include <time.h>

#define TRUE 1          // S1M
#define FALSE 0         // NA0
#define MAXHEIGHT 1000  // valor maximo de altura para ordenacao

typedef struct grafomatriz GrafoMatriz;

GrafoMatriz *cria_GrafoMatriz(int nro_vertices, int grau_max, int eh_ponderado);

void libera_GrafoMatriz(GrafoMatriz *gr);

int insere_ArestaMatriz(GrafoMatriz *gr, int orig, int dest, int eh_digrafo, float peso);

int remove_ArestaMatriz(GrafoMatriz *gr, int orig, int dest, int eh_digrafo);

void insere_CustoMatriz(GrafoMatriz *gr, int task, int custo);

int recebe_QtdVertices(GrafoMatriz *gr);

int recebe_AlturaR(GrafoMatriz *gr, int indice);

int recebe_CustoExec(GrafoMatriz *gr, int indice);

void calcula_AlturaMatriz(GrafoMatriz *gr);

void calcula_AlturaMatriz_rand(GrafoMatriz *gr);

int checa_Sequencia_Exec(GrafoMatriz *gr, int *sequencia);

void ordena_Sequencia_Exec(GrafoMatriz *gr, int *sequencia);

int checa_Precedentes(GrafoMatriz *gr, int *done_tasks, int task);

void debug_GrafoMatriz(GrafoMatriz* gf);

void shuffle(int *array, size_t n);
