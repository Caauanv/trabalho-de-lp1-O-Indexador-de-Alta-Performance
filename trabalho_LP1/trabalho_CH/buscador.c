#include "buscador.h"

RegistroIndice *tabela[TAM_TABELA];

/* transforma a palavra em um número (índice) para a tabela hash
   soma os valores ASCII de cada letra e divide pelo tamanho da tabela */
static int hash(const char *palavra)
{
    int soma = 0;
    int i = 0;
    while (palavra[i] != '\0')
    {
        soma += palavra[i];
        i++;
    }
    return soma % TAM_TABELA;
}

/* remove pontuação e converte tudo pra minúsculo
   ex: "Histórico," vira "historico" */
void normalizar_palavra(char *palavra)
{
    char temp[100];
    int i = 0, j = 0;
    while (palavra[i] != '\0')
    {
        /* isalnum: retorna verdadeiro se o caractere for letra (a-z, A-Z) ou numero (0-9)
   usamos para descartar pontuacao como virgulas, pontos e aspas da palavra */
        if (isalnum(palavra[i]))
            if (isalnum(palavra[i]))
            {
                temp[j] = tolower(palavra[i]);
                j++;
            }
        i++;
    }
    temp[j] = '\0';
    strcpy(palavra, temp);
}

/* lê o arquivo palavra por palavra, salvando o offset de cada uma
   e insere na tabela hash (só palavras com 5+ caracteres) */
void abrir_arq(const char *nomeArquivo)
{
    FILE *fp = fopen(nomeArquivo, "rt");
    if (fp == NULL)
    {
        printf("Erro ao abrir %s\n", nomeArquivo);
        return;
    }
    printf("arquivo aberto: %s\n", nomeArquivo);

    char palavra[100];
    long offset;

    while (1)
    {
        offset = ftell(fp); // salva posição ANTES de ler
        if (fscanf(fp, "%99s", palavra) != 1)
            break;

        normalizar_palavra(palavra);

        if (strlen(palavra) >= 5)
        {
            inserir_termo(palavra, nomeArquivo, offset);
        }
    }
    fclose(fp);
}

/* usa o comando 'dir' do Windows pra listar os .txt da pasta
   e chama abrir_arq pra cada um.
   _popen: executa um comando do terminal e retorna um FILE* pra ler a saida
   aqui roda o comando 'dir' do Windows pra listar os arquivos .txt da pasta
   _pclose: fecha esse "pipe", equivalente ao fclose para _popen

   */
void abrir_pasta(const char *caminho)
{
    char comando[200];
    /* snprintf: igual printf mas escreve numa string em vez da tela
   o segundo argumento limita o tamanho pra nao estourar o buffer */

    snprintf(comando, sizeof(comando), "dir \"%s\\*.txt\" /b", caminho);

    FILE *resultado = _popen(comando, "r");
    if (resultado == NULL)
    {
        printf("Erro ao ler a pasta\n");
        return;
    }

    char nomeArquivo[100];
    char caminhoCompleto[260];

    while (fscanf(resultado, "%99s", nomeArquivo) == 1)
    {
        snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s\\%s", caminho, nomeArquivo);
        abrir_arq(caminhoCompleto);
    }
    _pclose(resultado);
}

/* procura o termo na tabela hash e retorna o registro se achar
   percorre a lista encadeada da posição calculada pelo hash */
RegistroIndice *buscar_registro(const char *palavra)
{
    int indice = hash(palavra);
    RegistroIndice *atual = tabela[indice];
    while (atual != NULL)
    {
        if (strcmp(atual->termo, palavra) == 0)
            return atual;
        atual = atual->prox;
    }
    return NULL;
}

/* cria uma nova ocorrência e "pendura" no início da lista do registro */
void adicionar_ocorrencia(RegistroIndice *registro, const char *nome_ARQ, long offset)
{
    Ocorrencia *nova = (Ocorrencia *)malloc(sizeof(Ocorrencia));
    strcpy(nova->nome_arquivo, nome_ARQ);
    nova->offset = offset;
    nova->prox = registro->lista;
    registro->lista = nova;
}

/* insere o termo na tabela hash
   se já existe: só adiciona nova ocorrência
   se não existe: cria novo registro e adiciona a ocorrência */
