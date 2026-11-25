/**
 * @file audio.c
 * @brief Carrega, troca e reproduz as trilhas da Jornada e da Batalha.
 */

#include "audio.h"
#include <raylib.h>

// O índice de cada faixa é o valor de TrilhaAudio. O espaço do silêncio fica vazio.
static Music trilhas[TOTAL_TRILHAS] = {0};
static TrilhaAudio trilha_atual = TRILHA_SILENCIO;
static bool som_ligado = true;

/**
 * @brief Abre uma faixa para leitura em partes e configura o loop.
 * @param caminho Caminho do arquivo relativo à pasta do jogo.
 * @param volume Volume inicial da faixa, entre 0 e 1.
 * @return Faixa preparada ou um Music inválido se o arquivo não puder ser aberto.
 */
static Music carregar_trilha(const char *caminho, float volume)
{
    Music musica = LoadMusicStream(caminho);
    if (IsMusicValid(musica)) {
        musica.looping = true;
        SetMusicVolume(musica, volume);
    }
    return musica;
}

void iniciar_audio(void)
{
    InitAudioDevice();
    if (!IsAudioDeviceReady()) {
        TraceLog(LOG_WARNING, "Dispositivo de áudio indisponível");
        return; // O jogo pode seguir com o vetor de faixas vazio.
    }

    trilhas[TRILHA_JORNADA] = carregar_trilha("assets/audio/menu_jornada.ogg", 0.28f);
    trilhas[TRILHA_BATALHA] = carregar_trilha("assets/audio/batalha.wav", 0.35f);
}

void atualizar_audio(TrilhaAudio trilha)
{
    // Telas com a mesma trilha preservam a posição da música ao navegar.
    if (trilha != trilha_atual) {
        // Para a faixa anterior antes de iniciar a próxima, para que o som não fique sobreposto.
        if (IsMusicValid(trilhas[trilha_atual]))
            StopMusicStream(trilhas[trilha_atual]);

        trilha_atual = trilha;
        if (IsMusicValid(trilhas[trilha_atual]))
            PlayMusicStream(trilhas[trilha_atual]);
    }

    // A raylib lê a música em partes. Cada quadro dá à faixa ativa a chance de abastecer o buffer.
    if (IsMusicValid(trilhas[trilha_atual]))
        UpdateMusicStream(trilhas[trilha_atual]);
}

void alternar_som(void)
{
    som_ligado = !som_ligado;

    // A raylib continua avançando a música mesmo com o volume geral em zero.
    // Ao religar, cada faixa volta a usar o volume definido no carregamento.
    if (IsAudioDeviceReady())
        SetMasterVolume(som_ligado ? 1.0f : 0.0f);
}

bool som_esta_ligado(void)
{
    return som_ligado;
}

void encerrar_audio(void)
{
    if (!IsAudioDeviceReady())
        return;

    // Cada faixa possui seu próprio buffer. Libera todos antes de desligar o dispositivo.
    for (int faixa = TRILHA_JORNADA; faixa < TOTAL_TRILHAS; faixa++) {
        if (IsMusicValid(trilhas[faixa]))
            UnloadMusicStream(trilhas[faixa]);
        trilhas[faixa] = (Music){0};
    }
    trilha_atual = TRILHA_SILENCIO;
    CloseAudioDevice();
}
