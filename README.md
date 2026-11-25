# Dragão do Milhão

Um dragão parte para resgatar uma princesa aprisionada por um príncipe tirano. Para chegar ao castelo e vencer a batalha, o jogador responde a **15 perguntas sobre videogames**. Cada pergunta tem quatro alternativas. Três minigames oferecem ajuda durante a partida.

Projeto desenvolvido em **C**, com interface gráfica em **raylib**, pela equipe **Sete Copas** para a disciplina de Fundamentos de Programação 2 da UTFPR — Campus Apucarana.

## Como compilar e executar

Abra no VS Code a pasta `dragao-do-milhao`, onde está o `Makefile`. Execute os comandos no terminal dessa pasta para que o jogo encontre as perguntas, as imagens e as músicas.

| Ação                                       | Linux                           | Windows (PowerShell)                |
| ------------------------------------------ | ------------------------------- | -------------------------------     |
| Compilar                                   | `make`                          | `mingw32-make`                      |
| Compilar e abrir o jogo                    | `make run`                      | `mingw32-make run`                  |
| Abrir o cadastro no terminal após compilar | `./dragao-do-milhao --terminal` | `.\dragao-do-milhao.exe --terminal` |
| Remover o executável principal             | `make clean`                    | `mingw32-make clean`                |

No **Linux**, são necessários GCC, GNU Make e as bibliotecas de desenvolvimento de OpenGL e X11. Os cabeçalhos da raylib e a biblioteca para Linux acompanham o projeto em `include/` e `lib/`.

