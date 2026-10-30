package io.github.opaiva1721.narutodiag;

public final class ProbeSummaryTest {
    public static void main(String[] args) {
        assert ProbeSummary.status(false, false, 0).equals("Vulkan indisponível");
        assert ProbeSummary.status(true, false, 0).equals("Falha ao criar instância Vulkan");
        assert ProbeSummary.status(true, true, 0).equals("Nenhum dispositivo Vulkan encontrado");
        assert ProbeSummary.status(true, true, 2).equals("Consulta Vulkan concluída; renderização ainda não testada");
        assert ProbeSummary.status(true, true, -1).equals("Consulta de dispositivos Vulkan falhou");
        System.out.println("5 cenários de diagnóstico passaram.");
    }
}
