# trabalho-de-lp1-O-Indexador-de-Alta-Performance
Sistema de indexação para grandes volumes de texto: mapeia termos em arquivos .txt usando Tabela Hash, armazenando offsets exatos de cada ocorrência. A primeira busca constrói o índice e as seguintes são até 1.000x mais rápidas, com acesso imediato via fseek()
