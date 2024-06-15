#pragma once
#include "game/rdr/ScriptFunction/ScriptMemory/scrPattern.hpp"
#include "util/Joaat.hpp"
#include <script/scrThreadContext.hpp>

namespace rage
{
	class scrProgram;
	class scrThread;
}

namespace YimMenu::Scripts
{
	extern inline rage::scrProgram* FindScriptProgram(rage::joaat_t hash);
	extern rage::scrThread* FindScriptThread(joaat_t hash);
	extern void RunAsScript(rage::scrThread* thread, std::function<void()> callback);
	extern void SendScriptEvent(uint64_t* data, int count, int bits);
	extern inline const std::optional<uint32_t> GetCodeLocationByPattern(rage::scrProgram* program, const scrMemory::pattern& pattern);
	extern rage::eThreadState RunScript(uint64_t* stack, rage::scrProgram* program, rage::scrThreadContext* ctx);
}