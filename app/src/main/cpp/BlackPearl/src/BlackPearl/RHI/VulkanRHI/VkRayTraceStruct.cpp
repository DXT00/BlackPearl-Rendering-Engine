#include "pch.h"
#include "VkRayTraceStruct.h"
#include "VkBuffer.h"
namespace BlackPearl {
	VkDeviceOrHostAddressConstKHR getBufferAddress(IBuffer* _buffer, uint64_t offset)
	{
		VkDeviceOrHostAddressConstKHR address{};
		if (!_buffer)
			return address;

		Buffer* buffer = static_cast<Buffer*>(_buffer);

		address.deviceAddress = buffer->deviceAddress + size_t(offset);
		return address;
	}


	AccelStruct::~AccelStruct()
	{
	}
	uint64_t AccelStruct::getDeviceAddress() const
	{
#ifdef NVRHI_WITH_RTXMU
		if (!desc.isTopLevel)
			return m_Context.rtxMemUtil->GetDeviceAddress(rtxmuId);
#endif
		return getBufferAddress(dataBuffer, 0).deviceAddress;
	}



	OpacityMicromap::~OpacityMicromap()
	{
	}
	uint64_t OpacityMicromap::getDeviceAddress() const
	{
		return getBufferAddress(dataBuffer, 0).deviceAddress;
	}

}