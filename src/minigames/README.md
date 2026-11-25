# Minigames do Dragão do Milhão

Cada minigame fica em uma subpasta do módulo. Para usar suas funções em `telas.c`, inclua `../minigames/minigames.h`. O Makefile compila as fontes dessas subpastas.

## Sopro Ancestral

Chame `iniciarMinigameSopro()` uma vez quando o jogador abrir a tela. O marcador começa em 0 e vai até 50, depois volta. A zona de acerto é sorteada a cada tentativa.

Em cada quadro dessa tela, chame `atualizarMinigame(GetFrameTime(), IsKeyPressed(KEY_SPACE))`. `IsKeyPressed` conta o momento do aperto. Para desenhar, leia `posicao_barra`, `zona_inicio` e `zona_final` pelo ponteiro retornado por `obterEstadoMinigameSopro()`. Divida cada posição por `50.0f` e multiplique pela largura da barra para obter o deslocamento em pixels.

`obterResultadoMinigame()` retorna 0 enquanto a tentativa está em andamento, 1 se o jogador apertou na zona e -1 se apertou fora ou demorou 10 segundos.

## Trote Certeiro

Clique na ajuda **Burro** durante uma pergunta para abrir um jogo da memória com seis cartas e três pares. Clique em duas cartas diferentes para revelar seus símbolos. O par fica visível por **0,8 segundo**. Cartas iguais saem do tabuleiro. Cartas diferentes viram de volta e descontam uma das **três chances**. Encontrar os três pares dá vitória. O terceiro erro encerra a rodada em derrota.

O Burro tem **dois usos por partida**, com limite de um uso por pergunta. Cada rodada consome um uso ao abrir. O botão **Voltar à pergunta** aparece ao final e retoma a pergunta que estava aberta. Na vitória, o Burro aponta uma alternativa com **70% de chance de acertar**. Na derrota, a pergunta volta sem palpite.

A interface guarda a rodada em `MinigameBurro`. `iniciarMinigameBurro` reinicia e embaralha o tabuleiro. `selecionar_Carta_Burro` recebe o índice de um clique. `atualizarMinigameBurro` acumula o tempo de revelação e resolve o par. O campo `resultado` vale `0` durante a rodada, `1` na vitória e `-1` na derrota.

## Ligação com as ajudas

Na integração das ajudas, consuma um uso ao abrir a rodada. Aplique a recompensa à pergunta original ao retornar de uma vitória. O Eco da Torre ainda precisa da própria implementação.