No **Windows**, a equipe utiliza GCC, Make e raylib do ambiente **MSYS2 UCRT64**, com a pasta `ucrt64\bin` no `Path`. O Makefile também procura a biblioteca em `libwin/` caso ela seja fornecida separadamente. As instruções de instalação estão no [guia de configuração](documentacao/configuracao.md#windows).

Para iniciar pela interface do VS Code, abra **Terminal → Executar Tarefa** e escolha **Abrir jogo no modo visual**.

## Como jogar

No menu inicial, escolha **Jogar**. Clique em uma alternativa e pressione **Enter** para confirmar. Clicar novamente na alternativa selecionada também confirma a resposta. Após ver o resultado, avance para a próxima pergunta. Use o botão **Tela cheia** no menu ou alterne com **F11** ou **Alt+Enter** durante a partida. Para iniciar em tela cheia, execute `./dragao-do-milhao --tela-cheia` no Linux ou `.\dragao-do-milhao.exe --tela-cheia` no PowerShell.

O jogo sorteia uma pergunta do nível correspondente a cada etapa. O histórico mantém os IDs já sorteados durante toda a partida, inclusive após um renascimento.

| Questões | Dificuldade   | Etapa |
| -------- | ------------- | ----------------------------------------------------------------------------|
| 1 e 2    | Muito fácil   | Início da jornada até o castelo                                             |
| 3 e 4    | Fácil         | Avanço pelo mapa                                                            |
| 5 a 8    | Média         | Primeiro marco ao acertar a questão 5                                       |
| 9 a 12   | Difícil       | Início da batalha contra o príncipe e segundo marco ao acertar a questão 10 |
| 13 a 15  | Muito difícil | Despertar dracônico ao acertar a 13ª e resgate da princesa ao acertar a 15ª |

**Marcos e renascimento.** Cada marco conquistado concede um ovo. Se o jogador errar e tiver um ovo, ele é consumido e a partida recomeça a partir da pergunta seguinte ao último marco. Por exemplo, quem já alcançou o marco 10 volta à questão 11. Um erro sem ovos encerra a partida em derrota.

**Retirada Tática e ranking.** O jogador pode confirmar a retirada em qualquer pergunta antes de responder. A retirada encerra a partida e leva o dragão à gruta. O jogo pede o nome para registrar uma vitória ou uma retirada a partir do primeiro marco. A tela de ranking exibe os oito primeiros resultados, ordenados por mais acertos, mais ovos restantes e, em caso de novo empate, menor tempo com uma pergunta aberta.

### Ajudas e minigames

Cada ajuda consome um uso ao abrir o minigame. Ao terminar, escolha **Voltar à pergunta**. A vitória libera o efeito na pergunta que estava aberta. Se perder o desafio, o jogador ainda pode responder à mesma pergunta.

| Ajuda                        | Como vencer                                                        | Efeito da vitória                                    | Usos por partida           |
| ---------------------------- | ------------------------------------------------------------------ | ---------------------------------------------------- | -------------------------- |
| **Dragão · Sopro Ancestral** | Aperte **Espaço** repetidamente para encher a barra.               | Elimina duas alternativas incorretas.                | 1                          |
| **Burro · Trote Certeiro**   | Clique nas cartas e encontre os três pares antes do terceiro erro. | Aponta uma alternativa com 70% de chance de acertar. | 2, em perguntas diferentes |
| **Princesa · Eco da Torre**  | Aperte **Espaço** para bater as asas e guie a pomba por sete vãos. | Revela a alternativa correta.                        | 1                          |

Os parâmetros de cada desafio e a organização de suas funções estão no [guia dos minigames](src/minigames/README.md).

## Gerenciamento das perguntas

Escolha **Gerenciar perguntas** no menu gráfico para cadastrar, listar, buscar por ID, filtrar por dificuldade, editar ou excluir. A interface salva cada operação confirmada. No modo `--terminal`, o menu também permite navegar pelas perguntas de um nível. A opção **7** exporta o banco para CSV e a opção **0** encerra o cadastro e salva as alterações no binário.

Cada pergunta possui um ID próprio, preservado ao editar a pergunta ou deslocá-la no vetor. Ao cadastrar, o banco reaproveita primeiro os IDs excluídos, na ordem das exclusões.

### Arquivos de dados

| Arquivo                          | Finalidade                                                                             |
| -------------------------------- | -------------------------------------------------------------------------------------- |
| `dados/perguntas_iniciais.csv`   | Base usada na primeira execução.                                                       |
| `dados/perguntas_adicionais.csv` | Material auxiliar da equipe. A inicialização usa o CSV da linha anterior.              |
| `dados/perguntas.dat`            | Banco local com as perguntas, o próximo ID e os IDs disponíveis para reaproveitamento. |
| `dados/perguntas_exportadas.csv` | Cópia do banco atual gerada pela opção 7. Uma nova exportação substitui a anterior.    |
| `dados/ranking.dat`              | Resultados registrados após vitórias ou retiradas a partir do primeiro marco.          |

Na primeira execução, o jogo importa o CSV inicial e cria `dados/perguntas.dat` antes de abrir o menu. Nas execuções seguintes, carrega o DAT. Para usar uma nova versão do CSV inicial, feche o jogo, **mova o DAT atual para uma pasta de backup** e abra o jogo novamente. Isso preserva os cadastros anteriores na cópia de segurança. O ranking fica em um arquivo independente.

### Formato e distribuição do CSV

A base inicial contém **120 perguntas**, agrupadas em ordem crescente de dificuldade. Cada linha corresponde a uma pergunta e segue este cabeçalho:

```csv
id,enunciado,alternativa_a,alternativa_b,alternativa_c,alternativa_d,resposta,dificuldade
```

| Valor no CSV | Dificuldade   | Perguntas na base inicial |
| ------------ | ------------- | ------------------------- |
| 1            | Muito fácil   | 30                        |
| 2            | Fácil         | 30                        |
| 3            | Média         | 25                        |
| 4            | Difícil       | 20                        |
| 5            | Muito difícil | 15                        |

Ao preparar o CSV, use IDs inteiros únicos, gabaritos de `A` a `D` em maiúsculas e textos sem vírgulas ou quebras de linha. Respeite os limites de **500 bytes** para o enunciado e **100 bytes** por alternativa. Letras acentuadas podem ocupar mais de um byte. O importador lê até **150 perguntas**. A mesma restrição de vírgulas vale para perguntas cadastradas que serão exportadas.

## Organização do projeto

| Caminho                          | Responsabilidade                                                                                            |
| -------------------------------- | ----------------------------------------------------------------------------------------------------------- |
| [src/main.c](src/main.c)         | Carregamento inicial do banco, seleção do modo de execução e menu de terminal.                              |
| [src/perguntas/](src/perguntas/) | Estrutura da pergunta, operações sobre o vetor dinâmico, importação, exportação e persistência.             |
| [src/quiz/](src/quiz/)           | Sorteio, respostas, marcos, renascimento, retirada, PV do príncipe e classificação do ranking.              |
| [src/minigames/](src/minigames/) | Regras e estado do Sopro Ancestral, do Trote Certeiro e do Eco da Torre.                                    |
| [src/interface/](src/interface/) | Navegação entre telas, componentes gráficos, áudio e telas do menu, cadastro, partida, minigames e ranking. |
| [assets/](assets/)               | Imagens, fontes e trilhas sonoras usadas na interface.                                                      |
| [dados/](dados/)                 | Arquivos de perguntas e dados locais gerados pelo programa.                                                 |

## Equipe e créditos

**Enzo Andreassa Pizza**, **Gabriel Yudi Honji** e **Murilo Henrique Alves Martins** — equipe Sete Copas.

As atribuições das músicas estão nos [créditos das trilhas](assets/audio/creditos.md). A licença da Pixelify Sans acompanha o [arquivo da fonte](assets/fontes/OFL.txt).
