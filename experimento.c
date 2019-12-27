#include<stdio.h>
#include<stdlib.h>
#include<string.h>

#include"experimento.h"

struct dadosExperimento{

   int qtd_cpus;              // Quantidade de Processadores;
   int seed;                  // Valor de seed para rand();
   char nome_arquivo[DIRMAX]; // Diretorio e nome do arquivo;
   FILE *arquivo;             // Ponteiro do arquivo;
   int pop_size;              // Indica o tamanho da populacao
   int debug;                 // Indica se o debug será ativado


   int tipo_exp;              // Indica o tipo de experimento (debug ou sequencial)

   GrafoMatriz *gf;           // DAG do experimento construído sobre Matriz de Adjacencia

};

void aponta_Arquivo(DadosExperimento *exp);

void fecha_Arquivo(DadosExperimento *exp);

void gerarcodigo_Arquivo(char* codigoArquivo, int codigo, char *prefixo, char *sufixo);

int count_Digits(int number);

DadosExperimento *inicia_Experimento(int argc, char **argv){

   DadosExperimento *exp;  // Armazena todas as informações do experimento

   exp = (DadosExperimento*) malloc(sizeof(struct dadosExperimento));

   exp->arquivo = NULL; // Iniciando arquivo
   exp->gf = NULL;      // Iniciando DAG

   /*
   argc (quantidade) --> Quantidade de parâmetros enviados na chamada do programa.
   Automaticamente ele recebe um parâmetro, logo seu valor inical é 1.
   O primeiro parâmetro enviado automaticamente é o nome do programa,
   por isso argc começa com valor 1.

   argv (parametros) --> Matriz com parâmetros. Estes são vetores de caracters(strings)
   e são tratados no ínicio do protótipo.
   */
   if (argc == 1) {

      printf("\n[erro]Por favor utilize no exato 1 parâmetro!!!\n");
      printf("./bin [quantidade CPUs] [seed] [tamanho pop] [debug] [nomeArquivo]\n");
      printf("e.g.\n");
      printf("./bin 2 1 dag.stg\n");
   }
   else{

      // Para um menu de ajuda
      if(strcmp(argv[1], "-help") == 0){
         printf("\nAbrindo menu ajuda\n");
         printf("./bin [quantidade CPUs] [seed] [tamanho pop] [debug] [nomeArquivo]\n");
         printf("e.g.\n");
         printf("./bin 2 1 dag.stg\n");
      }

      // Obtendo os parametros gerais
      else{

         // Obtendo a quantidade de processadores
         if(argv[1] == NULL)
            exp->qtd_cpus = 2;
         else
            exp->qtd_cpus = atoi(argv[1]);

         // Obtendo o valor do seed para o rand
         if(argv[2] == NULL)
            exp->seed = 0;
         else
            exp->seed = atoi(argv[2]);

         // Obtendo o tamanho da populacao
         if(argv[3] == NULL)
            exp->pop_size = 2;
         else
            exp->pop_size = atoi(argv[3]);

         if(argv[4] == NULL)
            exp->debug = FALSE;
         else
            if(strcmp(argv[4], "true") == 0)
               exp->debug = TRUE;
            else
               exp->debug = FALSE;

         // Obtendo o caminho do pacote
         if(argv[5] == NULL){
            strcpy(exp->nome_arquivo, "benchmark/ExemplosBasicos/");
            exp->tipo_exp = 0;  // experimento de debug
         }
         else{
            if(strcmp(argv[5], "-pack1") == 0)
               strcpy(exp->nome_arquivo, "benchmark/StandardTaskGraphs-wt-CC-v1");
            else if(strcmp(argv[5], "-pack2") == 0)
               strcpy(exp->nome_arquivo, "benchmark/StandardTaskGraphs-wt-CC-v2");
            else if(strcmp(argv[5], "-pack3") == 0)
               strcpy(exp->nome_arquivo, "benchmark/StandardTaskGraphs-wt-CC-AppPrograms");

            exp->tipo_exp = 1; // Experimento sequencial
         }
      }

   }//fim[configurando experimento]

   if(exp->seed == 0)
      srand(time(NULL));
   else
      srand(exp->seed);

   return exp;

}//fim[inicia_Experimento]

