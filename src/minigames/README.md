# Minigames do Dragão do Milhão

Cada minigame fica em uma subpasta do módulo. Para usar suas funções em `telas.c`, inclua `../minigames/minigames.h`. O Makefile compila as fontes dessas subpastas.

## Sopro Ancestral

A ajuda do Dragão tem um uso por partida, consumido ao abrir a rodada. O jogador tem **5 segundos** para encher a barra de **0 a 100**. A carga sobe **7 pontos** a cada novo aperto de Espaço e cai **22 pontos por segundo**, com mínimo em zero. Chegar a 100 antes do prazo dá vitória.

A interface guarda a rodada em uma variável `MinigameSopro` e chama `iniciarMinigameSopro(&sopro)` ao abrir a tela. Em cada quadro da rodada em andamento, chama `atualizarMinigame(&sopro, GetFrameTime(), janela_em_foco && IsKeyPressed(KEY_SPACE))`. `GetFrameTime()` fornece a duração do quadro em segundos e `IsKeyPressed` identifica cada novo aperto. A tela lê `sopro.carga` para preencher a barra e `sopro.tempo` para calcular o timer e a vinheta. O prazo usado na lógica e na tela vem de `SOPRO_TEMPO_LIMITE`.

O campo `sopro.resultado` vale `0` durante a rodada, `1` na vitória e `-1` quando o tempo acaba. Ao clicar em **Voltar à pergunta**, o jogador retoma a pergunta que estava aberta. Na vitória, a ajuda elimina duas alternativas incorretas. Na derrota, as alternativas permanecem como estavam.

## Trote Certeiro

`jogarMinigame_burro()` roda no terminal. O jogador informa duas posições de 0 a 5 por tentativa. Há seis cartas e três pares. Um par encontrado sai da rodada. Após dois pares errados, a partida termina. A função retorna 1 ao encontrar os três pares e -1 ao perder ou se a entrada for interrompida.

Para virar uma tela do jogo, mova o baralho, os pares encontrados e as chances para um estado que possa ser atualizado a cada clique. A função atual usa `scanf` e espera pela entrada do terminal. O baralho usa o sorteio já inicializado pela aplicação.

## Ligação com as ajudas

Os botões do Burro e da Princesa ainda aplicam seus efeitos diretamente no quiz. Para integrar as próximas telas, siga o fluxo do Sopro e aplique a recompensa apenas ao voltar de uma rodada vencida. O Eco da Torre ainda precisa da própria implementação.
