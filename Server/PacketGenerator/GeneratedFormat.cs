namespace PacketGenerator
{
    using System;
    using System.Collections.Generic;
    using System.Linq;
    using System.Text;
    using System.Threading.Tasks;

    public static class GeneratedFormat
    {
        public static readonly string TypeFileName = "GeneratedPacketType.h";
        public static readonly string StructFileName = "GeneratedPacketStruct.h";
		public static readonly string FactoryFileName = "GeneratedPacketFactory.h";

		public static int StartTypeCode { get; set; } = 1000;

		public static string BaseFormatForStruct { get; private set; } =
@"#pragma once
#include ""GeneratedPacketType.h""
namespace pkt
{{
	namespace typecode
	{{
#ifndef PACKETCODESTRUCT
#define PACKETCODESTRUCT
		template<typename T>
		struct PacketTypeCodeStruct
		{{
			static constexpr int code = -1;
		}};

		template<int typeCode>
		struct PacketTypeStruct
		{{
			using type = void;
		}};

		template<typename T>
		static constexpr int getTypeCode() {{ return PacketTypeCodeStruct<T>::code; }}
#endif	
{0}
	}}
}}";

		public static string PerPacketFormatForStruct { get; private set; } = 
@"        template<>
		struct PacketTypeCodeStruct<{0}>
		{{
			static constexpr int code = {1};
		}};

		template<>
		struct PacketTypeStruct<PacketTypeCodeStruct<{0}>::code>
		{{
			using type = {0};
		}};
";

		public static string BaseFormatForType { get; private set; } =
@"#pragma once
#include ""Packet.h""

namespace pkt
{{
{0}
}}";

		public static string PerPacketFormatForType { get; private set; } =
@"{0}
{{
private:
{1}
public:
{2}
{3}
{4}
{5}
{6}
{7}
{8}
protected:
{9}
{10}
}};
";
		public static string BaseFormatForFactory { get; private set; } =
@"#pragma once
#include <unordered_map>
#include <functional>

#include ""IPacketFactory.h""
#include ""GeneratedPacketStruct.h""

namespace pkt
{{
	class Packet;

	class GeneratedPacketFactory : public IPacketFactory
	{{
	public:
		virtual void registerPackets() override
		{{
			using namespace pkt::typecode;
{0}
		}}
	}};
}}";

		public static string PerPacketFormatForFactory { get; private set; } =
@"            PacketFactory factory{0} = []() {{ return std::shared_ptr<Packet>(new PacketTypeStruct<{1}>::type()); }};
			mContainer[PacketTypeCodeStruct<{0}>::code] = factory{0};";
	}
}
