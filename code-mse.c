#include"code-mse.h"

struct mse{

   int id;         // Identificador na populacao
   int *ordenacao; // Ordem de execucao das tarefas
   int *alocacao;  // Relacao de alocado de processadores

   int *grau_cpu;  // Quantidade de tarefas alocadas em cada cpu
   int qtd_tasks;  // Quantidade de tarefas
   int qtd_cpus;   // Quantidade de processadores
   int makespan;   // Valor do makespan calculado

};//fim[mse];

MSE *cria_MSE(GrafoMatriz *dag, int qtd_processador){

   MSE *code = NULL;  // Codificacao que sera alocada

   code = (MSE*) malloc(sizeof(struct mse));

   if(code != NULL){

      code->makespan = 0;
      code->qtd_cpus = qtd_processador;
      code->qtd_tasks = recebe_QtdVertices(dag);

      //printf("Quantidade de tarefas: %d\n", code->qtd_tasks);

      code->ordenacao = NULL;
      code->alocacao = NULL;
      code->grau_cpu = NULL;

      code->ordenacao = (int*) calloc(code->qtd_tasks, sizeof(int));
      code->alocacao  = (int*) calloc(code->qtd_tasks, sizeof(int));
      code->grau_cpu  = (int*) calloc(code->qtd_cpus, sizeof(int));

      if(code->alocacao == NULL  ||
         code->ordenacao == NULL ||
         code->grau_cpu == NULL){
         free(code);
         printf("[erro]:[cria_MSE] Impossivel alocar vetores de ordenacao e alocacao!\n");
         return NULL;
      }

   }
   else{
      printf("[erro]:[cria_MSE] Impossivel alocar MSE!\n");
      return NULL;
   }

   distribui_Sequencia_MSE(code, dag);

   return code;

}//fim[cria_MSE]


void libera_MSE(MSE *code){

   // Destruindo os vetores da MSE
   free(code->ordenacao);
   free(code->alocacao);

   // Destruindo o vetor de grau
   free(code->grau_cpu);

   // Destruindo as informacoes restantes
   free(code);

}//fim[libera_MSE]

void distribui_Sequencia_MSE(MSE *code, GrafoMatriz *dag){

   int i, j;   // para laços
   int *combo; // sequencia de tarefas
   int flag;   // identifica se sequencia é válida

   combo = (int*) malloc(code->qtd_tasks * sizeof(int));

   // preenchendo a sequencia
   for(i = 0; i < code->qtd_tasks; i++)
      combo[i] = i;

   // Embaralhando a sequencia
   // É uma tentativa de garantir sequencias de execucao diferentes
   shuffle(combo, code->qtd_tasks);

   // Construindo uma sequencia válida
   ordena_Sequencia_Exec(dag, combo);

   // Distribuindo as tarefas ao vetor de ordenacao
   for(i = 0; i < code->qtd_tasks; i++){
      code->ordenacao[i] =  combo[i];
      code->alocacao[i] = rand()% code->qtd_cpus;
      code->grau_cpu[code->alocacao[i]]++;

   }//fim[distribuindo tarefas]

   /*[debug] Verificar o grau dos processadores
   printf("\nGrau dos processadores:\n");
   for(i = 0; i < code->qtd_processador; i++){
      printf("p[%d] grau %d\n", i, code->grau_processador[i]);
   }*/

   free(combo);

}//fim[distribui_Sequencia_MSE]

void atribuir_ID_MSE(MSE *code, int id){

   code->id = id;

}//fim[atribuir_ID_MSE]

int recebe_makespan_MSE(MSE *code){

   return code->makespan;

}//fim[recebe_makespan_MSE]

