#pragma once
#include <unordered_map>
#include <functional>

#include "GeneratedPacketStruct.h"

namespace pkt
{
	class Packet;

	class GeneratedPacketFactory : public IPacketFactory
	{
	public:
		virtual void registerPackets() override
		{
			using namespace pkt::typecode;
			PacketFactory factoryMyPacket = []() { return std::shared_ptr<Packet>(new PacketTypeStruct<1000>::type()); };
mContainer[PacketTypeCodeStruct<MyPacket>::code] = factoryMyPacket

		}
	};
}