void inserir_termo(const char *palavra, const char *nome_arq, long offset)
{
    RegistroIndice *registro = buscar_registro(palavra);
    if (registro != NULL)
    {
        adicionar_ocorrencia(registro, nome_arq, offset);
        return;
    }

    int indice = hash(palavra);
    RegistroIndice *novo = (RegistroIndice *)malloc(sizeof(RegistroIndice));
    strcpy(novo->termo, palavra);
    novo->lista = NULL;
    novo->prox = tabela[indice];
    tabela[indice] = novo;

    adicionar_ocorrencia(novo, nome_arq, offset);
}

/* vai até o offset no arquivo com fseek e lê 50 caracteres ao redor
   da palavra para mostrar o contexto

   fread: le blocos de bytes do arquivo diretamente para o buffer
aqui le 50 bytes a partir da posicao do fseek para montar o trecho
   */
void mostrar_contexto(const char *nomeArquivo, long offset)
{
    FILE *fp = fopen(nomeArquivo, "rt");
    if (fp == NULL)
    {
        return;
    }
    long inicio = offset - 20;
    if (inicio < 0)
    {
        inicio = 0;
    }
    fseek(fp, inicio, SEEK_SET);

    char trecho[51];
    int lidos = fread(trecho, sizeof(char), 50, fp);
    trecho[lidos] = '\0';

    for (int i = 0; i < lidos; i++)
    {

        if (trecho[i] == '\n' || trecho[i] == '\r')
        {
            trecho[i] = ' ';
        }
    }

    printf("...%s...\n", trecho);
    fclose(fp);
}

/* busca o termo na tabela hash e lista todas as ocorrências
   agrupa por arquivo: só printa o nome do arquivo uma vez */
void buscar_termo(const char *palavra)
{
    RegistroIndice *registro = buscar_registro(palavra);
    if (registro == NULL)
    {
        printf("\nPalavra nao encontrada.\n");
        return;
    }

    int quantidade = 0;
    Ocorrencia *atual = registro->lista;
    while (atual != NULL)
    {
        quantidade++;
        atual = atual->prox;
    }

    printf("\nResultados para '%s'\n", palavra);
    printf("Quantidade de ocorrencias: %d\n", quantidade);

    atual = registro->lista;
    while (atual != NULL)
    {
        /* pega só o nome do arquivo sem o caminho completo
        Esse nome++ serve pra pular a barra \ do caminho.O strrchr retorna um ponteiro apontando para a barra em si para printar o nome do arquivo sem essa barra
        */
        const char *nome = strrchr(atual->nome_arquivo, '\\');
        if (nome == NULL)
            nome = atual->nome_arquivo;
        else
            nome++;

        printf("\n> Arquivo: %s <\n", nome);
        mostrar_contexto(atual->nome_arquivo, atual->offset);

        atual = atual->prox;
    }
}

/* para termos curtos (menos de 5 chars): varre os arquivos na hora
mostra cada ocorrência e aguarda Enter para continuar ou Esc para sair

fflush: força o terminal a exibir tudo que esta no buffer imediatamente
   necessario aqui pois o printf anterior nao tem '\n' e poderia nao aparecer
*/
void buscar_termo_curto(const char *palavra, const char *caminho)
{
    char comando[200];
    snprintf(comando, sizeof(comando), "dir \"%s\\*.txt\" /b", caminho);

    FILE *resultado = _popen(comando, "r");
    if (resultado == NULL)
        return;

    char nomeArquivo[100];
    char caminhoCompleto[260];

    while (fscanf(resultado, "%99s", nomeArquivo) == 1)
    {
        snprintf(caminhoCompleto, sizeof(caminhoCompleto), "%s\\%s", caminho, nomeArquivo);

        FILE *fp = fopen(caminhoCompleto, "r");
        if (fp == NULL)
            continue;

        char termo_lido[100];
        long offset;
        int encontrou = 0;

        while (1)
        {
            offset = ftell(fp);
            if (fscanf(fp, "%99s", termo_lido) != 1)
                break;
            normalizar_palavra(termo_lido);

            if (strcmp(termo_lido, palavra) == 0)
            {
                if (!encontrou)
                    printf("\n> Arquivo: %s <\n", nomeArquivo);

                mostrar_contexto(caminhoCompleto, offset);
                encontrou = 1;

                printf("\n");
                printf("Enter para continuar ou Esc para voltar: ");
                fflush(stdout);
                int tecla = _getch();
                printf("\n");

                if (tecla == 27) // ESC
                {
                    fclose(fp);
                    _pclose(resultado);
                    return;
                }
            }
        }

        if (!encontrou)
            printf("\nNenhuma ocorrencia de '%s' em %s\n", palavra, nomeArquivo);

        fclose(fp);
    }
    _pclose(resultado);
}