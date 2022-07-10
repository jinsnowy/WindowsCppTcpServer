#pragma once
#include <memory>
#include "Logger.h"
#include "DefaultPacketStruct.h"
#include "IPacketFactory.h"

namespace pkt
{
	class PacketManager : public ISingleton<PacketManager>
	{
		friend class ISingleton<PacketManager>;
	private:
		Logger* mLogger;
		IPacketFactory* mImplDefault;
		IPacketFactory* mImpl;
		bool mDisconnectOnFailure;

	private:
		PacketManager() : 
			mLogger(Logger::getInstance()),
			mImplDefault(new DefaultPacketFactory()),
			mImpl(nullptr),
			mDisconnectOnFailure(true)
		{ 
		}

	public:
		~PacketManager()
		{
			clear();
		}

		void registerImpl(IPacketFactory* factory)
		{
			if (mImpl) { delete mImpl; mImpl = nullptr; }
			mImpl = factory;
			if (mImpl) { mImpl->registerPackets(); }
		}

		template<typename T, typename = std::enable_if_t<std::is_base_of_v<Packet, T>>>
		static constexpr int getTypeCode()
		{
			return pkt::typecode::getTypeCode<T>();
		}

	private:
		void clear()
		{
			if (mImpl) { delete mImpl; mImpl = nullptr; }
			if (mImplDefault) { delete mImplDefault; mImplDefault = nullptr; }
		}
	
	public:
		template<typename T, typename ...Args>
		static std::shared_ptr<T> create(Args&&... args) { return std::make_shared<T>(std::forward<Args>(args)...); }

		bool crackMessage(int typeCode, const std::vector<unsigned char>& data, std::shared_ptr<Packet>& packet)
		{
			try 
			{
				auto createPacket = mImpl->createPacket(typeCode);
				if (createPacket == nullptr)
				{
					createPacket = mImplDefault->createPacket(typeCode);
					if (createPacket == nullptr)
					{
						LOG_ERROR(mLogger, "cannot find packet for typecode(%d)", typeCode);
						return !mDisconnectOnFailure;
					}
				}

				createPacket->deserialize(data);
				packet = std::move(createPacket);
			}
			catch (std::exception e)
			{
				LOG_ERROR(mLogger, "crack message exception %s, typecode (%d), data length (%zd)", e.what(), typeCode, data.size());
				return false;
			}

			return true;
		}
	};
}
