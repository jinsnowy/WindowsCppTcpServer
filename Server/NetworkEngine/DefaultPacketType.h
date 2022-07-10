#pragma once
#include "Packet.h"

namespace pkt
{
	class EchoMessage : public Packet
	{
	private:
		std::string message;

	public:
		EchoMessage()
		{}

		EchoMessage(const std::string& messageIn)
			:
			message(messageIn)
		{
		}

		static std::shared_ptr<EchoMessage> create(const std::string& messageIn)
		{
			return std::make_shared<EchoMessage>(messageIn);
		}

		virtual int getSize() const override
		{
			int totalSize = Packet::getSize();
			totalSize += sizeof(unsigned int);
			totalSize += (unsigned int)message.size();

			return totalSize;
		}

		virtual int getTypeCode() const { return 0; };

		const std::string& getMessage() const { return message; }

		void setMessage(const std::string& messageIn) { message = messageIn; }

		std::shared_ptr<EchoMessage> deepCopy() { return create(message); }

	protected:
		virtual bool serialize(std::vector<unsigned char>& bytes, size_t& offset) override
		{
			if (!Packet::serialize(bytes, offset))
				return false;

			if (!Serializer::tryWriteBytes((unsigned int)message.size(), bytes, offset))
				return false;
			offset += sizeof(unsigned int);

			if (!Serializer::tryWriteBytes(message, bytes, offset))
				return false;
			offset += (unsigned int)message.size();

			return true;
		}

		virtual void deserialize(const std::vector<unsigned char>& bytes, size_t& offset) override
		{
			Packet::deserialize(bytes, offset);

			unsigned int length = Serializer::toUInt32(bytes, offset);
			offset += sizeof(unsigned int);

			message = Serializer::toString(bytes, length, offset);
		}
	};
}
