#ifndef BUSCADOR_H
#define BUSCADOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <conio.h>
#include <Windows.h>

#define TAM_PALAVRA 51
#define TAM_ARQUIVO 260
#define TAM_TABELA 1009

typedef struct ocorrencia
{
    char nome_arquivo[TAM_ARQUIVO];
    long offset;
    struct ocorrencia *prox;
} Ocorrencia;

typedef struct registro
{
    char termo[TAM_PALAVRA];
    Ocorrencia *lista;
    struct registro *prox;
} RegistroIndice;

extern RegistroIndice *tabela[TAM_TABELA];

static int hash(const char *palavra);
void abrir_arq(const char *nomeArquivo);
void abrir_pasta(const char *caminho);
void inserir_termo(const char *palavra, const char *nome_arq, long offset);
RegistroIndice *buscar_registro(const char *palavra);
void adicionar_ocorrencia(RegistroIndice *registro, const char *nome_ARQ, long offset);
void mostrar_contexto(const char *nomeArquivo, long offset);
void buscar_termo(const char *palavra);
void buscar_termo_curto(const char *palavra, const char *caminho);
void normalizar_palavra(char *palavra);

#include "buscador.c"
#endif