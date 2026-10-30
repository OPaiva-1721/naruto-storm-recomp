# Esqueleto Android — plano aprovado

Objetivo: um diagnóstico Android reproduzível no mesmo repositório do port PC,
sem executar Naruto, incluir dados de jogo ou alterar a versão Windows.

1. Reaproveitar ferramentas Gradle/NDK da base local, isolando o app de diagnóstico.
2. Mostrar ABI/processo, memória e consulta nativa ao loader/dispositivos Vulkan.
3. Tratar ausência de Vulkan, falha de instância e ausência de dispositivos sem crash.
4. Verificar build, testes do resumo e lint; conferir manifesto e conteúdo do APK.
5. Documentar build, teste manual de ciclo de vida e tarefas para colaboradores.
6. Revisar a branch inteira e abrir PR de fontes; não publicar APK nem dados derivados.

Critério de conclusão local: APK de diagnóstico compilado e verificado, testes
e documentação prontos. Abertura em aparelho, pausa/retomada e driver real ficam
explicitamente pendentes até o usuário disponibilizar o celular/porta adb.

Decisão: Java + JNI/C++ independente do runtime, sem SDL por enquanto. A base
antiga mistura runtime e jogo; trazê-la inteira impediria colaboradores de
compilar sem um dump. O futuro runtime deve entrar como módulo separado.
Versões conservadas da base: AGP 9.0.1, Gradle 9.1.0, NDK 28.2.13676358,
CMake 3.22.1; Java 17+, API mínima 29, compile/target 35. Sem atualização do SDK PS3.

O probe não cria superfície, swapchain nem desenha. Enumerar uma GPU não prova
renderização, compatibilidade do jogo ou desempenho. Nenhum teste automático
autoriza instalação/toques em aparelho nem publicação de binários.
