#pragma once
#include "scrPatternHandle.hpp"
#include "fwddec.hpp"

#include <cstdint>
#include <optional>
#include <string_view>
#include <vector>

namespace scrMemory
{
	class pattern
	{
		friend range;

	public:
		pattern(std::string_view ida_sig);

		inline pattern(const char* ida_sig) :
		    pattern(std::string_view(ida_sig))
		{
		}

		std::vector<std::optional<uint8_t>> m_bytes;
	};
}