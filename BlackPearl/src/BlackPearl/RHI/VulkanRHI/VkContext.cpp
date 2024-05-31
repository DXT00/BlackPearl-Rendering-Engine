#include "pch.h"
#if GE_API_VULKAN

#include "VkContext.h"

namespace BlackPearl {

    void VulkanContext::nameVKObject(const void* handle, VkDebugReportObjectTypeEXT objtype, const char* name) const
    {
        if (extensions.EXT_debug_marker && name && *name && handle)
        {
            VkDebugMarkerObjectNameInfoEXT info;
            info.objectType = objtype;
            info.object = reinterpret_cast<uint64_t>(handle);
            info.pObjectName = name;
        /*    auto info = VkDebugMarkerObjectNameInfoEXT()
                .setObjectType(objtype)
                .setObject(reinterpret_cast<uint64_t>(handle))
                .setPObjectName(name);*/
            //(void)device.debugMarkerSetObjectNameEXT(&info);

           // vkDebugMarkerSetObjectNameEXT(device ,&info);

        }
    }
    void VulkanContext::error(const std::string& message) const
    {
        messageCallback->message(MessageSeverity::Error, message.c_str());

    }
    void VulkanContext::warning(const std::string& message) const
    {
        messageCallback->message(MessageSeverity::Warning, message.c_str());

    }
}
#endif
