#pragma once
#include <unordered_map>
#include <functional>

#include "DefaultPacketStruct.h"

namespace pkt
{
	class Packet;

	class IPacketFactory
	{
	public:
		using PacketFactory = std::function<std::shared_ptr<Packet>()>;
	protected:
		std::unordered_map<int, PacketFactory> mContainer;
	public:
		IPacketFactory();

		virtual ~IPacketFactory() = default;

		virtual void registerPackets() = 0;

		virtual std::shared_ptr<Packet> createPacket(int typeCode) final
		{
			auto iter = mContainer.find(typeCode);
			if (iter == mContainer.end())
				return nullptr;

			return (iter->second)();
		}
	};

	class DefaultPacketFactory : public IPacketFactory
	{
	public:
		virtual void registerPackets() override
		{
			using namespace pkt::typecode;
			PacketFactory factory_0 = []() { return std::shared_ptr<Packet>(new PacketTypeStruct<-10>::type()); };

			mContainer[PacketTypeCodeStruct<EchoMessage>::code] = factory_0;
		};
	};
}