void calcula_makespan_MSE(MSE *code, GrafoMatriz *dag, short debug){

   int **alocacao = NULL;   // Ordem de execucao gerada para cada cpus

   int *done_tasks  = NULL; // tarefas já processadas
   int *ready_tasks = NULL; // tarefas prontas para executar
   int *cpu_points  = NULL; // tarefa que a CPU está apontando no momemnto
   int *cpu_cost    = NULL; // custo atual da tarefa associado a cada processador
   int *cpu_time    = NULL; // unidades de tempo de cada processador

   int makespan = 0;        // makespan calculado

   int i, j, k;        // para laços

   // Fazendo alocações
   alocacao = (int**) malloc(code->qtd_cpus * sizeof(int *));

   done_tasks = (int*) calloc(code->qtd_tasks, sizeof(int));
   ready_tasks = (int*) calloc(code->qtd_tasks, sizeof(int));
   cpu_points = (int*) calloc(code->qtd_cpus,sizeof(int));
   cpu_cost = (int*) calloc(code->qtd_cpus,sizeof(int));
   cpu_time = (int*) calloc(code->qtd_cpus,sizeof(int));

   if(done_tasks == NULL  ||
      ready_tasks == NULL ||
      cpu_points == NULL  ||
      cpu_cost == NULL    ||
      cpu_time == NULL    ||
      alocacao == NULL){

      printf("[erro]:[calcula_makespan_MSE] Alocacao dos vetores!\n");
      return;
   }

   // Alocando as listas de execucao
   for(i = 0; i < code->qtd_cpus; i++){
      alocacao[i] = (int*) malloc(code->qtd_tasks * sizeof(int));

   }//fim[alocacao execucao]

   // Distribuindo as tarefas para execucao
   for(i = 0; i < code->qtd_tasks; i++){

      alocacao[code->alocacao[i]][cpu_points[code->alocacao[i]]] = code->ordenacao[i];
      cpu_points[code->alocacao[i]]++;

   }//fim[distribuicao execucao]

   // [debug]
   if(debug){
      for(i = 0; i < code->qtd_cpus; i++){
         for(j = 0; j < cpu_points[i]; j++){
            printf("%d -> ", alocacao[i][j]);
         }
         printf("\n");

      }//fim[alocacao execucao]

   }//fim[imprimindo alocacao]

   // Resetando os ponteiros das listas
   for(i = 0; i < code->qtd_cpus; i++)
      cpu_points[i] = 0;


   // [debug] Imprimido cabeçalho + id do cromossomo
   if(debug){
      printf("\n-------------------------\n");
      printf("Cromossomo nº: %d\n", code->id);
      printf("-------------------------\n");
   }

   // Iniciando valores de custo das tarefas iniciais nos processadores
   for(i = 0; i < code->qtd_cpus; i++){

      if(code->grau_cpu[i] > 0)
         cpu_cost[i] = recebe_CustoExec(dag, alocacao[i][0]);

   }//fim[iniciando custos]

   // Segundo pacote STG, tarefa 0 não tem custo e, portanto,
   // não significa nada no fluxo temporal.
   for(i = 0; i < code->qtd_cpus; i++){
      if(alocacao[i][0] == 0){
         done_tasks[0] = TRUE;
         cpu_points[i]++;
         cpu_cost[i] = recebe_CustoExec(dag, alocacao[i][cpu_points[i]]);
      }

   }//fim[atualizando custos iniciais]

   //[debug] Verificando os custos iniciais
   if(debug){
      for(i = 0; i < code->qtd_cpus; i++){
         printf("Custo da CPU[%d] = %3d\n", i, cpu_cost[i]);
      }
   }


   // Enquanto a tarefa de saida nao tenha sido processada
   k = 0;
   while(done_tasks[code->qtd_tasks-1] != TRUE){

      //[debug] Imprimindo os ciclos do fluxo temporal
      if(debug){
         printf("=========================\n");
         printf("Ciclo: %d\n", k);
         printf("=========================\n");
      }

      for(i = 0; i < code->qtd_cpus; i++){

         if(code->grau_cpu[i] > 0){

            // [debug] Observando cada processador em cada
            // ciclo do fluxo temporal
            if(debug){
               printf("\nChecando CPU[%d]\n", i);
            }

            // Verifica se tarefa apontada pelo processadores
            // Foi concluída no laço anterior
            if(done_tasks[alocacao[i][cpu_points[i]]] == TRUE){

               //[debug]: Sinalizando tarefa concluída
               if(debug){
                  printf("Tarefa %d Concluida!\n", alocacao[i][cpu_points[i]]);
               }

               // Se sim: aponta para próxima tarefa da lista
               if(cpu_points[i] < code->grau_cpu[i]-1){

                  cpu_points[i]++;
                  cpu_cost[i] = recebe_CustoExec(dag, alocacao[i][cpu_points[i]]);

                  // Se a tarefa foi concluida, deve-se aproveitar o ciclo e observar se
                  // podemos começar a execução da próxima tarefa.

                  ready_tasks[alocacao[i][cpu_points[i]]] =
                     checa_Precedentes(dag, done_tasks, alocacao[i][cpu_points[i]]);

                  // Após verificar os pais, a resposta é.....
                  // [debug] Imprimindo a respostas das tarefas pais
                  if(debug){
                     if(ready_tasks[alocacao[i][cpu_points[i]]]){
                        printf("Resposta dos pais de t%d: S1M\n", alocacao[i][cpu_points[i]]);
                     }
                     else{
                        printf("Resposta dos pais de t%d: NA0\n", alocacao[i][cpu_points[i]]);
                     }
                  }

                  if(ready_tasks[alocacao[i][cpu_points[i]]] == TRUE){

                     // Se a tarefa possuir custo diferente e zero
                     if(recebe_CustoExec(dag, alocacao[i][cpu_points[i]]) != 0){

                        //[debug]: Sinalizando que tarefa nao foi finalizada no ciclo
                        if(debug){
                           printf("Tarefa t%d ainda nao acabou!\n", alocacao[i][cpu_points[i]]);
                        }

                        cpu_cost[i] -= 1;
                        cpu_time[i]++;
                     }
                     // Quando for a ultima tarefa
                     else if(alocacao[i][cpu_points[i]] == code->qtd_tasks - 1){
                        done_tasks[alocacao[i][cpu_points[i]]] = TRUE;
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
               ready_tasks[alocacao[i][cpu_points[i]]] =
                  checa_Precedentes(dag, done_tasks, alocacao[i][cpu_points[i]]);

               // Após verificar os pais, a resposta é.....
               // [debug] Imprimindo a respostas das tarefas pais
               if(debug){
                  if(ready_tasks[alocacao[i][cpu_points[i]]]){
                     printf("Resposta dos pais de t%d: S1M\n", alocacao[i][cpu_points[i]]);
                  }
                  else{
                     printf("Resposta dos pais de t%d: NA0\n", alocacao[i][cpu_points[i]]);
                  }
               }

               if(ready_tasks[alocacao[i][cpu_points[i]]] == TRUE){

                  // Se a tarefa possuir custo diferente de zero
                  if(recebe_CustoExec(dag, alocacao[i][cpu_points[i]]) != 0){

                     //[debug] Sinalizando que tarefa não terminou
                     if(debug){
                        printf("Tarefa t%d ainda nao acabou!\n", alocacao[i][cpu_points[i]]);
                     }

                     cpu_cost[i] -= 1;
                     cpu_time[i]++;
                  }
                  // Quando for a ultima tarefa
                  else if(alocacao[i][cpu_points[i]] == code->qtd_tasks - 1){
                     done_tasks[alocacao[i][cpu_points[i]]] = TRUE;
                  }
               }
               else{
                  // Senão: aumenta ociosidade
                  cpu_time[i]++;
               }

            }//fim[tarefa terminou?]

         }//fim[se cpu for não vazia]

      }//fim[percorrendo cpus]

      // Laço para finalizar as tarefas alocadas
      for(j = 0; j < code->qtd_cpus; j++){

         if(code->grau_cpu[j] > 0){
            // Verificar se tarefa acabou para cada processador;
            // "Se custo necessario foi finalizado e t não é a última tarefa"
            if(cpu_cost[j] == 0 && alocacao[j][cpu_points[j]] != code->qtd_tasks - 1){
               done_tasks[alocacao[j][cpu_points[j]]] = TRUE;
            }

         }//fim[Se cpu for não vazio]

      }//fim[atualizando cpus no mesmo ciclo]

      //[debug] Imprimindo o tempo de cada CPU no ciclo
      if(debug){
         printf("\nRelatorio das CPUS!!!\n");
         for(j = 0; j < code->qtd_cpus; j++){
            if(code->grau_cpu[j] > 0)
               printf("Tempos da CPU[%d] [point:%d]: %d\n", j, cpu_points[j], cpu_time[j]);
            else
               printf("Tempo da CPU[%d] [point:%d]: cpu vazia!", j, cpu_points[j]);
         }
      }//fim[relatorioCPU]

      // [debug] Numerando o ciclo
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

   //makespan = cpu_time[0];
   makespan = 0;
   for(i = 1; i < code->qtd_cpus; i++){

      if(code->grau_cpu[i] > 0){
         if(cpu_time[i] > makespan)
            makespan = cpu_time[i];

      }//fim[se cpu não for vazia]
   }
   code->makespan = makespan;

   // Liberando memória
   for(i = 0; i < code->qtd_cpus; i++)
      free(alocacao[i]);
   free(alocacao);
   free(cpu_cost);
   free(cpu_time);
   free(done_tasks);
   free(ready_tasks);
   free(cpu_points);

}//fim[calcula_makespan_MSE]

void debug_MSE(MSE *code){

   int i; // para laços

   printf("==========================================\n");
   printf("[info]: Definições da Codificacao MSE:\n");
   printf("==========================================\n\n");
   printf("ID do cromossomo:%d\n", code->id);
   printf("Quantidade de Tarefas:%d\n", code->qtd_tasks);
   printf("Quantidade de Processadores:%d\n",code->qtd_cpus);
   printf("Valor de makespan:%d\n\n",code->makespan);

   printf("Vetor de Ordenacao:\n");
   for(i = 0; i < code->qtd_tasks; i++){
      printf("%d ", code->ordenacao[i]);
   }//fim[processadores]
   printf("\n");

   printf("\nVetor de Alocacao:\n");
   for(i = 0; i < code->qtd_tasks; i++){
      printf("%d ", code->alocacao[i]);
   }//fim[processadores]
   printf("\n");

   printf("Grau dos processadores:\n");
   for(i = 0; i < code->qtd_cpus; i++){
      printf("CPU[%d] grau =  %d\n", i, code->grau_cpu[i]);
   }

}//fim[debug_MSE]
