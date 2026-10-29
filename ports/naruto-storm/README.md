# Naruto no Windows — desenvolvimento experimental

Edição usada: **BLES00371**. Não há garantia para outras regiões/atualizações.
Leia o [setup comum](../../docs/PC_SETUP.md), [status](../../docs/STATUS_PC.md)
e [controles de PC](../../docs/PC_CONTROLS.md).

Você fornece seu dump/ELF/firmware e gera o código e o executável localmente.
O preview não inclui jogo, lift, exe nem save. O SDK está fixado em `sdk.lock.json`.

## Execução

Configure `port.local.ini`, siga as etapas do setup e use:

```text
python tools/port.py naruto-storm run --interactive
```

O exemplo usa D3D12, configuração de controles e lift com `--fast-mem`. Vídeos
dependem do FFmpeg externo. Jobs SPURS coletados da sua execução podem exigir
um novo `lift-spu`; ainda não há validação completa do setup externo.

O perfil de teclado/mouse já foi exercitado na vila; controle físico, luta
completa e save novo/recarga precisam de verificação. Não inferir combate a
partir do FPS da vila. Android/Vulkan não são alvos deste preview.

Consulte o status para os limites observados nesta edição.
