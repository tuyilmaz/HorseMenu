#pragma once
#include "util/Joaat.hpp"
#include "ScriptMemory/scrPattern.hpp"
#include <script/scrThread.hpp>
#include <Script/scrProgram.hpp>

namespace YimMenu
{
	// a lightweight script function wrapper inspired by https://github.com/Parik27/V.Rainbomizer/blob/master/src/mission/missions_YscUtils.hh
	class script_function
	{
		joaat_t m_script;
		const scrMemory::pattern m_pattern;
		int32_t m_offset;
		int32_t m_ip;
		std::string m_name;

	public:
		script_function(const std::string& name, const joaat_t script, const std::string& pattern, int32_t offset);
		void populate_ip();
		void call(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args);
		void call_latent(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args, bool& done);

		// for pure functions that do not need access to thread stack
		void static_call(std::initializer_list<uint64_t> args);

		void operator()(std::initializer_list<uint64_t> args);
	};

	namespace scr_functions
	{
		static inline script_function give_reward("GR", "interactive_campfire"_J, "22 02 14 00 00 2F", 0);
		static inline script_function give_award("GA", "net_main_offline"_J, "22 05 24", 0);
	}
}