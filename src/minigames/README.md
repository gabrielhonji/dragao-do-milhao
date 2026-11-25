# Minigames do Dragão do Milhão

Cada minigame tem uma subpasta com as regras e o estado da rodada. As telas do Sopro e do Trote ficam em `src/interface/minigames/`, e as trocas de tela ficam em `src/interface/telas.c`. O Makefile compila as fontes dessas subpastas.

## Sopro Ancestral

A ajuda do Dragão tem um uso por partida, consumido ao abrir a rodada. O jogador tem **5 segundos** para encher a barra de **0 a 100**. A carga sobe **7 pontos** a cada novo aperto de Espaço e cai **22 pontos por segundo**, com mínimo em zero. Chegar a 100 antes do prazo dá vitória.

A interface guarda a rodada em uma variável `MinigameSopro` e chama `iniciarMinigameSopro(&sopro)` ao abrir a tela. Em cada quadro da rodada em andamento, chama `atualizarMinigame(&sopro, GetFrameTime(), janela_em_foco && IsKeyPressed(KEY_SPACE))`. `GetFrameTime()` fornece a duração do quadro em segundos e `IsKeyPressed` identifica cada novo aperto. A tela lê `sopro.carga` para preencher a barra e `sopro.tempo` para calcular o timer e a vinheta. O prazo usado na lógica e na tela vem de `SOPRO_TEMPO_LIMITE`.

O campo `sopro.resultado` vale `0` durante a rodada, `1` na vitória e `-1` quando o tempo acaba. Ao clicar em **Voltar à pergunta**, o jogador retoma a pergunta que estava aberta. Na vitória, a ajuda elimina duas alternativas incorretas. Na derrota, as alternativas permanecem como estavam.

## Trote Certeiro

Clique na ajuda **Burro** durante uma pergunta para abrir um jogo da memória com seis cartas e três pares. Clique em duas cartas diferentes para revelar seus símbolos. O par fica visível por **0,8 segundo**. Cartas iguais saem do tabuleiro. Cartas diferentes viram de volta e descontam uma das **três chances**. Encontrar os três pares dá vitória. O terceiro erro encerra a rodada em derrota.

O Burro tem **dois usos por partida**, com limite de um uso por pergunta. Cada rodada consome um uso ao abrir. O botão **Voltar à pergunta** aparece ao final e retoma a pergunta que estava aberta. Na vitória, o Burro aponta uma alternativa com **70% de chance de acertar**. Na derrota, a pergunta volta sem palpite.

A interface do Trote fica em `src/interface/minigames/tela_burro.c`. A rodada fica em `MinigameBurro`. `iniciarMinigameBurro` reinicia e embaralha o tabuleiro. `selecionar_Carta_Burro` recebe o índice de um clique. `atualizarMinigameBurro` acumula o tempo de revelação e resolve o par. O campo `resultado` vale `0` durante a rodada, `1` na vitória e `-1` na derrota.

## Ligação com as ajudas

O Sopro e o Trote consomem um uso ao abrir a rodada e aplicam a recompensa à pergunta original ao retornar de uma vitória. O botão da Princesa ainda revela a resposta diretamente no quiz. O protótipo do Eco da Torre fica em `minigame_flappybird/` e ainda precisa ser ligado a essa ajuda.
