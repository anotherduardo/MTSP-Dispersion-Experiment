
typedef struct grafo Grafo;

Grafo *cria_Grafo(int nro_vertices, int grau_max, int eh_ponderado);

void libera_Grafo(Grafo *gr);

int insere_Aresta(Grafo *gr, int orig, int dest, int eh_digrafo, float peso);

int remove_Aresta(Grafo *gr, int orig, int dest, int eh_digrafo);

void insere_Custo(Grafo *gr, int task, int custo);

void debug_Grafo(Grafo* gf);
