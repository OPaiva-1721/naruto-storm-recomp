#define VK_NO_PROTOTYPES
#include <vulkan/vulkan.h>
#include <jni.h>
#include <dlfcn.h>
#include <algorithm>
#include <sstream>
#include <string>
#include <vector>

namespace {
std::string quoted(const char* value) {
    std::ostringstream out;out << '"';
    const char hex[]="0123456789abcdef";
    for (const unsigned char* p=reinterpret_cast<const unsigned char*>(value);*p;++p) {
        if (*p=='"' || *p=='\\') out << '\\' << static_cast<char>(*p);
        else if (*p<32) out << "\\u00" << hex[*p>>4] << hex[*p&15];
        else out << static_cast<char>(*p);
    }
    out << '"';return out.str();
}
std::string version(uint32_t v) {
    return std::to_string(VK_VERSION_MAJOR(v))+"."+std::to_string(VK_VERSION_MINOR(v))+"."+std::to_string(VK_VERSION_PATCH(v));
}
std::string probe() {
    void* library=dlopen("libvulkan.so",RTLD_NOW|RTLD_LOCAL);
    if (!library) return R"({"loaderAvailable":false,"instanceCreated":false,"error":"libvulkan.so indisponível"})";
    auto get=reinterpret_cast<PFN_vkGetInstanceProcAddr>(dlsym(library,"vkGetInstanceProcAddr"));
    auto create=get?reinterpret_cast<PFN_vkCreateInstance>(get(VK_NULL_HANDLE,"vkCreateInstance")):nullptr;
    if (!create) {dlclose(library);return R"({"loaderAvailable":true,"instanceCreated":false,"error":"vkCreateInstance indisponível"})";}
    VkApplicationInfo app{};app.sType=VK_STRUCTURE_TYPE_APPLICATION_INFO;
    app.pApplicationName="Naruto diagnostics";app.apiVersion=VK_API_VERSION_1_0;
    VkInstanceCreateInfo info{};info.sType=VK_STRUCTURE_TYPE_INSTANCE_CREATE_INFO;info.pApplicationInfo=&app;
    VkInstance instance=VK_NULL_HANDLE;VkResult created=create(&info,nullptr,&instance);
    if (created!=VK_SUCCESS) {
        dlclose(library);
        return "{\"loaderAvailable\":true,\"instanceCreated\":false,\"vkResult\":"+std::to_string(created)+"}";
    }
    auto destroy=reinterpret_cast<PFN_vkDestroyInstance>(get(instance,"vkDestroyInstance"));
    auto enumerate=reinterpret_cast<PFN_vkEnumeratePhysicalDevices>(get(instance,"vkEnumeratePhysicalDevices"));
    auto properties=reinterpret_cast<PFN_vkGetPhysicalDeviceProperties>(get(instance,"vkGetPhysicalDeviceProperties"));
    std::ostringstream out;out << R"({"loaderAvailable":true,"instanceCreated":true,"surfaceTested":false,"physicalDeviceCount":)";
    uint32_t count=0;VkResult result=enumerate?enumerate(instance,&count,nullptr):VK_ERROR_INITIALIZATION_FAILED;
    if (result==VK_SUCCESS && count<=64 && properties) {
        std::vector<VkPhysicalDevice> devices(count);
        if (count) result=enumerate(instance,&count,devices.data());
        if (result==VK_SUCCESS && count<=devices.size()) {
            out << count << ",\"devices\":[";
            for (uint32_t i=0;i<count;++i) {
                VkPhysicalDeviceProperties p{};properties(devices[i],&p);
                p.deviceName[VK_MAX_PHYSICAL_DEVICE_NAME_SIZE-1]='\0';
                if (i) out << ',';
                out << "{\"name\":" << quoted(p.deviceName) << ",\"apiVersion\":" << quoted(version(p.apiVersion).c_str())
                    << ",\"vendorId\":" << p.vendorID << ",\"deviceType\":" << p.deviceType << '}';
            }
            out << ']';
        } else out << "-1,\"error\":\"enumeração incompleta ou alterada\",\"vkResult\":" << result;
    } else out << "-1,\"error\":\"consulta de dispositivos falhou\",\"vkResult\":" << result;
    out << '}';
    if (destroy) {destroy(instance,nullptr);dlclose(library);}
    // On a broken loader without destroy, keep it loaded rather than unload a live instance.
    return out.str();
}
}

extern "C" JNIEXPORT jbyteArray JNICALL
Java_io_github_opaiva1721_narutodiag_MainActivity_probeVulkan(JNIEnv* env,jclass) {
    const auto text=probe();
    auto bytes=env->NewByteArray(static_cast<jsize>(text.size()));
    if (bytes) env->SetByteArrayRegion(bytes,0,static_cast<jsize>(text.size()),reinterpret_cast<const jbyte*>(text.data()));
    return bytes;
}
