# Dragão do Milhão

Projeto em equipe de um jogo de perguntas e respostas em C, com uma história de fantasia medieval em pixel art. O repositório inclui código da interface com raylib e um banco inicial de perguntas em CSV.

## Estado atual

O programa abre um menu no terminal para cadastrar, listar, pesquisar, editar e excluir perguntas. O fluxo de jogo completo faz parte do conceito abaixo.

## Compilação

No Linux, o Makefile usa GCC, make, raylib e as bibliotecas de desenvolvimento do OpenGL e do X11. Na raiz do projeto, rode `make run` para compilar e abrir o programa. No Windows, o Makefile espera uma biblioteca raylib compatível em `libwin/`, que precisa ser fornecida separadamente.

- [Código do jogo](src/)
- [Perguntas iniciais](dados/perguntas_iniciais.csv)

## Conceito do jogo

O projeto continua sendo um Jogo do Milhão em essência: perguntas de múltipla escolha, quinze questões, progressão por dificuldade e uso de ajudas. Em vez de um programa apenas textual, o jogo contará uma pequena narrativa de fantasia medieval em pixel art: um dragão tenta libertar uma princesa que foi aprisionada por um príncipe tirano
