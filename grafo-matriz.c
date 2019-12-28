#include"grafo-matriz.h"

struct grafomatriz{

   int eh_ponderado; // Ponderado ou não
   int nro_vertices; // Quantidade de Vertices
   int grau_max;     // Quantidade máxima de arestas
   int **matrizAdj;  // Conexões do grafo (n x n vertices)
   float **pesos;    // Pesos, caso ponderado
   int *grau;        // Para cada vertice, quantidade de conexões

   int *custoExec;   // Custo de Execução da Tarefa
   int *altura;      // Valor de Altura para cada Tarefa
   int *alturaR;     // valor de altura baseado em Hou, 1994 (altura')

};//fim[struct]

GrafoMatriz *cria_GrafoMatriz(int nro_vertices, int grau_max, int eh_ponderado){

   GrafoMatriz *gr = NULL; // Grafo baseado em Matriz de Adjacência
   int i;                  // Para laços

   gr = (GrafoMatriz*) malloc(sizeof(struct grafomatriz));

   if(gr != NULL){
      gr->nro_vertices = nro_vertices;
      gr->grau_max = grau_max;
      gr->eh_ponderado = (eh_ponderado != 0)?1:0;
      gr->grau= (int*) calloc(nro_vertices,sizeof(int));
      gr->custoExec = (int*) calloc(nro_vertices,sizeof(int));
      gr->altura = (int*) calloc(nro_vertices,sizeof(int));
      gr->alturaR = (int*) calloc(nro_vertices,sizeof(int));

      if(gr->grau == NULL){
         printf("[erro]:[cria_GrafoMatriz][grau] Alocacao do Grafo!\n");
         return NULL;
      }

      gr->matrizAdj = (int**) calloc(nro_vertices, sizeof(int*));
      if(gr->matrizAdj == NULL){
         printf("[erro]:[cria_GrafoMatriz][matrizAdj] Alocacao do Grafo!\n");
         return NULL;
      }

      for(i = 0; i < nro_vertices; i++){
         gr->matrizAdj[i] = (int*) calloc(nro_vertices, sizeof(int));

         if(gr->eh_ponderado){
            gr->pesos = (float**) malloc(nro_vertices * sizeof(float*));

            if(gr->pesos == NULL){
               printf("[erro]:[cria_Grafo][pesos] Alocacao do Grafo!\n");
               return NULL;
            }

            for(i = 0; i < nro_vertices; i++)
               gr->pesos[i] = (float*) malloc (nro_vertices * sizeof(float));

         }//fim[eh_ponderado]

      }//fim[laço matrizAdj]
   }
   else
      printf("[erro]:[cria_GrafoMatriz][gr=null] Alocacao do Grafo!\n");

   return gr;

}//fim[libera_GrafoMatriz]

void libera_GrafoMatriz(GrafoMatriz *gr){

   if(gr != NULL){
      int i;
      for(i = 0; i < gr->nro_vertices; i++)
         free(gr->matrizAdj[i]);
      free(gr->matrizAdj);

      if(gr->eh_ponderado){
         for(i = 0; i < gr->nro_vertices; i++)
            free(gr->pesos[i]);
         free(gr->pesos);
      }
      free(gr->grau);
      free(gr->custoExec);
      free(gr->altura);
      free(gr->alturaR);
      free(gr);
   }
   else{
      printf("[erro]:[libera_Grafo] Grafo vazio!\n");
   }

}//fim[libera_GrafoMatriz]

int insere_ArestaMatriz(GrafoMatriz *gr, int orig, int dest, int eh_digrafo, float peso){

   if(gr == NULL){
      printf("[erro]:[insere_Aresta][gr=null] Grafo nulo!\n");
      return FALSE;
   }

   if(orig < 0 || orig >= gr->nro_vertices){
      printf("[erro]:[insere_Aresta][orig] Origem fora de dimensao!\n");
      return FALSE;
   }

   if(dest < 0 || dest >= gr->nro_vertices){
      printf("[erro]:[insere_Aresta][dest] Destino fora de dimensao!\n");
      return FALSE;
   }
   gr->matrizAdj[orig][dest] = 1;

   if(gr->eh_ponderado)
      gr->pesos[orig][dest] = peso;

   gr->grau[orig]++;

   if(eh_digrafo == 0)
      insere_ArestaMatriz(gr, dest, orig, 1, peso);

   return TRUE;

}//fim[insere_ArestaMatriz]

