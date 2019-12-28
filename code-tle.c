#include"code-tle.h"
#include"grafo-matriz.h"

struct tle{

   int id;                // identificador do cromossomos na populacao
   int makespan;          // tempo de finalizacao
   int qtd_task;          // quantidade de tarefas;
   int qtd_processador;   // quantidade de processadores
   int **alocacao;        // matriz de alocacao (processador x tarefas)
   int *grau_processador; // quantidade de tarefas por processador

};//fim[struct]

TLE *cria_TLE(GrafoMatriz *dag, int qtd_processador){

   TLE *code = NULL;  // codificacao que sera alocada
   int i;             // para laços

   code = (TLE*) malloc(sizeof(struct tle));

   code->id = 0;
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

   // Atribuindo a sequencia de tarefas
   distribui_Sequencia_TLE(code, dag, FALSE);

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

void distribui_Sequencia_TLE(TLE *code, GrafoMatriz *dag, short tipo){

   int i, j;   // para laços
   int *combo; // sequencia de tarefas
   int slot;   // Guarda o slot sorteado pelo rand


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
   // [info] Distribuição Sequencial!
   // Neste modo: uma tarefa é distribuída para cada processador
   //  -> Melhores resultados e Balanceamento de carga mais ideal
   if(tipo == TRUE){
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
   }//fim[distribuicao melhorada]
   else{
      for(i = 0; i < code->qtd_task; i++){

         slot = rand()% code->qtd_processador;
         code->alocacao[slot][code->grau_processador[slot]] = combo[i];
         code->grau_processador[slot]++;

      }//fim[distribuindo tarefas]

   }//fim[distr]

   /*[debug] Verificar o grau dos processadores
   printf("\nGrau dos processadores:\n");
   for(i = 0; i < code->qtd_processador; i++){
      printf("p[%d] grau %d\n", i, code->grau_processador[i]);
   }*/

   free(combo);

}//fim[calcula_makespan]

void atribuir_ID_TLE(TLE *code, int id){

   code->id = id;

}//fim[atribuir_ID_TLE]

int recebe_makespan_TLE(TLE *code){

   return code->makespan;

}//fim[recebe_makespan_TLE]

void calcula_makespan_TLE(TLE *code, GrafoMatriz *dag, short debug){

   int *done_tasks  = NULL; // tarefas já processadas
   int *ready_tasks = NULL; // tarefas prontas para executar
   int *cpu_points  = NULL; // tarefa que a CPU está apontando no momemnto
   int *cpu_cost    = NULL; // custo atual da tarefa associado a cada processador
   int *cpu_time    = NULL; // unidades de tempo de cada processador

   int makespan = 0;        // makespan calculado

   int i, j, k;        // para laços

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

   // [debug] Imprimido cabeçalho + id do cromossomo
   if(debug){
      printf("\n-------------------------\n");
      printf("Cromossomo nº: %d\n", code->id);
      printf("-------------------------\n");
   }

   // Iniciando valores de custo das tarefas iniciais nos processadores
   for(i = 0; i < code->qtd_processador; i++){

      if(code->grau_processador[i] > 0)
         cpu_cost[i] = recebe_CustoExec(dag, code->alocacao[i][0]);
   }

   // Segundo pacote STG, tarefa 0 não tem custo e, portanto,
   // não significa nada no fluxo temporal.
   for(i = 0; i < code->qtd_processador; i++){
      if(code->alocacao[i][0] == 0){
         done_tasks[0] = TRUE;
         if(code->grau_processador[i] > 1){
            cpu_points[i]++;
            cpu_cost[i] = recebe_CustoExec(dag, code->alocacao[i][cpu_points[i]]);
         }
      }

   }//fim[atualizando custos iniciais]

   //[debug] Verificando os custos iniciais
   if(debug){
      for(i = 0; i < code->qtd_processador; i++)
         printf("Custo da CPU[%d] = %3d\n", i, cpu_cost[i]);
   }

   // Enquanto a tarefa de saida nao tenha sido processada
   k = 0;
   while(done_tasks[code->qtd_task-1] != TRUE){

      //[debug] Imprimindo os ciclos do fluxo temporal
      if(debug){
         printf("=========================\n");
         printf("Ciclo: %d\n", k);
         printf("=========================\n");
      }

      for(i = 0; i < code->qtd_processador; i++){

         if(code->grau_processador[i] > 0){

            // [debug] Observando cada processador em cada
            // ciclo do fluxo temporal
            if(debug){
               printf("\nChecando CPU[%d]\n", i);
            }

            // Verifica se tarefa apontada pelo processadores
            // Foi concluída no laço anterior
            if(done_tasks[code->alocacao[i][cpu_points[i]]] == TRUE){

               //[debug]: Sinalizando tarefa concluída
               if(debug){
                  printf("Tarefa %d Concluida!\n", code->alocacao[i][cpu_points[i]]);
               }

               // Se sim: aponta para próxima tarefa da lista
               if(cpu_points[i] < code->grau_processador[i]-1){

                  cpu_points[i]++;
                  cpu_cost[i] = recebe_CustoExec(dag, code->alocacao[i][cpu_points[i]]);

                  // Se a tarefa foi concluida, deve-se aproveitar o ciclo e observar se
                  // podemos começar a execução da próxima tarefa.

                  ready_tasks[code->alocacao[i][cpu_points[i]]] =
                     checa_Precedentes(dag, done_tasks, code->alocacao[i][cpu_points[i]]);

                  // Após verificar os pais, a resposta é.....
                  // [debug] Imprimindo a respostas das tarefas pais
                  if(debug){
                     if(ready_tasks[code->alocacao[i][cpu_points[i]]]){
                        printf("Resposta dos pais de t%d: S1M\n", code->alocacao[i][cpu_points[i]]);
                     }
                     else{
                        printf("Resposta dos pais de t%d: NA0\n", code->alocacao[i][cpu_points[i]]);
                     }
                  }

                  if(ready_tasks[code->alocacao[i][cpu_points[i]]] == TRUE){

                     // Se a tarefa possuir custo diferente e zero
                     if(recebe_CustoExec(dag, code->alocacao[i][cpu_points[i]]) != 0){

                        //[debug]: Sinalizando que tarefa nao foi finalizada no ciclo
                        if(debug){
                           printf("Tarefa t%d ainda nao acabou!\n", code->alocacao[i][cpu_points[i]]);
                        }

                        cpu_cost[i] -= 1;
                        cpu_time[i]++;
                     }
                     // Quando for a ultima tarefa
                     else if(code->alocacao[i][cpu_points[i]] == code->qtd_task - 1){
                        done_tasks[code->alocacao[i][cpu_points[i]]] = TRUE;
                     }

                  }
                  else{
                     // Senão: aumenta ociosidade
                     cpu_time[i]++;
                  }

               }//fim[tarefa concluida, tentanto executar proxima]
            }
            else{

               // Senão: verifica se pode executar
               ready_tasks[code->alocacao[i][cpu_points[i]]] =
                  checa_Precedentes(dag, done_tasks, code->alocacao[i][cpu_points[i]]);

               // Após verificar os pais, a resposta é.....
               // [debug] Imprimindo a respostas das tarefas pais
               if(debug){
                  if(ready_tasks[code->alocacao[i][cpu_points[i]]]){
                     printf("Resposta dos pais de t%d: S1M\n", code->alocacao[i][cpu_points[i]]);
                  }
                  else{
                     printf("Resposta dos pais de t%d: NA0\n", code->alocacao[i][cpu_points[i]]);
                  }
               }

               if(ready_tasks[code->alocacao[i][cpu_points[i]]] == TRUE){

                  // Se a tarefa possuir custo diferente de zero
                  if(recebe_CustoExec(dag, code->alocacao[i][cpu_points[i]]) != 0){

                     //[debug] Sinalizando que tarefa não terminou
                     if(debug){
                        printf("Tarefa t%d ainda nao acabou!\n", code->alocacao[i][cpu_points[i]]);
                     }

                     cpu_cost[i] -= 1;
                     cpu_time[i]++;
                  }
                  // Quando for a ultima tarefa
                  else if(code->alocacao[i][cpu_points[i]] == code->qtd_task - 1){
                     done_tasks[code->alocacao[i][cpu_points[i]]] = TRUE;
                  }
               }
               else{
                  // Senão: aumenta ociosidade
                  cpu_time[i]++;
               }

            }//fim[tarefa terminou?]

         }//fim[Se CPU não for vazia]

      }//fim[percorrendo cpus]

      // Laço para finalizar as tarefas alocadas
      for(j = 0; j < code->qtd_processador; j++){

         if(code->grau_processador[j] > 0){
            // Verificar se tarefa acabou para cada processador;
            // "Se custo necessario foi finalizado e t não é a última tarefa"
            if(cpu_cost[j] == 0 && code->alocacao[j][cpu_points[j]] != code->qtd_task - 1){
               done_tasks[code->alocacao[j][cpu_points[j]]] = TRUE;
            }
         }

      }//fim[atualizando cpus no mesmo ciclo]

      //[debug] Imprimindo o tempo de cada CPU no ciclo
      if(debug){
         printf("\nRelatorio das CPUS!!!\n");
         for(j = 0; j < code->qtd_processador; j++){

            if(code->grau_processador[j] > 0)
               printf("Tempos da CPU[%d] [point:%d]: %d\n", j, cpu_points[j], cpu_time[j]);
            else
               printf("Tempo da CPU[%d] [point:%d]: cpu vazia!\n", j, cpu_points[j]);
         }
      }

      // [debug] NUmerando o ciclo
      k++;

   }//fim[simulacao de processamento]

   /*printf("Relatorio das CPUS!!!");
   for(i = 0; i < code->qtd_processador; i++)
      printf("Tempos das CPU[%d]: %d\n", i, cpu_time[i]);
   printf("\nVetor de tarefas finalizadas:\n");
   for(j = 0; j < code->qtd_task; j++)
      printf("%d ", done_tasks[j]);
   printf("\n");
   */

   // Obtendo maior tempo de finalização
   makespan = 0;
   for(i = 0; i < code->qtd_processador; i++){

      if(code->grau_processador[i] > 0){
         if(cpu_time[i] > makespan)
            makespan = cpu_time[i];

      }//fim[se cpu não for vazia]
   }
   code->makespan = makespan;

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
   printf("ID do cromossomo:%d\n", code->id);
   printf("Quantidade de Tarefas:%d\n", code->qtd_task);
   printf("Quantidade de Processadores:%d\n",code->qtd_processador);
   printf("Valor de makespan:%d\n\n",code->makespan);

   printf("Relacao de alocacao entre processadores e tarefas\n");

   for(i = 0; i < code->qtd_processador; i++){
      printf("p[%d] [grau:%d] ", i, code->grau_processador[i]);
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
