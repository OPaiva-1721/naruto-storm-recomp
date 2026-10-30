# Tarefas Android para colaboradores

Este roteiro não diz que o jogo já funciona no Android. Cada PR deve trazer
reprodução, ferramenta/versão, evidência e limites. Dados derivados ficam locais.

| Ordem | Tarefa | Critério de conclusão |
| --- | --- | --- |
| 1 | Diagnóstico validado em um ARM64; ampliar cobertura | Abertura/consulta observadas e ciclo de vida confirmado pelo usuário em API35/Mali-G52 MC2 em 30/10/2026; falta repetir em outros drivers/aparelhos |
| 2 | Exercitar erros do probe nativo | Testes determinísticos para loader/funções ausentes e enumeração que muda/falha, sem substituir teste de driver |
| 3 | Surface e primeiro desenho Vulkan | Criar/destruir superfície e swapchain, desenhar padrão sintético, recuperar rotação/retomada em hardware; sem Naruto/ROM |
| 4 | Build ARM64 do runtime isolado | Usar SDK fixado, compilar testes sintéticos com NDK e executar os aplicáveis em dispositivo; separar falhas Win32/SSE/ABI |
| 5 | Importação local e boot do jogo | Projetar importação da cópia do usuário para armazenamento privado, validar versão/ELF; nenhum download de jogo/firmware e nenhuma permissão ampla de armazenamento |
| 6 | Integração do runtime com a superfície | Comparar Vulkan no PC e Android com cenas corretas; não esconder draws/efeitos para ganhar FPS |
| 7 | Entrada, áudio, save e desempenho | Controle físico/touch e ciclo de vida; depois medir CPU/GPU/RAM/aquecimento, save/load e estabilidade |

As tarefas 3 e 4 podem avançar separadamente; a 5 precisa de uma proposta revisada
antes de implementar cópia de arquivos ou carregamento. A 6 depende de 3/4/5.
Touch/otimização não são o primeiro bloqueador enquanto não houver imagem correta.

Uma boa primeira contribuição: documentar o resultado real de 1, sem alterar o
runtime. Uma contribuição em 2 exige testes de erro com driver falso/host, não
marcar resultado sintético como equivalência de hardware.
