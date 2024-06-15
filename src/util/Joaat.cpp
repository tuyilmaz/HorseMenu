#include "Joaat.hpp"

namespace YimMenu
{
	constexpr joaat_t Joaat(const std::string_view str)
	{
		joaat_t hash = 0;
		for (auto c : str)
		{
			hash += ToLower(c);
			hash += (hash << 10);
			hash ^= (hash >> 6);
		}
		hash += (hash << 3);
		hash ^= (hash >> 11);
		hash += (hash << 15);
		return hash;
	}

	int is_num(const std::string& s)
	{
		std::string::const_iterator it = s.begin();

		if (s.starts_with("-"s))
			++it;

		while (it != s.end() && std::isdigit(*it))
			++it;
		return !s.empty() && it == s.end();
	}

	joaat_t GetJoaatHashOfString(const std::string s)
	{
		joaat_t hash;

		if (is_num(s))
		{
			hash = std::stoi(s);
		}
		else
		{
			if (s.starts_with("0x"s))
			{
				hash = std::stoi(s, nullptr, 16);
			}
			else
			{
				hash = Joaat(s);
			}
		}

		return hash;
	}
}