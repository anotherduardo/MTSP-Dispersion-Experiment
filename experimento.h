#include"grafo.h"
#include"grafo-matriz.h"

#include"code-tle.h"

#define TRUE 1     // S1M
#define FALSE 0    // NA0
#define DIRMAX 200 // Tamanho maximo do caminho do diretorio
#define QTDDIR 15  // Quantidade maxima de diretorios

typedef struct dadosExperimento DadosExperimento;

DadosExperimento *inicia_Experimento(int argc, char **argv);

void executa_Experimento(DadosExperimento *exp);

void debug_Experimento(DadosExperimento *exp);

void libera_Experimento(DadosExperimento *exp);

Grafo *ler_arquivo_stg(FILE *arquivo);

GrafoMatriz *ler_arquivo_stg_Matriz(FILE *arquivo);
