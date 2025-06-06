#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <ctype.h>

#define MAX_SENSORES 100
#define PASSO_REALOC 1000
#define TAM_LINHA 100

struct Leitura
{
    long timestamp;
    char id_sensor[20];
    char valor[50];
};

int comparar(const void *a, const void *b)
{
    struct Leitura *x = (struct Leitura *)a;
    struct Leitura *y = (struct Leitura *)b;
    return (x->timestamp - y->timestamp);
}

void para_maiusculas(char *str)
{
    for (int i = 0; str[i]; i++)
    {
        str[i] = toupper((unsigned char)str[i]);
    }
}

int main(int argc, char *argv[])
{
    if (argc < 2)
    {
        printf("Uso: %s <arquivo_dados>\n", argv[0]);
        return 1;
    }

    FILE *entrada = fopen(argv[1], "r");
    if (!entrada)
    {
        perror("Erro ao abrir arquivo");
        return 1;
    }

    struct Leitura *leituras[MAX_SENSORES] = {NULL};
    int contadores[MAX_SENSORES] = {0};
    int capacidade[MAX_SENSORES] = {0};
    char sensores[MAX_SENSORES][20];
    int total_sensores = 0;

    char linha[TAM_LINHA];
    while (fgets(linha, sizeof(linha), entrada))
    {
        struct Leitura l;
        if (sscanf(linha, "%ld %s %s", &l.timestamp, l.id_sensor, l.valor) != 3)
        {
            fprintf(stderr, "Linha inválida ignorada: %s", linha);
            continue;
        }

        int i;
        for (i = 0; i < total_sensores; i++)
        {
            if (strcmp(sensores[i], l.id_sensor) == 0)
                break;
        }

        if (i == total_sensores)
        {
            if (total_sensores >= MAX_SENSORES)
            {
                fprintf(stderr, "Número máximo de sensores excedido.\n");
                break;
            }
            strcpy(sensores[total_sensores], l.id_sensor);
            capacidade[total_sensores] = PASSO_REALOC;
            leituras[total_sensores] = malloc(PASSO_REALOC * sizeof(struct Leitura));
            if (!leituras[total_sensores])
            {
                fprintf(stderr, "Erro de memória ao alocar sensor %s\n", l.id_sensor);
                exit(1);
            }
            total_sensores++;
        }

        if (contadores[i] == capacidade[i])
        {
            capacidade[i] += PASSO_REALOC;
            struct Leitura *tmp = realloc(leituras[i], capacidade[i] * sizeof(struct Leitura));
            if (!tmp)
            {
                fprintf(stderr, "Erro ao realocar memória para sensor %s\n", sensores[i]);
                exit(1);
            }
            leituras[i] = tmp;
        }
        leituras[i][contadores[i]++] = l;
    }
    fclose(entrada);

    for (int i = 0; i < total_sensores; i++)
    {
        qsort(leituras[i], contadores[i], sizeof(struct Leitura), comparar);

        char nome_arquivo[30];
        strcpy(nome_arquivo, sensores[i]);
        para_maiusculas(nome_arquivo);
        strcat(nome_arquivo, ".txt");

        if (access(nome_arquivo, F_OK) == 0)
        {
            printf("Aviso: sobrescrevendo o arquivo %s\n", nome_arquivo);
        }

        FILE *saida = fopen(nome_arquivo, "w");
        if (!saida)
        {
            fprintf(stderr, "Erro ao criar arquivo %s\n", nome_arquivo);
            continue;
        }

        for (int j = 0; j < contadores[i]; j++)
        {
            fprintf(saida, "%ld %s %s\n",
                    leituras[i][j].timestamp,
                    sensores[i],
                    leituras[i][j].valor);
        }
        fclose(saida);
        free(leituras[i]);
    }

    return 0;
}
