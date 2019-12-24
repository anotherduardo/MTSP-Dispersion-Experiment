#include<stdio.h>
#include<stdlib.h>
#include"grafo.h"

struct grafo{
   int eh_ponderado; // Ponderado ou não
   int nro_vertices; // Quantidade de Vertices
   int grau_max;     // Quantidade máxima de arestas
   int **arestas;    // Conexões do grafo (linhas-vertices,colunas-conexões)
   float **pesos;    // Pesos, caso ponderado
   int *grau;        // Para cada vertice, quantidade de conexões
   int *custoExec;   // Custo de Execução da Tarefa

};//fim[struct]

Grafo *cria_Grafo(int nro_vertices, int grau_max, int eh_ponderado){

   Grafo *gr = NULL;  // Ponteiro para o grafo criado
   int i;             // Para laços

   gr = (Grafo*) malloc(sizeof(struct grafo));

   if(gr != NULL){
      gr->nro_vertices = nro_vertices;
      gr->grau_max = grau_max;
      gr->eh_ponderado = (eh_ponderado != 0)?1:0;
      gr->grau= (int*) calloc(nro_vertices,sizeof(int));
      gr->custoExec = (int*) calloc(nro_vertices,sizeof(int));

      if(gr->grau == NULL){
         printf("[erro]:[cria_Grafo][grau] Alocacao do Grafo!\n");
         return NULL;
      }

      gr->arestas = (int**) malloc(nro_vertices * sizeof(int*));
      if(gr->arestas == NULL){
         printf("[erro]:[cria_Grafo][arestas] Alocacao do Grafo!\n");
         return NULL;
      }

      for(i = 0; i < nro_vertices; i++){
         gr->arestas[i] = (int*) malloc(grau_max*sizeof(int));
         if(gr->eh_ponderado){
            gr->pesos = (float**) malloc(nro_vertices * sizeof(float*));

            if(gr->pesos == NULL){
               printf("[erro]:[cria_Grafo][pesos] Alocacao do Grafo!\n");
               return NULL;
            }

            for(i = 0; i < nro_vertices; i++)
               gr->pesos[i] = (float*) malloc (grau_max * sizeof(float));

         }//fim[eh_ponderado]
      }

   }
   else
      printf("[erro]:[cria_Grafo] Alocacao do Grafo!\n");

   return gr;

}//fim[cria_Grafo]

void libera_Grafo(Grafo *gr){

   if(gr != NULL){
      int i;
      for(i = 0; i < gr->nro_vertices; i++)
         free(gr->arestas[i]);
      free(gr->arestas);

      if(gr->eh_ponderado){
         for(i = 0; i < gr->nro_vertices; i++)
            free(gr->pesos[i]);
         free(gr->pesos);
      }
      free(gr->grau);
      free(gr->custoExec);
      free(gr);
   }
   else{
      printf("[erro]:[libera_Grafo] Grafo nao vazio!\n");
   }

}//fim[libera_Grafo]

int insere_Aresta(Grafo *gr, int orig, int dest, int eh_digrafo, float peso){

   if(gr == NULL)
      return 0;

   if(orig < 0 || orig >= gr->nro_vertices)
      return 0;

   if(dest < 0 || dest >= gr->nro_vertices)
      return 0;

   gr->arestas[orig][gr->grau[orig]] = dest;

   if(gr->eh_ponderado)
      gr->pesos[orig][gr->grau[orig]] = peso;

   gr->grau[orig]++;

   if(eh_digrafo == 0)
      insere_Aresta(gr, dest, orig, 1, peso);

}//fim[insere_Aresta]

int remove_Aresta(Grafo *gr, int orig, int dest, int eh_digrafo){

   if(gr == NULL)
      return 0;
   if(orig < 0 || orig >= gr->nro_vertices)
      return 0;
   if(dest < 0 || dest >= gr-> nro_vertices)
      return 0;

   int i = 0;

   while(i < gr->grau[orig] && gr->arestas[orig][i] !=dest)
      i++;

   if(i == gr->grau[orig]){// elemento não encontrado
      printf("[info]:[remove_Aresta] Elemento nao encontrado!\n");
      return 0;
   }

   gr->grau[orig]--;
   gr->arestas[orig][i] = gr->arestas[orig][gr->grau[orig]];

   if(gr->eh_ponderado)
      gr->pesos[orig][i] = gr->pesos[orig][gr->grau[orig]];

   if(eh_digrafo == 0)
      remove_Aresta(gr, dest, orig, 1);

   return 1;

}//fim[remove_Aresta]

void insere_Custo(Grafo *gr, int task, int custo){

   gr->custoExec[task] = custo;

}//fim[insere_Custo]

void debug_Grafo(Grafo *gf){

	int i;     // Para laços
	int j;     // Para laços

   printf("[info]: Definições do Grafo:\n\n");
	printf("Quantidade de Vertices:%d\n", gf->nro_vertices);
	printf("Quantidade maxima de conexoes:%d\n\n",gf->grau_max);

   printf("Quantidade de conexoes por Vertice\n");
	for(i = 0; i < gf->nro_vertices; i++){
		printf("Vertice %d = %d \t Custo computacional %d\n", i, gf->grau[i], gf->custoExec[i]);
	}//fim[imprindo Arestas]

	printf("\nArestas do Grafo\n");
	for(i = 0; i < gf->nro_vertices; i++){
      printf("t %2d  ", i);
		for(j = 0; j < gf->nro_vertices; j++){
         if(gf->grau[i] == 0)
			   printf("xx ");
         else if(j >= gf->grau[i] && j != 0)
            printf("xx ");
         else
            printf("%2d ",gf->arestas[i][j]);
		}
		printf("\n");

	}//fim[imprindo Arestas]

	if(gf->eh_ponderado){
		for(i = 0; i < gf->nro_vertices; i++){
			for(j = 0; j < gf->nro_vertices; j++){
				printf("%f ",gf->pesos[i][j]);
			}
			printf("\n");
		}//fim[imprindo pesos]
	}
	else
	   printf("Grafo nao ponderado.\n");

}//fim[debug_Grafo]
