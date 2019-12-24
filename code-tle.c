#include"code-tle.h"
#include"grafo-matriz.h"

struct tle{

   int makespan;          // tempo de finalizacao
   int qtd_task;          // quantidade de tarefas;
   int qtd_processador;   // quantidade de processadores
   int **alocacao;        // matriz de alocacao (processador x tarefas)
   int *grau_processador; // quantidade de tarefas por processador

};//fim[struct]

void shuffle(int *array, size_t n);

TLE *cria_TLE(GrafoMatriz *dag, int qtd_processador){

   TLE *code = NULL;  // codificacao que sera alocada
   int i;             // para laços

   code = (TLE*) malloc(sizeof(struct tle));

   if(code != NULL){

      code->makespan = 0;
      code->qtd_processador = qtd_processador;
      code->alocacao = (int**) malloc(qtd_processador * sizeof(int *));
      code->qtd_task = recebe_QtdVertices(dag);
      code->grau_processador = (int*) calloc(qtd_processador, sizeof(int));

      if(code->alocacao != NULL){

         for(i = 0; i < qtd_processador; i++){
            code->alocacao[i] = (int *) malloc(code->qtd_task * sizeof(int));

         }//fim[alocacao]
      }
      else{
         printf("[erro]:[cria_TLE] Alocacao vazia!\n");
         return NULL;
      }
   }
   else{
      printf("[erro]:[cria_TLE] Codificacao vazia!\n");
      return NULL;
   }
   return code;

}//fim[cria_TLE]

void libera_TLE(TLE *code){

   int i; // para laços;

   for(i = 0; i < code->qtd_processador; i++){

      free(code->alocacao[i]);

   }//fim[matriz alocacao]

   free(code->alocacao);
   free(code->grau_processador);
   free(code);

}//fim[libera_TLE]

void distribui_Sequencia_TLE(TLE *code, GrafoMatriz *dag){

   int i, j;   // para laços
   int *combo; // sequencia de tarefas
   int flag;   // identifica se sequencia é válida

   combo = (int*) malloc(code->qtd_task * sizeof(int));

   // preenchendo a sequencia
   for(i = 0; i < code->qtd_task; i++)
      combo[i] = i;

   // Embaralhando a sequencia
   // É uma tentativa de garantir sequencias de execucao diferentes
   shuffle(combo, code->qtd_task);

   /*
   Tentativa de gerar sequencia totalmente areatória

   flag = checa_Sequencia_Exec(dag, combo);
   while(flag == FALSE){

      printf("[info]:[calcula_makespan_TLE] Sequencia Invalida!\n");
      shuffle(combo, code->qtd_task);
      flag = checa_Sequencia_Exec(dag, combo);

   }//fim[sequencia]

   printf("[info]:[calcula_makespan_TLE] Sequencia Válida!\n"); */

   // Construindo uma sequencia válida
   ordena_Sequencia_Exec(dag, combo);

   // Distribuindo as tarefas aos processadores
   i = 0;
   while(i < code->qtd_task){
      for(j = 0; j < code->qtd_processador; j++){

         if(i < code->qtd_task){
            code->alocacao[j][code->grau_processador[j]] = combo[i];
            code->grau_processador[j]++;
            i++;
         }
      }
   }

   printf("\nGrau dos processadores:\n");
   for(i = 0; i < code->qtd_processador; i++){
      printf("p[%d] grau %d\n", i, code->grau_processador[i]);
   }

   free(combo);

}//fim[calcula_makespan]

