/**
 * @file audio.c
 * @brief Controla as trilhas da partida e os efeitos das ações do jogador.
 */

#include "audio.h"
#include <raylib.h>

// O índice de cada faixa é o valor de TrilhaAudio. O espaço do silêncio fica vazio.
static Music trilhas[TOTAL_TRILHAS] = {0};
static TrilhaAudio trilha_atual = TRILHA_SILENCIO;
static bool som_ligado = true;
static Sound efeitos[TOTAL_EFEITOS] = {0};

/** @brief Carrega o efeito inteiro na memória para responder imediatamente ao jogador. */
static void carregar_efeito(EfeitoSonoro efeito, const char *arquivo, float volume)
{
    efeitos[efeito] = LoadSound(arquivo);
    if (IsSoundValid(efeitos[efeito]))
        SetSoundVolume(efeitos[efeito], volume);
}

void tocar_efeito(EfeitoSonoro efeito)
{
    if (som_ligado && IsAudioDeviceReady() && IsSoundValid(efeitos[efeito]))
        PlaySound(efeitos[efeito]);
}

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

    carregar_efeito(SOM_BOTAO, "assets/audio/efeitos/botao.ogg", 0.53f);
    carregar_efeito(SOM_ALTERNATIVA, "assets/audio/efeitos/alternativa.ogg", 0.50f);
    carregar_efeito(SOM_ACERTO, "assets/audio/efeitos/acerto.ogg", 0.50f);
    carregar_efeito(SOM_ERRO, "assets/audio/efeitos/erro.ogg", 0.42f);
    carregar_efeito(SOM_CARTA, "assets/audio/efeitos/virar_carta.ogg", 0.60f);
    carregar_efeito(SOM_ASA, "assets/audio/efeitos/batida_de_asa.wav", 0.60f);
    carregar_efeito(SOM_SOPRO, "assets/audio/efeitos/sopro_do_dragao.wav", 0.42f);
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
    for (int i = 0; i < TOTAL_EFEITOS; i++) {
        if (IsSoundValid(efeitos[i]))
            UnloadSound(efeitos[i]);
        efeitos[i] = (Sound){0};
    }
    CloseAudioDevice();
}
