#include "trabalho_CH/buscador.h"

int main()
{
    SetConsoleOutputCP(CP_UTF8); // terminal aceita UTF-8
    SetConsoleCP(CP_UTF8);

    for (int i = 0; i < TAM_TABELA; i++)
    {
        tabela[i] = NULL;
    }

    char caminhoPasta[200];
    printf("Digite o caminho da pasta com os arquivos .txt: ");
    scanf("%199s", caminhoPasta);
    abrir_pasta(caminhoPasta);

    // loop de buscas até o usuário digitar 'sair'
    char palavraBusca[51];
    while (1)
    {
        printf("\nPesquise o termo (ou 'sair' para parar): ");
        scanf("%50s", palavraBusca);

        if (strcmp(palavraBusca, "sair") == 0)
            break;

        normalizar_palavra(palavraBusca);

        if (strlen(palavraBusca) < 5)
        {
            buscar_termo_curto(palavraBusca, caminhoPasta);
        }
        else
        {
            buscar_termo(palavraBusca);
        }
    }

    return 0;
}