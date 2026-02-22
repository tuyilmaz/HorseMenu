#include "core/scripting/LuaLibrary.hpp"
#include "core/scripting/LuaScript.hpp"
#include "core/scripting/LuaUtils.hpp"

#include "game/rdr/ScriptLocal.hpp"
#include "game/rdr/Scripts.hpp"
#include <script/scrThread.hpp>

namespace YimMenu::Lua
{
	class ScriptLocal : LuaLibrary
	{
		using LuaLibrary::LuaLibrary;

		// ScriptLocal.new("script_name" or hash, index)
		static int New(lua_State* state)
		{
			auto script_hash = GetHashArgument(state, 1);
			auto index = luaL_checkinteger(state, 2);

			auto thread = Scripts::FindScriptThread(script_hash);
			if (!thread || !thread->m_Stack)
			{
				lua_pushnil(state);
				return 1;
			}

			CreateObject<YimMenu::ScriptLocal>(state, thread->m_Stack, index);
			return 1;
		}

		// local:at(offset [, size])
		static int At(lua_State* state)
		{
			auto num_args = lua_gettop(state) - 1; // first argument is self
			auto& local = GetObject<YimMenu::ScriptLocal>(state, 1);

			auto size = num_args > 1 ? luaL_checkinteger(state, 3) : 0;
			if (size != 0)
				CopyObject<YimMenu::ScriptLocal>(state, local.At(luaL_checkinteger(state, 2), size));
			else
				CopyObject<YimMenu::ScriptLocal>(state, local.At(luaL_checkinteger(state, 2)));

			return 1;
		}

		// local:get_int()
		static int GetInt(lua_State* state)
		{
			auto& local = GetObject<YimMenu::ScriptLocal>(state, 1);
			int value = *local.As<int*>();
			lua_pushinteger(state, value);
			return 1;
		}

		// local:get_float()
		static int GetFloat(lua_State* state)
		{
			auto& local = GetObject<YimMenu::ScriptLocal>(state, 1);
			float value = *local.As<float*>();
			lua_pushnumber(state, value);
			return 1;
		}

		// local:set_int(value)
		static int SetInt(lua_State* state)
		{
			auto& local = GetObject<YimMenu::ScriptLocal>(state, 1);
			*local.As<int*>() = luaL_checkinteger(state, 2);
			return 0;
		}

		// local:set_float(value)
		static int SetFloat(lua_State* state)
		{
			auto& local = GetObject<YimMenu::ScriptLocal>(state, 1);
			*local.As<float*>() = static_cast<float>(luaL_checknumber(state, 2));
			return 0;
		}

		virtual void Register(lua_State* state) override
		{
			luaL_newmetatable(state, "ScriptLocal");
			{
				lua_newtable(state);
				{
					SetFunction(state, At, "at");
					SetFunction(state, GetInt, "get_int");
					SetFunction(state, GetFloat, "get_float");
					SetFunction(state, SetInt, "set_int");
					SetFunction(state, SetFloat, "set_float");
				}
				lua_setfield(state, -2, "__index"); // prototype
			}
			Metatable<YimMenu::ScriptLocal>::Register(state);

			lua_newtable(state);
			SetConstructor<New>(state);
			lua_setglobal(state, "ScriptLocal");
		}
	};

	ScriptLocal _ScriptLocal;
}