void calcula_makespan_TLE(TLE *code, GrafoMatriz *dag){

   int *done_tasks  = NULL; // tarefas já processadas
   int *ready_tasks = NULL; // tarefas prontas para executar
   int *cpu_points  = NULL; // tarefa que a CPU está apontando no momemnto
   int *cpu_cost    = NULL; // custo atual da tarefa associado a cada processador
   int *cpu_time    = NULL; // unidades de tempo de cada processador

   int i, j, k;        // para laços

   int debug;

   // Fazendo alocações
   done_tasks = (int*) calloc(code->qtd_task, sizeof(int));
   ready_tasks = (int*) calloc(code->qtd_task, sizeof(int));
   cpu_points = (int*) calloc(code->qtd_processador,sizeof(int));
   cpu_cost = (int*) calloc(code->qtd_processador,sizeof(int));
   cpu_time = (int*) calloc(code->qtd_processador,sizeof(int));

   if(done_tasks == NULL ||
      ready_tasks == NULL ||
      cpu_points == NULL ||
      cpu_cost == NULL ||
      cpu_time == NULL){

      printf("[erro]:[calcula_makespan_TLE] Alocacao dos vetores!\n");
      return;
   }

   // Iniciando valores de custo das tarefas iniciais nos processadores
   for(i = 0; i < code->qtd_processador; i++)
       cpu_cost[i] = recebe_CustoExec(dag, code->alocacao[i][0]);


   // Debug do custo
   for(i = 0; i < code->qtd_processador; i++)
      printf("Custo %d\n", cpu_cost[i]);

   // Tarefa 0 não tem custo e não significa nada no fluxo temporal
   done_tasks[0] = TRUE;
   cpu_points[0]++;
   cpu_cost[0] = recebe_CustoExec(dag, code->alocacao[0][cpu_points[0]]);
   // Debug do custo
   for(i = 0; i < code->qtd_processador; i++)
      printf("Custo %d\n", cpu_cost[i]);

   // Enquanto a tarefa de saida nao tenha sido processada
   k = 0;
   while(done_tasks[code->qtd_task-1] != TRUE){

      printf("=========================\n");
      printf("Ciclo: %d.\n", k);
      printf("=========================\n");

      for(i = 0; i < code->qtd_processador; i++){

         printf("\nChecando processador %d\n", i);

         // Verifica se tarefa apontada pelo processadores
         // Foi concluída no laço anterior
         if(done_tasks[code->alocacao[i][cpu_points[i]]] == TRUE){

            printf("Tarefa %d Concluida!\n", code->alocacao[i][cpu_points[i]]);
            //scanf("%d", &debug);

            // Se sim: aponta para próxima tarefa da lista
            if(cpu_points[i] < code->grau_processador[i]){
               cpu_points[i]++;
               cpu_cost[i] = recebe_CustoExec(dag, code->alocacao[i][cpu_points[i]]);

               // Se a tarefa foi concluida, deve-se aproveitar o ciclo e observar se
               // podemos começar a execução da próxima tarefa.

               ready_tasks[code->alocacao[i][cpu_points[i]]] =
                  checa_Precedentes(dag, done_tasks, code->alocacao[i][cpu_points[i]]);

               // Após verificar os pais, a resposta é.....
               if(ready_tasks[code->alocacao[i][cpu_points[i]]]){
                  printf("Resposta dos pais de %d: S1M\n", code->alocacao[i][cpu_points[i]]);
               }
               else{
                  printf("Resposta dos pais de %d: NA0\n", code->alocacao[i][cpu_points[i]]);
               }

               if(ready_tasks[code->alocacao[i][cpu_points[i]]] == TRUE){

                  // Se a tarefa possuir custo diferente e zero
                  if(recebe_CustoExec(dag, code->alocacao[i][cpu_points[i]]) != 0){

                     printf("Tarefa t%d ainda nao acabou!\n", code->alocacao[i][cpu_points[i]]);
                     cpu_cost[i] -= 1;
                     cpu_time[i]++;
                  }
                  // Quando for a ultima tarefa
                  else if(code->alocacao[i][cpu_points[i]] == code->qtd_task - 1){
                     //printf("ULTIMA TAREFA LOCALIZADA\n");
                     done_tasks[code->alocacao[i][cpu_points[i]]] = TRUE;
                  }

               }
               else{
                  // Senão: aumenta ociosidade
                  //cpu_cost[i] += 1;
                  cpu_time[i]++;
               }

            }//fim[tarefa concluida, tentanto executar proxima]
         }
         else{

            // Senão: verifica se pode executar
            ready_tasks[code->alocacao[i][cpu_points[i]]] =
               checa_Precedentes(dag, done_tasks, code->alocacao[i][cpu_points[i]]);

            // Após verificar os pais, a resposta é.....
            if(ready_tasks[code->alocacao[i][cpu_points[i]]]){
               printf("Resposta dos pais de %d: S1M\n", code->alocacao[i][cpu_points[i]]);
            }
            else{
               printf("Resposta dos pais de %d: NA0\n", code->alocacao[i][cpu_points[i]]);
            }

            //scanf("%d", &debug);
            if(ready_tasks[code->alocacao[i][cpu_points[i]]] == TRUE){

               // Se a tarefa possuir custo diferente de zero
               if(recebe_CustoExec(dag, code->alocacao[i][cpu_points[i]]) != 0){

                  printf("Tarefa t%d ainda nao acabou!\n", code->alocacao[i][cpu_points[i]]);
                  cpu_cost[i] -= 1;
                  cpu_time[i]++;
               }
               // Quando for a ultima tarefa
               else if(code->alocacao[i][cpu_points[i]] == code->qtd_task - 1){
                  //printf("ULTIMA TAREFA LOCALIZADA\n");
                  done_tasks[code->alocacao[i][cpu_points[i]]] = TRUE;
               }
            }
            else{
               // Senão: aumenta ociosidade
               //cpu_cost[i] += 1;
               cpu_time[i]++;
            }

         }

      }//fim[percorrendo cpus]

      // Laço para finalizar os processadores
      for(j = 0; j < code->qtd_processador; j++){

         // Verificar se tarefa acabou para cada processador;
         if(cpu_cost[j] == 0 && code->alocacao[j][cpu_points[j]] != code->qtd_task - 1){
            done_tasks[code->alocacao[j][cpu_points[j]]] = TRUE;
         }

      }//fim[atualizando cpus no mesmo ciclo]


      printf("\nRelatorio das CPUS!!!\n");
      for(j = 0; j < code->qtd_processador; j++)
         printf("Tempos das CPU[%d]: %d\n", j, cpu_time[j]);

      k++;

   }//fim[simulacao de processamento]

   /*printf("Relatorio das CPUS!!!");
   for(i = 0; i < code->qtd_processador; i++)
      printf("Tempos das CPU[%d]: %d\n", i, cpu_time[i]);
   */
   printf("\nVetor de tarefas finalizadas:\n");
   for(j = 0; j < code->qtd_task; j++)
      printf("%d ", done_tasks[j]);
   printf("\n");

   // Liberando memória
   free(cpu_cost);
   free(cpu_time);
   free(done_tasks);
   free(ready_tasks);
   free(cpu_points);

}//fim[calcula_makespan_TLE]

