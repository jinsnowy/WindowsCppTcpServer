#pragma once
#include "DefaultPacketType.h"

namespace pkt
{
	namespace typecode
	{
#ifndef PACKETCODESTRUCT
#define PACKETCODESTRUCT
		template<typename T>
		struct PacketTypeCodeStruct
		{
			static constexpr int code = -1;
		};

		template<int typeCode>
		struct PacketTypeStruct
		{
			using type = void;
		};

		template<typename T>
		static constexpr int getTypeCode() { return PacketTypeCodeStruct<T>::code; }
#endif // !PACKETCODESTRUCT

		template<>
		struct PacketTypeCodeStruct<EchoMessage>
		{
			static constexpr int code = -10;
		};

		template<>
		struct PacketTypeStruct<PacketTypeCodeStruct<EchoMessage>::code>
		{
			using type = EchoMessage;
		};
	}
}
