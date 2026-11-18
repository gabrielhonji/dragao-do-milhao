/**
 * @file arquivos_perguntas.h
 * @author Murilo Henrique Alves Martins (murilomartins.2007@alunos.utfpr.edu.br)
 * @brief Estrturação dos arquivos de pergunta - header
 * @version 0.1
 * @date 2026-11-15
 * 
 * @copyright Copyright (c) 2026
 * 
 */

 #ifndef ARQUIVOS_PERGUNTAS_H
    #define ARQUIVOS_PERGUNTAS_H

    #include "banco_perguntas.h"

    int carregar_csv(Banco_perguntas *banco_perguntas, FILE **arquivo_csv); //carrega o arquivo csv inicial com as perguntas

    int salvar_binario(Banco_perguntas *banco_perguntas, FILE **arquivo_dat); //salva os dados em um arquivo binário

    int carregar_binario(Banco_perguntas *banco_perguntas, FILE **arquivo_dat); //carrega os dados salvos do arquivo binário

    int exportar_csv(Banco_perguntas *banco_perguntas, FILE **arquivo_exportacao_csv); //exporta as perguntas para um outro arquivo_csv, contendo as alterações feitas nas perguntas

 #endif /*ARQUIVOS_PERGUNTAS_H*/