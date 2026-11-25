# Minigames do Dragão do Milhão

Cada minigame tem uma subpasta com as regras e o estado da rodada. As telas ficam em `src/interface/minigames/` e as trocas de tela ficam em `src/interface/telas.c`. O Makefile compila as fontes dessas subpastas.

## Sopro Ancestral

A ajuda do Dragão tem um uso por partida, consumido ao abrir a rodada. O jogador tem **5 segundos** para encher a barra de **0 a 100**. A carga sobe **7 pontos** a cada novo aperto de Espaço e cai **22 pontos por segundo**, com mínimo em zero. Chegar a 100 antes do prazo dá vitória.

A interface guarda a rodada em uma variável `MinigameSopro` e chama `iniciarMinigameSopro(&sopro)` ao abrir a tela. Em cada quadro da rodada em andamento, chama `atualizarMinigame(&sopro, GetFrameTime(), janela_em_foco && IsKeyPressed(KEY_SPACE))`. `GetFrameTime()` fornece a duração do quadro em segundos e `IsKeyPressed` identifica cada novo aperto. A tela lê `sopro.carga` para preencher a barra e `sopro.tempo` para calcular o timer e a vinheta. O prazo usado na lógica e na tela vem de `SOPRO_TEMPO_LIMITE`.

O campo `sopro.resultado` vale `0` durante a rodada, `1` na vitória e `-1` quando o tempo acaba. Ao clicar em **Voltar à pergunta**, o jogador retoma a pergunta que estava aberta. Na vitória, a ajuda elimina duas alternativas incorretas. Na derrota, as alternativas permanecem como estavam.

## Trote Certeiro

Clique na ajuda **Burro** durante uma pergunta para abrir um jogo da memória com seis cartas e três pares. Clique em duas cartas diferentes para revelar seus símbolos. O par fica visível por **0,8 segundo**. Cartas iguais saem do tabuleiro. Cartas diferentes viram de volta e descontam uma das **três chances**. Encontrar os três pares dá vitória. O terceiro erro encerra a rodada em derrota.

O Burro tem **dois usos por partida**, com limite de um uso por pergunta. Cada rodada consome um uso ao abrir. O botão **Voltar à pergunta** aparece ao final e retoma a pergunta que estava aberta. Na vitória, o Burro aponta uma alternativa com **60% de chance de acertar**. Na derrota, a pergunta volta sem palpite.

A interface do Trote fica em `src/interface/minigames/tela_burro.c`. A rodada fica em `MinigameBurro`. `iniciarMinigameBurro` reinicia e embaralha o tabuleiro. `selecionar_Carta_Burro` recebe o índice de um clique. `atualizarMinigameBurro` acumula o tempo de revelação e resolve o par. O campo `resultado` vale `0` durante a rodada, `1` na vitória e `-1` na derrota.

## Eco da Torre

Clique na ajuda **Princesa** e aperte **Espaço** para começar. Cada novo aperto faz a pomba subir. Passe por **sete vãos** para entregar a carta. Bater em um obstáculo, no topo ou no chão encerra a rodada.

A Princesa tem **um uso por partida**, consumido ao abrir o desafio. Ao clicar em **Voltar à pergunta**, a vitória revela a resposta correta. Em caso de derrota, a pergunta volta sem receber a dica.

`FlappyBird` guarda a posição da pomba, o obstáculo e o placar. `atualizaMinigame_flappybird` recebe a duração de um quadro e a entrada de Espaço. `tela_eco.c` usa as mesmas medidas da lógica para desenhar o campo e as partes sólidas do obstáculo.
