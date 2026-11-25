# Dragão do Milhão

Projeto desenvolvido em linguagem C pela equipe **Sete Copas**, para a disciplina de Fundamentos de Programação 2 da UTFPR — Campus Apucarana.

A proposta é um Jogo do Milhão com temática de games e narrativa de fantasia medieval em pixel art. Um dragão parte para libertar uma princesa aprisionada por um príncipe tirano. A partida planejada terá 15 perguntas, progressão por dificuldade e ajudas associadas a minigames.

A interface gráfica reúne o quiz e o gerenciamento das perguntas. O cadastro também está acessível pelo terminal.

- [Código do jogo](src/)
- [Perguntas iniciais](dados/perguntas_iniciais.csv)

## Equipe

- Enzo Andreassa Pizza;
- Gabriel Yudi Honji;
- Murilo Henrique Alves Martins.

## Funcionalidades implementadas

- Cadastrar perguntas em um vetor dinâmico;
- Listar todas as perguntas;
- Pesquisar, alterar e excluir perguntas pelo ID;
- Navegar pelas perguntas de uma dificuldade escolhida;
- Importar as perguntas iniciais de um CSV;
- Salvar e recuperar o banco em arquivo binário;
- Exportar o banco atual para CSV.

Cada pergunta possui identificador, enunciado, quatro alternativas, resposta correta e dificuldade. Os IDs são preservados durante alterações e deslocamentos no vetor.

## Compilação e execução

Para compilar no Linux, são necessários GCC, GNU Make e as bibliotecas de sistema usadas pelo Makefile, incluindo OpenGL e X11. Os cabeçalhos e a biblioteca da raylib fornecidos pelo professor estão nas pastas `include/` e `lib/`.

No Windows, o Makefile espera uma biblioteca raylib compatível na pasta `libwin/`, que precisa ser fornecida separadamente.

Execute os comandos **na raiz do repositório**, onde está o Makefile:

| Comando      | Operação                           |
| ------------ | ---------------------------------- |
| `make`       | Compila o projeto                  |
| `make run`   | Compila e abre a interface gráfica |
| `make clean` | Remove o executável gerado         |

Para abrir o cadastro no terminal, compile com `make` e execute `./dragao-do-milhao --terminal`. No Windows, depois de compilar com `mingw32-make`, execute `.\dragao-do-milhao.exe --terminal` no PowerShell.

No terminal, escolha uma opção pelo número correspondente. A opção `0` encerra o programa e salva o banco em binário. Na interface gráfica, o banco é salvo ao confirmar uma alteração em **Gerenciar perguntas**.

## Arquivos de dados

| Arquivo                          | Finalidade                         |
| -------------------------------- | ---------------------------------- |
| `dados/perguntas_iniciais.csv`   | Base inicial preparada pela equipe |
| `dados/perguntas.dat`            | Banco atualizado pelos dois modos  |
| `dados/perguntas_exportadas.csv` | Exportação solicitada pela opção 7 |

Na primeira execução, quando o binário ainda não existe, o programa importa o CSV inicial. Nas execuções seguintes, recupera o banco pelo binário. Alterar o CSV inicial depois disso não atualiza automaticamente o banco salvo.

Para carregar um CSV inicial atualizado, feche o jogo e, se `dados/perguntas.dat` existir, mova o arquivo para fora do repositório. Depois, execute `make run`. O banco anterior preserva as alterações feitas no cadastro e pode ser guardado como cópia de segurança. Como o binário é gerado localmente e ignorado pelo Git, cada cópia do projeto pode ter seu próprio banco salvo.

A exportação gera uma cópia do banco atual em texto e substitui uma exportação anterior.

## Formato do CSV

O CSV utiliza uma pergunta por linha, seguindo este cabeçalho:

```csv
id,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,resposta,dificuldade
```

As respostas devem ser letras maiúsculas de `A` a `D`. Os níveis de dificuldade são:

| Valor | Dificuldade   |
| ----- | ------------- |
| 1     | Muito fácil   |
| 2     | Fácil         |
| 3     | Médio         |
| 4     | Difícil       |
| 5     | Muito difícil |

A implementação atual utiliza CSV simples, com textos sem vírgulas ou quebras de linha dentro dos campos. A importação comporta até 150 perguntas. Os limites de texto são 500 bytes para o enunciado e 100 para cada alternativa

Os campos numéricos da interface pressupõem a digitação de números. Valores fora dos intervalos permitidos são tratados pelo programa.

## Organização do código

A organização principal do código é:

```text
src/
├── main.c
└── perguntas/
    ├── pergunta.h
    ├── banco_perguntas.c
    ├── banco_perguntas.h
    ├── arquivos_perguntas.c
    └── arquivos_perguntas.h
```

A `main` coordena a inicialização e o menu. No terminal, ela lê os dados, chama o banco e mostra a mensagem correspondente ao retorno da operação. O módulo `banco_perguntas` gerencia a coleção e faz as consultas. O módulo `arquivos_perguntas` cuida da importação, persistência e exportação.

O terminal e a interface gráfica podem usar as mesmas funções de cadastro. `inserir_pergunta` recebe uma `Pergunta` preenchida e atribui um ID, reaproveitando os IDs excluídos na ordem das exclusões. `alterar_pergunta` recebe o ID e uma estrutura com os novos dados e mantém o identificador original. `excluir_pergunta` recebe o ID a excluir. A inserção e a alteração verificam a resposta e a dificuldade antes de modificar o banco.

Essas operações atualizam a coleção em memória e devolvem um código para o chamador. Os parâmetros e os códigos de retorno de cada função estão documentados em [banco_perguntas.c](src/perguntas/banco_perguntas.c). No fluxo do terminal, o banco é salvo ao encerrar o programa.

## Banco inicial de perguntas

O arquivo `dados/perguntas_iniciais.csv` contém 120 perguntas da equipe, agrupadas em ordem crescente de dificuldade.

| Dificuldade   | Quantidade |
| ------------- | ---------: |
| Muito fácil   |         30 |
| Fácil         |         30 |
| Médio         |         25 |
| Difícil       |         20 |
| Muito difícil |         15 |
| **Total**     |    **120** |
