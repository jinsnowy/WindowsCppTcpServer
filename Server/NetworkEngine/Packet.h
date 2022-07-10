#pragma once
#include <string>
#include <vector>
#include <memory>
#include <map>
#include <set>
#include <unordered_map>
#include <unordered_set>
#include <functional>
#include "Serializer.h"

namespace pkt
{
	class Packet;

	static constexpr size_t MAX_PACKET_SIZE = 1024;
	static constexpr size_t HEADER_SIZE = sizeof(int) + sizeof(int);
	
	using PacketHandlerCallback = std::function<void(const std::shared_ptr<Packet>&)>;

	class Packet
	{
	public:
		Packet() = default;

		virtual ~Packet() = 0;

		virtual int getSize() const { return HEADER_SIZE; };

		virtual int getTypeCode() const = 0;

		std::vector<unsigned char> serialize()
		{
			size_t offset = 0;
			std::vector<unsigned char> bytes(getSize(), 0);
			if (!serialize(bytes, offset))
				return {};

			return bytes;
		}

		virtual void deserialize(const std::vector<unsigned char>& bytes)
		{
			size_t offset = 0;
			deserialize(bytes, offset);
		}

	protected:
		virtual bool serialize(std::vector<unsigned char>& bytes, size_t& offset)
		{
			int packetSize = getSize();
			if (!Serializer::tryWriteBytes(packetSize, bytes, offset))
				return false;
			offset += sizeof(int);

			int typeCode = getTypeCode();
			if (!Serializer::tryWriteBytes(typeCode, bytes, offset))
				return false;
			offset += sizeof(int);

			return true;
		}

		virtual void deserialize(const std::vector<unsigned char>& bytes, size_t& offset)
		{
			int packetSize = Serializer::toInt32(bytes, offset);
			offset += sizeof(int);
			int typeCode = Serializer::toInt32(bytes, offset);
			offset += sizeof(int);
		}
	};
}
