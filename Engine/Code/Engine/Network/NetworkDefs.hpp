#pragma once
#include <string>

struct MessageHeader
{
	std::uint16_t m_id;
	std::uint16_t m_size;
};