void executa_Experimento(DadosExperimento *exp){

   char dir_original[DIRMAX]; // Backup do caminho original
   char dir_codigo[DIRMAX];   // String que identifica cada arquivo
   int dir[QTDDIR];           // Numero de cada diretorio

   char dir_log_original[DIRMAX]; // Guarda caminho original dos relatórios
   char dir_log_atual[DIRMAX];    // Caminho do log em processamento
   char dir_log_codigo[DIRMAX];   // Codigo que identifica o experimento

   int i, j;                  // Para laços

   Populacao *pop;            // População de cromossomos.

   // Inicializando as strings do relatorio
   dir_log_atual[0]='\0';
   dir_log_original[0]='\0';

   if(strcmp(exp->nome_arquivo, "benchmark/ExemplosBasicos/") == 0){

      strcat(dir_log_original, "log/ExemplosBasicos/");

      char teste_basicos[5][50] = {
         "dag.stg",
         "dag2.stg",
         "dag3.stg",
         "dag4.stg",
         "dag5.stg" // <- antigo dag altura
      };

      for(i = 0; i < 5; i++){

         // Resetando os diretorios originais
         strcpy(dir_log_atual, dir_log_original);
         strcpy(dir_original, exp->nome_arquivo);

         printf("\nNomes dos arquivos: %s\n", exp->nome_arquivo);

         // Importando o DAG
         strcat(exp->nome_arquivo, teste_basicos[i]);
         aponta_Arquivo(exp);
         exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
         calcula_AlturaMatriz_rand(exp->gf);

         // Espaço para testar as codificacoes
         // ...

         // Calculando o makespan

         pop = NULL;
         pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
         calcula_makespan_Populacao(pop, exp->gf, exp->debug);

         // Imprimindo relatório
         strcat(dir_log_atual,teste_basicos[i]);
         imprime_Log_Experimento(pop, dir_log_atual);


         //.....
         libera_Populacao(pop);
         libera_GrafoMatriz(exp->gf);
         fecha_Arquivo(exp);
         strcpy(exp->nome_arquivo, dir_original);

         /*
         strcpy(dir_original, exp->nome_arquivo);
         strcat(exp->nome_arquivo, teste_basicos[i]);

         //printf("Nomes dos arquivos:\n%s\n", exp->nome_arquivo);

         aponta_Arquivo(exp);
         exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);

         // Espaço para testar as codificacoes
         // ...
         debug_GrafoMatriz(exp->gf);
         calcula_AlturaMatriz_rand(exp->gf);
         debug_GrafoMatriz(exp->gf);

         //------ tle ....
         code = cria_TLE(exp->gf, exp->qtd_cpus);
         debug_TLE(code);

         distribui_Sequencia_TLE(code, exp->gf);
         debug_TLE(code);

         calcula_makespan_TLE(code, exp->gf, FALSE);

         libera_TLE(code);
         //------
         libera_GrafoMatriz(exp->gf);
         fecha_Arquivo(exp);
         strcpy(exp->nome_arquivo, dir_original);
         */

      }//fim[percorrer pacote basico]

   }//fim[packexemplo]

   else if(strcmp(exp->nome_arquivo, "benchmark/StandardTaskGraphs-wt-CC-v1") == 0){

      strcat(dir_log_original, "log/StandardTaskGraphs-wt-CC-v1/");

      dir[0]  = 50;
      dir[1]  = 100;
      dir[2]  = 300;
      dir[3]  = 500;
      dir[4]  = 700;
      dir[5]  = 900;
      dir[6]  = 1100;
      dir[7]  = 1300;
      dir[8]  = 1500;
      dir[9]  = 1700;
      dir[10] = 1900;
      dir[11] = 2100;
      dir[12] = 2300;
      dir[13] = 2500;
      dir[14] = 2700;

      for(i = 0; i < 15; i++){

         switch(dir[i]){

            // dir = 0
            case 50:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"50");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"50");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;


            // dir = 1
            case 100:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"100");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"100");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;



            // dir = 2
            case 300:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"300");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"300");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 3
            case 500:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"500");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"500");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;


            // dir = 4
            case 700:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"700");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"700");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 5
            case 900:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"900");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"900");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 6
            case 1100:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1100");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"1100");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 7
            case 1300:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1300");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"1300");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 8
            case 1500:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1500");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"1500");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 9
            case 1700:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1700");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"1700");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 10
            case 1900:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1900");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"1900");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 11
            case 2100:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"2100");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"2100");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 12
            case 2300:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"2300");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"2300");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 13
            case 2500:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"2500");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"2500");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 14
            case 2700:

               for(j = 0; j < 60; j++){

                  strcpy(dir_log_atual, dir_log_original);
                  strcpy(dir_original, exp->nome_arquivo);

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"2700");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo, j, "rand", ".stg");
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);

                  // Espaço para testar as codificações

                  aponta_Arquivo(exp);
                  exp->gf = ler_arquivo_stg_Matriz(exp->arquivo);
                  calcula_AlturaMatriz_rand(exp->gf);

                  // Espaço para testar as codificacoes
                  // ...

                  // Calculando o makespan
                  pop = NULL;
                  pop = cria_Populacao(exp->pop_size, exp->gf, exp->qtd_cpus);
                  calcula_makespan_Populacao(pop, exp->gf, exp->debug);

                  // Imprimindo relatório
                  dir_log_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_log_codigo, j, "sample",".csv");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,"2700");
                  strcat(dir_log_atual,"/");
                  strcat(dir_log_atual,dir_log_codigo);
                  imprime_Log_Experimento(pop, dir_log_atual);

                  //.....
                  libera_Populacao(pop);
                  libera_GrafoMatriz(exp->gf);
                  fecha_Arquivo(exp);
                  strcpy(exp->nome_arquivo, dir_original);

                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

         }//[switch]

      }//fim[diretorios-pack1]

   }//fim[pack1]

   /*
   else if(strcmp(exp->nome_arquivo, "benchmark/StandardTaskGraphs-wt-CC-v2") == 0){


      dir[0]  = 50;
      dir[1]  = 100;
      dir[2]  = 300;
      dir[3]  = 500;
      dir[4]  = 750;
      dir[5]  = 1000;
      dir[6]  = 1250;
      dir[7]  = 1500;
      dir[8]  = 1750;
      dir[9]  = 2000;
      dir[10] = 2500;
      dir[11] = 3000;
      dir[12] = 3500;
      dir[13] = 4000;
      dir[14] = 5000;

      for(i = 0; i < 15; i++){

         switch(dir[i]){
            // dir = 0
            case 50:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"50");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 1
            case 100:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"100");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 2
            case 300:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"300");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 3
            case 500:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"500");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 4
            case 750:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"750");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 5
            case 1000:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1000");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 6
            case 1250:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1250");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 7
            case 1500:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1500");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 8
            case 1750:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"1750");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 9
            case 2000:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"2000");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 10
            case 2500:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"2500");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 11
            case 3000:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"3000");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 12
            case 3500:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"3500");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 13
            case 4000:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"4000");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            // dir = 14
            case 5000:

               strcpy(dir_original, exp->nome_arquivo);

               for(j = 0; j < 180; j++){

                  // mesclando nome do  diretorio
                  strcat(exp->nome_arquivo,"/");
                  strcat(exp->nome_arquivo,"5000");
                  strcat(exp->nome_arquivo,"/");

                  // mesclando nome do arquivo
                  dir_codigo[0] = '\0';
                  gerarcodigo_Arquivo(dir_codigo,j);
                  strcat(exp->nome_arquivo,dir_codigo);

                  printf("J = %2d file:%s\n", j, exp->nome_arquivo);
                  // Espaço para testar as codificações
                  //...
                  // recuperando caminho original
                  strcpy(exp->nome_arquivo, dir_original);

               }//fim[arquivos]
               break;

            }//fim[switch]

      }//fim[diretorios-pack2]

   }//fim[pack2]

   else if(strcmp(exp->nome_arquivo, "benchmark/StandardTaskGraphs-wt-CC-AppPrograms") == 0){

      // Neste diretorio existem somente tres arquivos.
      // Portanto, nao haverá lacos neste bloco.

      //----------
      // FPPPP
      //----------
      strcpy(dir_original, exp->nome_arquivo);

      // mesclando o nome do arquivo
      strcat(exp->nome_arquivo,"/fpppp.stg");

      printf("File:%s\n", exp->nome_arquivo);
      // Espaço para testar as codificações
      //...

      // recuperando caminho original
      strcpy(exp->nome_arquivo, dir_original);


      //----------
      // ROBOT
      //----------
      strcpy(dir_original, exp->nome_arquivo);

      // mesclando o nome do arquivo
      strcat(exp->nome_arquivo,"/robot.stg");

      printf("File:%s\n", exp->nome_arquivo);
      // Espaço para testar as codificações
      //...

      // recuperando caminho original
      strcpy(exp->nome_arquivo, dir_original);

      //----------
      // SPARSE
      //----------
      strcpy(dir_original, exp->nome_arquivo);

      // mesclando o nome do arquivo
      strcat(exp->nome_arquivo,"/sparse.stg");

      printf("File:%s\n", exp->nome_arquivo);
      // Espaço para testar as codificações
      //...

      // recuperando caminho original
      strcpy(exp->nome_arquivo, dir_original);

   }//fim[pack3]
   */

}//fim[executa_Experimento]

