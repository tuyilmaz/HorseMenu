#include "core/hooking/DetourHook.hpp"
#include "game/hooks/NativeHooks/NativeHook.hpp"
#include "game/hooks/Hooks.hpp"

namespace YimMenu::Hooks
{
	bool Script::InitNativeTables(rage::scrProgram* program)
	{
		bool ret = BaseHook::Get<Script::InitNativeTables, DetourHook<decltype(&Script::InitNativeTables)>>()->Original()(program);

		if (program->m_code_size && program->m_code_blocks)
		{
			NativeHooks->hook_program(program);
		}

		return ret;
	}
}