int remove_ArestaMatriz(GrafoMatriz *gr, int orig, int dest, int eh_digrafo){

   if(gr == NULL){
      printf("[erro]:[remove_Aresta][gr=null] Grafo nulo!\n");
      return FALSE;
   }

   if(orig < 0 || orig >= gr->nro_vertices){
      printf("[erro]:[remove_Aresta][orig] Origem fora de dimensao!\n");
      return FALSE;
   }

   if(dest < 0 || dest >= gr->nro_vertices){
      printf("[erro]:[remove_Aresta][dest] Destino fora de dimensao!\n");
      return FALSE;
   }

   gr->matrizAdj[orig][dest] = 0;

   if(gr->eh_ponderado)
      gr->pesos[orig][dest] = 0;

   if(eh_digrafo == 0)
      remove_ArestaMatriz(gr, dest, orig, 1);

   return TRUE;

}//fim[remove_ArestaMatriz]

void insere_CustoMatriz(GrafoMatriz *gr, int task, int custo){

   gr->custoExec[task] = custo;

}//fim[insere_CustoMatriz]

int recebe_QtdVertices(GrafoMatriz *gr){

   return(gr->nro_vertices);

}//fim[recebe_QtdVertices]

int recebe_AlturaR(GrafoMatriz *gr, int indice){

   return gr->alturaR[indice];

}//fim[recebe_AlturaR]

int recebe_CustoExec(GrafoMatriz *gr, int indice){

   return gr->custoExec[indice];

}//fim[recebe_CustoExec]

void calcula_AlturaMatriz(GrafoMatriz *gr){

   int maior;   // maior valor de altura encontrado
   int i, j;    // para laços

   gr->altura[0] = 0; // tarefa 0 nao tem precedentes

   for(i = 1; i < gr->nro_vertices; i++){

      maior = 0; // resetando o valor de maior para cada tarefa
      for(j = 0; j < gr->nro_vertices; j++){

         if(gr->matrizAdj[j][i] == 1){

            if(gr->altura[j] > maior)
               maior = gr->altura[j];
         }

      }//fim[colunas]

      // conforme a expressão altura
      gr->altura[i] = maior + 1;

   }//fim[tarefas]

}//fim[calcula_AlturaMatriz]

void calcula_AlturaMatriz_rand(GrafoMatriz *gr){

   int menor;   // menor valor de altura encontrado
   int i, j;    // para laços
   int limite;  // flag para tarefa de saida

   calcula_AlturaMatriz(gr);


   limite = gr->nro_vertices - 1;
   for(i = 1; i < limite; i++){

      // obtendo menor sucessor
      menor = MAXHEIGHT; // iniciando o valor de altura com uma grandeza
      for(j = 0; j < gr->nro_vertices; j++){

         if(gr->matrizAdj[i][j] == 1){

            if(gr->altura[j] < menor)
               menor = gr->altura[j];
         }

      }

      menor -= 1; // conforme a expressão altura
      //printf("Valor de menor %d\n", menor);

      // conforme a expressao de Hou,1994
      //printf("Altura: %d\n", gr->altura[i]);

      if(gr->altura[i] == menor)
         gr->alturaR[i] = menor;
      else{
         // regra do rand: (rand() % invertavalo) + inicio

         //[debug]: resolvendo problema da divisao por zero
         //printf("Valor do intervalo do Rand: %d\n", (menor+1)-gr->altura[i]);
         gr->alturaR[i] = (rand() % ((menor+1)-gr->altura[i])) + gr->altura[i];
      }

   }//fim[tarefas]

   // atualizando a tarefa de saída
   gr->alturaR[limite] = gr->altura[limite];


}//fim[calcula_AlturaMatriz]

