# Rodar o Naruto no PC sem ninguém no controle

Scripts para o Claude e o Codex abrirem o jogo, navegarem até a vila e medirem. A entrada vai pelo `PAD_FILE` do `cellPad.c`: o script escreve `<máscara> [quadros]` em `pad.txt`.

```
bash tools/naruto/run.sh vila PERF=1 &      # abre o jogo; o log vai para build/run/vila.log
bash tools/naruto/to_village.sh v           # ~85 s até o carregamento; gera v_mode.png e v_load.png
bash tools/naruto/nav.sh v_in 90            # espera 90 s e tira um print da vila
powershell -c "Stop-Process -Name NarutoStorm -Force"
```

- As variáveis ficam em `env.sh` (`PS3_MAIN`, `NARUTO_BUILD`, `NARUTO_OUT`). Numa worktree do Codex, o padrão é o build da própria worktree; o ROM e o hdd0 vêm da cópia principal, só para leitura.
- `PERF=1` imprime o tempo por quadro, `DRAWSTAT=1` imprime os draws por quadro e `WATCHDOG_AT=<s>` imprime o lr de cada thread. `PS3_NULL_READ=0` já vem ligado, para o log não passar de 10 GB.
- Rode um jogo por vez, porque dois brigam pela GPU e pela janela. Prints e logs são derivados do jogo e não vão para o git: `build/` já é ignorada.
- Os tempos de `to_village.sh` foram medidos com o menu a 30 fps. Se uma mudança deixar o boot mais lento, confira `v_mode.png` (seleção de modo) e `v_load.png` (tela de carregamento).