void imprime_Log_Experimento(Populacao *pop, char *path){

   FILE *arquivo; // Arquivo para exportar os resultados
   int i, j;      // Para laços

   arquivo = NULL;
   arquivo = fopen(path, "w");

   if(arquivo == NULL){
      printf("[erro]:[imprime_Log_Experimento] Impossivel abrir arquivo!\n");
   }
   else{
      fprintf(arquivo, "tle,mse\n");
      j = recebe_pop_size(pop);

      // TLE
      for(i = 0; i < j; i++){
         fprintf(arquivo, "%d,", recebe_makespan_TLE((recebe_TLE(pop, i))));
         fprintf(arquivo, "%d,\n", recebe_makespan_MSE((recebe_MSE(pop, i))));
      }

      // MSE
      /*for(i = 0; i < j; i++){
         printf("%d,\n", recebe_makespan_TLE((recebe_TLE(pop, i))));
      }*/

   }//fim[arquivo aberto]
   fclose(arquivo);

}//fim[imprime_Log_Experimento]

void gerarcodigo_Arquivo(char* codigoArquivo, int codigo, char *prefixo, char *sufixo){

	char buffer[4];

   strcat(codigoArquivo, prefixo);

	switch (count_Digits(codigo)) {
      case 0:
		   strcat(codigoArquivo, "000");
		   break;
		case 1:
		   strcat(codigoArquivo, "000");
			break;
		case 2:
		   strcat(codigoArquivo, "00");
			break;
		case 3:
			strcat(codigoArquivo, "0");
			break;
	}
	sprintf(buffer, "%d", codigo);
	strcat(codigoArquivo, buffer);
	strcat(codigoArquivo, sufixo);

}//fim[gerarcodigo_Arquivo]

