#pragma once
#include <memory>
#include "ISender.h"
#include "Packet.h"
#include "PacketManager.h"

class IPacketHandler
{
protected:
	std::unordered_map<int, std::unique_ptr<pkt::PacketHandlerCallback>> mPacketHandlers;

public:
	IPacketHandler() = default;

	virtual ~IPacketHandler() = default;

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<pkt::Packet, T>>>
	void registerHandler(std::function<void(const std::shared_ptr<T>&)> handler)
	{
		constexpr int typeCode = pkt::PacketManager::getTypeCode<T>();
		static_assert(typeCode != -1, "Packet Is Not Defined");

		const auto internalHandler = [handler](const std::shared_ptr<pkt::Packet>& packet)
		{
			handler(std::static_pointer_cast<T>(packet));
		};

		mPacketHandlers[typeCode].reset(new pkt::PacketHandlerCallback(internalHandler));
	}

	bool getHandler(int typeCode, const pkt::PacketHandlerCallback** handler)
	{
		auto iter = mPacketHandlers.find(typeCode);
		if (iter == mPacketHandlers.end())
			return false;

		*handler = iter->second.get();
		return true;
	}
};

template<typename Handler>
class PacketHandler : public IPacketHandler
{
protected:
	PacketHandler() = default;

	template<typename T, typename = std::enable_if_t<std::is_base_of_v<pkt::Packet, T>>>
	void registerHandler(void (Handler::* handler)(const std::shared_ptr<T>&))
	{
		constexpr int typeCode = pkt::PacketManager::getTypeCode<T>();
		static_assert(typeCode != -1, "Packet Is Not Defined");

		const auto internalHandler = [handler, this](const std::shared_ptr<pkt::Packet>& packet)
		{
			(static_cast<Handler*>(this)->*handler)(std::static_pointer_cast<T>(packet));
		};

		mPacketHandlers[typeCode].reset(new pkt::PacketHandlerCallback(internalHandler));
	}
};
