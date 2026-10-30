# Android: diagnóstico, ainda sem jogo

O app em `android/` é o primeiro marco do port Android. Ele não carrega Naruto,
não importa dumps e não inclui o runtime PS3. É independente do build Windows.
Contribuir neste marco não exige possuir o jogo.

Mostra modelo, API Android, ABIs, processo 64-bit, memória total/disponível e
consulta nativa de Vulkan. O JNI abre `libvulkan.so`, cria uma instância 1.0,
enumera dispositivos e consulta nome/API/vendor/tipo. Não cria dispositivo lógico,
superfície ou swapchain, e não desenha. Uma resposta positiva não garante
compatibilidade do jogo nem FPS. Um dispositivo pode ser um renderizador por CPU.
O JSON conserva `deviceType` para não confundir software com GPU física.

Não pede permissão de internet, armazenamento, conta ou identificadores únicos.
O relatório fica na tela/logcat; só sai pelo compartilhamento quando você tocar
no botão e escolher um destino. Confira/remova o modelo do aparelho antes de postar.

## Compilar a partir de checkout limpo

- JDK 17 ou 21 no PATH (`java` e `javac`).
- Android SDK com plataforma 35, Build Tools 36.0.0, NDK 28.2.13676358 e CMake 3.22.1.
- Gradle 9.1.0 via wrapper; AGP 9.0.1 fixado. Não precisa Gradle global.
- `ANDROID_HOME` apontando seu SDK, ou `android/local.properties` ignorado com
  `sdk.dir=/caminho/do/sdk` (Windows: `sdk.dir=C\:/Android`).

Pelo SDK Manager do Android Studio, instale os componentes acima. Pela linha
de comando, usando seu SDK e aceitando seus termos quando solicitado:

```text
sdkmanager "platforms;android-35" "build-tools;36.0.0" "ndk;28.2.13676358" "cmake;3.22.1"
python tools/check_android_summary.py
```

Windows, na raiz do repo:

```text
android\gradlew.bat -p android --no-daemon :app:assembleDebug :app:lintDebug
python tools/check_android_apk.py android/app/build/outputs/apk/debug/app-debug.apk
```

Linux/macOS:

```sh
cd android
./gradlew --no-daemon :app:assembleDebug :app:lintDebug
cd ..
python3 tools/check_android_apk.py android/app/build/outputs/apk/debug/app-debug.apk
```

Resultado local: `android/app/build/outputs/apk/debug/app-debug.apk`. Assinatura
de debug gerada localmente, sem chave privada versionada. O CI compila/testa e
não publica APKs. O diagnóstico inclui ARM64 e x86_64; o segundo é útil para
emuladores, mas não valida um driver de celular. API mínima: Android 10/29.

Wrapper JAR oficial Gradle 9.1.0:
`76805e32c009c0cf0dd5d206bddc9fb22ea42e84db904b764f3047de095493f3`.
O ZIP Gradle também tem checksum fixado em gradle-wrapper.properties.
[Compatibilidade AGP](https://developer.android.com/build/releases/agp-9-0-0-release-notes)
e [checksums oficiais](https://gradle.org/release-checksums/).

## Teste manual — somente no aparelho autorizado

Não instalar automaticamente nem reutilizar porta antiga de adb sem confirmar.
Com o aparelho desbloqueado e o serial/endereço atual disponibilizado:

```text
adb -s SERIAL_ATUAL install -r android/app/build/outputs/apk/debug/app-debug.apk
adb -s SERIAL_ATUAL shell am start -n io.github.opaiva1721.narutodiag/.MainActivity
adb -s SERIAL_ATUAL logcat -d -s NarutoDiag AndroidRuntime
```

1. Abrir e conferir que aparece “Este app ainda não executa o jogo”.
2. Conferir modelo/ABI/RAM e resultado Vulkan; falha deve gerar relatório, não crash.
3. Atualizar algumas vezes; comparar resultado com as características conhecidas.
4. Ir à tela inicial e voltar, girar o aparelho, bloquear/desbloquear manualmente.
   O app deve sobreviver à recriação e permitir novo diagnóstico, sem manter a tela ligada.
5. Compartilhar o texto para um destino escolhido pelo usuário; cancelar também deve funcionar.

Registre commit, modelo, Android, resultado e o que foi efetivamente exercitado.
Não enviar dumps/firmware/chaves, código gerado ou arquivos de jogo em issues.

## Validação e limites

Os testes puros cobrem indisponibilidade do loader, falha de instância, zero
dispositivos, erro na consulta e consulta bem-sucedida sem alegar renderização.
O auditor de APK confere integridade, bibliotecas esperadas e ELF64 de cada ABI.
Lint cobre o uso das APIs Android. Esses testes não simulam um driver real.
O build local/CI não prova abertura em celular, ciclo de vida, importação de jogo,
renderização, áudio ou desempenho. A validação física do diagnóstico está registrada abaixo.

Verificação local em 29/10/2026: `assembleDebug` das duas ABIs e `lintDebug`
passaram (restam avisos de compile/target API 35, conservados da base). Assinatura
APK v2 conferida pelo apksigner; aapt2 confirmou API mínima 29, duas ABIs e
nenhuma permissão solicitada. Os cinco cenários puros, 38 testes do instalador
Windows e 34 testes de ferramentas passaram. A revisão de código pediu modo
executável do wrapper no Git, corrigido antes da publicação da branch.

Em 30/10/2026, o diagnóstico do commit `bfdff09` foi instalado e iniciado em um
aparelho ARM64 com Android API 35 e Mali-G52 MC2. O adb confirmou inicialização
a frio e os logs do próprio app confirmaram processo 64-bit, criação da instância
e um dispositivo Vulkan 1.3.278. O usuário confirmou funcionamento de atualizar,
girar o aparelho, sair/voltar e compartilhar/cancelar sem envio. Esses passos de
interface são confirmação manual do usuário, não automação nem teste instrumentado.
No PID observado após a confirmação não apareceu FATAL EXCEPTION.

Esse resultado conclui o marco de diagnóstico nesse aparelho. Não houve teste
de superfície/swapchain, desenho, boot de Naruto, áudio ou desempenho do jogo.
Outros aparelhos e drivers continuam sem validação; os logs brutos ficam locais.

## Como ajudar

Veja [tarefas Android](ANDROID_TASKS.md) e [contribuição](../CONTRIBUTING.md).
Comece pelo diagnóstico reproduzível; não prometa um port jogável com base no
FPS Windows. D3D12 não roda no Android. O backend Vulkan e os caminhos ARM64 do
runtime precisam de validação própria antes de plugar o jogo nesta aplicação.