int count_Digits(int number){

	int count = 0;

	while(number != 0){
		number /= 10;
		++count;
	}//fim[contando casas]

	return count;

}//fim[count_Digits]

void aponta_Arquivo(DadosExperimento *exp){

   exp->arquivo = fopen(exp->nome_arquivo,"r");

}//fim[aponta_Arquivo]

void fecha_Arquivo(DadosExperimento *exp){

   fclose(exp->arquivo);

}//fim[fecha_Arquivo]

void debug_Experimento(DadosExperimento *exp){

   printf("==========================================\n");
   printf("[info]: Detalhes do Experimento:\n");
   printf("==========================================\n\n");
   printf("Quantidade de Processadores:%d\n", exp->qtd_cpus);
   if(exp->seed == 0)
      printf("Valor de Seed:%d (aleatorio total)\n", exp->seed);
   else
      printf("Valor de Seed:%d\n", exp->seed);

   printf("Caminho do arquivo: %s\n", exp->nome_arquivo);
   if(exp->arquivo == NULL)
      printf("Status do arquivo: Não Carregado!\n");
   else
      printf("Status do arquivo: Carregado!\n");

   printf("Tamanho maximo da populacao: %d\n", exp->pop_size);

   if(exp->debug)
      printf("Debug ativado!\n");
   else
      printf("Debug desativado!\n");

}//fim[debug_Experimento]

