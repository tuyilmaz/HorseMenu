#include "Hooks/AwardHook.hpp"
#include "Hooks/BasicHooks.hpp"
#include "NativeHook.hpp"
#include <script/scrProgramTable.hpp>
#include "game/pointers/Pointers.hpp"

namespace YimMenu
{
	native_hook::native_hook(rage::scrProgram* program, const std::unordered_map<NativeIndex, rage::scrNativeHandler>& native_replacements)
	{
		hook_instance(program, native_replacements);
	}

	native_hook::~native_hook()
	{
		if (m_handler_hook)
		{
			m_handler_hook->disable();
			m_handler_hook.reset();
		}

		if (m_vmt_hook)
		{
			m_vmt_hook->disable();
			m_vmt_hook.reset();
		}
	}

	void native_hook::hook_instance(rage::scrProgram* program, const std::unordered_map<NativeIndex, rage::scrNativeHandler>& native_replacements)
	{
		m_program  = program;
		m_vmt_hook = std::make_unique<vmt_hook>(m_program, 9);
		m_vmt_hook->hook(6, &scrprogram_dtor);
		m_vmt_hook->enable();

		m_handler_hook = std::make_unique<vmt_hook>(&m_program->m_native_entrypoints, m_program->m_native_count);
		m_handler_hook->enable();

		std::unordered_map<rage::scrNativeHandler, rage::scrNativeHandler> handler_replacements;

		for (auto& [replacement_index, replacement_handler] : native_replacements)
		{
			auto og_handler                  = NativeInvoker::get_handlers()[static_cast<int>(replacement_index)];
			handler_replacements[og_handler] = replacement_handler;
		}

		for (int i = 0; i < m_program->m_native_count; i++)
		{
			if (auto it = handler_replacements.find((rage::scrNativeHandler)program->m_native_entrypoints[i]);
			    it != handler_replacements.end())
			{
				m_handler_hook->hook(i, it->second);
			}
		}
	}

	void native_hook::scrprogram_dtor(rage::scrProgram* this_, char free_memory)
	{
		if (auto it = NativeHooks->m_native_hooks.find(this_); it != NativeHooks->m_native_hooks.end())
		{
			auto og_func = it->second->m_vmt_hook->get_original<decltype(&native_hook::scrprogram_dtor)>(6);
			it->second->m_vmt_hook->disable();
			it->second->m_vmt_hook.reset();
			it->second->m_handler_hook->disable();
			it->second->m_handler_hook.reset();
			NativeHooks->m_native_hooks.erase(it);
			og_func(this_, free_memory);
		}
		else
		{
			LOG(FATAL) << "Cannot find hook for program";
		}
	}

	constexpr auto ALL_SCRIPT_HASH = "ALL_SCRIPTS"_J;

	native_hooks::native_hooks()
	{
		add_native_detour(NativeIndex::NETWORK_AWARD_HAS_REACHED_MAXCLAIM, award_hooks::NETWORK_AWARD_HAS_REACHED_MAXCLAIM);

		for (auto& entry : *Pointers.ScriptProgramTable)
			if (entry.m_program)
				hook_program(entry.m_program);

		NativeHooks = this;
	}

	native_hooks::~native_hooks()
	{
		m_native_hooks.clear();
		NativeHooks = nullptr;
	}

	void native_hooks::add_native_detour(NativeIndex index, rage::scrNativeHandler detour)
	{
		add_native_detour(ALL_SCRIPT_HASH, index, detour);
	}

	void native_hooks::add_native_detour(rage::joaat_t script_hash, NativeIndex index, rage::scrNativeHandler detour)
	{
		if (const auto& it = m_native_registrations.find(script_hash); it != m_native_registrations.end())
		{
			it->second.emplace_back(index, detour);
			return;
		}

		m_native_registrations.emplace(script_hash, std::vector<native_detour>({{index, detour}}));
	}

	void native_hooks::hook_program(rage::scrProgram* program)
	{
		std::unordered_map<NativeIndex, rage::scrNativeHandler> native_replacements;
		const auto script_hash = program->m_name_hash;

		// Functions that need to be detoured for all scripts
		if (const auto& pairtt = m_native_registrations.find(ALL_SCRIPT_HASH); pairtt != m_native_registrations.end())
			for (const auto& native_hook_reg : pairtt->second)
				native_replacements.insert(native_hook_reg);

		// Functions that only need to be detoured for a specific script
		if (const auto& pairtt = m_native_registrations.find(script_hash); pairtt != m_native_registrations.end())
			for (const auto& native_hook_reg : pairtt->second)
				native_replacements.insert(native_hook_reg);

		if (!native_replacements.empty())
		{
			m_native_hooks.emplace(program, std::make_unique<native_hook>(program, native_replacements));
		}
	}
}