int checa_Sequencia_Exec(GrafoMatriz *gr, int *sequencia){

   int i;      // para laços
   int limite; // até onde a verfificação vai

   limite = gr->nro_vertices - 1;

   for(i = 0; i < limite; i++){

      if(gr->alturaR[sequencia[i]] > gr->alturaR[sequencia[i+1]])
         return FALSE;

   }//fim[checando...]

   return TRUE;

}//fim[checa_Sequencia_Exec]

void ordena_Sequencia_Exec(GrafoMatriz *gr, int *sequencia){

   // Bubble Sort

   int k, j; // para laços
   int aux;  // variavel pra troca
   int n;    // quantidade total de tarefas verificadas;

   n = gr->nro_vertices;

   for (k = 0; k < n - 1; k++) {
      // printf("\n[%d] ", k);

      for (j = 0; j < n - k - 1; j++) {
         //printf("%d, ", j);

         // Compara-se os valores de alura das tarefas, porem a troca é efetuada no
         // vetor de sequencia.
         if (gr->alturaR[sequencia[j]] > gr->alturaR[sequencia[j + 1]]) {

            // Efetuando a troca
            aux = sequencia[j];
            sequencia[j] = sequencia[j + 1];
            sequencia[j + 1] = aux;
         }

      }//fim[Otimizando o Laço]

   }//fim[quantidadeTarefas]

}//fim[ordena_Sequencia_Exec]

int checa_Precedentes(GrafoMatriz *gr, int *done_tasks, int task){

   int j; // para laços
   int n; // quantidade de tarefas;

   n = gr->nro_vertices;

   for(j = 0; j < n; j++){

      // Tarefa verificada é precedente de task?
      if(gr->matrizAdj[j][task] == TRUE){

         if(done_tasks[j] == FALSE)
            return FALSE;
      }
   }
   return TRUE;

}//fim[checa_Precedentes]

void debug_GrafoMatriz(GrafoMatriz* gf){

   int i;     // para laços
   int j;     // para laços

   printf("==========================================\n");
   printf("[info]: Definições do Grafo:\n");
   printf("==========================================\n\n");
   printf("Quantidade de Vertices:%d\n", gf->nro_vertices);
   printf("Quantidade maxima de conexoes:%d\n\n",gf->grau_max);

   printf("Quantidade de conexoes por Vertice\n");
   for(i = 0; i < gf->nro_vertices; i++){
      printf("T %2d = %d  CE %d\t Altura %d  AlturaR %d\n",
         i,
         gf->grau[i],
         gf->custoExec[i],
         gf->altura[i],
         gf->alturaR[i]);

   }//fim[imprindo Arestas]

   printf("\nMatriz de Adjacencia:\n");
   for(i = 0; i < gf->nro_vertices; i++){
      printf("t %2d  ", i);
      for(j = 0; j < gf->nro_vertices; j++){
         printf("%2d ",gf->matrizAdj[i][j]);
      }
      printf("\n");

   }//fim[imprindo Arestas]

   if(gf->eh_ponderado){
      for(i = 0; i < gf->nro_vertices; i++){
         for(j = 0; j < gf->nro_vertices; j++){
            printf("%2f ",gf->pesos[i][j]);
         }
         printf("\n");
      }//fim[imprindo pesos]
   }
   else
      printf("Grafo nao ponderado.\n");

}//fim[debug_GrafoMatriz]

void shuffle(int *array, size_t n){

   //Fonte: https://stackoverflow.com/questions/6127503/shuffle-array-in-c
   //printf("Entrou no Shuffle!\n");
   if (n > 1){

      size_t i;
      for (i = 0; i < n - 1; i++){

         size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
         int t = array[j];
         array[j] = array[i];
         array[i] = t;
      }
   }
   //printf("Shuffle Executado!\n");

}//fim[shuffle]