void libera_Experimento(DadosExperimento *exp){

   //libera_GrafoMatriz(exp->gf);
   free(exp);

}//fim[libera_Experimento]

Grafo *ler_arquivo_stg(FILE *arquivo){

   Grafo *gr = NULL; // Grafo que será criado
   int quant_task;   // Quantidade de tarefas
   int quant_succs;  // Quantidade de sucessoras por tarefa
   int task_id;      // Identificacao da tarefa
   int task_cost;    // Custo de execução da Tarefa
   int task_succ;    // Cada tarefa sucessora identificada
   int i, j;         // Para laços

   if(arquivo == NULL){
      printf("[erro]:[ler_arquivo_stg] Arquivo vazio!\n");
   }

   fscanf(arquivo, "%d", &quant_task);

   quant_task += 2;  // Tarefas + Tarefas de entrada e saída

   gr = cria_Grafo(quant_task, quant_task, 0);

   //printf("Quantidade de tarefas: %d\n",quant_task);
   for(i = 0; i < quant_task; i++){

      fscanf(arquivo, "%d", &task_id);
      fscanf(arquivo, "%d", &task_cost);
      fscanf(arquivo, "%d", &quant_succs);

      insere_Custo(gr, i, task_cost);
      for(j = 0; j < quant_succs; j++){

         fscanf(arquivo, "%d", &task_succ);
         insere_Aresta(gr, task_succ, i, TRUE, 0);

      }//fim[laço-sucessoras]

   }//fim[laço-tarefas]

   return gr;

}//fim[ler_arquivo_stg]

GrafoMatriz *ler_arquivo_stg_Matriz(FILE *arquivo){

   GrafoMatriz *gr = NULL; // Grafo que será criado
   int quant_task;         // Quantidade de tarefas
   int quant_succs;        // Quantidade de sucessoras por tarefa
   int task_id;            // Identificacao da tarefa
   int task_cost;          // Custo de execução da Tarefa
   int task_succ;          // Cada tarefa sucessora identificada
   int i, j;               // Para laços

   if(arquivo == NULL){
      printf("[erro]:[ler_arquivo_stg] Arquivo vazio!\n");
   }

   fscanf(arquivo, "%d", &quant_task);

   quant_task += 2;  // Tarefas + Tarefas de entrada e saída

   gr = cria_GrafoMatriz(quant_task, quant_task, 0);

   //printf("Quantidade de tarefas: %d\n",quant_task);
   for(i = 0; i < quant_task; i++){

      fscanf(arquivo, "%d", &task_id);
      fscanf(arquivo, "%d", &task_cost);
      fscanf(arquivo, "%d", &quant_succs);

      insere_CustoMatriz(gr, i, task_cost);
      for(j = 0; j < quant_succs; j++){

         fscanf(arquivo, "%d", &task_succ);
         insere_ArestaMatriz(gr, task_succ, i, TRUE, 0);

      }//fim[laço-sucessoras]

   }//fim[laço-tarefas]

   return gr;

}//fim[ler_arquivo_stg]
