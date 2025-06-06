#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>

struct Leitura
{
    long timestamp;
    char id_sensor[20];
    char valor[50];
};

long abs_diff(long a, long b)
{
    return a > b ? a - b : b - a;
}

long converter_para_timestamp(const char *data_str)
{
    struct tm t = {0};
    if (sscanf(data_str, "%d/%d/%d_%d:%d:%d", &t.tm_mday, &t.tm_mon, &t.tm_year,
               &t.tm_hour, &t.tm_min, &t.tm_sec) != 6)
    {
        return -1;
    }
    t.tm_mon -= 1;
    t.tm_year -= 1900;
    t.tm_isdst = -1;
    return mktime(&t);
}

void para_maiusculas(char *str)
{
    for (int i = 0; str[i]; i++)
        str[i] = toupper((unsigned char)str[i]);
}

int main(int argc, char *argv[])
{
    if (argc < 3)
    {
        printf("Uso: %s <ID_SENSOR> <DATA no formato DD/MM/AAAA_HH:MM:SS>\n", argv[0]);
        return 1;
    }

    char nome_arquivo[30];
    strcpy(nome_arquivo, argv[1]);
    para_maiusculas(nome_arquivo);
    strcat(nome_arquivo, ".txt");

    long alvo = converter_para_timestamp(argv[2]);
    if (alvo == -1)
    {
        fprintf(stderr, "Data inválida. Use o formato DD/MM/AAAA_HH:MM:SS\n");
        return 1;
    }

    FILE *arquivo = fopen(nome_arquivo, "r");
    if (!arquivo)
    {
        perror("Erro ao abrir arquivo do sensor");
        return 1;
    }

    struct Leitura *dados = NULL;
    int capacidade = 1000, n = 0;
    dados = malloc(capacidade * sizeof(struct Leitura));

    while (fscanf(arquivo, "%ld %s %s", &dados[n].timestamp, dados[n].id_sensor, dados[n].valor) == 3)
    {
        n++;
        if (n == capacidade)
        {
            capacidade *= 2;
            dados = realloc(dados, capacidade * sizeof(struct Leitura));
            if (!dados)
            {
                fprintf(stderr, "Erro ao realocar memória\n");
                fclose(arquivo);
                return 1;
            }
        }
    }
    fclose(arquivo);

    int esquerda = 0, direita = n - 1, melhor = -1;
    while (esquerda <= direita)
    {
        int meio = (esquerda + direita) / 2;
        if (melhor == -1 || abs_diff(dados[meio].timestamp, alvo) < abs_diff(dados[melhor].timestamp, alvo))
        {
            melhor = meio;
        }
        if (dados[meio].timestamp < alvo)
        {
            esquerda = meio + 1;
        }
        else
        {
            direita = meio - 1;
        }
    }

    if (melhor != -1)
    {
        printf("Leitura mais próxima:\n%ld %s %s\n", dados[melhor].timestamp, dados[melhor].id_sensor, dados[melhor].valor);
    }
    else
    {
        printf("Nenhuma leitura encontrada.\n");
    }

    free(dados);
    return 0;
}
