# Minigames do Dragão do Milhão

Os protótipos do Enzo ficam nas subpastas do módulo. Para consultar as duas interfaces a partir de `telas.c`, inclua `../minigames/minigames.h`. O Makefile compila os dois módulos.

## Sopro Ancestral

Chame `iniciarMinigameSopro()` uma vez quando o jogador abrir a tela. O marcador começa em 0 e vai até 50, depois volta. A zona de acerto é sorteada a cada tentativa.

Em cada quadro dessa tela, chame `atualizarMinigame(GetFrameTime(), IsKeyPressed(KEY_SPACE))`. `IsKeyPressed` conta o momento do aperto. Para desenhar, leia `posicao_barra`, `zona_inicio` e `zona_final` pelo ponteiro retornado por `obterEstadoMinigameSopro()`. Divida cada posição por `50.0f` e multiplique pela largura da barra para obter o deslocamento em pixels.

`obterResultadoMinigame()` retorna 0 enquanto a tentativa está em andamento, 1 se o jogador apertou na zona e -1 se apertou fora ou demorou 10 segundos.

## Trote Certeiro

`jogarMinigame_burro()` roda no terminal. O jogador informa duas posições de 0 a 5 por tentativa. Há seis cartas e três pares. Um par encontrado sai da rodada. Após dois pares errados, a partida termina. A função retorna 1 ao encontrar os três pares e -1 ao perder ou se a entrada for interrompida.

Para virar uma tela do jogo, mova o baralho, os pares encontrados e as chances para um estado que possa ser atualizado a cada clique. A função atual usa `scanf` e espera pela entrada do terminal. Os dois protótipos usam o sorteio já inicializado pela aplicação.

## Ligação com as ajudas

As ajudas ainda aplicam seus efeitos diretamente no quiz. Ao criar as telas, gaste um uso da ajuda quando o minigame abrir e aplique o efeito à pergunta atual quando o resultado for 1. O Eco da Torre ainda precisa da própria implementação.
