//#include"code-matriz.h"
//#include"code-priority.h"

#include<stdio.h>
#include<stdlib.h>
#include<time.h>
#include<string.h>

#include"experimento.h"
#include"code-tle.h"

#include"grafo.h"
#include"grafo-matriz.h"

int main(int argc, char **argv){

   DadosExperimento *exp = inicia_Experimento(argc, argv);

   debug_Experimento(exp);

   executa_Experimento(exp);

   libera_Experimento(exp);

   /*
   GrafoMatriz *gf;

   FILE *arquivo;
   arquivo = fopen("dag4.stg", "r");
   gf = ler_arquivo_stg_Matriz(arquivo);

   //calcula_AlturaMatriz(gf);
   debug_GrafoMatriz(gf);
   calcula_AlturaMatriz_rand(gf);
   debug_GrafoMatriz(gf);

   fclose(arquivo);

   TLE *code;
   code = cria_TLE(gf, 2);
   debug_TLE(code);

   distribui_Sequencia_TLE(code, gf);
   debug_TLE(code);

   calcula_makespan_TLE(code, gf);

   libera_TLE(code);
   libera_GrafoMatriz(gf);

   return 0;
   */
   printf("FIM!\n");

}//fim[main]