void debug_TLE(TLE *code){

   int i, j; // para laços

   printf("==========================================\n");
   printf("[info]: Definições da Codificacao TLE:\n");
   printf("==========================================\n\n");
   printf("Quantidade de Tarefas:%d\n", code->qtd_task);
   printf("Quantidade de Processadores:%d\n",code->qtd_processador);
   printf("Valor de makespan:%d\n\n",code->makespan);

   printf("Relacao de alocacao entre processadores e tarefas\n");

   for(i = 0; i < code->qtd_processador; i++){
      printf("p[%d] ", i);
      for(j = 0; j < code->grau_processador[i]; j++){
         printf("t%d -> ", code->alocacao[i][j]);

      }//fim[tarefas]
      printf("\n");

   }//fim[processadores]

   /*printf("\nGrau dos processadores:\n");
   for(i = 0; i < code->qtd_processador; i++){
      printf("p[%d] grau %d\n", i, code->grau_processador[i]);
   }*/

}//fim[debug_TLE]

void shuffle(int *array, size_t n){

   //Fonte: https://stackoverflow.com/questions/6127503/shuffle-array-in-c
   if (n > 1){

      size_t i;
      for (i = 0; i < n - 1; i++){

         size_t j = i + rand() / (RAND_MAX / (n - i) + 1);
         int t = array[j];
         array[j] = array[i];
         array[i] = t;
      }
   }

}//fim[shuffle]
