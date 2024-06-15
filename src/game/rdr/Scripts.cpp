#include "Scripts.hpp"
#include <script/scrThread.hpp>
#include <rage/tlsContext.hpp>
#include "game/pointers/Pointers.hpp"
#include "game/rdr/Natives.hpp"

namespace YimMenu::Scripts
{
	inline rage::scrProgram* FindScriptProgram(rage::joaat_t hash)
	{
		auto program = Pointers.GetScriptProgram(Pointers.scrProgramDirectory, hash);

		if (program)
			return program;

		return nullptr;
	}

	rage::scrThread* FindScriptThread(joaat_t hash)
	{
		for (auto& thread : *Pointers.ScriptThreads)
		{
			if (thread && thread->m_Context.m_ThreadId && thread->m_Context.m_ScriptHash == hash)
			{
				return thread;
			}
		}

		return nullptr;
	}

	void RunAsScript(rage::scrThread* thread, std::function<void()> callback)
	{
		auto og_thread = *Pointers.CurrentScriptThread;
		auto og_running_in_scrthread  = rage::tlsContext::Get()->m_RunningScript;
		*Pointers.CurrentScriptThread = thread;
		rage::tlsContext::Get()->m_RunningScript = true; // required to evade thread checks
		callback();
		rage::tlsContext::Get()->m_RunningScript = og_running_in_scrthread;
		*Pointers.CurrentScriptThread = og_thread;
	}

	void SendScriptEvent(uint64_t* data, int count, int bits)
	{
		if (auto thread = FindScriptThread("net_main_offline"_J))
		{
			RunAsScript(thread, [data, count, &bits] {
				SCRIPTS::TRIGGER_SCRIPT_EVENT(1, data, count, 0, &bits);
			});
		}
	}

	inline const std::optional<uint32_t> GetCodeLocationByPattern(rage::scrProgram* program, const scrMemory::pattern& pattern)
	{
		uint32_t code_size = program->m_code_size;
		for (uint32_t i = 0; i < (code_size - pattern.m_bytes.size()); i++)
		{
			for (uint32_t j = 0; j < pattern.m_bytes.size(); j++)
				if (pattern.m_bytes[j].has_value())
					if (pattern.m_bytes[j].value() != *program->get_code_address(i + j))
						goto incorrect;

			return i;
		incorrect:
			continue;
		}

		return std::nullopt;
	}

	rage::eThreadState RunScript(uint64_t* stack, rage::scrProgram* program, rage::scrThreadContext* ctx)
	{
		return Pointers.ScriptVM(stack, Pointers.ScriptGlobals, *(__int64*)Pointers.ScriptVMByte, program, ctx);
	}
}