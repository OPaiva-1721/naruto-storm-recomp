package io.github.opaiva1721.narutodiag;

public final class ProbeSummary {
    private ProbeSummary() {}
    public static String status(boolean loader, boolean instance, int devices) {
        if (!loader) return "Vulkan indisponível";
        if (!instance) return "Falha ao criar instância Vulkan";
        if (devices < 0) return "Consulta de dispositivos Vulkan falhou";
        if (devices == 0) return "Nenhum dispositivo Vulkan encontrado";
        return "Consulta Vulkan concluída; renderização ainda não testada";
    }
}
