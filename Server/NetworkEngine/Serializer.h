#pragma once
#include <vector>
#include <string>

namespace serialize_type_helper
{
	template<typename T>
	struct is_literal_type : std::false_type {};

	template<> struct is_literal_type<bool> : std::true_type {};
	template<> struct is_literal_type<int> : std::true_type {};
	template<> struct is_literal_type<unsigned int> : std::true_type {};
	template<> struct is_literal_type<char> : std::true_type {};
	template<> struct is_literal_type<unsigned char> : std::true_type {};
	template<> struct is_literal_type<short> : std::true_type {};
	template<> struct is_literal_type<unsigned short> : std::true_type {};
	template<> struct is_literal_type<long long> : std::true_type {};
	template<> struct is_literal_type<unsigned long long> : std::true_type {};
	template<> struct is_literal_type<float> : std::true_type {};
	template<> struct is_literal_type<double> : std::true_type {};
}

class Serializer
{
private:
	using Byte = unsigned char;
	using ByteArr = std::vector<Byte>;

public:
	template<typename T, typename = std::enable_if_t<serialize_type_helper::is_literal_type<std::decay_t<T>>::value>>
	static ByteArr getBytes(T&& data)
	{
		constexpr size_t N = sizeof(T);

		ByteArr bytes(N);
		memcpy_s(bytes, N, &data, N);

		return bytes;
	}

	static ByteArr getBytes(const std::string& value)
	{
		return ByteArr(value.begin(), value.end());
	}

	static int toInt32(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<int>(bytes, offset);
	}

	static unsigned int toUInt32(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<unsigned int>(bytes, offset);
	}

	static short toInt16(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<short>(bytes, offset);
	}

	static unsigned short toUInt16(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<unsigned short>(bytes, offset);
	}

	static long long toInt64(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<long long>(bytes, offset);
	}

	static unsigned long long toUInt64(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<unsigned long long>(bytes, offset);
	}

	static bool toBoolean(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<bool>(bytes, offset);
	}

	static Byte toByte(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<Byte>(bytes, offset);
	}

	static char toChar(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<char>(bytes, offset);
	}

	static float ToSingle(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<float>(bytes, offset);
	}

	static double toDouble(const ByteArr& bytes, const size_t& offset = 0)
	{
		return toValue<double>(bytes, offset);
	}

	static std::string toString(const ByteArr& bytes, const size_t& length, const size_t& offset = 0)
	{
		return std::string(bytes.begin() + offset, bytes.begin() + offset + length);
	}

	static int toInt32(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<int>(bytes, offset);
	}

	static unsigned int toUInt32(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<unsigned int>(bytes, offset);
	}

	static short toInt16(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<short>(bytes, offset);
	}

	static unsigned short toUInt16(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<unsigned short>(bytes, offset);
	}

	static long long toInt64(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<long long>(bytes, offset);
	}

	static unsigned long long toUInt64(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<unsigned long long>(bytes, offset);
	}

	static bool toBoolean(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<bool>(bytes, offset);
	}

	static Byte toByte(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<Byte>(bytes, offset);
	}

	static char toChar(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<char>(bytes, offset);
	}

	static float ToSingle(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<float>(bytes, offset);
	}

	static double toDouble(const Byte* bytes, const size_t& offset = 0)
	{
		return toValue<double>(bytes, offset);
	}

	static std::string toString(const Byte* bytes, const size_t& length, const size_t& offset = 0)
	{
		return std::string(bytes + offset, bytes + offset + length);
	}

	template<typename T, typename = std::enable_if_t<serialize_type_helper::is_literal_type<std::decay_t<T>>::value>>
	static inline T toValue(const ByteArr& bytes, const size_t& offset)
	{
		T value; std::memcpy(&value, bytes.data() + offset, sizeof(T));
		return value;
	}

	template<typename T, typename = std::enable_if_t<serialize_type_helper::is_literal_type<std::decay_t<T>>::value>>
	static inline T toValue(const Byte* bytePtr, const size_t& offset)
	{
		T value; std::memcpy(&value, bytePtr + offset, sizeof(T));
		return value;
	}
public:
	template<typename T, typename = std::enable_if_t<serialize_type_helper::is_literal_type<std::decay_t<T>>::value>>
	static bool tryWriteBytes(T&& data, ByteArr& bytes, const size_t& offset = 0)
	{
		constexpr size_t length = sizeof(T);
		if (length + offset > bytes.size())
			return false;

		memcpy_s(bytes.data() + offset, length, &data, length);

		return true;
	}

	static bool tryWriteBytes(Byte* data, size_t length, ByteArr& bytes, const size_t& offset = 0)
	{
		if (length + offset > bytes.size())
			return false;

		memcpy_s(bytes.data() + offset, length, data, length);

		return true;
	}

	static bool tryWriteBytes(const std::string& data, ByteArr& bytes, const size_t& offset = 0)
	{
		size_t length = data.size();
		if (length + offset > bytes.size())
			return false;

		memcpy_s(bytes.data() + offset, length, data.data(), length);

		return true;
	}
};

