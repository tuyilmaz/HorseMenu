#include "scriptFunction.hpp"

#include "game/backend/ScriptMgr.hpp"
#include "game/backend/FiberPool.hpp"
#include "game/pointers/Pointers.hpp"
#include "game/rdr/Scripts.hpp"

#include <rage/tlsContext.hpp>
#include <script/scrProgramTable.hpp>

namespace YimMenu
{
	script_function::script_function(const std::string& name, const rage::joaat_t script, const std::string& pattern, int32_t offset) :
	    m_name(name),
	    m_script(script),
	    m_pattern(pattern),
	    m_offset(offset),
	    m_ip(0)
	{
	}

	void script_function::populate_ip()
	{
		if (m_ip == 0)
		{
			auto program = Scripts::FindScriptProgram(m_script);

			if (!program)
				return;

			auto location = Scripts::GetCodeLocationByPattern(program, m_pattern);

			if (!location)
				LOG(FATAL) << "Failed to find pattern " << m_name << " in script " << program->m_name;
			else
				LOG(VERBOSE) << "Found pattern " << m_name << " in script " << program->m_name;

			m_ip = location.value() + m_offset;
		}
	}

	void script_function::call(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args)
	{
		auto tls_ctx   = rage::tlsContext::Get();
		auto stack     = (uint64_t*)thread->m_Stack;
		auto og_thread = *Pointers.CurrentScriptThread;

		*Pointers.CurrentScriptThread = thread;
		tls_ctx->m_RunningScript      = true;

		rage::scrThreadContext ctx = thread->m_Context;

		for (auto& arg : args)
			stack[ctx.m_StackPointer++] = arg;

		stack[ctx.m_StackPointer++] = 0;
		ctx.m_ProgramCounter    = m_ip;
		ctx.m_State                  = rage::eThreadState::idle;

		Scripts::RunScript(stack, program, &ctx);

		*Pointers.CurrentScriptThread         = og_thread;
		tls_ctx->m_RunningScript = og_thread != nullptr;
	}

	void script_function::call_latent(rage::scrThread* thread, rage::scrProgram* program, std::initializer_list<uint64_t> args, bool& done)
	{
        FiberPool::Push([this, thread, program, args, &done] {
            auto stack = (uint64_t*)thread->m_Stack;

			rage::eThreadState result = rage::eThreadState::idle;

			rage::scrThreadContext ctx = thread->m_Context;

			for (auto& arg : args)
				stack[ctx.m_StackPointer++] = arg;

			stack[ctx.m_StackPointer++] = 0;
			ctx.m_ProgramCounter    = m_ip;
			ctx.m_State                  = rage::eThreadState::idle;

			while (result != rage::eThreadState::killed)
			{
				auto tls_ctx   = rage::tlsContext::Get();
				auto og_thread = *Pointers.CurrentScriptThread;

				*Pointers.CurrentScriptThread = thread;
				tls_ctx->m_RunningScript      = true;

				auto old_ctx      = thread->m_Context;
				thread->m_Context = ctx;
				result = Scripts::RunScript(stack, program, &thread->m_Context);
				thread->m_Context = old_ctx;

				*Pointers.CurrentScriptThread = og_thread;
				tls_ctx->m_RunningScript      = og_thread != nullptr;

				ScriptMgr::Yield();
			}

			done = true;
		});
	}

	void script_function::static_call(std::initializer_list<uint64_t> args)
	{
		populate_ip();

		rage::scrThread* thread = (rage::scrThread*)new uint8_t[sizeof(rage::scrThread)];
		memcpy(thread, *Pointers.CurrentScriptThread , sizeof(rage::scrThread));

		void* stack 					 = new uint64_t[25000];
		thread->m_Stack                  = (rage::scrValue*)stack;
		thread->m_Context.m_StackSize    = 25000;
		thread->m_Context.m_StackPointer = 1;

		call(thread, Scripts::FindScriptProgram(m_script), args);

		delete[] stack;
		delete[] (uint8_t*)thread; // without the cast it ends up calling the destructor which leads to some pretty funny crashes
	}

	void script_function::operator()(std::initializer_list<uint64_t> args)
	{
		populate_ip();

		if (m_ip == 0)
			return;

		LOG(INFO) << 1;
		auto thread  = *Pointers.CurrentScriptThread;
		auto program = Scripts::FindScriptProgram(m_script);

		if (program)
		{
			LOG(INFO) << 2;
			call(thread, program, args);
		}
	}
}