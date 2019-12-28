#include"populacao.h"

struct populacao{

   int pop_size; // tamanho máximo da populacao

   TLE **pop_tle; // 1000 individuos na codificacao TLE
   MSE **pop_mse; // 1000 individuos na codificacao MSE

};//fim[populacao]

Populacao *cria_Populacao(int pop_size, GrafoMatriz *dag, int qtd_cpus){

   int i;                 // Para laços
   Populacao *pop = NULL; // Populacao alocada

   pop = (Populacao*) malloc(sizeof(struct populacao));

   if(pop != NULL){

      pop->pop_size = pop_size;
      pop->pop_tle = (TLE**) malloc(pop->pop_size * sizeof(TLE*));
      pop->pop_mse = (MSE**) malloc(pop->pop_size * sizeof(MSE*));

      //printf("Laco da TLE\n");
      for(i = 0; i < pop->pop_size; i++){

         // -------
         // TLE
         // -------
         pop->pop_tle[i] = cria_TLE(dag, qtd_cpus);      // construindo TLE com grafo
         pop->pop_mse[i] = cria_MSE(dag, qtd_cpus);      // construindo MSE com grafo

         atribuir_ID_TLE(pop->pop_tle[i], i);            // marcando o id na TLE
         atribuir_ID_MSE(pop->pop_mse[i], i);            // marcando o id na MSE

         //[debug] Para checar as codificacoe geradas
         //debug_TLE(pop->pop_tle[i]);
         //debug_MSE(pop->pop_mse[i]);

      }//fim[alocando as populacoes]
   }
   else{
      printf("[erro]:[cria_Populacao] Erro na Alocacao da Populacao\n");
      return NULL;
   }

   return pop;

}//fim[cria_Populacao]

TLE *recebe_TLE(Populacao *pop, int indice){

   return(pop->pop_tle[indice]);

}//fim[recebe_TLE]

MSE *recebe_MSE(Populacao *pop, int indice){

   return(pop->pop_mse[indice]);

}//fim[recebe_MSE]

int recebe_pop_size(Populacao *pop){

   return pop->pop_size;

}//fim[recebe_pop_size]

void calcula_makespan_Populacao(Populacao *pop, GrafoMatriz *dag, int debug){

   int i; // Para laços

   for(i = 0; i < pop->pop_size; i++){

      // [makespan]: TLE
      //debug_TLE(pop->pop_tle[i]);
      calcula_makespan_TLE(pop->pop_tle[i], dag, debug);

      // [makespan]: MSE
      //debug_MSE(pop->pop_mse[i]);
      calcula_makespan_MSE(pop->pop_mse[i], dag, debug);

   }//fim[calculando cada makespan]

}//fim[calcula_makespan_Populacao]

void libera_Populacao(Populacao *pop){

   int i; // Para laços

   // Liberando a memoria das populacoes internas
   for(i = 0; i < pop->pop_size; i++){
      libera_TLE(pop->pop_tle[i]);
      libera_MSE(pop->pop_mse[i]);
   }

   // Destruindo o resto
   free(pop->pop_tle);
   free(pop->pop_mse);
   free(pop);

}//fim[libera_Populacao]

void debug_Populacao(Populacao *pop){

   printf("==========================================\n");
   printf("[info]: Definições da Populacao:\n");
   printf("==========================================\n\n");
   printf("Quantidade de Cromossomo por pop:%d\n", pop->pop_size);


}//fim[debug_Populacao]
