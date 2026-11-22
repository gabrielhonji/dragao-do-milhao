# Pendências

## Gabriel

- [x] Testar as perguntas do banco, a confirmação, o avanço e as ajudas no painel de jornada e batalha.
- [x] Montar e testar a prévia da gruta com quatro estados e filhotes pelos ovos restantes.
- [ ] Criar a confirmação de Retirada Tática. Cancelar volta à mesma pergunta. Ligar vitória, derrota e retirada confirmada à gruta quando o quiz informar o desfecho.
- [ ] Montar o pergaminho para digitar o nome e a tela de ranking. Combinar com Murilo os dados exibidos e os desempates.
- [ ] Refinar as telas no Figma e aplicar pixel art, transições e sons de cliques e de resposta.
- [ ] Retestar o F11 no Windows com música tocando e outra janela sobre o quiz.
- [ ] Depois do refino, adaptar o jogo de memória do Burro para a janela e ligar o resultado à ajuda.
- [ ] Depois do Burro, implementar o minigame de Espaço do Dragão e o voo da carta da Princesa.

## Murilo

- [ ] Fechar a lógica de acerto, erro e retirada. Cada marco concluído (5 ou 10) concede um ovo uma vez por tentativa. Um erro com ovo consome um e retoma após o último marco, na questão 6 ou 11. Um erro sem ovos encerra a tentativa.
- [ ] Separar a posição da jornada do histórico de IDs e preservar o histórico ao recuar. Sortear perguntas inéditas da dificuldade correta na retomada. Tratar banco insuficiente como erro de disponibilidade.
- [ ] Combinar com Gabriel o resultado que o quiz entrega à interface: desfecho, progresso atingido, ovos restantes e tempo ativo. Alinhar quando o tempo pausa nas transições e no menu de confirmação.
- [ ] Combinar com Gabriel os critérios de registro e desempate do ranking. O nome vem do pergaminho e a derrota fica fora do ranking.

## Enzo

- [ ] Combinar com Murilo a ligação das ajudas. Só aplicar o efeito se vencer.
