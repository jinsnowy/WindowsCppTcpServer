#pragma once

namespace pkt
{
	class Packet;
}

class ISender
{
public:
	virtual ~ISender() = 0;

	virtual bool write(const std::shared_ptr<pkt::Packet>& packet) = 0;
};