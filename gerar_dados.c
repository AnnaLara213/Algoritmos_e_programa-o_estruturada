#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

long gerar_timestamp_aleatorio(time_t inicio, time_t fim)
{
    return inicio + rand() % (fim - inicio + 1);
}

void gerar_valor(char *tipo, char *saida)
{
    if (strcmp(tipo, "int") == 0)
    {
        sprintf(saida, "%d", rand() % 100);
    }
    else if (strcmp(tipo, "float") == 0)
    {
        sprintf(saida, "%.2f", (float)(rand() % 10000) / 100);
    }
    else if (strcmp(tipo, "double") == 0)
    {
        sprintf(saida, "%.4lf", (double)(rand() % 100000) / 1000);
    }
    else if (strcmp(tipo, "bool") == 0)
    {
        sprintf(saida, "%s", rand() % 2 ? "true" : "false");
    }
    else
    {
        sprintf(saida, "0");
    }
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
    if (argc < 5 || (argc - 3) % 2 != 0)
    {
        printf("Uso: %s <DD/MM/AAAA_HH:MM:SS_INICIO> <DD/MM/AAAA_HH:MM:SS_FIM> <SENSOR1> <TIPO1> [<SENSOR2> <TIPO2> ...]\n", argv[0]);
        return 1;
    }

    srand(time(NULL));

    struct tm tm_inicio = {0}, tm_fim = {0};
    if (sscanf(argv[1], "%d/%d/%d_%d:%d:%d", &tm_inicio.tm_mday, &tm_inicio.tm_mon, &tm_inicio.tm_year,
               &tm_inicio.tm_hour, &tm_inicio.tm_min, &tm_inicio.tm_sec) != 6 ||
        sscanf(argv[2], "%d/%d/%d_%d:%d:%d", &tm_fim.tm_mday, &tm_fim.tm_mon, &tm_fim.tm_year,
               &tm_fim.tm_hour, &tm_fim.tm_min, &tm_fim.tm_sec) != 6)
    {
        fprintf(stderr, "Formato de data inválido. Use DD/MM/AAAA_HH:MM:SS\n");
        return 1;
    }

    tm_inicio.tm_mon -= 1;
    tm_inicio.tm_year -= 1900;
    tm_fim.tm_mon -= 1;
    tm_fim.tm_year -= 1900;

    time_t inicio = mktime(&tm_inicio);
    time_t fim = mktime(&tm_fim);

    if (inicio == -1 || fim == -1 || fim <= inicio)
    {
        fprintf(stderr, "Intervalo de tempo inválido.\n");
        return 1;
    }

    FILE *saida = fopen("dados_gerados.txt", "w");
    if (!saida)
    {
        perror("Erro ao criar arquivo de saída");
        return 1;
    }

    for (int i = 3; i < argc; i += 2)
    {
        char sensor[20];
        strncpy(sensor, argv[i], 19);
        sensor[19] = '\0';
        para_maiusculas(sensor);

        char *tipo = argv[i + 1];
        if (strcmp(tipo, "int") && strcmp(tipo, "float") && strcmp(tipo, "double") && strcmp(tipo, "bool"))
        {
            fprintf(stderr, "Tipo inválido para sensor %s: %s\n", sensor, tipo);
            continue;
        }

        for (int j = 0; j < 2000; j++)
        {
            long ts = gerar_timestamp_aleatorio(inicio, fim);
            char valor[50];
            gerar_valor(tipo, valor);
            fprintf(saida, "%ld %s %s\n", ts, sensor, valor);
        }
    }

    fclose(saida);
    printf("Arquivo 'dados_gerados.txt' criado com sucesso.\n");
    return 0;
}
